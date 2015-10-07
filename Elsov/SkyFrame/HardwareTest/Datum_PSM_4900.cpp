#include "stdafx.h"
#include "Datum_PSM_4900.h"

static char CR=0x0D;
static char LF=0x0A;
static char szCRLF[]={CR, LF};


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
	Mode = -1;
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
const char *CDatumPsm4900::doGetScramblerModeName(int Mode)
{
	return DATUM_PSM4900_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::doGetScramblerMode(int &mode, int modulator)
{
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	mode = ScramblerField;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::doSetScramblerMode(int &mode, int modulator)
{
	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Scrambling flag set
	m_WriteData[6] = (char)mode;

	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 20);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

//virtual
int CDatumPsm4900::GetDescramblerModesCount()
{
	return sizeof(DATUM_PSM4900_SCRAMBLER_MODES)/sizeof(DATUM_PSM4900_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatumPsm4900::doGetDescramblerModeName(int mode)
{
	return DATUM_PSM4900_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CDatumPsm4900::doGetDescramblerMode(int &mode, int demodulator)
{
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[6];
	ScramblerField &= 0x0F;		// 4 bits
	mode = ScramblerField;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm4900::doSetDescramblerMode(int &mode, int demodulator)
{
	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1<<5;	// Descrambling flag set
	m_WriteData[6] = (char)mode;

	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 30);
	MC_ErrorCode EC = Command(CommandLength);

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



