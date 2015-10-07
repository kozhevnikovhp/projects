/*	Radyne.cpp - implementation file for Radyne modems interfaces

	Author: Evgeny A. Kozhevnikov, Zelenograd, Moscow, Russia evgeny@kozhevnikov.org

Revision history:
	20/Aug/2003 - initial creation
	22/Aug/2003 - remote Tcp/Ip control added
	22/Sep/2006	- M&C bus address can be changed (plus scanning capability)
*/

#include "stdafx.h"
#include "Radyne.h"

const unsigned char SYNC = 0x16;

const int RClock_External = 0;
const int RClock_Internal = 1;
const int RClock_EXC = 2;
const int RClock_RX_SAT = 3;

const int TClock_SCT_Internal = 0;
const int TClock_SCT_SCR = 1;
const int TClock_SCT_ExtBNC = 2;
const int TClock_External_SCTE = 3;

static const char *RADYNE_MODEM_SCRAMBLER_MODES[] = {
	"Disable",		// 0
	"Enable"		// 1
};

static const char *RADYNE_MODEM_DIFF_CODING_MODES[] = { "Disabled", "Enabled" };

static const char *RADYNE_MODEM_R_CLOCK_SOURCES[] = {"SCTE(Ext)", "SCT(Int)", "EXC", "RX SAT"};
static const char *RADYNE_MODEM_T_CLOCK_SOURCES[] = {"SCT(Int)", "SCT (SCR)", "SCT(Ext.BNC)", "(SCTE(Ext)"};

// CRadyneModem implementation

CRadyneModem::CRadyneModem()
{
	m_bParamSavingInNvRamRightAfterSettingEnabled = FALSE;
	m_ModemAddress = 32;
	m_ControllerAddress = 200;
	m_FSN = 0;
	m_MCReplyLength = 0;
}

CRadyneModem::~CRadyneModem()
{
}

void CRadyneModem::WriteLog(unsigned char *pszString, int length)
{
	if (!IsLogFileEnabled()) return;
}

//virtual
CTerminal *CRadyneModem::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
		return &m_SerialPort;
	case MC_TELNET:
		return &m_TcpIpSocket;
	}
	return NULL;
}

//virtual
void CRadyneModem::OnMcConnected()
{
	if (m_ControlType == MC_TELNET)
	{
		GetTerminal()->SetWriteTimeout(2000);
		GetTerminal()->SetReadTimeout(2000);
	}
}

MC_ErrorCode CRadyneModem::Command(int CommandLength)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, CommandLength);

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	return EC;
}

//virtual
MC_ErrorCode CRadyneModem::ReadReplyUntilPrompt()
{
	memset(m_pszReply, 0, sizeof(m_pszReply));
	m_MCReplyLength = 0;
	MC_ErrorCode EC;

	// "Eat" garbage until sync byte
	do
	{
		if (!GetTerminal()->Read((char *)m_pszReply, 1))
			return MC_DEVICE_NOT_RESPONDING;
	} while (m_pszReply[0] != SYNC);
	m_MCReplyLength = 1;
	
	// Header (data bytes count, src/dst addresses, FSN, OpCode)
	if (!GetTerminal()->Read((char *)m_pszReply+1, 7))
		return MC_DEVICE_NOT_RESPONDING;
	m_MCReplyLength += 7;

	// Check SRC address
	if (m_pszReply[3] != m_ModemAddress)
		return MC_INVALID_ADDRESS;

	// Check DST address
	if (m_pszReply[4] != m_ControllerAddress)
		return MC_INVALID_ADDRESS;

	// Read data bytes
	unsigned short DataLength = MAKEWORD(m_pszReply[2], m_pszReply[1]);
	if (!GetTerminal()->Read((char *)m_pszReply+8, DataLength))
		return MC_DEVICE_NOT_RESPONDING;
	m_MCReplyLength += DataLength;

	// Read CRC
	if (!GetTerminal()->Read((char *)(m_pszReply+8+DataLength), 1))
		return MC_DEVICE_NOT_RESPONDING;
	++m_MCReplyLength;

	unsigned char CalculatedCRC = CheckSum(m_pszReply+1, 7+DataLength);
	if (CalculatedCRC != m_pszReply[8+DataLength])
		return MC_BAD_CRC;

	// Analyze Error code
	unsigned char ErrorCode = m_pszReply[7];
	switch (ErrorCode)
	{
	case 0x00: EC = MC_OK; break;
	case 0xFA: EC = MC_INVALID_ADDRESS; break;
	case 0xFB:
	case 0xFC:
	case 0xFE: EC = MC_COMMAND_NOT_SUPPORTED; break;
	case 0xFD: EC = MC_BAD_CRC; break;
	case 0xFF: EC = MC_INVALID_PARAMETERS; break;
	default: EC = MC_GENERIC_ERROR;
	}
	
	GetTerminal()->ClearAllBuffers();
	return EC;
}

//virtual
void CRadyneModem::SetMCBusAddress(unsigned int Address)
{
	m_ModemAddress = LOBYTE(LOWORD(Address));
}

//virtual
BOOL CRadyneModem::CheckMCBusAddress(unsigned int Address)
{
	if (Address < GetMinMCBusAddressScanning() ||
		Address > GetMaxMCBusAddressScanning())
		return FALSE;
	if (!IsControllable())
		return FALSE;

	BOOL bMyAddress = FALSE;
	unsigned int freq = 0;
	unsigned int StoredAddress = GetMCBusAddress();
	SetMCBusAddress(Address);
	MC_ErrorCode EC = GetRFrequency(freq, 1);
	if (EC == MC_OK)
		bMyAddress = TRUE;
	SetMCBusAddress(StoredAddress);
	return bMyAddress;
}

int CRadyneModem::FillCommandBuffer(unsigned short CommandCode, unsigned char *pcDataBytes, unsigned short nDataBytes)
{
	memset(m_pszCommand, 0, sizeof(m_pszCommand));
	int i=0;
	m_pszCommand[i++] = SYNC; // opening SYNC byte
	// Data bytes counter
	m_pszCommand[i++] = HIBYTE(nDataBytes);
	m_pszCommand[i++] = LOBYTE(nDataBytes);
	// Controller address
	m_pszCommand[i++] = m_ControllerAddress; // 
	// Modem address
	m_pszCommand[i++] = m_ModemAddress; // 
	m_pszCommand[i++] = m_FSN++; // Frame sequence number
	// OPCODE
	m_pszCommand[i++] = HIBYTE(CommandCode);
	m_pszCommand[i++] = LOBYTE(CommandCode);
	//memcpy(m_pszCommand+i, &CommandCode, sizeof(CommandCode)); i += sizeof(CommandCode);
	// Data bytes
	if (pcDataBytes && nDataBytes)
	{
		for (int j=0; j<nDataBytes; j++)
			m_pszCommand[i++] = pcDataBytes[j]; // data bytes
	}
	// CRC
	m_pszCommand[i++] = CheckSum(m_pszCommand+1, i-1);
	return i;
}

unsigned char CRadyneModem::CheckSum(unsigned char *pData, int Count)
{
	unsigned int sum = 0;
	for (int i=0; i<Count; i++)
		sum += pData[i];
	sum = sum % 256;
	unsigned char ret = (unsigned char)sum;
	return ret;
}

unsigned int CRadyneModem::RawDataToInt(unsigned char *pszRawData)
{
	unsigned int IntValue = MAKELONG(MAKEWORD(pszRawData[3], pszRawData[2]), MAKEWORD(pszRawData[1], pszRawData[0]));
	return IntValue;
}

short CRadyneModem::RawDataToSignedShort(unsigned char *pszRawData)
{
	unsigned int IntValue = MAKEWORD(pszRawData[1], pszRawData[0]);
	return IntValue;
}

void CRadyneModem::IntToRawData(unsigned int IntValue, unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	pszRawData[3] = (unsigned char)((IntValue & 0x000000FF) >> 0);
	pszRawData[2] = (unsigned char)((IntValue & 0x0000FF00) >> 8);
	pszRawData[1] = (unsigned char)((IntValue & 0x00FF0000) >> 16);
	pszRawData[0] = (unsigned char)((IntValue & 0xFF000000) >> 24);
}

void CRadyneModem::SignedShortToRawData(short ShortValue, unsigned char *pszRawData)
{
	pszRawData[1] = (unsigned char)((ShortValue & 0x00FF) >> 0);
	pszRawData[0] = (unsigned char)((ShortValue & 0xFF00) >> 8);
}

// Common device commands
// 10 MHz reference type
// Frequency commands
// Modulation Type commands
// Output
// Fec Type commands
// Data clock source
// Data commands
// BER
// Differential encoding/decoding commands
// Scrambling/Descrambling
// Spectral inversion
// Data inversion


// CRadyneDMD2401 implementation

char *CRadyneDMD2401::m_pszName = "Radyne DMD-2401 (140 MHz modem)";

static const char *RADYNE_DMD2401_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK"
};

static const char *RADYNE_2401_FEC_MODES[] = {
	"None",
	"Viterbi 1/2",
	"Viterbi 3/4",
	"Viterbi 7/8"
};

CRadyneDMD2401::CRadyneDMD2401()
{
}

CRadyneDMD2401::~CRadyneDMD2401()
{
}

// Common device commands

//virtual
MC_ErrorCode CRadyneDMD2401::Identify(BOOL &bIdentified)
{
	bIdentified = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

/*	int CommandLength = FillCommandBuffer(0x90, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	m_pszReply[m_ReturnedDataLength] = 0x00;
	if (EC != MC_OK)
		return EC;
	if (strstr((char *)m_pszReply, "PSM-2100"))
		bIdentified = TRUE;

	return EC; // == OK!*/
	return MC_OK;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetDeviceVersionString(char *&pszString)
{
/*	pszString = m_szVersion;
	m_szVersion[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	// Model
	strcpy(m_szVersion, "Model : ");
	int CommandLength = FillCommandBuffer(0x90, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	m_pszReply[m_ReturnedDataLength] = 0x00;
	strcat(m_szVersion, (char *)m_pszReply);
	strcat(m_szVersion, szCRLF);

	// Software version
	CommandLength = FillCommandBuffer(0x91, modeRead, NULL, 0);
	EC = Command(CommandLength);
	m_pszReply[m_ReturnedDataLength] = 0x00;
	strcat(m_szVersion, "Software version : ");
	strcat(m_szVersion, (char *)m_pszReply);
	strcat(m_szVersion, szCRLF);

	// Serial number
	CommandLength = FillCommandBuffer(0x92, modeRead, NULL, 0);
	EC = Command(CommandLength);
	unsigned int SN = RawDataToInt(m_pszReply);
	strcat(m_szVersion, "Serial number : ");
	char szSerial[16];
	sprintf(szSerial, "%d", SN);
	strcat(m_szVersion, szSerial);
	strcat(m_szVersion, szCRLF);

	return EC;*/
	pszString = "Version = STUB";
	return MC_OK;
}

// Non Volatile RAM

// 10 MHz reference

// IF params and status

//virtual
MC_ErrorCode CRadyneDMD2401::GetRIfParams(CDemIfParams &Params, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Params.m_Frequency = RawDataToInt(m_pszReply+9) / 1000;
	Params.m_SearchRange = m_pszReply[17]*1000; // Hz -> KHz
	if (m_pszReply[24])
		Params.m_ModulationType = 0; // BPSK
	else
		Params.m_ModulationType = 1; // QPSK

	Params.m_bSpectrumInversion = (m_pszReply[34] != 0); 

	return MC_OK;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetRIfStatus(CDemIfStatus &Status, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = IsRCarrier(Status.m_bCarrier, Demodulator);
	if (EC != MC_OK)
		return EC;
	Status.m_bDemLock = Status.m_bCarrier; // TODO: find out if there is separate param in Radyne as well as in Datum, the same value for now (quick fix)

	int CommandLength = FillCommandBuffer(0x240D, NULL, 0);
	EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Status.m_SN = RawDataToSignedShort(m_pszReply+12) / 100.;
	Status.m_InputLevel = -m_pszReply[17];

	EC = GetDemodulatorOffset(Status.m_Offset, Demodulator); 

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTIfParams(CModIfParams &Params, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_Frequency = RawDataToInt(m_pszReply+9) / 1000;
	if (m_pszReply[24])
		Params.m_ModulationType = 0; // BPSK
	else
		Params.m_ModulationType = 1; // QPSK
	Params.m_bOutputEnabled = (m_pszReply[36] != 0); 
	Params.m_OutputLevel = RawDataToSignedShort(m_pszReply+33)/10.;
	Params.m_bSpectrumInversion = (m_pszReply[38] != 0);

	return EC;
}

// Data params

//virtual
MC_ErrorCode CRadyneDMD2401::GetRDataParams(CModemDataParams &Params, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_DataRate = RawDataToInt(m_pszReply+13);
	Params.m_ClockSource = CalcCombinedRClockSource(m_pszReply[39]);
	Params.m_bClockInverted = (m_pszReply[40] != 0);
	
	switch (m_pszReply[25])
	{
	case 0: Params.m_FecMode = 0; break; // None
	case 1: Params.m_FecMode = 1; break; // Viterbi 1/2
	case 3: Params.m_FecMode = 2; break; // Viterbi 3/4
	case 5: Params.m_FecMode = 3; break; // Viterbi 7/8
	default: Params.m_FecMode = 0; break; // None
	}

	if (m_pszReply[31])
		Params.m_DifferentialCodingMode = 1; // ON
	else
		Params.m_DifferentialCodingMode = 0; // OFF

	if (m_pszReply[32])
		Params.m_ScramblingMode = 1; // ON
	else
		Params.m_ScramblingMode = 0; // OFF

	Params.m_bDataInverted = (m_pszReply[77] != 0);

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTDataParams(CModemDataParams &Params, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_DataRate = RawDataToInt(m_pszReply+16);
	Params.m_ClockSource = CalcCombinedTClockSource(m_pszReply[40], m_pszReply[42]);;
	Params.m_bClockInverted = (m_pszReply[41] != 0);
	
	switch (m_pszReply[25])
	{
	case 0: Params.m_FecMode = 0; break; // None
	case 1: Params.m_FecMode = 1; break; // Viterbi 1/2
	case 3: Params.m_FecMode = 2; break; // Viterbi 3/4
	case 5: Params.m_FecMode = 3; break; // Viterbi 7/8
	default: Params.m_FecMode = 0; break; // None
	}

	if (m_pszReply[35])
		Params.m_DifferentialCodingMode = 1; // ON
	else
		Params.m_DifferentialCodingMode = 0; // OFF

	if (m_pszReply[32])
		Params.m_ScramblingMode = 1; // ON
	else
		Params.m_ScramblingMode = 0; // OFF
	Params.m_bDataInverted = (m_pszReply[74] != 0);

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetRFrequency(unsigned int &Frequency, int Demodulator)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Frequency = RawDataToInt(m_pszReply+9) / 1000;
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetRFrequency(unsigned int &Frequency, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFrequency(Frequency, Demodulator))
		return MC_OK; // already set

	IntToRawData(Frequency * 1000, m_pszIntValue);
	int CommandLength = FillCommandBuffer(0x2A01, m_pszIntValue, sizeof(m_pszIntValue));
	MC_ErrorCode EC = Command(CommandLength);

	GetRFrequency(Frequency, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTFrequency(unsigned int &Frequency, int Modulator)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Frequency = RawDataToInt(m_pszReply+9) / 1000;
	return MC_OK;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFrequency(Frequency, Modulator))
		return MC_OK; // already set

	IntToRawData(Frequency * 1000, m_pszIntValue);
	int CommandLength = FillCommandBuffer(0x2602, m_pszIntValue, sizeof(m_pszIntValue));
	MC_ErrorCode EC = Command(CommandLength);

	GetTFrequency(Frequency, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	SearchRange = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	SearchRange = m_pszReply[17]*1000; // Hz -> KHz
	return MC_OK;
}

//virtual 
MC_ErrorCode CRadyneDMD2401::SetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateSearchRange(SearchRange, Demodulator))
		return MC_OK; // already set

	SearchRange /= 1000; // Hz -> KHz
	unsigned char cSearchRange = (unsigned char)(SearchRange & 0x000000FF);
	int CommandLength = FillCommandBuffer(0x2A04, &cSearchRange, sizeof(cSearchRange));
	MC_ErrorCode EC = Command(CommandLength);

	GetSearchRange(SearchRange, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetEbNo(double &EbNo, int Demodulator)
{
	EbNo = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x240D, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	EbNo = RawDataToSignedShort(m_pszReply+12) / 100.;
	return MC_OK;
}

//virtual
MC_ErrorCode CRadyneDMD2401::IsRCarrier(BOOL &bValid, int Demodulator)
{
	bValid = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2437, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	bValid = ((m_pszReply[8] & 1) != 0);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetDemodulatorOffset(int &Offset, int Demodulator)
{
	Offset = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x240C, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Offset = RawDataToInt(m_pszReply+35);
	if (m_pszReply[44])
		Offset = -Offset;
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetDemodulatorOffset(int &Offset, int Demodulator)
{
	return MC_OK;
/*	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDemodulatorShift(Offset, Demodulator))
		return MC_OK; // already set

	IntToRawData(Offset, m_pszIntValue); // Hz
	int CommandLength = FillCommandBuffer(0x56, modeExecute, m_pszIntValue, sizeof(m_pszIntValue));
	MC_ErrorCode EC = Command(CommandLength);

	return EC;*/
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetInputLevel(double &InputLevel, int Demodulator)
{
	InputLevel = -10000.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x240D, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	InputLevel = -m_pszReply[17];
	return EC;
}

// Modulation type

//virtual
int CRadyneDMD2401::GetRModulationTypeCount()
{
	return sizeof(RADYNE_DMD2401_MODULATION_TYPES)/sizeof(RADYNE_DMD2401_MODULATION_TYPES[0]);
}

//virtual 
const char *CRadyneDMD2401::GetRModulationTypeName(int Type)
{
	return RADYNE_DMD2401_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetRModulationType(int &Type, int Demodulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[24])
		Type = 0; // BPSK
	else
		Type = 1; // QPSK
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetRModulationType(int &Type, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRModulationType(Type, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0; // QPSK
	if (Type == 0)
		cData = 1;  // BPSK

	int CommandLength = FillCommandBuffer(0x2A07, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	GetRModulationType(Type, Demodulator);
	return EC;
}

//virtual
int CRadyneDMD2401::GetTModulationTypeCount()
{
	return sizeof(RADYNE_DMD2401_MODULATION_TYPES)/sizeof(RADYNE_DMD2401_MODULATION_TYPES[0]);
}

//virtual 
const char *CRadyneDMD2401::GetTModulationTypeName(int Type)
{
	return RADYNE_DMD2401_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTModulationType(int &Type, int Modulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[24])
		Type = 0; // BPSK
	else
		Type = 1; // QPSK
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetTModulationType(int &Type, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTModulationType(Type, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // QPSK
	if (Type == 0)
		cData = 1;  // BPSK

	int CommandLength = FillCommandBuffer(0x2606, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	GetTModulationType(Type, Modulator);
	return EC;
}


// Data clock source

static char *pszUnknownClockSource = "Unknown";

//virtual
int CRadyneDMD2401::GetRDataClockSourcesCount()
{
	return sizeof(RADYNE_MODEM_R_CLOCK_SOURCES)/sizeof(RADYNE_MODEM_R_CLOCK_SOURCES[0]);
}

//virtual
const char *CRadyneDMD2401::GetRDataClockSourceName(int source)
{
	return RADYNE_MODEM_R_CLOCK_SOURCES[source];
}

//virtual
int CRadyneDMD2401::GetTDataClockSourcesCount()
{
	return sizeof(RADYNE_MODEM_T_CLOCK_SOURCES)/sizeof(RADYNE_MODEM_T_CLOCK_SOURCES[0]);
}

//virtual
const char *CRadyneDMD2401::GetTDataClockSourceName(int source)
{
	return RADYNE_MODEM_T_CLOCK_SOURCES[source];
}

int CRadyneDMD2401::CalcCombinedTClockSource(unsigned char cControl, unsigned char cSource)
{
	if (m_pszReply[40] == 1)
	{
		return TClock_External_SCTE;
	}
	else
	{
		switch (m_pszReply[42])
		{
		case 0: return TClock_SCT_Internal; break;
		case 1: return TClock_SCT_SCR; break;
		case 2: return TClock_SCT_ExtBNC; break;
		default: return 0;
		}
	}
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTDataClockSource(int &Source, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Source = CalcCombinedTClockSource(m_pszReply[40], m_pszReply[42]);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetTDataClockSource(int &Source, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataClockSource(Source, Modulator))
		return MC_OK; // already set

	unsigned char cControl;
	if (Source == TClock_External_SCTE)
		cControl = 1;
	else
		cControl = 0;

	unsigned char cSource;
	switch (Source)
	{
	case TClock_SCT_Internal: cSource = 0; break;
	case TClock_SCT_SCR: cSource = 1; break;
	case TClock_SCT_ExtBNC: cSource = 2; break;
	case TClock_External_SCTE: cSource = 3; break;
	default: ASSERT(FALSE);
	}

	int CommandLength = FillCommandBuffer(0x260B, &cControl, sizeof(cControl));
	MC_ErrorCode EC = Command(CommandLength);

	CommandLength = FillCommandBuffer(0x260D, &cSource, sizeof(cSource));
	EC = Command(CommandLength);

	GetTDataClockSource(Source, Modulator);
	return EC;
}

int CRadyneDMD2401::CalcCombinedRClockSource(unsigned char cControl)
{
	switch (cControl)
	{
	case 0: return RClock_External;
	case 1: return  RClock_Internal;
	case 2: return  RClock_EXC;
	case 3: return  RClock_RX_SAT;
	default: return 0;
	}
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetRDataClockSource(int &Source, int Modulator)
{
	Source = DCS_Unknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Source = CalcCombinedRClockSource(m_pszReply[39]);
	switch (m_pszReply[39])
	{
	case 0: Source = RClock_External; break;
	case 1: Source = RClock_Internal; break;
	case 2: Source = RClock_EXC; break;
	case 3: Source = RClock_RX_SAT; break;
	default: Source = 0;
	}
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetRDataClockSource(int &Source, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataClockSource(Source, Demodulator))
		return MC_OK; // already set

	unsigned char cData;
	switch (Source)
	{
	case RClock_External: cData = 0; break;
	case RClock_Internal: cData = 1; break;
	case RClock_EXC: cData = 2; break;
	case RClock_RX_SAT: cData = 3; break;
	default: ASSERT(FALSE);
	}

	int CommandLength = FillCommandBuffer(0x2A11, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	GetRDataClockSource(Source, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::IsTDataClockInvEnabled(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bOn = (m_pszReply[41] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::EnableTDataClockInv(BOOL &bOn, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataClockInv(bOn, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // Normal
	if (bOn)
		cData = 1;  // Inverted

	int CommandLength = FillCommandBuffer(0x260C, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	IsTDataClockInvEnabled(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::IsRDataClockInvEnabled(BOOL &bOn, int Demodulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	bOn = (m_pszReply[40] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::EnableRDataClockInv(BOOL &bOn, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataClockInv(bOn, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0; // Normal
	if (bOn)
		cData = 1;  // Inverted

	int CommandLength = FillCommandBuffer(0x2A12, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	IsRDataClockInvEnabled(bOn, Demodulator);
	return EC;
}

// Output

//virtual
MC_ErrorCode CRadyneDMD2401::IsOutputOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bOn = (m_pszReply[36] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::TurnOutputOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputOn(bOn, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bOn)
		cData = 1;  // off

	int CommandLength = FillCommandBuffer(0x2609, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	IsOutputOn(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetOutputLevel(double &Level, int Modulator)
{
	Level = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Level = RawDataToSignedShort(m_pszReply+33)/10.;
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetOutputLevel(double &Level, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputLevel(Level, Modulator))
		return MC_OK; // already set

	SignedShortToRawData((short)(Level*10.), m_pszShortValue); // dB*10
	int CommandLength = FillCommandBuffer(0x260F, m_pszShortValue, sizeof(m_pszShortValue));
	MC_ErrorCode EC = Command(CommandLength);
	GetOutputLevel(Level, Modulator);

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::IsContiniousWaveOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bOn = (m_pszReply[37] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::TurnContiniousWaveOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	unsigned char cData = 0; // off
	if (bOn)
		cData = 1;  // off

	int CommandLength = FillCommandBuffer(0x260A, &cData, sizeof(cData));
	MC_ErrorCode EC = Command(CommandLength);
	IsContiniousWaveOn(bOn, Modulator);
	return EC;
}


// Data rate

//virtual
MC_ErrorCode CRadyneDMD2401::GetRDataRate(unsigned int &DataRate, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	DataRate = RawDataToInt(m_pszReply+13);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetRDataRate(unsigned int &DataRate, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataRate(DataRate, Demodulator))
		return MC_OK; // already set

	IntToRawData(DataRate, m_pszIntValue);
	int CommandLength = FillCommandBuffer(0x2A02, m_pszIntValue, sizeof(m_pszIntValue));
	MC_ErrorCode EC = Command(CommandLength);

	GetRDataRate(DataRate, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTDataRate(unsigned int &DataRate, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	DataRate = RawDataToInt(m_pszReply+16);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetTDataRate(unsigned int &DataRate, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataRate(DataRate, Modulator))
		return MC_OK; // already set

	IntToRawData(DataRate, m_pszIntValue);
	int CommandLength = FillCommandBuffer(0x2604, m_pszIntValue, sizeof(m_pszIntValue));
	MC_ErrorCode EC = Command(CommandLength);

	GetTDataRate(DataRate, Modulator);
	return EC;
}

// BER

//virtual
MC_ErrorCode CRadyneDMD2401::GetBER(double &BER, int Demodulator)
{
	return MC_OK;
/*	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x5a, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	int mantissa = RawDataToInt(m_pszReply+4);
	int NegativeExponent = RawDataToInt(m_pszReply);
	BER = -(double)mantissa;
	for (int i = 0; i<NegativeExponent; i++)
		BER /= 10.;
	return EC;*/
}

//virtual
BOOL CRadyneDMD2401::CanBerTest(int Power)
{
	return (Power == 0 || Power == 11);
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetTBerTest(int &Power, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanBerTest(Power))
		return MC_INVALID_PARAMETERS;

	unsigned char cData = 0; // off
	if (Power != 0)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2612, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);

	GetTBerTest(Power, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTBerTest(int &Power, int Modulator)
{
	Power = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_OK && m_pszReply[39] != 0)
		Power = 11;
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetRBerTest(int &Power, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanBerTest(Power))
		return MC_INVALID_PARAMETERS;

	unsigned char cData = 0; // off
	if (Power != 0)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2A17, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);

	GetRBerTest(Power, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetRBerTest(int &Power, int Demodulator)
{
	Power = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_OK && m_pszReply[41] != 0)
		Power = 11;
	return EC;
}

MC_ErrorCode CRadyneDMD2401::GetBerTestStatus
(
	double &BER,
	unsigned int &SyncLoss,
	double &Errors,
	double &TotalBits,
	double &ErrorFreeSecsPercentage,
	unsigned int &ErredSecs,
	unsigned int &TotalTestDuration
)
{
	BER = 0.; SyncLoss = 0; Errors = 0; TotalBits = 0; ErrorFreeSecsPercentage = 0; ErredSecs = 0; TotalTestDuration = 0;

	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x240C, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	//BER
	BER = RawDataToSignedShort(m_pszReply+39)/100.;
	unsigned char exponent = m_pszReply[43];
	for (unsigned char i = 0; i < exponent; i++)
		BER /= 10.;

	// Error count
	Errors = RawDataToInt(m_pszReply+25);
	return MC_OK;
}

//virtual
MC_ErrorCode CRadyneDMD2401::RestartBerTest(int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int Power = 0, PowerForDisabling = 0;
	MC_ErrorCode EC = GetRBerTest(Power, Demodulator);
	if (EC != MC_OK)
		return EC;
	EC = SetRBerTest(PowerForDisabling, Demodulator);
	if (EC != MC_OK)
		return EC;
	Sleep(1000);
	EC = SetRBerTest(Power, Demodulator);
	return EC;
}


// FEC

//virtual
int CRadyneDMD2401::GetRFecModesCount()
{
	return (sizeof(RADYNE_2401_FEC_MODES)/sizeof(RADYNE_2401_FEC_MODES[0]));
}

//virtual
const char *CRadyneDMD2401::GetRFecModeName(int Mode)
{
	return RADYNE_2401_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetRFecMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	
	switch (m_pszReply[25])
	{
	case 0: Mode = 0; break; // None
	case 1: Mode = 1; break; // Viterbi 1/2
	case 3: Mode = 2; break; // Viterbi 3/4
	case 5: Mode = 3; break; // Viterbi 7/8
	default: Mode = 0; break; // None
	}

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetRFecMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFecMode(Mode, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0;
	switch (Mode)
	{
	case 1: cData = 1; break;
	case 2: cData = 3; break;
	case 3: cData = 5; break;
	}
	int CommandLength = FillCommandBuffer(0x2A08, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);

	GetRFecMode(Mode, Demodulator);
	return EC;
}

//virtual
int CRadyneDMD2401::GetTFecModesCount()
{
	return (sizeof(RADYNE_2401_FEC_MODES)/sizeof(RADYNE_2401_FEC_MODES[0]));
}

//virtual
const char *CRadyneDMD2401::GetTFecModeName(int Mode)
{
	return RADYNE_2401_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetTFecMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	switch (m_pszReply[25])
	{
	case 0: Mode = 0; break; // None
	case 1: Mode = 1; break; // Viterbi 1/2
	case 3: Mode = 2; break; // Viterbi 3/4
	case 5: Mode = 3; break; // Viterbi 7/8
	default: Mode = 0; break; // None
	}
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetTFecMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFecMode(Mode, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0;
	switch (Mode)
	{
	case 1: cData = 1; break;
	case 2: cData = 3; break;
	case 3: cData = 5; break;
	}
	int CommandLength = FillCommandBuffer(0x2607, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);

	GetTFecMode(Mode, Modulator);
	return EC;
}

// Differential encoding/decoding

//virtual
int CRadyneDMD2401::GetDiffDecoderModeCount()
{
	return sizeof(RADYNE_MODEM_DIFF_CODING_MODES)/sizeof(RADYNE_MODEM_DIFF_CODING_MODES[0]);
}

//virtual
const char *CRadyneDMD2401::GetDiffDecoderModeName(int Mode)
{
	return RADYNE_MODEM_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetDiffDecoderMode(int &Mode, int Demodulator)
{
	Mode = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[31] != 0)
		Mode = 1;
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetDiffDecoderMode(int &Mode, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffDecoderMode(Mode, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (Mode)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2A09, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	GetDiffDecoderMode(Mode, Demodulator);
	return EC;
}

//virtual
int CRadyneDMD2401::GetDiffEncoderModeCount()
{
	return sizeof(RADYNE_MODEM_DIFF_CODING_MODES)/sizeof(RADYNE_MODEM_DIFF_CODING_MODES[0]);
}

//virtual
const char *CRadyneDMD2401::GetDiffEncoderModeName(int Mode)
{
	return RADYNE_MODEM_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CRadyneDMD2401::GetDiffEncoderMode(int &Mode, int Demodulator)
{
	Mode = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[35] != 0)
		Mode = 1;
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::SetDiffEncoderMode(int &Mode, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffDecoderMode(Mode, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (Mode)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2608, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	GetDiffEncoderMode(Mode, Modulator);
	return EC;
}

// Scrambling/Descrambling

//virtual
int CRadyneDMD2401::GetScramblerModesCount()
{
	return sizeof(RADYNE_MODEM_SCRAMBLER_MODES)/sizeof(RADYNE_MODEM_SCRAMBLER_MODES[0]);
}

//virtual
const char *CRadyneDMD2401::doGetScramblerModeName(int mode)
{
	return RADYNE_MODEM_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CRadyneDMD2401::doGetScramblerMode(int &mode, int modulator)
{
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	
	if (m_pszReply[32] != 0)
		mode = 1;
	else
		mode = 0;

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::doSetScramblerMode(int &mode, int modulator)
{
	unsigned char cData = 0; // off
	if (mode == 1)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2613, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

//virtual
int CRadyneDMD2401::GetDescramblerModesCount()
{
	return sizeof(RADYNE_MODEM_SCRAMBLER_MODES)/sizeof(RADYNE_MODEM_SCRAMBLER_MODES[0]);
}

//virtual
const char *CRadyneDMD2401::doGetDescramblerModeName(int mode)
{
	return RADYNE_MODEM_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CRadyneDMD2401::doGetDescramblerMode(int &mode, int demodulator)
{
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	
	if (m_pszReply[32] != 0)
		mode = 1;
	else
		mode = 0;

	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::doSetDescramblerMode(int &mode, int demodulator)
{
	unsigned char cData = 0; // off
	if (mode == 1)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2A0D, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

// Spectral inversion

//virtual
MC_ErrorCode CRadyneDMD2401::IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	bEnabled = (m_pszReply[34] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::EnableRSpectralInv(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSpectralInvEnabled(bEnabled, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bEnabled)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2A0F, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	IsRSpectralInvEnabled(bEnabled, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = (m_pszReply[38] != 0);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::EnableTSpectralInv(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTSpectralInvEnabled(bEnabled, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bEnabled)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2611, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	IsTSpectralInvEnabled(bEnabled, Modulator);
	return EC;
}

// Data inversion

//virtual
MC_ErrorCode CRadyneDMD2401::IsRDataInvEnabled(BOOL &bEnabled, int Demodulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2401, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	bEnabled = (m_pszReply[77] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::EnableRDataInv(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataInvEnabled(bEnabled, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bEnabled)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2A1F, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	IsRDataInvEnabled(bEnabled, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::IsTDataInvEnabled(BOOL &bEnabled, int Modulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x2400, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = (m_pszReply[74] != 0); 
	return EC;
}

//virtual
MC_ErrorCode CRadyneDMD2401::EnableTDataInv(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataInvEnabled(bEnabled, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bEnabled)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x2623, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	IsTDataInvEnabled(bEnabled, Modulator);
	return EC;
}
