#include "stdafx.h"
#include "Datum.h"

static char CR=0x0D;
static char LF=0x0A;
static char szCRLF[]={CR, LF};

// CDatumModem implementation

CDatumModem::CDatumModem()
{
	m_bParamSavingInNvRamRightAfterSettingEnabled = FALSE;
	m_ModemAddress = 12;
	m_ControllerAddress = 0x01;
	m_cStatusByte = 0;
	m_cErrorByte = 0;
	m_cDataBytes = 0;
	m_ReturnedDataLength = 0;
}

CDatumModem::~CDatumModem()
{
}

void CDatumModem::WriteLog(unsigned char *pszString, int length)
{
	if (!IsLogFileEnabled()) return;
}

//virtual
CTerminal *CDatumModem::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
		return &m_SerialPort;
	case MC_TELNET:
		return &m_TcpIpSocket;
	case MC_SNMP:
		return NULL;
	}
	return NULL;
}

//virtual
void  CDatumModem::OnMcConnected()
{
}

MC_ErrorCode CDatumModem::Command(int CommandLength, BOOL bWaitForReply/* = TRUE */)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, CommandLength);
	if (!bWaitForReply)
		return MC_OK;

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	// Analize error byte
	if (m_ReturnedDataLength == 0)
		EC = MC_DEVICE_NOT_RESPONDING;
	else
	{
		EC = MC_OK;
		switch (m_cErrorByte>>4)
		{
		case 0x00: EC = MC_OK; break;				// all right
		case 0x01: EC = MC_INVALID_COMMAND; break;	// read-only value
		case 0x02: EC = MC_INVALID_COMMAND; break;	// invalid request
		case 0x03: EC = MC_COMMAND_NOT_SUPPORTED; break; // option n/a
		case 0x04: EC = MC_INVALID_COMMAND; break;	// read-only value
		default:   EC = MC_GENERIC_ERROR; break;	// all other cases
		}
	}

	return EC;
}

//virtual
MC_ErrorCode CDatumModem::ReadReplyUntilPrompt()
{
	memset(m_pDataBytes, 0, sizeof(m_pDataBytes));
	memset(m_pRawReply, 0, sizeof(m_pRawReply));
	unsigned char byte;
	BOOL bPad = TRUE;
	BOOL bOpened = FALSE;
	BOOL bMyReply = FALSE;
	BOOL bMyModem = FALSE;
	BOOL bCommandCode = FALSE;
	BOOL bStatusByte = FALSE;
	BOOL bErrorByte = FALSE;
	BOOL bDataBytes = FALSE;
	BOOL bClosed = FALSE;
	m_ReturnedDataLength = 0;
	m_RawReplyLength = 0;

	while (1)
	{
		byte = 0x00;
		if (!GetTerminal()->Read((char *)&byte, 1))
			return MC_DEVICE_NOT_RESPONDING;
		m_pRawReply[m_RawReplyLength++] = byte;

		if (bPad && byte == PAD)
			continue; // bypass leading pad bytes
		bPad = FALSE;

		if (!bOpened && byte == REPLY_OPEN_FLAG)
		{
			bOpened = TRUE;
			continue;
		}
		if (!bOpened)
			break; // error in reply! TO DO: handle it!

		if (!bMyReply && byte == m_ControllerAddress)
		{
			bMyReply = TRUE;
			continue;
		}
		if (!bMyReply)
			break;

		if (!bMyModem && byte == m_ModemAddress)
		{
			bMyModem = TRUE;
			continue;
		}
		if (!bMyModem)
			break;

		if (!bCommandCode)
		{
			bCommandCode = TRUE;
			continue;
		}
		if (!bCommandCode)
			break;

		if (!bStatusByte)
		{
			bStatusByte = TRUE;
			m_cStatusByte = byte;
			continue;
		}
		if (!bStatusByte)
			break;

		if (!bErrorByte)
		{
			bErrorByte = TRUE;
			m_cErrorByte = byte;
			continue;
		}
		if (!bErrorByte)
			break;

		if (!bDataBytes)
		{
			bDataBytes = TRUE;
			m_cDataBytes = byte;
			continue;
		}
		if (!bDataBytes)
			break;

		if (m_ReturnedDataLength < m_cDataBytes)
		{
			m_pDataBytes[m_ReturnedDataLength++] = byte;
			continue;
		}

		if (!bClosed && byte == CLOSE_FLAG)
		{
			bClosed = TRUE;
			continue;
		}

		// here checksum is (byte variable)
		if (bClosed)
			break;

	}
	GetTerminal()->ClearAllBuffers();
	return MC_OK;
}

void CDatumModem::SetMCBusAddress(unsigned int Address)
{
	m_ModemAddress = LOBYTE(LOWORD(Address));
}

//virtual
BOOL CDatumModem::CheckMCBusAddress(unsigned int Address)
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

int CDatumModem::FillCommandBuffer(unsigned char CommandCode, unsigned char Mode, unsigned char *pcDataBytes, unsigned char nDataBytes)
{
	int i=0;
	m_pszCommand[i++] = PAD; // opening pad byte
	m_pszCommand[i++] = COMMAND_OPEN_FLAG; // opening flag
	m_pszCommand[i++] = m_ModemAddress; // 
	m_pszCommand[i++] = m_ControllerAddress; // 
	m_pszCommand[i++] = CommandCode;

	unsigned char ModeByte = GetModeField(Mode);
	m_pszCommand[i++] = ModeByte;

	m_pszCommand[i++] = nDataBytes;
	for (int j=0; j<nDataBytes; j++)
	{
		m_pszCommand[i++] = pcDataBytes[j]; // data bytes
	}
	m_pszCommand[i++] = (char)0x96; // closing flag
	m_pszCommand[i++] = CheckSum(1, i-1);
	m_pszCommand[i++] = PAD; // closing pad byte
	return i;
}

unsigned char CDatumModem::CheckSum(int Start, int Stop)
{
	return CheckSum(m_pszCommand, Start, Stop);
}

unsigned char CDatumModem::CheckSum(unsigned char *pcData, int Start, int Stop)
{
	unsigned int sum = 0;
	for (int i=Start; i<=Stop; i++)
		sum += pcData[i];
	sum = 256 - sum;
	sum = sum % 256;
	unsigned char ret = (unsigned char)sum;
	return ret;
}

unsigned int CDatumModem::RawDataToInt(unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	unsigned int IntValue = *((unsigned int *)pszRawData);
	return IntValue;
}

short CDatumModem::RawDataToSignedShort(unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	short ShortValue = *((short *)pszRawData);
	return ShortValue;
}

void CDatumModem::IntToRawData(unsigned int IntValue, unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	pszRawData[0] = (unsigned char)((IntValue & 0x000000FF) >> 0);
	pszRawData[1] = (unsigned char)((IntValue & 0x0000FF00) >> 8);
	pszRawData[2] = (unsigned char)((IntValue & 0x00FF0000) >> 16);
	pszRawData[3] = (unsigned char)((IntValue & 0xFF000000) >> 24);
}

void CDatumModem::SignedShortToRawData(short ShortValue, unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	memcpy(pszRawData, &ShortValue, sizeof(ShortValue));
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

double CDatumModem::ParseBER()
{
	unsigned short ber = *((unsigned short *)(m_pDataBytes+16));
	int mantissa = ber & 0x0FFF;			// bits 0...11
	int NegativeExponent = ber & 0xF000;	// bits 12...15
	NegativeExponent >>= 12;
	double BER = (double)mantissa;
	for (int i = 0; i < NegativeExponent; i++)
		BER /= 10.;

	return BER;
}

double CDatumModem::ParseSER()
{
	unsigned short ser = *((unsigned short *)(m_pDataBytes+18));
	int mantissa = ser & 0x0FFF;			// bits 0...11
	int NegativeExponent = ser & 0xF000;	// bits 12...15
	NegativeExponent >>= 12;
	double SER = (double)mantissa;
	for (int i = 0; i < NegativeExponent; i++)
		SER /= 10.;

	return SER;
}

// Differential encoding/decoding commands
// Scrambling/Descrambling
// Spectral inversion
// Data inversion



// CDatumPsm4900 implementation

static const char *DATUM_PSM4900_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK",
	"OQPSK",
	"8PSK",
	"8QAM",
	"16QAM"
};

static const char *DATUM_PSM4900_FEC_MODES[] = {
	"Viterbi 1/2",
	"Viterbi 3/4",
	"Viterbi 7/8"
};

static const char *DATUM_PSM4900_SCRAMBLER_MODES[] = {
	"Disable",		// 0
	"IESS-308",		// 1
	"IESS-309",		// 2
	"V.35",			// 3
	"Alt V.35",		// 4
	"Intelsat",		// 5
	"Alt Intelsat",	// 6
	"TPC Synchro"	// 7
};

static const char *DATUM_PSM4900_DIFF_CODING_MODES[] = { "Disabled", "Enabled" };

static const char *DATUM_PSM4900_R_CLOCK_SOURCES[] = {"RCV_Clock", "Internal", "External", "Mod_Clock"};
static const char *DATUM_PSM4900_T_CLOCK_SOURCES[] = {"Internal", "TT_Clock", "External", "RCV_Clock"};

static const char *DATUM_PSM4900_R_SWEEP_MODES[] = { "Normal (fast)", "Search" };
static const char *DATUM_PSM4900_R_POWER_SUPPLY_MODES[] = { "None", "13 Volts", "18 Volts" };
static const char *DATUM_PSM4900_T_POWER_SUPPLY_MODES[] = { "None", "24 Volts" };

static const char *DATUM_PSM4900_REED_SOLOMON_MODES[] = { "Disabled", "IESS-308", "IESS-309", "IESS-310", "Custom", "CT220,200" };

CDatumPsm4900::CDatumPsm4900()
{
}

CDatumPsm4900::~CDatumPsm4900()
{
}

// Common device commands

//virtual
MC_ErrorCode CDatumPsm4900::Identify(BOOL &bIdentified)
{
	bIdentified = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	m_pDataBytes[m_ReturnedDataLength] = 0x00;
	if (EC != MC_OK)
		return EC;
	if (strstr((char *)m_pDataBytes[8], "PSM-4900"))
		bIdentified = TRUE;

	return EC; // == OK!
}

//virtual
MC_ErrorCode CDatumPsm4900::GetDeviceVersionString(char *&pszString)
{
	pszString = m_szVersion;
	m_szVersion[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	// Model
	strcpy(m_szVersion, "Model : ");
	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	m_pDataBytes[m_ReturnedDataLength] = 0x00;
	strcat(m_szVersion, (char *)(m_pDataBytes+25));
	strcat(m_szVersion, szCRLF);

	// Software version
	strcat(m_szVersion, "Software version : ");
	strcat(m_szVersion, (char *)(m_pDataBytes+46));
	strcat(m_szVersion, szCRLF);

	// Serial number
	unsigned int SN = RawDataToInt(m_pDataBytes+42);
	strcat(m_szVersion, "Serial number : ");
	char szSerial[16];
	sprintf(szSerial, "%d", SN);
	strcat(m_szVersion, szSerial);
	strcat(m_szVersion, szCRLF);

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetModelName(std::string &strModel)
{
	strModel = "";
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_OK)
	{
		m_pDataBytes[m_ReturnedDataLength] = 0x00;
		strModel = (char *)(m_pDataBytes+25);
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetSerialNo(unsigned int &serialNo)
{
	serialNo = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);

	serialNo = RawDataToInt(m_pDataBytes+42);
	
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetSoftwareVersion(std::string &strVersion)
{
	strVersion = "";
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_OK)
	{
		m_pDataBytes[m_ReturnedDataLength] = 0x00;
		strVersion = (char *)(m_pDataBytes+46);
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetUnitName(char *&pszString)
{	
	pszString = m_szUnitName;
	m_szUnitName[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	m_pDataBytes[m_ReturnedDataLength] = 0x00;
	strcat(m_szUnitName, (char *)(m_pDataBytes+8));

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetUnitName(char *pszString)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = (char)1<<3;	// Frequency flag set
	if (strlen(pszString) < 16)
		strcpy((char *)(m_WriteData+8), pszString);
	else
		strncpy((char *)(m_WriteData+8), pszString, 15);
	int CommandLength = FillCommandBuffer(0x00, modeExecute, m_WriteData, 68);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

//virtual
void CDatumPsm4900::OnMcConnected()
{
	if (m_ControlType == MC_TELNET ||
		m_ControlType == MC_SNMP)
	{
		GetTerminal()->SetWriteTimeout(2000);
		GetTerminal()->SetReadTimeout(2000);
	}

	// send broadcast packet to change M&C address to 12 == m_ModemAddress
/*	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 2;	//  Write enable flag - address only
	m_WriteData[8] = m_ModemAddress;	// New  address
	
	int i=0;
	m_pszCommand[i++] = PAD; // opening pad byte
	m_pszCommand[i++] = COMMAND_OPEN_FLAG; // opening flag
	m_pszCommand[i++] = 0xFF; // broadcast address
	m_pszCommand[i++] = m_ControllerAddress; // 
	m_pszCommand[i++] = 0x04; // unit remote

	m_pszCommand[i++] = GetModeField(modeExecute);

	m_pszCommand[i++] = 12;
	for (int j=0; j<12; j++)
	{
		m_pszCommand[i++] = m_WriteData[j]; // data bytes
	}
	m_pszCommand[i++] = (char)0x96; // closing flag
	m_pszCommand[i++] = CheckSum(1, i-1);
	m_pszCommand[i++] = PAD; // closing pad byte

	Command(i, FALSE);*/
}

// Non Volatile RAM

// 10 MHz reference type

// IF params and status

//virtual
MC_ErrorCode CDatumPsm4900::GetRIfParams(CDemIfParams &Params, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	Params.m_SearchRange = RawDataToInt(m_pDataBytes+14); // Hz
	Params.m_SweepTime = RawDataToSignedShort(m_pDataBytes+18);
	Params.m_LowEbno = RawDataToSignedShort(m_pDataBytes+20)/10.;
	Params.m_LowLevel = RawDataToSignedShort(m_pDataBytes+22)/10.;
	
	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Params.m_ModulationType = 0; // BPSK
	else if (ModField == 1)
		Params.m_ModulationType = 1; // QPSK
	else if (ModField == 2)
		Params.m_ModulationType = 2; // OQPSK
	else if (ModField == 3)
		Params.m_ModulationType = 3; // 8PSK
	else if (ModField == 4)
		Params.m_ModulationType = 4; // 8QAM
	else if (ModField == 6)
		Params.m_ModulationType = 5; // 16QAM

	Params.m_bSpectrumInversion = ((m_pDataBytes[4] & 1<<5) != 0);

	EC = IsR10MHzSupplierEnabled(Params.m_b10MHzSupplyEnabled, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = GetRPowerSupplyMode(Params.m_PowerSupplyMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRIfStatus(CDemIfStatus &Status, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	Status.m_bCarrier = ((m_pDataBytes[4] & 1<<1) != 0);
	Status.m_bDemLock = ((m_pDataBytes[4] & 1<<0) != 0);
	Status.m_SN = RawDataToSignedShort(m_pDataBytes+14) / 10.;
	Status.m_Offset = RawDataToInt(m_pDataBytes+8);
	Status.m_InputLevel = RawDataToSignedShort(m_pDataBytes+12);
	Status.m_BER = ParseBER();
	return MC_OK;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTIfParams(CModIfParams &Params, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	Params.m_Offset =  RawDataToInt(m_pDataBytes+14);

	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Params.m_ModulationType = 0; // BPSK
	else if (ModField == 1)
		Params.m_ModulationType = 1; // QPSK
	else if (ModField == 2)
		Params.m_ModulationType = 2; // OQPSK
	else if (ModField == 3)
		Params.m_ModulationType = 3; // 8PSK
	else if (ModField == 4)
		Params.m_ModulationType = 4; // 8QAM
	else if (ModField == 6)
		Params.m_ModulationType = 5; // 16QAM

	Params.m_bOutputEnabled = (m_pDataBytes[4] & 1);
	Params.m_OutputLevel = RawDataToSignedShort(m_pDataBytes+18)/10.;

	Params.m_bSpectrumInversion = (m_pDataBytes[4] & 1<<5) != 0;

	EC = IsContiniousWaveOn(Params.m_bContiniousWave, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = IsT10MHzSupplierEnabled(Params.m_b10MHzSupplyEnabled, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = GetTPowerSupplyMode(Params.m_PowerSupplyMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	
	return EC;
}

// Data params

//virtual
MC_ErrorCode CDatumPsm4900::GetRDataParams(CModemDataParams &Params, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_DataRate = RawDataToInt(m_pDataBytes+10);

	unsigned char ClockSrc = m_pDataBytes[6] >> 4;
	Params.m_ClockSource = ClockSrc & 0x07; // 3 bits only
	
	unsigned char CodeRateField = m_pDataBytes[5] >> 2;
	CodeRateField = CodeRateField & 0x0F; // 4 bits
	switch (CodeRateField)
	{
	case 0: Params.m_FecMode = 1; break; // 1/2
	case 1: Params.m_FecMode = 2; break; // 3/4
	case 2: Params.m_FecMode = 3; break; // 7/8
	default: Params.m_FecMode = 0; break; // none
	}

	Params.m_DifferentialCodingMode = ((m_pDataBytes[5] & 1<<6) != 0);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	Params.m_ScramblingMode = ScramblerField;

	// Doppler buffer
	Params.m_DopplerBufferDelay = RawDataToInt(m_pDataBytes+20);
	Params.m_DopplerBufferSize = RawDataToInt(m_pDataBytes+24);

	// the following three calls set fields to constants
	IsRDataInvEnabled(Params.m_bDataInverted, Demodulator);
	IsRDataClockInvEnabled(Params.m_bClockInverted, Demodulator);
	
	return MC_OK;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTDataParams(CModemDataParams &Params, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_DataRate = RawDataToInt(m_pDataBytes+10);

	unsigned char ClockSrc = m_pDataBytes[6] >> 4;
	Params.m_ClockSource = ClockSrc & 0x07; // 3 bits only
	
	unsigned char CodeRateField = m_pDataBytes[5] >> 2;
	CodeRateField = CodeRateField & 0x0F; // 4 bits
	switch (CodeRateField)
	{
	case 0: Params.m_FecMode = 1; break; // 1/2
	case 1: Params.m_FecMode = 2; break; // 3/4
	case 2: Params.m_FecMode = 3; break; // 7/8
	default: Params.m_FecMode = 0; break; // none
	}

	Params.m_DifferentialCodingMode = ((m_pDataBytes[5] & 1<<6) != 0);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	Params.m_ScramblingMode = ScramblerField;

	// the following three calls set fields to constants
	IsTDataInvEnabled(Params.m_bDataInverted, Modulator);
	IsTDataClockInvEnabled(Params.m_bClockInverted, Modulator);
	
	return MC_OK;
}

// Frequency (carrier)

//virtual
MC_ErrorCode CDatumPsm4900::GetRFrequency(unsigned int &Frequency, int Demodulator)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRFrequency(unsigned int &Frequency, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFrequency(Frequency, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = (char)1;	// Frequency flag set
	IntToRawData(Frequency * 1000, m_WriteData+8);
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetRFrequency(Frequency, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTFrequency(unsigned int &Frequency, int Modulator)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFrequency(Frequency, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Frequency flag set
	IntToRawData(Frequency * 1000, m_WriteData+8);
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetTFrequency(Frequency, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	SearchRange = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	SearchRange = RawDataToInt(m_pDataBytes+14); // Hz
	return EC;
}

//virtual 
MC_ErrorCode CDatumPsm4900::SetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateSearchRange(SearchRange, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<1;	// Sweep range flag set
	IntToRawData(SearchRange, m_WriteData+14);
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetSearchRange(SearchRange, Demodulator);
	return EC;
}

//virtual
int CDatumPsm4900::GetRSweepModesCount()
{
	return sizeof(DATUM_PSM4900_R_SWEEP_MODES)/sizeof(DATUM_PSM4900_R_SWEEP_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetRSweepModeName(int Mode)
{
	return DATUM_PSM4900_R_SWEEP_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRSweepMode(int &Mode, int Demodulator)
{
	Mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Mode = (m_pDataBytes[4] & 3);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRSweepMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSweepMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<2;	// Sweep mode flag set
	IntToRawData(Mode, m_WriteData+0);
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 38);
	MC_ErrorCode EC = Command(CommandLength);

	GetRSweepMode(Mode, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetInputLevel(double &InputLevel, int Demodulator)
{
	InputLevel = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	InputLevel = RawDataToSignedShort(m_pDataBytes+12);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetModulatorShift(int &Shift, int Modulator)
{
	Shift = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Shift = RawDataToInt(m_pDataBytes+14);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetModulatorShift(int &Shift, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateModulatorShift(Shift, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 1;	// Offset flag set
	IntToRawData(Shift, m_WriteData+14);
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetModulatorShift(Shift, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::IsContiniousWaveOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x44, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = (m_pDataBytes[4] & 3);
	bOn = (bits == 1);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::TurnContiniousWaveOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Output flag set
	if (bOn)
		m_WriteData[4] = 1;		// Pure carrier
	else
		m_WriteData[4] = 0;		// Normal
	int CommandLength = FillCommandBuffer(0x44, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	IsContiniousWaveOn(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetEbNo(double &EbNo, int Demodulator)
{
	EbNo = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	EbNo = RawDataToSignedShort(m_pDataBytes+14) / 10.;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::IsRCarrier(BOOL &bValid, int Demodulator)
{
	bValid = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bValid = ((m_pDataBytes[4] & 1<<1) != 0);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetDemodulatorOffset(int &Offset, int Demodulator)
{
	Offset = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Offset = RawDataToInt(m_pDataBytes+8);
	return EC;
}

// Demodulator reference generator fine tune

//virtual
MC_ErrorCode CDatumPsm4900::GetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator)
{
	FineTuneValue = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x05, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	FineTuneValue = RawDataToSignedShort(m_pDataBytes+6);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDemodulatorRefFineTune(FineTuneValue, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 2;	// Ref fine tune flag
	SignedShortToRawData(LOWORD(FineTuneValue), m_WriteData+6);
	int CommandLength = FillCommandBuffer(0x05, modeExecute, m_WriteData, 12);
	MC_ErrorCode EC = Command(CommandLength);

	GetDemodulatorRefFineTune(FineTuneValue, Demodulator);
	return EC;
}

// Doppler Buffer Size
//virtual
MC_ErrorCode CDatumPsm4900::GetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	BufferSize = RawDataToInt(m_pDataBytes+18);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDemodulatorDopplerBufferSize(BufferSize, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1 << 0;	// buffer size enable flag
	IntToRawData(BufferSize, m_WriteData+24);
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetDemodulatorDopplerBufferSize(BufferSize, Demodulator);
	return EC;
}

// Modulation type

//virtual
int CDatumPsm4900::GetRModulationTypeCount()
{
	return sizeof(DATUM_PSM4900_MODULATION_TYPES)/sizeof(DATUM_PSM4900_MODULATION_TYPES[0]);
}

//virtual
const char *CDatumPsm4900::GetRModulationTypeName(int Type)
{
	return DATUM_PSM4900_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRModulationType(int &Type, int Demodulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Type = 0; // BPSK
	else if (ModField == 1)
		Type = 1; // QPSK
	else if (ModField == 2)
		Type = 2; // OQPSK
	else if (ModField == 3)
		Type = 3; // 8PSK
	else if (ModField == 4)
		Type = 4; // 8QAM
	else if (ModField == 6)
		Type = 5; // 16QAM

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRModulationType(int &Type, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRModulationType(Type, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Modulation flag set
	if (Type == 0)
		m_WriteData[4] = 0;	// BPSK
	else if (Type == 1)
		m_WriteData[4] = 1;	// QPSK
	else if (Type == 2)
		m_WriteData[4] = 2;	// OQPSK
	else if (Type == 3)
		m_WriteData[4] = 3;	// 8PSK
	else if (Type == 4)
		m_WriteData[4] = 4;	// 8QAM
	else if (Type == 5)
		m_WriteData[4] = 6;	// 16QAM (not 5, 5 is not in use doc says)
	m_WriteData[4] <<= 2;

	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetRModulationType(Type, Demodulator);
	return EC;
}

//virtual
int CDatumPsm4900::GetTModulationTypeCount()
{
	return sizeof(DATUM_PSM4900_MODULATION_TYPES)/sizeof(DATUM_PSM4900_MODULATION_TYPES[0]);
}

//virtual
const char *CDatumPsm4900::GetTModulationTypeName(int Type)
{
	return DATUM_PSM4900_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTModulationType(int &Type, int Modulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Type = 0; // BPSK
	else if (ModField == 1)
		Type = 1; // QPSK
	else if (ModField == 2)
		Type = 2; // OQPSK
	else if (ModField == 3)
		Type = 3; // 8PSK
	else if (ModField == 4)
		Type = 4; // 8QAM
	else if (ModField == 6)
		Type = 5; // 16QAM
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTModulationType(int &Type, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTModulationType(Type, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Modulation flag set
	if (Type == 0)
		m_WriteData[4] = 0;	// BPSK
	else if (Type == 1)
		m_WriteData[4] = 1;	// QPSK
	else if (Type == 2)
		m_WriteData[4] = 2;	// OQPSK
	else if (Type == 3)
		m_WriteData[4] = 3;	// 8PSK
	else if (Type == 4)
		m_WriteData[4] = 4;	// 8QAM
	else if (Type == 5)
		m_WriteData[4] = 6;	// 16QAM (not 5, 5 is not in use doc says)
	m_WriteData[4] <<= 2;

	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetTModulationType(Type, Modulator);
	return EC;
}

// Data clock source

//virtual
int CDatumPsm4900::GetRDataClockSourcesCount()
{
	return sizeof(DATUM_PSM4900_R_CLOCK_SOURCES)/sizeof(DATUM_PSM4900_R_CLOCK_SOURCES[0]);
}

//virtual
const char *CDatumPsm4900::GetRDataClockSourceName(int source)
{
	return DATUM_PSM4900_R_CLOCK_SOURCES[source];
}

//virtual
int CDatumPsm4900::GetTDataClockSourcesCount()
{
	return sizeof(DATUM_PSM4900_T_CLOCK_SOURCES)/sizeof(DATUM_PSM4900_T_CLOCK_SOURCES[0]);
}

//virtual
const char *CDatumPsm4900::GetTDataClockSourceName(int source)
{
	return DATUM_PSM4900_T_CLOCK_SOURCES[source];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRDataClockSource(int &Source, int Modulator)
{
	Source = DCS_Unknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ClockSrc = m_pDataBytes[6] >> 4;
	Source = ClockSrc & 0x07; // 3 bits only
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRDataClockSource(int &Source, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataClockSource(Source, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<6;	// Clock source flag set
	m_WriteData[6] = Source<<4;
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetRDataClockSource(Source, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTDataClockSource(int &Source, int Modulator)
{
	Source = DCS_Unknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ClockSrc = m_pDataBytes[6] >> 4;
	Source = ClockSrc & 0x07; // 3 bits only
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTDataClockSource(int &Source, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataClockSource(Source, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<6;	// Clock source flag set
	m_WriteData[6] = Source<<4;
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	GetTDataClockSource(Source, Modulator);
	return EC;
}

// Output

//virtual
MC_ErrorCode CDatumPsm4900::IsOutputOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bOn = (m_pDataBytes[4] & 1);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::TurnOutputOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputOn(bOn, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// Output flag set
	if (bOn)
		m_WriteData[4] = 1;		// Enable
	else
		m_WriteData[4] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	IsOutputOn(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetOutputLevel(double &Level, int Modulator)
{
	Level = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Level = RawDataToSignedShort(m_pDataBytes+18)/10.;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetOutputLevel(double &Level, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputLevel(Level, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<2;	// Output level flag set
	SignedShortToRawData((short)(Level*10.), m_WriteData+18); // dB*10
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetOutputLevel(Level, Modulator);
	return EC;
}

// Data rate

//virtual
MC_ErrorCode CDatumPsm4900::GetRDataRate(unsigned int &DataRate, int Demodulator)
{
	DataRate = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	DataRate = RawDataToInt(m_pDataBytes+10);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRDataRate(unsigned int &DataRate, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataRate(DataRate, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Bit rate flag set
	IntToRawData(DataRate, m_WriteData+10);
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetRDataRate(DataRate, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTDataRate(unsigned int &DataRate, int Modulator)
{
	DataRate = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	DataRate = RawDataToInt(m_pDataBytes+10);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTDataRate(unsigned int &DataRate, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataRate(DataRate, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Bit rate flag set
	IntToRawData(DataRate, m_WriteData+10);
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	GetTDataRate(DataRate, Modulator);
	return EC;
}

// 10 MHz reference 

//virtual
MC_ErrorCode CDatumPsm4900::IsR10MHzSupplierEnabled(BOOL &bEnable, int Demodulator)
{
	bEnable = FALSE;
	if (!CanR10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x87, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = m_pDataBytes[4];
	bits >>= 3;
	bits = bits & 0x07;
	bEnable = (bits != 0);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::EnableR10MHzSupplier(BOOL &bEnable, int Demodulator)
{
	if (!CanR10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 6;	// Output flag set for enable/disable
	if (bEnable)
		m_WriteData[4] = 1 << 3;	// Enabled
	else
		m_WriteData[4] = 0;			// Disabled

	int CommandLength = FillCommandBuffer(0x87, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	IsR10MHzSupplierEnabled(bEnable, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::IsT10MHzSupplierEnabled(BOOL &bEnable, int Modulator)
{
	bEnable = FALSE;
	if (!CanT10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x47, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = m_pDataBytes[4];
	bits >>= 3;
	bits = bits & 0x07;
	bEnable = (bits != 0);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::EnableT10MHzSupplier(BOOL &bEnable, int Modulator)
{
	if (!CanT10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 6;	// Output flag set for enable/disable
	if (bEnable)
		m_WriteData[4] = 1 << 3;	// Enabled
	else
		m_WriteData[4] = 0;			// Disabled

	int CommandLength = FillCommandBuffer(0x47, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	IsT10MHzSupplierEnabled(bEnable, Modulator);
	return EC;
}

// BER

//virtual
MC_ErrorCode CDatumPsm4900::GetBER(double &BER, int Demodulator)
{
	BER = 1.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	BER = ParseBER();

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetSER(double &SER, int Demodulator)
{
	SER = 1.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	SER = ParseSER();

	return EC;
}

// FEC
//virtual
int CDatumPsm4900::GetRFecModesCount()
{
	return (sizeof(DATUM_PSM4900_FEC_MODES)/sizeof(DATUM_PSM4900_FEC_MODES[0]));
}

//virtual
const char *CDatumPsm4900::GetRFecModeName(int Mode)
{
	return DATUM_PSM4900_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRFecMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char CodeRateField = m_pDataBytes[5] >> 2;
	CodeRateField = CodeRateField & 0x0F; // 4 bits
	if (EC == MC_OK)
	{
		switch (CodeRateField)
		{
		case 0: Mode = 0; break; // Viterbi 1/2
		case 1: Mode = 1; break; // Viterbi 3/4
		case 2: Mode = 2; break; // Viterbi 7/8
		default: Mode = 0; break; // 
		}
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRFecMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFecMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// FEC flag set
	switch (Mode)
	{
	case 0: m_WriteData[5] = 0; break; // Viterbi 1/2
	case 1: m_WriteData[5] = 1<<2; break; // Viterbi 3/4
	case 2: m_WriteData[5] = 1<<3; break; // Viterbi 7/8
	}
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetRFecMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatumPsm4900::GetTFecModesCount()
{
	return (sizeof(DATUM_PSM4900_FEC_MODES)/sizeof(DATUM_PSM4900_FEC_MODES[0]));
}

//virtual
const char *CDatumPsm4900::GetTFecModeName(int Mode)
{
	return DATUM_PSM4900_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTFecMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char CodeRateField = m_pDataBytes[5] >> 2;
	CodeRateField = CodeRateField & 0x0F; // 4 bits
	if (EC == MC_OK)
	{
		switch (CodeRateField)
		{
		case 0: Mode = 0; break; // Viterbi 1/2
		case 1: Mode = 1; break; // Viterbi 3/4
		case 2: Mode = 2; break; // Viterbi 7/8
		default: Mode = 0; break; // None
		}
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTFecMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFecMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// FEC mode flag set
	switch (Mode)
	{
	case 0: m_WriteData[5] = 0; break; // Viterbi 1/2
	case 1: m_WriteData[5] = 1<<2; break; // Viterbi 3/4
	case 2: m_WriteData[5] = 1<<3; break; // Viterbi 7/8
	}
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	GetTFecMode(Mode, Modulator);
	return EC;
}

// Reed-Solomon

//virtual
int CDatumPsm4900::GetReedSolomonModesCount()
{
	return (sizeof(DATUM_PSM4900_REED_SOLOMON_MODES)/sizeof(DATUM_PSM4900_REED_SOLOMON_MODES[0]));
}

//virtual
const char *CDatumPsm4900::GetReedSolomonModeName(int Mode)
{
	return DATUM_PSM4900_REED_SOLOMON_MODES[Mode];
}


// Differential encoding/decoding

//virtual
int CDatumPsm4900::GetDiffDecoderModeCount()
{
	return sizeof(DATUM_PSM4900_DIFF_CODING_MODES)/sizeof(DATUM_PSM4900_DIFF_CODING_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetDiffDecoderModeName(int Mode)
{
	return DATUM_PSM4900_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetDiffDecoderMode(int &Mode, int Demodulator)
{
	Mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if ((m_pDataBytes[5] & (1<<6)) != 0)
		Mode = 1; // enable
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetDiffDecoderMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffDecoderMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Differential decoding enabling flag set
	if (Mode == 0)
		m_WriteData[5] = 1<<6;	// Enable
	else
		m_WriteData[5] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetDiffDecoderMode(Mode, Demodulator);
	return EC;
}

//virtual
//virtual
int CDatumPsm4900::GetDiffEncoderModeCount()
{
	return sizeof(DATUM_PSM4900_DIFF_CODING_MODES)/sizeof(DATUM_PSM4900_DIFF_CODING_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetDiffEncoderModeName(int Mode)
{
	return DATUM_PSM4900_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetDiffEncoderMode(int &Mode, int Modulator)
{
	Mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if ((m_pDataBytes[5] & (1<<6)) != 0)
		Mode = 1; // enable
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetDiffEncoderMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffEncoderMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Differential decoding enabling flag set
	if (Mode == 0)
		m_WriteData[5] = 1<<6;	// Enable
	else
		m_WriteData[5] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetDiffEncoderMode(Mode, Modulator);
	return EC;
}


// Scrambling/Descrambling

//virtual
int CDatumPsm4900::GetScramblerModesCount()
{
	return sizeof(DATUM_PSM4900_SCRAMBLER_MODES)/sizeof(DATUM_PSM4900_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetScramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_PSM4900_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetScramblerMode(int &mode, int Modulator)
{
	mode = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	mode = ScramblerField;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetScramblerMode(int &mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateScramblerMode(mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Scrambling flag set
	m_WriteData[6] = (char)mode;

	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	GetScramblerMode(mode, Modulator);
	return EC;
}

//virtual
int CDatumPsm4900::GetDescramblerModesCount()
{
	return sizeof(DATUM_PSM4900_SCRAMBLER_MODES)/sizeof(DATUM_PSM4900_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetDescramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_PSM4900_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetDescramblerMode(int &mode, int Demodulator)
{
	mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	mode = ScramblerField;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetDescramblerMode(int &mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDescramblerMode(mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Descrambling flag set
	m_WriteData[6] = (char)mode;

	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetDescramblerMode(mode, Demodulator);
	return EC;
}

// Spectral inversion

//virtual
MC_ErrorCode CDatumPsm4900::IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = (m_pDataBytes[4] & 1<<5) != 0;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::EnableRSpectralInv(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSpectralInvEnabled(bEnabled, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Output flag set
	if (bEnabled)
		m_WriteData[4] = 1<<5;	// Enable
	else
		m_WriteData[4] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	IsRSpectralInvEnabled(bEnabled, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = (m_pDataBytes[4] & 1<<5) != 0;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::EnableTSpectralInv(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTSpectralInvEnabled(bEnabled, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Output flag set
	if (bEnabled)
		m_WriteData[4] = 1<<5;	// Enable
	else
		m_WriteData[4] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	IsTSpectralInvEnabled(bEnabled, Modulator);
	return EC;
}

// DC suppliers

//virtual
int CDatumPsm4900::GetRPowerSupplyModesCount()
{
	return sizeof(DATUM_PSM4900_R_POWER_SUPPLY_MODES)/sizeof(DATUM_PSM4900_R_POWER_SUPPLY_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetRPowerSupplyModeName(int Mode)
{
	return DATUM_PSM4900_R_POWER_SUPPLY_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRPowerSupplyMode(int &Mode, int Demodulator)
{
	Mode = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x87, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char LNBfield = m_pDataBytes[4] & 0x07; // 3 bits [0,1,2]
	if (EC != MC_OK)
		return EC;
	
	switch (LNBfield)
	{
	case 0: Mode = 0; break;
	case 1: Mode = 1; break;
	case 2: Mode = 2; break;
	default: Mode = 0; break;
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRPowerSupplyMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRPowerSupplyMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// bit [0]
	switch (Mode)
	{
	case 0: m_WriteData[4] = 0; break;
	case 1: m_WriteData[4] = 1; break;
	case 2: m_WriteData[4] = 2; break;
	}
	int CommandLength = FillCommandBuffer(0x87, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetRPowerSupplyMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatumPsm4900::GetTPowerSupplyModesCount()
{
	return sizeof(DATUM_PSM4900_T_POWER_SUPPLY_MODES)/sizeof(DATUM_PSM4900_T_POWER_SUPPLY_MODES[0]);
}

//virtual
const char *CDatumPsm4900::GetTPowerSupplyModeName(int Mode)
{
	return DATUM_PSM4900_T_POWER_SUPPLY_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTPowerSupplyMode(int &Mode, int Modulator)
{
	Mode = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x47, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char BUCfield = m_pDataBytes[4] & 0x07; // 3 bits [0,1,2]
	if (EC != MC_OK)
		return EC;
	switch (BUCfield)
	{
	case 0: Mode = 0; break;
	case 1: Mode = 1; break;
	default: Mode = 0; break;
	}

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTPowerSupplyMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTPowerSupplyMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// bit [0]
	switch (Mode)
	{
	case 0: m_WriteData[4] = 0; break;
	case 1: m_WriteData[4] = 1; break;
	}
	int CommandLength = FillCommandBuffer(0x47, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetTPowerSupplyMode(Mode, Modulator);
	return EC;
}

// Data inversion

//virtual
/*MC_ErrorCode CDatumPsm4900::IsRDataInvEnabled(BOOL &bEnabled, int Demodulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x4f, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = m_pDataBytes[0];
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::EnableRDataInv(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataInvEnabled(bEnabled, Demodulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bEnabled)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x4f, modeExecute, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	IsRDataInvEnabled(bEnabled, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::IsTDataInvEnabled(BOOL &bEnabled, int Modulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x0f, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = m_pDataBytes[0];
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::EnableTDataInv(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataInvEnabled(bEnabled, Modulator))
		return MC_OK; // already set

	unsigned char cData = 0; // off
	if (bEnabled)
		cData = 1;  // on

	int CommandLength = FillCommandBuffer(0x0f, modeExecute, &cData, 1);
	MC_ErrorCode EC = Command(CommandLength);
	IsTDataInvEnabled(bEnabled, Modulator);
	return EC;
}*/

// BER-test

//virtual
BOOL CDatumPsm4900::CanBerTest(int Power)
{
	return (Power == 0 || Power == 11 || Power == 23);
}

//virtual
MC_ErrorCode CDatumPsm4900::SetTBerTest(int &Power, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanBerTest(Power))
		return MC_INVALID_PARAMETERS;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<2;	// Output flag set
	switch (Power)
	{
	case 0: m_WriteData[4] = 0; break; // disable
	case 11: m_WriteData[4] = 1<<2; break; // 2^11-1 = 2047
	case 23: m_WriteData[4] = 1<<3; break; // 2^23-1
	}
	int CommandLength = FillCommandBuffer(0xC3, modeExecute, m_WriteData, 14);
	MC_ErrorCode EC = Command(CommandLength);

	GetTBerTest(Power, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetTBerTest(int &Power, int Modulator)
{
	Power = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0xC3, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char BERfield = m_pDataBytes[4] & 0x3C; // 4 bits [2,3,4,5]
	BERfield >>= 2;
	if (EC == MC_OK)
	{
		switch (BERfield)
		{
		case 0: Power = 0; break;
		case 1: Power = 11; break;
		case 2: Power = 23; break;
		default: Power = 0; break;
		}
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::SetRBerTest(int &Power, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanBerTest(Power))
		return MC_INVALID_PARAMETERS;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// Output flag set
	switch (Power)
	{
	case 0: m_WriteData[4] = 0; break; // disable
	case 11: m_WriteData[4] = 1<<6; break; // 2^11-1 = 2047
	case 23: m_WriteData[4] = 1<<7; break; // 2^23-1
	}
	int CommandLength = FillCommandBuffer(0xC3, modeExecute, m_WriteData, 14);
	MC_ErrorCode EC = Command(CommandLength);

	GetRBerTest(Power, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::GetRBerTest(int &Power, int Demodulator)
{
	Power = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0xC3, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char LowerBERfield = m_pDataBytes[4] & 0xC0; // 2 lower bits [6,7]
	LowerBERfield >>= 6;
	unsigned char UpperBERfield  = m_pDataBytes[5] & 0x03; // 2 upper bits [1,2];
	UpperBERfield <<= 2;
	unsigned char BERfield = LowerBERfield | UpperBERfield;
	if (EC == MC_OK)
	{
		switch (BERfield)
		{
		case 0: Power = 0; break;
		case 1: Power = 11; break;
		case 2: Power = 23; break;
		default: Power = 0; break;
		}
	}
	return EC;
}

MC_ErrorCode CDatumPsm4900::GetBerTestStatus
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
	int CommandLength = FillCommandBuffer(0xC0, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	
	// BER
	short mantissa = RawDataToSignedShort(m_pDataBytes+8);
	BER = mantissa/1000.;
	short exponent = RawDataToSignedShort(m_pDataBytes+10);
	for (int i = 0; i<exponent; i++)
		BER /= 10.;

	// SyncLoss
	SyncLoss = RawDataToInt(m_pDataBytes+12);

	// Error count
	Errors = RawDataToInt(m_pDataBytes+20);
	Errors *= 4294967296.;
	Errors += RawDataToInt(m_pDataBytes+16);

	// Bit count
	TotalBits = RawDataToInt(m_pDataBytes+28);
	TotalBits *= 4294967296.;
	TotalBits += RawDataToInt(m_pDataBytes+24);

	// Error free seconds percentage
	ErrorFreeSecsPercentage = RawDataToSignedShort(m_pDataBytes+32)/100.;

	// Errored seconds
	ErredSecs = RawDataToInt(m_pDataBytes+34);

	// Total test duration
	TotalTestDuration = RawDataToInt(m_pDataBytes+38);
	
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::RestartBerTest(int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int Power = 0, PowerForDisabling = 0;
	MC_ErrorCode EC = GetRBerTest(Power, Demodulator);
	if (EC != MC_OK)
		return EC;
	EC = SetRBerTest(PowerForDisabling, Demodulator);
	if (EC != MC_OK)
		return EC;
	EC = SetRBerTest(Power, Demodulator);
	return EC;
}

//virtual
BOOL CDatumPsm4900::Insert1ErrorForBerTest()
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<2;	// Output flag set
	m_WriteData[4] = 3<<2;	// Insert 1 error (undocumented (app.B of manual) but works)

	int CommandLength = FillCommandBuffer(0xC3, modeExecute, m_WriteData, 14);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

// LNB
//virtual
MC_ErrorCode CDatumPsm4900::GetLnbStatus(CLnbStatus &Status, int Demodulator)
{
	// Attention! The same code exists for L-band model. Duplicate all the changes for that type too.
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x87, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Status.m_Current = RawDataToSignedShort(m_pDataBytes+12) / 1000.;
	Status.m_CurrentMax = RawDataToSignedShort(m_pDataBytes+14) / 1000.;
	Status.m_CurrentMin = RawDataToSignedShort(m_pDataBytes+16) / 1000.;
	
	return MC_OK;
}

// BUC
//virtual
MC_ErrorCode CDatumPsm4900::GetBucStatus(CBucStatus &Status, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x47, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Status.m_Voltage = RawDataToSignedShort(m_pDataBytes+8) / 10.;
	Status.m_Current = RawDataToSignedShort(m_pDataBytes+12) / 100.;
	Status.m_CurrentMax = RawDataToSignedShort(m_pDataBytes+14) / 100.;
	Status.m_CurrentMin = RawDataToSignedShort(m_pDataBytes+16) / 100.;
	
	return MC_OK;
}

///////////////////////////////////////////////////////////////////////
// CDatumPsm4900_70MHz implementation

char *CDatumPsm4900_70MHz::m_pszName="Datum PSM-4900 (70 MHz modem)";

CDatumPsm4900_70MHz::CDatumPsm4900_70MHz()
{
}


///////////////////////////////////////////////////////////////////////
//virtual
CDatumPsm4900_70MHz::~CDatumPsm4900_70MHz()
{
}


///////////////////////////////////////////////////////////////////////
// CDatumPsm4900_140MHz implementation

char *CDatumPsm4900_140MHz::m_pszName="Datum PSM-4900 (140 MHz modem)";

CDatumPsm4900_140MHz::CDatumPsm4900_140MHz()
{
}

//virtual
CDatumPsm4900_140MHz::~CDatumPsm4900_140MHz()
{
}


///////////////////////////////////////////////////////////////////////
// CDatumPsm4900_Hybrid implementation

char *CDatumPsm4900_Hybrid::m_pszName="Datum PSM-4900 (Hybrid 70MHz and L-band modem)";

CDatumPsm4900_Hybrid::CDatumPsm4900_Hybrid()
{
}

//virtual
CDatumPsm4900_Hybrid::~CDatumPsm4900_Hybrid()
{
}


///////////////////////////////////////////////////////////////////////
// CDatumPsm4900_LBand implementation

char *CDatumPsm4900_LBand::m_pszName="Datum PSM-4900 (L-band modem)";

CDatumPsm4900_LBand::CDatumPsm4900_LBand()
{
}

//virtual
CDatumPsm4900_LBand::~CDatumPsm4900_LBand()
{
}

// IF params

//virtual
MC_ErrorCode CDatumPsm4900_LBand::GetRIfParams(CDemIfParams &Params, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = CDatumPsm4900::GetRIfParams(Params, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = IsR10MHzSupplierEnabled(Params.m_b10MHzSupplyEnabled, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = GetRPowerSupplyMode(Params.m_PowerSupplyMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return EC;
}


///////////////////////////////////////////////////////////////////////
// CDatumPsm500_LBand implementation

static const char *DATUM_PSM500_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK",
	"OQPSK",
	"8PSK",
	"8QAM",
	"16QAM"
};

static const char *DATUM_PSM500_FEC_MODES[] = {
	"None",					// 0

	"Viterbi 1/2",			// 1
	"Viterbi 3/4",			// 2
	"Viterbi 5/6",			// 3
	"Viterbi 7/8",			// 4

	"LDPC 256 block 1/2",		// 5
	"LDPC 256 block 2/3",		// 6
	"LDPC 256 block 3/4",		// 7
	"LDPC 256 block 14/17",		// 8
	"LDPC 256 block 7/8",		// 9
	"LDPC 256 block 10/11",		// 10
	"LDPC 256 block 16/17",		// 11

	"LDPC 512 block 1/2",		// 12
	"LDPC 512 block 2/3",		// 13
	"LDPC 512 block 3/4",		// 14
	"LDPC 512 block 14/17",		// 15
	"LDPC 512 block 7/8",		// 16
	"LDPC 512 block 10/11",		// 17
	"LDPC 512 block 16/17",		// 18

	"LDPC 1K block 1/2",		// 19
	"LDPC 1K block 2/3",		// 20
	"LDPC 1K block 3/4",		// 21
	"LDPC 1K block 14/17",		// 22
	"LDPC 1K block 7/8",		// 23
	"LDPC 1K block 10/11",		// 24
	"LDPC 1K block 16/17",		// 25

	"LDPC 2K block 1/2",		// 26
	"LDPC 2K block 2/3",		// 27
	"LDPC 2K block 3/4",		// 28
	"LDPC 2K block 14/17",		// 29
	"LDPC 2K block 7/8",		// 30
	"LDPC 2K block 10/11",		// 31
	"LDPC 2K block 16/17",		// 32

	"LDPC 4K block 1/2",		// 33
	"LDPC 4K block 2/3",		// 34
	"LDPC 4K block 3/4",		// 35
	"LDPC 4K block 14/17",		// 36
	"LDPC 4K block 7/8",		// 37
	"LDPC 4K block 10/11",		// 38
	"LDPC 4K block 16/17",		// 39

	"LDPC 8K block 1/2",		// 40
	"LDPC 8K block 2/3",		// 41
	"LDPC 8K block 3/4",		// 42
	"LDPC 8K block 14/17",		// 43
	"LDPC 8K block 7/8",		// 44
	"LDPC 8K block 10/11",		// 45
	"LDPC 8K block 16/17",		// 46

	"LDPC 16K block 1/2",		// 47
	"LDPC 16K block 2/3",		// 48
	"LDPC 16K block 3/4",		// 49
	"LDPC 16K block 14/17",		// 50
	"LDPC 16K block 7/8",		// 51
	"LDPC 16K block 10/11",		// 52
	"LDPC 16K block 16/17"		// 53
};

static const char *DATUM_PSM500_SCRAMBLER_MODES[] = {
	"Disable",		// 0
	"Auto",			// 1
	"V.35",			// 2
	"Intelsat",		// 3
	"Alt V.35",		// 4
	"Alt Intelsat",	// 5
	"EFD",			// 6
	"RS Sync",		// 7
	"IBS Sync",		// 8
	"FEC Sync"		// 9
};

static const char *DATUM_PSM500_DIFF_CODING_MODES[] = { "Disabled", "Enabled", "DiffCoherent" };

static const char *DATUM_PSM500_R_CLOCK_SOURCES[] = { "RCV_Clock", "Internal", "External", "Mod_Clock"};
static const char *DATUM_PSM500_T_CLOCK_SOURCES[] = { "Internal", "TT_Clock", "External", "RCV_Clock"};

static const char *DATUM_PSM500_R_SWEEP_MODES[] = { "Normal (fast)", "Search" };
static const char *DATUM_PSM500_R_POWER_SUPPLY_MODES[] = { "None", "13 Volts", "18 Volts" };
static const char *DATUM_PSM500_T_POWER_SUPPLY_MODES[] = { "None", "24 Volts" };

static const char *DATUM_PSM500_REED_SOLOMON_MODES[] = { "Disabled", "IESS-308", "IESS-309", "IESS-310", "Custom", "CT220,200" };

CDatumPsm500::CDatumPsm500()
{
}

//virtual
CDatumPsm500::~CDatumPsm500()
{
}

//virtual
MC_ErrorCode CDatumPsm500::GetDeviceVersionString(char *&pszString)
{
	pszString = m_szVersion;
	m_szVersion[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	// Model
	strcpy(m_szVersion, "Model : ");
	m_pDataBytes[m_ReturnedDataLength] = 0x00;
	strcat(m_szVersion, (char *)(m_pDataBytes+27));
	strcat(m_szVersion, szCRLF);

	// Features
	strcat(m_szVersion, "Features : ");
	strcat(m_szVersion, (char *)(m_pDataBytes+44));
	strcat(m_szVersion, szCRLF);

	// Software version
	strcat(m_szVersion, "Software version : ");
	strcat(m_szVersion, (char *)(m_pDataBytes+61));
	strcat(m_szVersion, szCRLF);

	// Serial number
	unsigned int SN = RawDataToInt(m_pDataBytes+78);
	strcat(m_szVersion, "Serial No : ");
	char szSerial[64];
	sprintf(szSerial, "%d", SN);
	strcat(m_szVersion, szSerial);
	strcat(m_szVersion, szCRLF);

	char szFec[64];
	// FEC A
	strcat(m_szVersion, "FEC Slot A :");
	strcat(m_szVersion, szCRLF);
	unsigned short FecType = RawDataToSignedShort(m_pDataBytes+82);
	if (FecType)
	{
		sprintf(szFec, "\tFEC Type = %d", FecType);
		strcat(m_szVersion, szFec);
		strcat(m_szVersion, szCRLF);
		sprintf(szFec, "\tFEC Version = %.2f", m_pDataBytes[84]/100.);
		strcat(m_szVersion, szFec);
		strcat(m_szVersion, szCRLF);
		SN = RawDataToInt(m_pDataBytes+86);
		sprintf(szSerial, "\tFEC Serial No = %d", SN);
		strcat(m_szVersion, szSerial);
		strcat(m_szVersion, szCRLF);
	}
	else
	{
		strcat(m_szVersion, "\tNOT INSTALLED");
		strcat(m_szVersion, szCRLF);
	}

	// FEC B
	strcat(m_szVersion, "FEC Slot B :");
	strcat(m_szVersion, szCRLF);
	FecType = RawDataToSignedShort(m_pDataBytes+90);
	if (FecType)
	{
		sprintf(szFec, "\tFEC Type = %d", FecType);
		strcat(m_szVersion, szFec);
		strcat(m_szVersion, szCRLF);
		sprintf(szFec, "\tFEC Version = %f.2", m_pDataBytes[92]/100.);
		strcat(m_szVersion, szFec);
		strcat(m_szVersion, szCRLF);
		SN = RawDataToInt(m_pDataBytes+94);
		sprintf(szSerial, "\tFEC Serial No = %d", SN);
		strcat(m_szVersion, szSerial);
		strcat(m_szVersion, szCRLF);
	}
	else
	{
		strcat(m_szVersion, "\tNOT INSTALLED");
		strcat(m_szVersion, szCRLF);
	}


	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetUnitName(char *&pszString)
{	
	pszString = m_szUnitName;
	m_szUnitName[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	m_pDataBytes[m_ReturnedDataLength] = 0x00;
	strcat(m_szUnitName, (char *)(m_pDataBytes+10));

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetUnitName(char *pszString)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = (char)1<<3;	// Frequency flag set
	if (strlen(pszString) < 16)
		strcpy((char *)(m_WriteData+10), pszString);
	else
		strncpy((char *)(m_WriteData+10), pszString, 15);
	int CommandLength = FillCommandBuffer(0x00, modeExecute, m_WriteData, 128);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetModelName(std::string &strModel)
{
	strModel = "";
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_OK)
	{
		m_pDataBytes[m_ReturnedDataLength] = 0x00;
		strModel = (char *)(m_pDataBytes+27);
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetSerialNo(unsigned int &serialNo)
{
	serialNo = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);

	serialNo = RawDataToInt(m_pDataBytes+78);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetSoftwareVersion(std::string &strVersion)
{
	strVersion = "";
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x00, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_OK)
	{
		m_pDataBytes[m_ReturnedDataLength] = 0x00;
		strVersion = (char *)(m_pDataBytes+61);
	}
	return EC;
}

// Non Volatile RAM

// 10 MHz reference 

//virtual
MC_ErrorCode CDatumPsm500::IsR10MHzSupplierEnabled(BOOL &bEnable, int Demodulator)
{
	bEnable = FALSE;
	if (!CanR10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x86, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = m_pDataBytes[4];
	bits >>= 3;
	bits = bits & 0x07;
	bEnable = (bits != 0);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::EnableR10MHzSupplier(BOOL &bEnable, int Demodulator)
{
	if (!CanR10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 6;	// Output flag set for enable/disable
	if (bEnable)
		m_WriteData[4] = 1 << 3;	// Enabled
	else
		m_WriteData[4] = 0;			// Disabled

	int CommandLength = FillCommandBuffer(0x86, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	IsR10MHzSupplierEnabled(bEnable, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::IsT10MHzSupplierEnabled(BOOL &bEnable, int Modulator)
{
	bEnable = FALSE;
	if (!CanT10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x46, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = m_pDataBytes[4];
	bits >>= 3;
	bits = bits & 0x07;
	bEnable = (bits != 0);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::EnableT10MHzSupplier(BOOL &bEnable, int Modulator)
{
	if (!CanT10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 6;	// Output flag set for enable/disable
	if (bEnable)
		m_WriteData[4] = 1 << 3;	// Enabled
	else
		m_WriteData[4] = 0;			// Disabled

	int CommandLength = FillCommandBuffer(0x46, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	IsT10MHzSupplierEnabled(bEnable, Modulator);
	return EC;
}


// IF params and status

//virtual
MC_ErrorCode CDatumPsm500::GetRIfParams(CDemIfParams &Params, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	Params.m_SearchRange = RawDataToInt(m_pDataBytes+14); // Hz
	Params.m_SweepTime = RawDataToSignedShort(m_pDataBytes+18);
	Params.m_LowEbno = RawDataToSignedShort(m_pDataBytes+20)/10.;
	Params.m_LowLevel = RawDataToSignedShort(m_pDataBytes+22)/10.;

	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Params.m_ModulationType = 0; // BPSK
	else if (ModField == 1)
		Params.m_ModulationType = 1; // QPSK
	else if (ModField == 2)
		Params.m_ModulationType = 2; // OQPSK
	else if (ModField == 3)
		Params.m_ModulationType = 3; // 8PSK
	else if (ModField == 4)
		Params.m_ModulationType = 4; // 8QAM
	else if (ModField == 6)
		Params.m_ModulationType = 5; // 16QAM

	Params.m_bSpectrumInversion = ((m_pDataBytes[4] & 1<<5) != 0);

	EC = IsR10MHzSupplierEnabled(Params.m_b10MHzSupplyEnabled, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = GetRPowerSupplyMode(Params.m_PowerSupplyMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetRIfStatus(CDemIfStatus &Status, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Status.m_bCarrier = ((m_pDataBytes[4] & 1<<0) != 0);
	Status.m_bDemLock = ((m_pDataBytes[4] & 1<<0) != 0); // the same as bCarrier
	Status.m_SN = RawDataToSignedShort(m_pDataBytes+8) / 10.;
	Status.m_Offset = RawDataToInt(m_pDataBytes+10);
	Status.m_InputLevel = RawDataToSignedShort(m_pDataBytes+14) / 10.;
	Status.m_Buffer = RawDataToSignedShort(m_pDataBytes+20);
	Status.m_BER = ParseBER();
	return MC_OK;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTIfParams(CModIfParams &Params, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	Params.m_Offset =  RawDataToInt(m_pDataBytes+14);

	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Params.m_ModulationType = 0; // BPSK
	else if (ModField == 1)
		Params.m_ModulationType = 1; // QPSK
	else if (ModField == 2)
		Params.m_ModulationType = 2; // OQPSK
	else if (ModField == 3)
		Params.m_ModulationType = 3; // 8PSK
	else if (ModField == 4)
		Params.m_ModulationType = 4; // 8QAM
	else if (ModField == 6)
		Params.m_ModulationType = 5; // 16QAM

	Params.m_bOutputEnabled = (m_pDataBytes[4] & 1);
	Params.m_OutputLevel = RawDataToSignedShort(m_pDataBytes+18)/10.;

	Params.m_bSpectrumInversion = (m_pDataBytes[4] & 1<<5) != 0;

	EC = IsContiniousWaveOn(Params.m_bContiniousWave, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = IsT10MHzSupplierEnabled(Params.m_b10MHzSupplyEnabled, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	EC = GetTPowerSupplyMode(Params.m_PowerSupplyMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	
	return EC;
}

// Frequency (carrier)

//virtual
MC_ErrorCode CDatumPsm500::GetRFrequency(unsigned int &Frequency, int Demodulator)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRFrequency(unsigned int &Frequency, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFrequency(Frequency, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = (char)1;	// Frequency flag set
	IntToRawData(Frequency * 1000, m_WriteData+8);
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 38);
	MC_ErrorCode EC = Command(CommandLength);

	GetRFrequency(Frequency, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTFrequency(unsigned int &Frequency, int Modulator)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Frequency = RawDataToInt(m_pDataBytes+8) / 1000;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFrequency(Frequency, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Frequency flag set
	IntToRawData(Frequency * 1000, m_WriteData+8);
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetTFrequency(Frequency, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	SearchRange = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	SearchRange = RawDataToInt(m_pDataBytes+14); // Hz
	return EC;
}

//virtual 
MC_ErrorCode CDatumPsm500::SetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateSearchRange(SearchRange, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<1;	// Sweep range flag set
	IntToRawData(SearchRange, m_WriteData+14);
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 38);
	MC_ErrorCode EC = Command(CommandLength);

	GetSearchRange(SearchRange, Demodulator);
	return EC;
}

//virtual
int CDatumPsm500::GetRSweepModesCount()
{
	return sizeof(DATUM_PSM500_R_SWEEP_MODES)/sizeof(DATUM_PSM500_R_SWEEP_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetRSweepModeName(int Mode)
{
	return DATUM_PSM500_R_SWEEP_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetRSweepMode(int &Mode, int Demodulator)
{
	Mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Mode = (m_pDataBytes[4] & 1);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRSweepMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSweepMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<2;	// Sweep mode flag set
	IntToRawData(Mode, m_WriteData+0);
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 38);
	MC_ErrorCode EC = Command(CommandLength);

	GetRSweepMode(Mode, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetInputLevel(double &InputLevel, int Demodulator)
{
	InputLevel = 0.;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	InputLevel = RawDataToSignedShort(m_pDataBytes+14) / 10.;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetModulatorShift(int &Shift, int Modulator)
{
	Shift = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Shift = RawDataToInt(m_pDataBytes+14);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetModulatorShift(int &Shift, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateModulatorShift(Shift, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 1;	// Offset flag set
	IntToRawData(Shift, m_WriteData+14);
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetModulatorShift(Shift, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::IsContiniousWaveOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x44, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = (m_pDataBytes[4] & 3);
	bOn = (bits == 1);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::TurnContiniousWaveOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Output flag set
	if (bOn)
		m_WriteData[4] = 1;		// Pure carrier
	else
		m_WriteData[4] = 0;		// Normal
	int CommandLength = FillCommandBuffer(0x44, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	IsContiniousWaveOn(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetEbNo(double &EbNo, int Demodulator)
{
	EbNo = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	EbNo = RawDataToSignedShort(m_pDataBytes+8) / 10.;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::IsRCarrier(BOOL &bValid, int Demodulator)
{
	bValid = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bValid = ((m_pDataBytes[4] & 1) != 0);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetDemodulatorOffset(int &Offset, int Demodulator)
{
	Offset = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Offset = RawDataToInt(m_pDataBytes+10);
	return EC;
}

// Demodulator reference generator fine tune

//virtual
MC_ErrorCode CDatumPsm500::GetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator)
{
	FineTuneValue = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x05, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	FineTuneValue = RawDataToSignedShort(m_pDataBytes+6);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDemodulatorRefFineTune(FineTuneValue, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 2;	// Ref fine tune flag
	SignedShortToRawData(LOWORD(FineTuneValue), m_WriteData+6);
	int CommandLength = FillCommandBuffer(0x05, modeExecute, m_WriteData, 12);
	MC_ErrorCode EC = Command(CommandLength);

	GetDemodulatorRefFineTune(FineTuneValue, Demodulator);
	return EC;
}

// Doppler Buffer Size
//virtual
MC_ErrorCode CDatumPsm500::GetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	BufferSize = RawDataToInt(m_pDataBytes+24);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDemodulatorDopplerBufferSize(BufferSize, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1 << 5;	// buffer size enable flag
	IntToRawData(BufferSize, m_WriteData+24);
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetDemodulatorDopplerBufferSize(BufferSize, Demodulator);
	return EC;
}

// Modulation types

//virtual
int CDatumPsm500::GetRModulationTypeCount()
{
	return sizeof(DATUM_PSM500_MODULATION_TYPES)/sizeof(DATUM_PSM500_MODULATION_TYPES[0]);
}

//virtual
const char *CDatumPsm500::GetRModulationTypeName(int Type)
{
	return DATUM_PSM500_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatumPsm500::GetRModulationType(int &Type, int Demodulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ModField = m_pDataBytes[4] >> 1;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Type = 0; // BPSK
	else if (ModField == 1)
		Type = 1; // QPSK
	else if (ModField == 2)
		Type = 2; // OQPSK
	else if (ModField == 3)
		Type = 3; // 8PSK
	else if (ModField == 4)
		Type = 4; // 8QAM
	else if (ModField == 6)
		Type = 5; // 16QAM
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRModulationType(int &Type, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRModulationType(Type, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Modulation flag set
	if (Type == 0)
		m_WriteData[4] = 0;	// BPSK
	else if (Type == 1)
		m_WriteData[4] = 1;	// QPSK
	else if (Type == 2)
		m_WriteData[4] = 2;	// OQPSK
	else if (Type == 3)
		m_WriteData[4] = 3;	// 8PSK
	else if (Type == 4)
		m_WriteData[4] = 4;	// 8QAM
	else if (Type == 5)
		m_WriteData[4] = 6;	// 16QAM (not 5, 5 is not in use doc says)
	m_WriteData[4] <<= 1;

	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 38);
	MC_ErrorCode EC = Command(CommandLength);

	GetRModulationType(Type, Demodulator);
	return EC;
}

//virtual
int CDatumPsm500::GetTModulationTypeCount()
{
	return sizeof(DATUM_PSM500_MODULATION_TYPES)/sizeof(DATUM_PSM500_MODULATION_TYPES[0]);
}

//virtual
MC_ErrorCode CDatumPsm500::GetTModulationType(int &Type, int Modulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ModField = m_pDataBytes[4] >> 1;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Type = 0; // BPSK
	else if (ModField == 1)
		Type = 1; // QPSK
	else if (ModField == 2)
		Type = 2; // OQPSK
	else if (ModField == 3)
		Type = 3; // 8PSK
	else if (ModField == 4)
		Type = 4; // 8QAM
	else if (ModField == 6) // 16QAM (not 5, 5 is not in use doc says)
		Type = 5; // 16QAM
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTModulationType(int &Type, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTModulationType(Type, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Modulation flag set
	if (Type == 0)
		m_WriteData[4] = 0;	// BPSK
	else if (Type == 1)
		m_WriteData[4] = 1;	// QPSK
	else if (Type == 2)
		m_WriteData[4] = 2;	// OQPSK
	else if (Type == 3)
		m_WriteData[4] = 3;	// 8PSK
	else if (Type == 4)
		m_WriteData[4] = 4;	// 8QAM
	else if (Type == 5)
		m_WriteData[4] = 6;	// 16QAM (not 5, 5 is not in use doc says)
	m_WriteData[4] <<= 1;

	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetTModulationType(Type, Modulator);
	return EC;
}

//virtual
const char *CDatumPsm500::GetTModulationTypeName(int Type)
{
	return DATUM_PSM500_MODULATION_TYPES[Type];
}

// Differential encoding/decoding

//virtual
int CDatumPsm500::GetDiffDecoderModeCount()
{
	return sizeof(DATUM_PSM500_DIFF_CODING_MODES)/sizeof(DATUM_PSM500_DIFF_CODING_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetDiffDecoderModeName(int Mode)
{
	return DATUM_PSM500_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetDiffDecoderMode(int &Mode, int Demodulator)
{
	Mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Mode = m_pDataBytes[5] & (3<<2);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetDiffDecoderMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffDecoderMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<1;	// ??? Differential decoding enabling flag set
	m_WriteData[5] = Mode << 2;
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetDiffDecoderMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatumPsm500::GetDiffEncoderModeCount()
{
	return sizeof(DATUM_PSM500_DIFF_CODING_MODES)/sizeof(DATUM_PSM500_DIFF_CODING_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetDiffEncoderModeName(int Mode)
{
	return DATUM_PSM500_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetDiffEncoderMode(int &Mode, int Modulator)
{
	Mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Mode = m_pDataBytes[5] & (3<<2);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetDiffEncoderMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffEncoderMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<1;	// ??? Differential encoding enabling flag set
	m_WriteData[5] = Mode << 2;
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetDiffEncoderMode(Mode, Modulator);
	return EC;
}

// Scrambling/Descrambling

//virtual
int CDatumPsm500::GetScramblerModesCount()
{
	return sizeof(DATUM_PSM500_SCRAMBLER_MODES)/sizeof(DATUM_PSM500_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetScramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_PSM500_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetScramblerMode(int &mode, int Modulator)
{
	mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[5];
	ScramblerField &= 0xF0;		// 4 bits
	mode = ScramblerField >> 4;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetScramblerMode(int &mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateScramblerMode(mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1<<2;	// Scrambling flag set
	m_WriteData[5] = (char)mode << 4;

	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	GetScramblerMode(mode, Modulator);
	return EC;
}

//virtual
int CDatumPsm500::GetDescramblerModesCount()
{
	return sizeof(DATUM_PSM500_SCRAMBLER_MODES)/sizeof(DATUM_PSM500_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetDescramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_PSM500_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetDescramblerMode(int &mode, int Demodulator)
{
	mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[5];
	ScramblerField &= 0xF0;		// 4 bits
	mode = ScramblerField >> 4;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetDescramblerMode(int &mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDescramblerMode(mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1<<2;	// Descrambling flag set
	m_WriteData[5] = (char)mode << 4;

	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetDescramblerMode(mode, Demodulator);
	return EC;
}

// Spectral inversion

//virtual
MC_ErrorCode CDatumPsm500::IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x81, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = (m_pDataBytes[4] & 1<<5) != 0;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::EnableRSpectralInv(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSpectralInvEnabled(bEnabled, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Output flag set
	if (bEnabled)
		m_WriteData[4] = 1<<5;	// Enable
	else
		m_WriteData[4] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 38);
	MC_ErrorCode EC = Command(CommandLength);

	IsRSpectralInvEnabled(bEnabled, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator)
{
	bEnabled = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bEnabled = (m_pDataBytes[4] & 1<<5) != 0;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::EnableTSpectralInv(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTSpectralInvEnabled(bEnabled, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Output flag set
	if (bEnabled)
		m_WriteData[4] = 1<<5;	// Enable
	else
		m_WriteData[4] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	IsTSpectralInvEnabled(bEnabled, Modulator);
	return EC;
}

// DC suppliers

//virtual
int CDatumPsm500::GetRPowerSupplyModesCount()
{
	return sizeof(DATUM_PSM500_R_POWER_SUPPLY_MODES)/sizeof(DATUM_PSM500_R_POWER_SUPPLY_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetRPowerSupplyModeName(int Mode)
{
	return DATUM_PSM500_R_POWER_SUPPLY_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetRPowerSupplyMode(int &Mode, int Demodulator)
{
	Mode = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x86, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char LNBfield = m_pDataBytes[4] & 0x07; // 3 bits [0,1,2]
	if (EC != MC_OK)
		return EC;
	
	switch (LNBfield)
	{
	case 0: Mode = 0; break;
	case 1: Mode = 1; break;
	case 2: Mode = 2; break;
	default: Mode = 0; break;
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRPowerSupplyMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRPowerSupplyMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// bit [0]
	switch (Mode)
	{
	case 0: m_WriteData[4] = 0; break;
	case 1: m_WriteData[4] = 1; break;
	case 2: m_WriteData[4] = 2; break;
	}
	int CommandLength = FillCommandBuffer(0x86, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetRPowerSupplyMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatumPsm500::GetTPowerSupplyModesCount()
{
	return sizeof(DATUM_PSM500_T_POWER_SUPPLY_MODES)/sizeof(DATUM_PSM500_T_POWER_SUPPLY_MODES[0]);
}

//virtual
const char *CDatumPsm500::GetTPowerSupplyModeName(int Mode)
{
	return DATUM_PSM500_T_POWER_SUPPLY_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetTPowerSupplyMode(int &Mode, int Modulator)
{
	Mode = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x46, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char BUCfield = m_pDataBytes[4] & 0x07; // 3 bits [0,1,2]
	if (EC != MC_OK)
		return EC;
	switch (BUCfield)
	{
	case 0: Mode = 0; break;
	case 1: Mode = 1; break;
	default: Mode = 0; break;
	}

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTPowerSupplyMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTPowerSupplyMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// bit [0]
	switch (Mode)
	{
	case 0: m_WriteData[4] = 0; break;
	case 1: m_WriteData[4] = 1; break;
	}
	int CommandLength = FillCommandBuffer(0x46, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetTPowerSupplyMode(Mode, Modulator);
	return EC;
}

// BER

//virtual
MC_ErrorCode CDatumPsm500::GetBER(double &BER, int Demodulator)
{
	BER = 1.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	BER = ParseBER();

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetSER(double &SER, int Demodulator)
{
	SER = 1.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x80, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	SER = ParseSER();

	return EC;
}

// FEC
//virtual
int CDatumPsm500::GetRFecModesCount()
{
	return (sizeof(DATUM_PSM500_FEC_MODES)/sizeof(DATUM_PSM500_FEC_MODES[0]));
}

//virtual
const char *CDatumPsm500::GetRFecModeName(int Mode)
{
	return DATUM_PSM500_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetRFecMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Mode = ParseReplyForFecMode();

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRFecMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFecMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	
	FillWriteDataWithFecMode(Mode);

	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetRFecMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatumPsm500::GetTFecModesCount()
{
	return (sizeof(DATUM_PSM500_FEC_MODES)/sizeof(DATUM_PSM500_FEC_MODES[0]));
}

//virtual
const char *CDatumPsm500::GetTFecModeName(int Mode)
{
	return DATUM_PSM500_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetTFecMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Mode = ParseReplyForFecMode();

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTFecMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFecMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	
	FillWriteDataWithFecMode(Mode);

	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	GetTFecMode(Mode, Modulator);
	return EC;
}

int CDatumPsm500::ParseReplyForFecMode() const
{
	int Mode = 0;
	unsigned char FecType = m_pDataBytes[12];
	if (FecType == 0)
	{ // None
		Mode = 0;
	}
	else if (FecType == 1)
	{ // Viterbi
		unsigned char CodeRate = m_pDataBytes[16];
		if (CodeRate == 0)
			Mode = 1; // Viterbi 1/2
		else if (CodeRate == 1)
			Mode = 2; // Viterbi 3/4
		else if (CodeRate == 2)
			Mode = 3; // Viterbi 5/6
		else if (CodeRate == 3)
			Mode = 4; // Viterbi 7/8
	}
	else if (FecType == 2)
	{
	}
	else if (FecType == 3)
	{
	}
	else if (FecType == 4)
	{
	}
	else if (FecType == 5)
	{ // LDPC
		unsigned char BlockSize = m_pDataBytes[14];
		unsigned char CodeRate = m_pDataBytes[16];
		if (BlockSize == 0)
		{ // LDPC 256 Block
			if (CodeRate == 0)
				Mode = 5; // 1/2
			else if (CodeRate == 1)
				Mode = 6;  // 1/2
			else if (CodeRate == 2)
				Mode = 7;  // 1/2
			else if (CodeRate == 3)
				Mode = 8;  // 1/2
			else if (CodeRate == 4)
				Mode = 9;  // 1/2
			else if (CodeRate == 5)
				Mode = 10;  // 1/2
			else if (CodeRate == 6)
				Mode = 11;  // 1/2
		}
		else if (BlockSize == 1)
		{ // LDPC 512 Block
			if (CodeRate == 0)
				Mode = 12; // 1/2
			else if (CodeRate == 1)
				Mode = 13;  // 1/2
			else if (CodeRate == 2)
				Mode = 14;  // 1/2
			else if (CodeRate == 3)
				Mode = 15;  // 1/2
			else if (CodeRate == 4)
				Mode = 16;  // 1/2
			else if (CodeRate == 5)
				Mode = 17;  // 1/2
			else if (CodeRate == 6)
				Mode = 18;  // 1/2
		}
		else if (BlockSize == 2)
		{ // LDPC 1K Block
			if (CodeRate == 0)
				Mode = 19; // 1/2
			else if (CodeRate == 1)
				Mode = 20;  // 1/2
			else if (CodeRate == 2)
				Mode = 21;  // 1/2
			else if (CodeRate == 3)
				Mode = 22;  // 1/2
			else if (CodeRate == 4)
				Mode = 23;  // 1/2
			else if (CodeRate == 5)
				Mode = 24;  // 1/2
			else if (CodeRate == 6)
				Mode = 25;  // 1/2
		}
		else if (BlockSize == 3)
		{ // LDPC 2K Block
			if (CodeRate == 0)
				Mode = 26; // 1/2
			else if (CodeRate == 1)
				Mode = 27;  // 1/2
			else if (CodeRate == 2)
				Mode = 28;  // 1/2
			else if (CodeRate == 3)
				Mode = 29;  // 1/2
			else if (CodeRate == 4)
				Mode = 30;  // 1/2
			else if (CodeRate == 5)
				Mode = 31;  // 1/2
			else if (CodeRate == 6)
				Mode = 32;  // 1/2
		}
		else if (BlockSize == 4)
		{ // LDPC 4K Block
			if (CodeRate == 0)
				Mode = 33; // 1/2
			else if (CodeRate == 1)
				Mode = 34;  // 1/2
			else if (CodeRate == 2)
				Mode = 35;  // 1/2
			else if (CodeRate == 3)
				Mode = 36;  // 1/2
			else if (CodeRate == 4)
				Mode = 37;  // 1/2
			else if (CodeRate == 5)
				Mode = 38;  // 1/2
			else if (CodeRate == 6)
				Mode = 39;  // 1/2
		}
		else if (BlockSize == 5)
		{ // LDPC 8K Block
			if (CodeRate == 0)
				Mode = 40; // 1/2
			else if (CodeRate == 1)
				Mode = 41;  // 1/2
			else if (CodeRate == 2)
				Mode = 42;  // 1/2
			else if (CodeRate == 3)
				Mode = 43;  // 1/2
			else if (CodeRate == 4)
				Mode = 44;  // 1/2
			else if (CodeRate == 5)
				Mode = 45;  // 1/2
			else if (CodeRate == 6)
				Mode = 46;  // 1/2
		}
		else if (BlockSize == 6)
		{ // LDPC 16K Block
			if (CodeRate == 0)
				Mode = 47; // 1/2
			else if (CodeRate == 1)
				Mode = 48;  // 1/2
			else if (CodeRate == 2)
				Mode = 49;  // 1/2
			else if (CodeRate == 3)
				Mode = 50;  // 1/2
			else if (CodeRate == 4)
				Mode = 51;  // 1/2
			else if (CodeRate == 5)
				Mode = 52;  // 1/2
			else if (CodeRate == 6)
				Mode = 53;  // 1/2
		}
	}
	else
	{
		//ASSERT(0);
	}
	return Mode;
}

void CDatumPsm500::FillWriteDataWithFecMode(int Mode)
{
	m_WriteData[0] |= 1<<2;	// FEC type flag set
	m_WriteData[0] |= 1<<3;	// FEC opt flag set
	m_WriteData[0] |= 1<<4;	// FEC code rate flag set
	switch (Mode)
	{
	case 0: //None
		m_WriteData[12] = 0;
		m_WriteData[14] = 0;
		m_WriteData[16] = 0;
		break;

	case 1: //Viterbi 1/2
		m_WriteData[12] = 1; // Viterbi
		m_WriteData[14] = 0;
		m_WriteData[16] = 0; // 1/2
		break;
	case 2: //Viterbi 3/4
		m_WriteData[12] = 1; // Viterbi
		m_WriteData[14] = 0;
		m_WriteData[16] = 1; // 3/4
		break;
	case 3: //Viterbi 5/6
		m_WriteData[12] = 1; // Viterbi
		m_WriteData[14] = 0;
		m_WriteData[16] = 2; // 5/6
		break;
	case 4: //Viterbi 7/8
		m_WriteData[12] = 1; // Viterbi
		m_WriteData[14] = 0;
		m_WriteData[16] = 3; // 7/8
		break;

	case 5: //LDPC 256 block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 0; // 1/2
		break;
	case 6: //LDPC 256 block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 1; // 2/3
		break;
	case 7: //LDPC 256 block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 2; // 3/4
		break;
	case 8: //LDPC 256 block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 3; // 14/17
		break;
	case 9: //LDPC 256 block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 4; // 7/8
		break;
	case 10: //LDPC 256 block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 5; // 10/11
		break;
	case 11: //LDPC 256 block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 0; // 256
		m_WriteData[16] = 6; // 16/17
		break;

	case 12: //LDPC 512 block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 0; // 1/2
		break;
	case 13: //LDPC 512 block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 1; // 2/3
		break;
	case 14: //LDPC 512 block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 2; // 3/4
		break;
	case 15: //LDPC 512 block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 3; // 14/17
		break;
	case 16: //LDPC 512 block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 4; // 7/8
		break;
	case 17: //LDPC 512 block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 5; // 10/11
		break;
	case 18: //LDPC 512 block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 1; // 512
		m_WriteData[16] = 6; // 16/17
		break;

	case 19: //LDPC 1K block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 0; // 1/2
		break;
	case 20: //LDPC 1K block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 1; // 2/3
		break;
	case 21: //LDPC 1K block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 2; // 2/3
		break;
	case 22: //LDPC 1K block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 3; // 14/17
		break;
	case 23: //LDPC 1K block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 4; // 7/8
		break;
	case 24: //LDPC 1K block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 5; // 10/11
		break;
	case 25: //LDPC 1K block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 2; // 1K
		m_WriteData[16] = 6; // 16/16
		break;

	case 26: //LDPC 2K block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 0; // 1/2
		break;
	case 27: //LDPC 2K block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 1; // 2/3
		break;
	case 28: //LDPC 2K block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 2; // 3/4
		break;
	case 29: //LDPC 2K block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 3; // 14/17
		break;
	case 30: //LDPC 2K block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 4; // 7/8
		break;
	case 31: //LDPC 2K block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 5; // 10/11
		break;
	case 32: //LDPC 2K block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 3; // 2K
		m_WriteData[16] = 6; // 16/17
		break;

	case 33: //LDPC 4K block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 0; // 1/2
		break;
	case 34: //LDPC 4K block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 1; // 2/3
		break;
	case 35: //LDPC 4K block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 2; // 3/4
		break;
	case 36: //LDPC 4K block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 3; // 14/17
		break;
	case 37: //LDPC 4K block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 4; // 7/8
		break;
	case 38: //LDPC 4K block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 5; // 10/11
		break;
	case 39: //LDPC 4K block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 4; // 4K
		m_WriteData[16] = 6; // 16/17
		break;

	case 40: //LDPC 8K block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 0; // 1/2
		break;
	case 41: //LDPC 8K block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 1; // 2/3
		break;
	case 42: //LDPC 8K block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 2; // 3/4
		break;
	case 43: //LDPC 8K block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 3; // 14/17
		break;
	case 44: //LDPC 8K block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 4; // 7/8
		break;
	case 45: //LDPC 8K block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 5; // 10/11
		break;
	case 46: //LDPC 8K block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 5; // 8K
		m_WriteData[16] = 6; // 16/17
		break;

	case 47: //LDPC 16K block 1/2
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 0; // 1/2
		break;
	case 48: //LDPC 16K block 2/3
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 1; // 2/3
		break;
	case 49: //LDPC 16K block 3/4
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 2; // 3/4
		break;
	case 50: //LDPC 16K block 14/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 3; // 14/17
		break;
	case 51: //LDPC 16K block 7/8
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 4; // 7/8
		break;
	case 52: //LDPC 16K block 10/11
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 5; // 10/11
		break;
	case 53: //LDPC 16K block 16/17
		m_WriteData[12] = 5; // LDPC
		m_WriteData[14] = 6; // 16K
		m_WriteData[16] = 6; // 16/17
		break;
	default:
		//ASSERT(0);
		break;
	}
}

// Reed-Solomon

//virtual
int CDatumPsm500::GetReedSolomonModesCount()
{
	return (sizeof(DATUM_PSM500_REED_SOLOMON_MODES)/sizeof(DATUM_PSM500_REED_SOLOMON_MODES[0]));
}

//virtual
const char *CDatumPsm500::GetReedSolomonModeName(int Mode)
{
	return DATUM_PSM500_REED_SOLOMON_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm500::GetRReedSolomonMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Mode = (m_pDataBytes[6] && 0xF0 ) >> 4; // bits 4,5,6,7

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTReedSolomonMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Mode = (m_pDataBytes[6] && 0xF0 ) >> 4; // bits 4,5,6,7

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

// Data clock source

//virtual
int CDatumPsm500::GetRDataClockSourcesCount()
{
	return sizeof(DATUM_PSM500_R_CLOCK_SOURCES)/sizeof(DATUM_PSM500_R_CLOCK_SOURCES[0]);
}

//virtual
const char *CDatumPsm500::GetRDataClockSourceName(int source)
{
	return DATUM_PSM500_R_CLOCK_SOURCES[source];
}

//virtual
int CDatumPsm500::GetTDataClockSourcesCount()
{
	return sizeof(DATUM_PSM500_T_CLOCK_SOURCES)/sizeof(DATUM_PSM500_T_CLOCK_SOURCES[0]);
}

//virtual
const char *CDatumPsm500::GetTDataClockSourceName(int source)
{
	return DATUM_PSM500_T_CLOCK_SOURCES[source];
}

//virtual
MC_ErrorCode CDatumPsm500::GetRDataClockSource(int &Source, int Modulator)
{
	Source = DCS_Unknown;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Source = m_pDataBytes[6] & 0x07; // 3 bits only
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRDataClockSource(int &Source, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataClockSource(Source, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1<<3;	// Clock source flag set
	m_WriteData[6] = Source;
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetRDataClockSource(Source, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTDataClockSource(int &Source, int Modulator)
{
	Source = DCS_Unknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Source = m_pDataBytes[6] & 0x07; // 3 bits only
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTDataClockSource(int &Source, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataClockSource(Source, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1<<3;	// Clock source flag set
	m_WriteData[6] = Source;
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	GetTDataClockSource(Source, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::IsOutputOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	bOn = (m_pDataBytes[4] & 1);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::TurnOutputOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputOn(bOn, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// Output flag set
	if (bOn)
		m_WriteData[4] = 1;		// Enable
	else
		m_WriteData[4] = 0;		// Disable
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	IsOutputOn(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetOutputLevel(double &Level, int Modulator)
{
	Level = 0.;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	Level = RawDataToSignedShort(m_pDataBytes+18)/10.;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetOutputLevel(double &Level, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputLevel(Level, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<2;	// Output level flag set
	SignedShortToRawData((short)(Level*10.), m_WriteData+18); // dB*10
	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetOutputLevel(Level, Modulator);
	return EC;
}

// Data rate

//virtual
MC_ErrorCode CDatumPsm500::GetRDataRate(unsigned int &DataRate, int Demodulator)
{
	DataRate = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	DataRate = RawDataToInt(m_pDataBytes+8);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRDataRate(unsigned int &DataRate, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataRate(DataRate, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Bit rate flag set
	IntToRawData(DataRate, m_WriteData+8);
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

	GetRDataRate(DataRate, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTDataRate(unsigned int &DataRate, int Modulator)
{
	DataRate = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	DataRate = RawDataToInt(m_pDataBytes+8);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetTDataRate(unsigned int &DataRate, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataRate(DataRate, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1;	// Bit rate flag set
	IntToRawData(DataRate, m_WriteData+8);
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	GetTDataRate(DataRate, Modulator);
	return EC;
}

// Data params

//virtual
MC_ErrorCode CDatumPsm500::GetRDataParams(CModemDataParams &Params, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_DataRate = RawDataToInt(m_pDataBytes+8);

	unsigned char ClockSrc = m_pDataBytes[6] >> 4;
	Params.m_ClockSource = ClockSrc & 0x07; // 3 bits only
	
	Params.m_FecMode = ParseReplyForFecMode();
	Params.m_ReedSolomonMode = (m_pDataBytes[6] && 0xF0 ) >> 4; // bits 4,5,6,7

	Params.m_DifferentialCodingMode = ((m_pDataBytes[5] & 1<<6) != 0);
	unsigned char ScramblerField = m_pDataBytes[5];
	ScramblerField &= 0xF0;		// 4 bits
	Params.m_ScramblingMode = ScramblerField >> 4;

	// Doppler buffer
	Params.m_DopplerBufferDelay = RawDataToInt(m_pDataBytes+20);
	Params.m_DopplerBufferSize = RawDataToInt(m_pDataBytes+24);

	// the following three calls set fields to constants
	IsRDataInvEnabled(Params.m_bDataInverted, Demodulator);
	IsRDataClockInvEnabled(Params.m_bClockInverted, Demodulator);
	
	return MC_OK;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTDataParams(CModemDataParams &Params, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	Params.m_DataRate = RawDataToInt(m_pDataBytes+8);

	unsigned char ClockSrc = m_pDataBytes[6] >> 4;
	Params.m_ClockSource = ClockSrc & 0x07; // 3 bits only
	
	Params.m_FecMode = ParseReplyForFecMode();
	Params.m_ReedSolomonMode = (m_pDataBytes[6] && 0xF0 ) >> 4; // bits 4,5,6,7

	Params.m_DifferentialCodingMode = ((m_pDataBytes[5] & 1<<6) != 0);
	unsigned char ScramblerField = m_pDataBytes[5];
	ScramblerField &= 0xF0;		// 4 bits
	Params.m_ScramblingMode = ScramblerField;

	// the following three calls set fields to constants
	IsTDataInvEnabled(Params.m_bDataInverted, Modulator);
	IsTDataClockInvEnabled(Params.m_bClockInverted, Modulator);
	
	return MC_OK;
}


char *CDatumPsm500_LBand::m_pszName="Datum PSM-500L (L-band modem)";

CDatumPsm500_LBand::CDatumPsm500_LBand()
{
}

//virtual
CDatumPsm500_LBand::~CDatumPsm500_LBand()
{
}

// BER-test

//virtual
BOOL CDatumPsm500::CanBerTest(int Power)
{
	return (Power == 0 || Power == 11 || Power == 23);
}

//virtual
MC_ErrorCode CDatumPsm500::SetTBerTest(int &Power, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanBerTest(Power))
		return MC_INVALID_PARAMETERS;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// Mod BER flag set
	switch (Power)
	{
	case 0: m_WriteData[4] = 0; break; // disable
	case 11: m_WriteData[4] = 1<<4; break; // 2^11-1 = 2047
	case 23: m_WriteData[4] = 1<<5; break; // 2^23-1
	}
	int CommandLength = FillCommandBuffer(0xC3, modeExecute, m_WriteData, 14);
	MC_ErrorCode EC = Command(CommandLength);

	GetTBerTest(Power, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetTBerTest(int &Power, int Modulator)
{
	Power = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0xC3, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char BERfield = m_pDataBytes[4] & 0xF0; // 4 bits [4,5,6,7]
	BERfield >>= 4;
	if (EC != MC_OK)
		return EC;

	switch (BERfield)
	{
	case 0: Power = 0; break;
	case 1: Power = 11; break;
	case 2: Power = 23; break;
	default: Power = 0; break;
	}
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::SetRBerTest(int &Power, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanBerTest(Power))
		return MC_INVALID_PARAMETERS;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Dem BER flag set
	switch (Power)
	{
	case 0: m_WriteData[5] = 0; break; // disable
	case 11: m_WriteData[5] = 1<<0; break; // 2^11-1 = 2047
	case 23: m_WriteData[5] = 1<<1; break; // 2^23-1
	}
	int CommandLength = FillCommandBuffer(0xC3, modeExecute, m_WriteData, 14);
	MC_ErrorCode EC = Command(CommandLength);

	GetRBerTest(Power, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetRBerTest(int &Power, int Demodulator)
{
	Power = 0;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0xC3, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char BERfield = m_pDataBytes[5] & 0x0F;
	if (EC != MC_OK)
		return EC;
	switch (BERfield)
	{
	case 0: Power = 0; break;
	case 1: Power = 11; break;
	case 2: Power = 23; break;
	default: Power = 0; break;
	}
	return EC;
}

MC_ErrorCode CDatumPsm500::GetBerTestStatus
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

	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0xC0, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	
	// BER
	short mantissa = RawDataToSignedShort(m_pDataBytes+8);
	BER = mantissa/1000.;
	short exponent = RawDataToSignedShort(m_pDataBytes+10);
	for (int i = 0; i<exponent; i++)
		BER /= 10.;

	// SyncLoss
	SyncLoss = RawDataToInt(m_pDataBytes+12);

	// Error count
	Errors = RawDataToInt(m_pDataBytes+20);
	Errors *= 4294967296.;
	Errors += RawDataToInt(m_pDataBytes+16);

	// Bit count
	TotalBits = RawDataToInt(m_pDataBytes+28);
	TotalBits *= 4294967296.;
	TotalBits += RawDataToInt(m_pDataBytes+24);

	// Error free seconds percentage
	ErrorFreeSecsPercentage = RawDataToSignedShort(m_pDataBytes+32)/100.;

	// Errored seconds
	ErredSecs = RawDataToInt(m_pDataBytes+34);

	// Total test duration
	TotalTestDuration = RawDataToInt(m_pDataBytes+38);
	
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::RestartBerTest(int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int Power = 0, PowerForDisabling = 0;
	MC_ErrorCode EC = GetRBerTest(Power, Demodulator);
	if (EC != MC_OK)
		return EC;
	EC = SetRBerTest(PowerForDisabling, Demodulator);
	if (EC != MC_OK)
		return EC;
	EC = SetRBerTest(Power, Demodulator);
	return EC;
}

//virtual
BOOL CDatumPsm500::Insert1ErrorForBerTest()
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// Mod BER flag set
	m_WriteData[4] = 3<<4;	// Insert 1 error (undocumented (app.B of manual) but works)

	int CommandLength = FillCommandBuffer(0xC3, modeExecute, m_WriteData, 14);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

// Alarms

//virtual
MC_ErrorCode CDatumPsm500::GetUnitAlarms(unsigned int *&pAlarms)
{
	pAlarms = m_UnitAlarms;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x08, modeRead, m_WriteData, 0);
	MC_ErrorCode EC = Command(CommandLength);

	m_UnitAlarms[DATUM500_UNIT_REFERENCE_ALARM] = m_pDataBytes[4] & 0x0F; // Bits 0,1,2,3
	m_UnitAlarms[DATUM500_UNIT_OCXO_OVEN_ALARM] = (m_pDataBytes[4] & 0xF0) >> 4; // Bits 4,5,6,7
	m_UnitAlarms[DATUM500_UNIT_TEST_ACTIVE_ALARM] = m_pDataBytes[5] & 0x03; // Bits 0,1
	m_UnitAlarms[DATUM500_UNIT_HARDWARE_ALARM] = (m_pDataBytes[5] & 0x0C) >> 2; // Bits 2, 3
	m_UnitAlarms[DATUM500_UNIT_BEEPER_ALARM] = (m_pDataBytes[5] & 0x30) >> 4; // Bits 4, 5

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetInterfaceAlarms(unsigned int *&pAlarms)
{
	pAlarms = m_InterfaceAlarms;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0xC2, modeRead, m_WriteData, 0);
	MC_ErrorCode EC = Command(CommandLength);

	m_InterfaceAlarms[DATUM500_INTERFACE_TEST_ALARM] = m_pDataBytes[4] & 0x03; // Bits 0,1
	m_InterfaceAlarms[DATUM500_INTERFACE_SDMS_ALARM] = (m_pDataBytes[4] & 0x30) >> 4; // Bits 4,5
	m_InterfaceAlarms[DATUM500_INTERFACE_BER_ALARM] = (m_pDataBytes[4] & 0x03) >> 2; // Bits 2,3

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetModulatorAlarms(unsigned int *&pAlarms)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x43, modeRead, m_WriteData, 0);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::GetDemodulatorAlarms(unsigned int *&pAlarms)
{
	pAlarms = m_DemodulatorAlarms;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	int CommandLength = FillCommandBuffer(0x83, modeRead, m_WriteData, 0);
	MC_ErrorCode EC = Command(CommandLength);

	m_DemodulatorAlarms[DATUM500_DEMODULATOR_CXR_ALARM] = m_pDataBytes[4] & 0x07; // Bits 0,1,2
	m_DemodulatorAlarms[DATUM500_DEMODULATOR_LVL_ALARM] = ((m_pDataBytes[4] & 0x80) >> 7) | ((m_pDataBytes[4] & 0x01) << 1); // [4] Bit 7 and [5] bit 0
	m_DemodulatorAlarms[DATUM500_DEMODULATOR_EBNO_ALARM] = (m_pDataBytes[4] & 0x60) >> 5; // Bits 5, 6
	m_DemodulatorAlarms[DATUM500_DEMODULATOR_TSTACT_ALARM] = (m_pDataBytes[5] & 0x06) >> 1; // Bits 1, 2
	m_DemodulatorAlarms[DATUM500_DEMODULATOR_HARD_ALARM] = (m_pDataBytes[5] & 0x18) >> 3; // Bits 3,4
	m_DemodulatorAlarms[DATUM500_DEMODULATOR_BCK_ALARM] = (m_pDataBytes[5] & 0x60) >> 5; // Bits 5, 6
	m_DemodulatorAlarms[DATUM500_DEMODULATOR_LNB_ALARM] = ((m_pDataBytes[5] & 0x80) >> 7) | ((m_pDataBytes[6] & 0x01) << 1); // [5] Bit 7 and [6] bit 0

	return EC;
}

// LNB
//virtual
MC_ErrorCode CDatumPsm500::GetLnbStatus(CLnbStatus &Status, int Demodulator)
{
	// Attention! The same code exists for L-band model. Duplicate all the changes for that type too.
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x86, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Status.m_Current = RawDataToSignedShort(m_pDataBytes+12) / 1000.;
	Status.m_CurrentMax = RawDataToSignedShort(m_pDataBytes+14) / 1000.;
	Status.m_CurrentMin = RawDataToSignedShort(m_pDataBytes+16) / 1000.;
	
	return MC_OK;
}

// BUC
//virtual
MC_ErrorCode CDatumPsm500::GetBucStatus(CBucStatus &Status, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x46, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Status.m_Voltage = RawDataToSignedShort(m_pDataBytes+8) / 10.;
	Status.m_Current = RawDataToSignedShort(m_pDataBytes+12) / 100.;
	Status.m_CurrentMax = RawDataToSignedShort(m_pDataBytes+14) / 100.;
	Status.m_CurrentMin = RawDataToSignedShort(m_pDataBytes+16) / 100.;
	
	return MC_OK;
}

//////////////////////////////////////////////////////////////////////
//
// 

static const char *DATUM_M7_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK",
	"OQPSK",
	"8PSK",
	"8QAM",
	"16QAM"
};

static const char *DATUM_M7_FEC_MODES[] = {
	"Viterbi 1/2",
	"Viterbi 3/4",
	"Viterbi 7/8"
};

static const char *DATUM_M7_SCRAMBLER_MODES[] = {
	"Disable",		// 0
	"IESS-308",		// 1
	"IESS-309",		// 2
	"V.35",			// 3
	"Alt V.35",		// 4
	"Intelsat",		// 5
	"Alt Intelsat",	// 6
	"TPC Synchro"	// 7
};

static const char *DATUM_M7_DIFF_CODING_MODES[] = { "Disabled", "Enabled" };

static const char *DATUM_M7_R_CLOCK_SOURCES[] = {"RCV_Clock", "Internal", "External", "Mod_Clock"};
static const char *DATUM_M7_T_CLOCK_SOURCES[] = {"Internal", "TT_Clock", "External", "RCV_Clock"};

static const char *DATUM_M7_R_SWEEP_MODES[] = { "Normal (fast)", "Search" };
static const char *DATUM_M7_R_POWER_SUPPLY_MODES[] = { "None", "13 Volts", "18 Volts" };
static const char *DATUM_M7_T_POWER_SUPPLY_MODES[] = { "None", "24 Volts" };

static const char *DATUM_M7_REED_SOLOMON_MODES[] = { "Disabled", "IESS-308", "IESS-309", "IESS-310", "Custom", "CT220,200" };

CDatum_M7::CDatum_M7()
{
}

//virtual
CDatum_M7::~CDatum_M7()
{
}

unsigned short CRC_CCITT_BYTE(unsigned char byte)
{
	unsigned short crc[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
	};

	return crc[byte];
}

unsigned short CRC_CCITT(unsigned char *pBuffer, size_t len)
{
	unsigned short crcValue=0xFFFF;
	for (int i = 0; i < len; i++)
	{
		unsigned short tmp = crcValue ^ pBuffer[i];
		crcValue = (crcValue >> 8) ^ CRC_CCITT_BYTE(tmp & 0xFF);
	}
	return crcValue;
}

// Modulation type

//virtual
int CDatum_M7::GetRModulationTypeCount()
{
	return sizeof(DATUM_M7_MODULATION_TYPES)/sizeof(DATUM_M7_MODULATION_TYPES[0]);
}

//virtual
const char *CDatum_M7::GetRModulationTypeName(int Type)
{
	return DATUM_M7_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatum_M7::GetRModulationType(int &Type, int Demodulator)
{
	Type = 0;
	//if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING;

	unsigned char bb[] = { 0xA5, 0x01, 0xCC, 0x00, 0x03, 0x00, 0x02, 0x11, 0x00, 0xBB, 0xCC };
	int len = 9;
	unsigned short crc = CRC_CCITT(bb, len);
	memcpy(bb+len, &crc, 2);

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRModulationType(int &Type, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRModulationType(Type, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Modulation flag set
	if (Type == 0)
		m_WriteData[4] = 0;	// BPSK
	else if (Type == 1)
		m_WriteData[4] = 1;	// QPSK
	else if (Type == 2)
		m_WriteData[4] = 2;	// OQPSK
	else if (Type == 3)
		m_WriteData[4] = 3;	// 8PSK
	else if (Type == 4)
		m_WriteData[4] = 4;	// 8QAM
	else if (Type == 5)
		m_WriteData[4] = 6;	// 16QAM (not 5, 5 is not in use doc says)
	m_WriteData[4] <<= 2;

	int CommandLength = FillCommandBuffer(0x81, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	GetRModulationType(Type, Demodulator);
	return EC;
}

//virtual
int CDatum_M7::GetTModulationTypeCount()
{
	return sizeof(DATUM_M7_MODULATION_TYPES)/sizeof(DATUM_PSM4900_MODULATION_TYPES[0]);
}

//virtual
const char *CDatum_M7::GetTModulationTypeName(int Type)
{
	return DATUM_M7_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatum_M7::GetTModulationType(int &Type, int Modulator)
{
	Type = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x41, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ModField = m_pDataBytes[4] >> 2;
	ModField = ModField & 0x07; // 3 bits only
	if (ModField == 0)
		Type = 0; // BPSK
	else if (ModField == 1)
		Type = 1; // QPSK
	else if (ModField == 2)
		Type = 2; // OQPSK
	else if (ModField == 3)
		Type = 3; // 8PSK
	else if (ModField == 4)
		Type = 4; // 8QAM
	else if (ModField == 6)
		Type = 5; // 16QAM
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTModulationType(int &Type, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTModulationType(Type, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<4;	// Modulation flag set
	if (Type == 0)
		m_WriteData[4] = 0;	// BPSK
	else if (Type == 1)
		m_WriteData[4] = 1;	// QPSK
	else if (Type == 2)
		m_WriteData[4] = 2;	// OQPSK
	else if (Type == 3)
		m_WriteData[4] = 3;	// 8PSK
	else if (Type == 4)
		m_WriteData[4] = 4;	// 8QAM
	else if (Type == 5)
		m_WriteData[4] = 6;	// 16QAM (not 5, 5 is not in use doc says)
	m_WriteData[4] <<= 2;

	int CommandLength = FillCommandBuffer(0x41, modeExecute, m_WriteData, 36);
	MC_ErrorCode EC = Command(CommandLength);

	GetTModulationType(Type, Modulator);
	return EC;
}

// FEC
//virtual
int CDatum_M7::GetRFecModesCount()
{
	return (sizeof(DATUM_PSM4900_FEC_MODES)/sizeof(DATUM_PSM4900_FEC_MODES[0]));
}

//virtual
const char *CDatum_M7::GetRFecModeName(int Mode)
{
	return DATUM_PSM4900_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetRFecMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char CodeRateField = m_pDataBytes[5] >> 2;
	CodeRateField = CodeRateField & 0x0F; // 4 bits
	if (EC == MC_OK)
	{
		switch (CodeRateField)
		{
		case 0: Mode = 0; break; // Viterbi 1/2
		case 1: Mode = 1; break; // Viterbi 3/4
		case 2: Mode = 2; break; // Viterbi 7/8
		default: Mode = 0; break; // 
		}
	}
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRFecMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFecMode(Mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// FEC flag set
	switch (Mode)
	{
	case 0: m_WriteData[5] = 0; break; // Viterbi 1/2
	case 1: m_WriteData[5] = 1<<2; break; // Viterbi 3/4
	case 2: m_WriteData[5] = 1<<3; break; // Viterbi 7/8
	}
	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetRFecMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatum_M7::GetTFecModesCount()
{
	return (sizeof(DATUM_PSM4900_FEC_MODES)/sizeof(DATUM_PSM4900_FEC_MODES[0]));
}

//virtual
const char *CDatum_M7::GetTFecModeName(int Mode)
{
	return DATUM_PSM4900_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetTFecMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char CodeRateField = m_pDataBytes[5] >> 2;
	CodeRateField = CodeRateField & 0x0F; // 4 bits
	if (EC == MC_OK)
	{
		switch (CodeRateField)
		{
		case 0: Mode = 0; break; // Viterbi 1/2
		case 1: Mode = 1; break; // Viterbi 3/4
		case 2: Mode = 2; break; // Viterbi 7/8
		default: Mode = 0; break; // None
		}
	}
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTFecMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFecMode(Mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<3;	// FEC mode flag set
	switch (Mode)
	{
	case 0: m_WriteData[5] = 0; break; // Viterbi 1/2
	case 1: m_WriteData[5] = 1<<2; break; // Viterbi 3/4
	case 2: m_WriteData[5] = 1<<3; break; // Viterbi 7/8
	}
	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	GetTFecMode(Mode, Modulator);
	return EC;
}

// Reed-Solomon

//virtual
int CDatum_M7::GetReedSolomonModesCount()
{
	return (sizeof(DATUM_PSM500_REED_SOLOMON_MODES)/sizeof(DATUM_PSM500_REED_SOLOMON_MODES[0]));
}

//virtual
const char *CDatum_M7::GetReedSolomonModeName(int Mode)
{
	return DATUM_PSM500_REED_SOLOMON_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetRReedSolomonMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Mode = (m_pDataBytes[6] && 0xF0 ) >> 4; // bits 4,5,6,7

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::GetTReedSolomonMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;

	Mode = (m_pDataBytes[6] && 0xF0 ) >> 4; // bits 4,5,6,7

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

// Scrambling/Descrambling

//virtual
int CDatum_M7::GetScramblerModesCount()
{
	return sizeof(DATUM_PSM4900_SCRAMBLER_MODES)/sizeof(DATUM_PSM4900_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatum_M7::GetScramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_PSM4900_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetScramblerMode(int &mode, int Modulator)
{
	mode = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	mode = ScramblerField;
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetScramblerMode(int &mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateScramblerMode(mode, Modulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Scrambling flag set
	m_WriteData[6] = (char)mode;

	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	GetScramblerMode(mode, Modulator);
	return EC;
}

//virtual
int CDatum_M7::GetDescramblerModesCount()
{
	return sizeof(DATUM_PSM4900_SCRAMBLER_MODES)/sizeof(DATUM_PSM4900_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatum_M7::GetDescramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_PSM4900_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetDescramblerMode(int &mode, int Demodulator)
{
	mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	mode = ScramblerField;
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetDescramblerMode(int &mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDescramblerMode(mode, Demodulator))
		return MC_OK; // already set

	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Descrambling flag set
	m_WriteData[6] = (char)mode;

	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

	GetDescramblerMode(mode, Demodulator);
	return EC;
}


//////////////////////////////////////////////////////////////////////
//
// CDatum_M7_LBand

char *CDatum_M7_LBand::m_pszName="Datum M7 (L-band modem)";

CDatum_M7_LBand::CDatum_M7_LBand()
{
	unsigned char bb[] = { 0xA5, 0x00, 0x00, 0x03, 0x00, 0x02, 0x11, 0x00 };
	unsigned short crc = CRC_CCITT(bb, sizeof(bb));
}

//virtual
CDatum_M7_LBand::~CDatum_M7_LBand()
{
}

