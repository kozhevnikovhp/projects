#include "stdafx.h"
#include "Datum_M500.h"

static char CR=0x0D;
static char LF=0x0A;
static char szCRLF[]={CR, LF};


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

static const char *DATUM_PSM500_10MHZ_MODES[] = { "Disabled", "Enabled" };

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
int CDatumPsm500::GetR10MHzModesCount()
{
	return sizeof(DATUM_PSM500_10MHZ_MODES)/sizeof(DATUM_PSM500_10MHZ_MODES[0]);
}

//virtual
const char *CDatumPsm500::doGetR10MHzModeName(int mode)
{
	return DATUM_PSM500_10MHZ_MODES[mode];
}

//virtual
MC_ErrorCode CDatumPsm500::doGetR10MHzMode(int &mode, int demodulator)
{
	int CommandLength = FillCommandBuffer(0x86, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = m_pDataBytes[4];
	bits >>= 3;
	bits = bits & 0x07;
	mode = 0;
	if (bits != 0)
		mode = 1;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::doSetR10MHzMode(int &mode, int demodulator)
{
	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 6;	// Output flag set for enable/disable
	if (mode == 1)
		m_WriteData[4] = 1 << 3;	// Enabled
	else
		m_WriteData[4] = 0;			// Disabled

	int CommandLength = FillCommandBuffer(0x86, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

//virtual
int CDatumPsm500::GetT10MHzModesCount()
{
	return sizeof(DATUM_PSM500_10MHZ_MODES)/sizeof(DATUM_PSM500_10MHZ_MODES[0]);
}

//virtual
const char *CDatumPsm500::doGetT10MHzModeName(int mode)
{
	return DATUM_PSM500_10MHZ_MODES[mode];
}

//virtual
MC_ErrorCode CDatumPsm500::doGetT10MHzMode(int &mode, int modulator)
{
	int CommandLength = FillCommandBuffer(0x46, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned int bits = m_pDataBytes[4];
	bits >>= 3;
	bits = bits & 0x07;
	mode = 0;
	if (bits != 0)
		mode = 1;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::doSetT10MHzMode(int &mode, int Modulator)
{
	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[0] = 1 << 6;	// Output flag set for enable/disable
	if (mode == 1)
		m_WriteData[4] = 1 << 3;	// Enabled
	else
		m_WriteData[4] = 0;			// Disabled

	int CommandLength = FillCommandBuffer(0x46, modeExecute, m_WriteData, 34);
	MC_ErrorCode EC = Command(CommandLength);
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

	EC = GetR10MHzMode(Params.m_10MHzSupplyMode, Demodulator);
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

	EC = GetT10MHzMode(Params.m_10MHzSupplyMode, Modulator);
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
const char *CDatumPsm500::doGetScramblerModeName(int mode)
{
	return DATUM_PSM500_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CDatumPsm500::doGetScramblerMode(int &mode, int modulator)
{
	int CommandLength = FillCommandBuffer(0x42, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[5];
	ScramblerField &= 0xF0;		// 4 bits
	mode = ScramblerField >> 4;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::doSetScramblerMode(int &mode, int modulator)
{
	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1<<2;	// Scrambling flag set
	m_WriteData[5] = (char)mode << 4;

	int CommandLength = FillCommandBuffer(0x42, modeExecute, m_WriteData, 32);
	MC_ErrorCode EC = Command(CommandLength);

	return EC;
}

//virtual
int CDatumPsm500::GetDescramblerModesCount()
{
	return sizeof(DATUM_PSM500_SCRAMBLER_MODES)/sizeof(DATUM_PSM500_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatumPsm500::doGetDescramblerModeName(int mode)
{
	return DATUM_PSM500_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CDatumPsm500::doGetDescramblerMode(int &mode, int Demodulator)
{
	int CommandLength = FillCommandBuffer(0x82, modeRead, NULL, 0);
	MC_ErrorCode EC = Command(CommandLength);
	unsigned char ScramblerField = m_pDataBytes[5];
	ScramblerField &= 0xF0;		// 4 bits
	mode = ScramblerField >> 4;
	return EC;
}

//virtual
MC_ErrorCode CDatumPsm500::doSetDescramblerMode(int &mode, int Demodulator)
{
	memset(m_WriteData, 0, sizeof(m_WriteData));
	m_WriteData[1] = 1<<2;	// Descrambling flag set
	m_WriteData[5] = (char)mode << 4;

	int CommandLength = FillCommandBuffer(0x82, modeExecute, m_WriteData, 42);
	MC_ErrorCode EC = Command(CommandLength);

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

	m_UnitAlarms[DATUM_UNIT_REFERENCE_ALARM] = m_pDataBytes[4] & 0x0F; // Bits 0,1,2,3
	m_UnitAlarms[DATUM_UNIT_OCXO_OVEN_ALARM] = (m_pDataBytes[4] & 0xF0) >> 4; // Bits 4,5,6,7
	m_UnitAlarms[DATUM_UNIT_TEST_ACTIVE_ALARM] = m_pDataBytes[5] & 0x03; // Bits 0,1
	m_UnitAlarms[DATUM_UNIT_HARDWARE_ALARM] = (m_pDataBytes[5] & 0x0C) >> 2; // Bits 2, 3
	m_UnitAlarms[DATUM_UNIT_BEEPER_ALARM] = (m_pDataBytes[5] & 0x30) >> 4; // Bits 4, 5

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

	m_InterfaceAlarms[DATUM_INTERFACE_TEST_ALARM] = m_pDataBytes[4] & 0x03; // Bits 0,1
	m_InterfaceAlarms[DATUM_INTERFACE_SDMS_ALARM] = (m_pDataBytes[4] & 0x30) >> 4; // Bits 4,5
	m_InterfaceAlarms[DATUM_INTERFACE_BER_ALARM] = (m_pDataBytes[4] & 0x03) >> 2; // Bits 2,3

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

	m_DemodulatorAlarms[DATUM_DEMODULATOR_CXR_ALARM] = m_pDataBytes[4] & 0x07; // Bits 0,1,2
	m_DemodulatorAlarms[DATUM_DEMODULATOR_LVL_ALARM] = ((m_pDataBytes[4] & 0x80) >> 7) | ((m_pDataBytes[4] & 0x01) << 1); // [4] Bit 7 and [5] bit 0
	m_DemodulatorAlarms[DATUM_DEMODULATOR_EBNO_ALARM] = (m_pDataBytes[4] & 0x60) >> 5; // Bits 5, 6
	m_DemodulatorAlarms[DATUM_DEMODULATOR_TSTACT_ALARM] = (m_pDataBytes[5] & 0x06) >> 1; // Bits 1, 2
	m_DemodulatorAlarms[DATUM_DEMODULATOR_HARD_ALARM] = (m_pDataBytes[5] & 0x18) >> 3; // Bits 3,4
	m_DemodulatorAlarms[DATUM_DEMODULATOR_BCK_ALARM] = (m_pDataBytes[5] & 0x60) >> 5; // Bits 5, 6
	m_DemodulatorAlarms[DATUM_DEMODULATOR_LNB_ALARM] = ((m_pDataBytes[5] & 0x80) >> 7) | ((m_pDataBytes[6] & 0x01) << 1); // [5] Bit 7 and [6] bit 0

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

