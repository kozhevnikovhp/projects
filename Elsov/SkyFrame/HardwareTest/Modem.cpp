#include "stdafx.h"
#include "Modem.h"

/////////////////////////////////////////////////////////////////////
// CIfParams class implementation

CIfParams::CIfParams()
{
	m_Frequency = 0;
	m_ModulationType = -1;
	m_SpectrumMode = -1;
	m_PowerSupplyMode = -1;
	m_10MHzSupplyMode = -1;
}

//virtual
CIfParams::~CIfParams()
{
}


/////////////////////////////////////////////////////////////////////
// CDemIfParams class implementation

CDemIfParams::CDemIfParams()
{
	m_SearchRange = 0;
}

//virtual
CDemIfParams::~CDemIfParams()
{
}


/////////////////////////////////////////////////////////////////////
// CModIfParams class implementation

CModIfParams::CModIfParams()
{
	m_Offset = 0;
	m_bOutputEnabled = FALSE;
	m_OutputLevel = -1000;
	m_bContiniousWave = FALSE;
}

//virtual
CModIfParams::~CModIfParams()
{
}


/////////////////////////////////////////////////////////////////////
// CDemIfStatus class implementation

CDemIfStatus::CDemIfStatus()
{
	m_bCarrier = FALSE;
	m_InputLevel = -1000;
	m_SN = -1;
	m_Offset = 0;
	m_Buffer = -1;
}

//virtual
CDemIfStatus::~CDemIfStatus()
{
}


/////////////////////////////////////////////////////////////////////
// CDataParams class implementation

CModemDataParams::CModemDataParams()
{
	m_DifferentialCodingMode = 0;
	m_ScramblingMode = 0;
	m_bDataInverted = FALSE;
	m_DataRate = 0;
	m_ClockSource = -1;
	m_bClockInverted = FALSE;
	m_FecMode = -1; // Unknown
	m_FecOption = -1; // Unknown
	m_FecCodeRate = -1; // Unknown
	m_ReedSolomonMode = 0;
	m_DopplerBufferDelay = 0;
	m_DopplerBufferSize = 0;
}

//virtual
CModemDataParams::~CModemDataParams()
{
}


/////////////////////////////////////////////////////////////////////
// CLnbStatus class implementation

CLnbStatus::CLnbStatus()
{
	m_Current = -1;
	m_CurrentMin = m_CurrentMax = -1;
	m_Voltage = -1;
}

//virtual
CLnbStatus::~CLnbStatus()
{
}


/////////////////////////////////////////////////////////////////////
// CBucStatus class implementation

CBucStatus::CBucStatus()
{
	m_Current = -1;
	m_CurrentMin = m_CurrentMax = -1;
	m_Voltage = -1;
}

//virtual
CBucStatus::~CBucStatus()
{
}


/////////////////////////////////////////////////////////////////////
// CModem class implementation

CModem::CModem()
{
}

CModem::~CModem()
{
}

// Common device commands
// 10 MHz reference type

//virtual
MC_ErrorCode CModem::GetRIfParams(CDemIfParams &Params, int Demodulator)
{
	// default implementation calls all methods one after another
	MC_ErrorCode EC = GetRFrequency(Params.m_Frequency, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetR10MHzMode(Params.m_10MHzSupplyMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRModulationType(Params.m_ModulationType, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING; 
	EC = GetRPowerSupplyMode(Params.m_PowerSupplyMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRSpectrumMode(Params.m_SpectrumMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetSearchRange(Params.m_SearchRange, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return MC_OK;
}

//virtual
MC_ErrorCode CModem::GetRIfStatus(CDemIfStatus &Status, int Demodulator)
{
	// default implementation calls all methods one after another
	MC_ErrorCode EC = IsRCarrier(Status.m_bCarrier, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetEbNo(Status.m_SN, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetDemodulatorOffset(Status.m_Offset, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetInputLevel(Status.m_InputLevel, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return MC_OK;
}

//virtual
MC_ErrorCode CModem::GetTIfParams(CModIfParams &Params, int Modulator)
{
	// default implementation calls all methods one after another
	MC_ErrorCode EC = GetTFrequency(Params.m_Frequency, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetModulatorShift(Params.m_Offset, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetT10MHzMode(Params.m_10MHzSupplyMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetTModulationType(Params.m_ModulationType, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING; 
	EC = IsContiniousWaveOn(Params.m_bContiniousWave, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING; 
	EC = GetRPowerSupplyMode(Params.m_PowerSupplyMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetTSpectrumMode(Params.m_SpectrumMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetOutputLevel(Params.m_OutputLevel, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = IsOutputOn(Params.m_bOutputEnabled, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return MC_OK;
}

// Data params
//virtual
MC_ErrorCode CModem::GetRDataParams(CModemDataParams &Params, int Demodulator)
{
	// default implementation calls all methods one after another
	MC_ErrorCode EC = GetRDataRate(Params.m_DataRate, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRDataClockSource(Params.m_ClockSource, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = IsRDataClockInvEnabled(Params.m_bClockInverted, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRFecMode(Params.m_FecMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRFecOption(Params.m_FecOption, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRFecCodeRate(Params.m_FecCodeRate, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetDiffDecoderMode(Params.m_DifferentialCodingMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetDescramblerMode(Params.m_ScramblingMode, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = IsRDataInvEnabled(Params.m_bDataInverted, Demodulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return MC_OK;
}

//virtual
MC_ErrorCode CModem::GetTDataParams(CModemDataParams &Params, int Modulator)
{
	MC_ErrorCode EC = GetTDataRate(Params.m_DataRate, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetTDataClockSource(Params.m_ClockSource, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = IsTDataClockInvEnabled(Params.m_bClockInverted, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetTFecMode(Params.m_FecMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetTFecOption(Params.m_FecOption, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetRFecCodeRate(Params.m_FecCodeRate, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetDiffEncoderMode(Params.m_DifferentialCodingMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = GetScramblerMode(Params.m_ScramblingMode, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;
	EC = IsTDataInvEnabled(Params.m_bDataInverted, Modulator);
	if (EC == MC_DEVICE_NOT_RESPONDING)
		return MC_DEVICE_NOT_RESPONDING;

	return MC_OK;
}

//Frequency

//virtual
BOOL CModem::NeedToUpdateRFrequency(unsigned int Frequency, int Demodulator)
{
	unsigned int CurrentFreq = 0;
	GetRFrequency(CurrentFreq, Demodulator);
	return (CurrentFreq != Frequency);
}

//virtual
BOOL CModem::NeedToUpdateTFrequency(unsigned int Frequency, int Modulator)
{
	unsigned int CurrentFreq = 0;
	GetTFrequency(CurrentFreq, Modulator);
	return (CurrentFreq != Frequency);
}

//virtual
BOOL CModem::NeedToUpdateSearchRange(unsigned int SearchRange, int Demodulator)
{
	unsigned int CurrentValue = 0;
	GetSearchRange(CurrentValue, Demodulator);
	return (CurrentValue != SearchRange);
}

//virtual
BOOL CModem::NeedToUpdateRSweepMode(int Mode, int Demodulator)
{
	int CurrentValue = 0;
	GetRSweepMode(CurrentValue, Demodulator);
	return (CurrentValue != Mode);
}

//virtual
BOOL CModem::NeedToUpdateDemodulatorShift(int Shift, int Demodulator)
{
	int CurrentValue = 0;
	GetDemodulatorShift(CurrentValue, Demodulator);
	return (CurrentValue != Shift);
}

//virtual
BOOL CModem::NeedToUpdateModulatorShift(int Shift, int Modulator)
{
	int CurrentValue = 0;
	GetModulatorShift(CurrentValue, Modulator);
	return (CurrentValue != Shift);
}

//virtual
BOOL CModem::NeedToUpdateDemodulatorRefFineTune(int FineTuneValue, int Demodulator)
{
	int CurrentValue = 0;
	GetDemodulatorRefFineTune(CurrentValue, Demodulator);
	return (CurrentValue != FineTuneValue);
}

//virtual
BOOL CModem::NeedToUpdateModulatorRefFineTune(int FineTuneValue, int Modulator)
{
	int CurrentValue = 0;
	GetModulatorRefFineTune(CurrentValue, Modulator);
	return (CurrentValue != FineTuneValue);
}

//virtual
BOOL CModem::NeedToUpdateDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator)
{
	unsigned int CurrentValue = 0;
	GetDemodulatorDopplerBufferSize(CurrentValue, Demodulator);
	return (CurrentValue != BufferSize);
}

// Power Supply

//virtual
BOOL CModem::NeedToUpdateRPowerSupplyMode(int Mode, int Demodulator)
{
	int CurrentValue = 0;
	GetRPowerSupplyMode(CurrentValue, Demodulator);
	return (CurrentValue != Mode);
}

//virtual
BOOL CModem::NeedToUpdateTPowerSupplyMode(int Mode, int Modulator)
{
	int CurrentValue = 0;
	GetTPowerSupplyMode(CurrentValue, Modulator);
	return (CurrentValue != Mode);
}

// 10 MHz Supply

const char *CModem::GetR10MHzModeName(int mode)
{
	if (mode < 0 || mode >= GetR10MHzModesCount())
		return "";
	return doGetR10MHzModeName(mode);
}

MC_ErrorCode CModem::GetR10MHzMode(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanR10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetR10MHzMode(mode, demodulator);
}

MC_ErrorCode CModem::SetR10MHzMode(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanR10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateR10MHzMode(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetR10MHzMode(mode, demodulator);
	GetR10MHzMode(mode, demodulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateR10MHzMode(int mode, int demodulator)
{
	int current_mode = 0;
	GetR10MHzMode(current_mode, demodulator);
	return (current_mode != mode);
}

const char *CModem::GetT10MHzModeName(int mode)
{
	if (mode < 0 || mode >= GetT10MHzModesCount())
		return "";
	return doGetT10MHzModeName(mode);
}

MC_ErrorCode CModem::GetT10MHzMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanT10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetT10MHzMode(mode, modulator);
}

MC_ErrorCode CModem::SetT10MHzMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanT10MHzSupply())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateT10MHzMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetT10MHzMode(mode, modulator);
	GetT10MHzMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateT10MHzMode(int mode, int modulator)
{
	int current_mode = 0;
	GetT10MHzMode(current_mode, modulator);
	return (current_mode != mode);
}


// Modulation type

//virtual
BOOL CModem::NeedToUpdateRModulationType(int ModulationType, int Demodulator)
{
	int CurrentValue = 0;
	GetRModulationType(CurrentValue, Demodulator);
	return (CurrentValue != ModulationType);
}

//virtual
BOOL CModem::NeedToUpdateTModulationType(int ModulationType, int Demodulator)
{
	int CurrentValue = 0;
	GetTModulationType(CurrentValue, Demodulator);
	return (CurrentValue != ModulationType);
}

// Data clock
static const char *pszClockSource[2] = { "Internal", "External" };
static const char *pszUnknownClockSource = "Unknown";

//virtual
const char *CModem::GetRDataClockSourceName(int source)
{
	if (source < 0 || 
		source >= sizeof(pszClockSource)/sizeof(pszClockSource[0]))
		return pszUnknownClockSource;
	return pszClockSource[source];
}

//virtual
const char *CModem::GetTDataClockSourceName(int source)
{
	if (source < 0 || 
		source >= sizeof(pszClockSource)/sizeof(pszClockSource[0]))
		return pszUnknownClockSource;
	return pszClockSource[source];
}

//virtual
BOOL CModem::NeedToUpdateTDataClockSource(int &Source, int Modulator)
{
	int CurrentValue = -1;
	GetTDataClockSource(CurrentValue, Modulator);
	return CurrentValue != Source;
}

//virtual
BOOL CModem::NeedToUpdateRDataClockSource(int &Source, int Demodulator)
{
	int CurrentValue = -1;
	GetRDataClockSource(CurrentValue, Demodulator);
	return CurrentValue != Source;
}

//virtual
BOOL CModem::NeedToUpdateTDataClockInv(BOOL bOn, int Modulator)
{
	BOOL bCurrentValue = 0;
	IsTDataClockInvEnabled(bCurrentValue, Modulator);
	return bCurrentValue != bOn;
}

//virtual
BOOL CModem::NeedToUpdateRDataClockInv(BOOL bOn, int Demodulator)
{
	BOOL bCurrentValue = 0;
	IsRDataClockInvEnabled(bCurrentValue, Demodulator);
	return bCurrentValue != bOn;
}

// Output

//virtual
BOOL CModem::NeedToUpdateOutputOn(BOOL &bOn, int Modulator)
{
	BOOL bCurrentValue = 0;
	IsOutputOn(bCurrentValue, Modulator);
	return bCurrentValue != bOn;
}

//virtual
BOOL CModem::NeedToUpdateOutputLevel(double Level, int Modulator)
{
	double CurrentValue = 0;
	GetOutputLevel(CurrentValue, Modulator);
	return (int)(CurrentValue*10.) != (int)(Level*10.);
}

MC_ErrorCode CModem::ChangeOutputLevel(double DeltaLevel, int Modulator)
{
	double CurrentLevel;
	MC_ErrorCode EC = GetOutputLevel(CurrentLevel, Modulator);
	if (EC != MC_OK)
		return EC;
	double NewLevel = CurrentLevel + DeltaLevel;
	if (NewLevel < GetMinOutputLevel())
		NewLevel = GetMinOutputLevel();
	if (NewLevel > GetMaxOutputLevel())
		NewLevel = GetMaxOutputLevel();
	EC = SetOutputLevel(NewLevel, Modulator);
	return EC;
}

// DataRate

//virtual
BOOL CModem::NeedToUpdateRDataRate(unsigned int DataRate, int Demodulator)
{
	unsigned int CurrentValue = 0;
	GetRDataRate(CurrentValue, Demodulator);
	return CurrentValue != DataRate;
}

//virtual
BOOL CModem::NeedToUpdateTDataRate(unsigned int DataRate, int Modulator)
{
	unsigned int CurrentValue = 0;
	GetTDataRate(CurrentValue, Modulator);
	return CurrentValue != DataRate;
}

// FEC mode

const char *CModem::GetRFecModeName(int mode)
{
	if (mode < 0 || mode >= GetRFecModeCount())
		return "";
	return doGetRFecModeName(mode);
}

MC_ErrorCode CModem::GetRFecMode(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanRFecMode())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetRFecMode(mode, demodulator);
}

MC_ErrorCode CModem::SetRFecMode(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanRFecMode())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateRFecMode(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetRFecMode(mode, demodulator);
	GetRFecMode(mode, demodulator);

	return EC;
}

int CModem::getRFecSnmpValueByMode(int mode)
{
	if (mode < 0 || mode >= GetRFecModeCount())
		return 0;
	return doGetRFecSnmpValueByMode(mode);
}

int CModem::getRFecModeBySnmpValue(int SnmpValue)
{
	for (int mode = 0; mode < GetRFecModeCount(); mode++)
	{
		if (getRFecSnmpValueByMode(mode) == SnmpValue)
			return mode;
	}
	return -1;
}

//virtual
BOOL CModem::NeedToUpdateRFecMode(int mode, int demodulator)
{
	int currentMode = 0;
	GetRFecMode(currentMode, demodulator);
	return (currentMode != mode);
}

const char *CModem::GetTFecModeName(int mode)
{
	if (mode < 0 || mode >= GetTFecModeCount())
		return "";
	return doGetTFecModeName(mode);
}

MC_ErrorCode CModem::GetTFecMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanTFecMode())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetTFecMode(mode, modulator);
}

MC_ErrorCode CModem::SetTFecMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanTFecMode())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateTFecMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTFecMode(mode, modulator);
	GetTFecMode(mode, modulator);

	return EC;
}

int CModem::getTFecSnmpValueByMode(int mode)
{
	if (mode < 0 || mode >= GetTFecModeCount())
		return 0;
	return doGetTFecSnmpValueByMode(mode);
}

int CModem::getTFecModeBySnmpValue(int SnmpValue)
{
	for (int mode = 0; mode < GetTFecModeCount(); mode++)
	{
		if (getTFecSnmpValueByMode(mode) == SnmpValue)
			return mode;
	}
	return -1;
}

//virtual
BOOL CModem::NeedToUpdateTFecMode(int mode, int modulator)
{
	int currentMode = 0;
	GetTFecMode(currentMode, modulator);
	return (currentMode != mode);
}

// FEC option

const char *CModem::GetRFecOptionName(int option)
{
	if (option < 0 || option >= GetRFecOptionCount())
		return "";
	return doGetRFecOptionName(option);
}

MC_ErrorCode CModem::GetRFecOption(int &option, int demodulator)
{
	option = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanRFecOption())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetRFecOption(option, demodulator);
}

MC_ErrorCode CModem::SetRFecOption(int &option, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanRFecOption())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateRFecOption(option, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetRFecOption(option, demodulator);
	GetRFecOption(option, demodulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateRFecOption(int option, int demodulator)
{
	int currentOption = 0;
	GetRFecOption(currentOption, demodulator);
	return (currentOption != option);
}

const char *CModem::GetTFecOptionName(int option)
{
	if (option < 0 || option >= GetTFecOptionCount())
		return "";
	return doGetTFecOptionName(option);
}

MC_ErrorCode CModem::GetTFecOption(int &option, int modulator)
{
	option = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanTFecOption())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetTFecOption(option, modulator);
}

MC_ErrorCode CModem::SetTFecOption(int &option, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanTFecOption())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateTFecOption(option, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTFecOption(option, modulator);
	GetTFecOption(option, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTFecOption(int option, int modulator)
{
	int currentOption = 0;
	GetTFecOption(currentOption, modulator);
	return (currentOption != option);
}

// FEC code rate

const char *CModem::GetRFecCodeRateName(int mode)
{
	if (mode < 0 || mode >= GetRFecCodeRateCount())
		return "";
	return doGetRFecCodeRateName(mode);
}

MC_ErrorCode CModem::GetRFecCodeRate(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanRFecCodeRate())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetRFecCodeRate(mode, demodulator);
}

MC_ErrorCode CModem::SetRFecCodeRate(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanRFecCodeRate())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateRFecCodeRate(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetRFecCodeRate(mode, demodulator);
	GetRFecCodeRate(mode, demodulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateRFecCodeRate(int mode, int demodulator)
{
	int currentMode = 0;
	GetRFecCodeRate(currentMode, demodulator);
	return (currentMode != mode);
}

const char *CModem::GetTFecCodeRateName(int mode)
{
	if (mode < 0 || mode >= GetTFecCodeRateCount())
		return "";
	return doGetTFecCodeRateName(mode);
}

MC_ErrorCode CModem::GetTFecCodeRate(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanTFecCodeRate())
		return MC_COMMAND_NOT_SUPPORTED;

	return doGetTFecCodeRate(mode, modulator);
}

MC_ErrorCode CModem::SetTFecCodeRate(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!CanTFecCodeRate())
		return MC_COMMAND_NOT_SUPPORTED;
	if (!NeedToUpdateTFecCodeRate(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTFecCodeRate(mode, modulator);
	GetTFecCodeRate(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTFecCodeRate(int mode, int modulator)
{
	int currentMode = 0;
	GetTFecCodeRate(currentMode, modulator);
	return (currentMode != mode);
}

// Reed-Solomon

//virtual
BOOL CModem::NeedToUpdateRReedSolomonMode(int Mode, int Demodulator)
{
	int CurrentMode = 0;
	GetRReedSolomonMode(CurrentMode, Demodulator);
	return (CurrentMode != Mode);
}

//virtual
BOOL CModem::NeedToUpdateTReedSolomonMode(int Mode, int Modulator)
{
	int CurrentMode = 0;
	GetTReedSolomonMode(CurrentMode, Modulator);
	return (CurrentMode != Mode);
}


// Differential decoding

//virtual
const char *CModem::GetDiffDecoderModeName(int mode)
{
	if (mode < 0 || mode >= GetDiffDecoderModesCount())
		return "";
	return doGetDiffDecoderModeName(mode);
}

MC_ErrorCode CModem::GetDiffDecoderMode(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetDiffDecoderMode(mode, demodulator);
}

MC_ErrorCode CModem::SetDiffDecoderMode(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffDecoderMode(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetDiffDecoderMode(mode, demodulator);
	GetDiffDecoderMode(mode, demodulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateDiffDecoderMode(int mode, int demodulator)
{
	int current_mode = 0;
	GetDiffDecoderMode(current_mode, demodulator);
	return (current_mode != mode);
}

// Differential deencoding

//virtual
const char *CModem::GetDiffEncoderModeName(int mode)
{
	if (mode < 0 || mode >= GetDiffEncoderModesCount())
		return "";
	return doGetDiffEncoderModeName(mode);
}

MC_ErrorCode CModem::GetDiffEncoderMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetDiffEncoderMode(mode, modulator);
}

MC_ErrorCode CModem::SetDiffEncoderMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffEncoderMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetDiffEncoderMode(mode, modulator);
	GetDiffEncoderMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateDiffEncoderMode(int mode, int modulator)
{
	int current_mode = 0;
	GetDiffEncoderMode(current_mode, modulator);
	return (current_mode != mode);
}

// Scrambler/decrambler
const char *CModem::GetDescramblerModeName(int mode)
{
	if (mode < 0 || mode >= GetDescramblerModesCount())
		return "";
	return doGetDescramblerModeName(mode);
}

MC_ErrorCode CModem::GetDescramblerMode(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetDescramblerMode(mode, demodulator);
}

MC_ErrorCode CModem::SetDescramblerMode(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDescramblerMode(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetDescramblerMode(mode, demodulator);
	GetDescramblerMode(mode, demodulator);

	return EC;
}

int CModem::getDescramblerSnmpValueByMode(int mode)
{
	if (mode < 0 || mode >= GetDescramblerModesCount())
		return 0;
	return doGetDescramblerSnmpValueByMode(mode);
}

int CModem::getDescramblerModeBySnmpValue(int SnmpValue)
{
	for (int mode = 0; mode < GetDescramblerModesCount(); mode++)
	{
		if (getDescramblerSnmpValueByMode(mode) == SnmpValue)
			return mode;
	}
	return -1;
}

//virtual
BOOL CModem::NeedToUpdateDescramblerMode(int mode, int demodulator)
{
	int current_mode = 0;
	GetDescramblerMode(current_mode, demodulator);
	return (current_mode != mode);
}

const char *CModem::GetScramblerModeName(int mode)
{
	if (mode < 0 || mode >= GetScramblerModesCount())
		return "";
	return doGetScramblerModeName(mode);
}

MC_ErrorCode CModem::GetScramblerMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetScramblerMode(mode, modulator);
}

MC_ErrorCode CModem::SetScramblerMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateScramblerMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetScramblerMode(mode, modulator);
	GetScramblerMode(mode, modulator);

	return EC;
}

int CModem::getScramblerSnmpValueByMode(int mode)
{
	if (mode < 0 || mode >= GetScramblerModesCount())
		return 0;
	return doGetScramblerSnmpValueByMode(mode);
}

int CModem::getScramblerModeBySnmpValue(int SnmpValue)
{
	for (int mode = 0; mode < GetScramblerModesCount(); mode++)
	{
		if (getScramblerSnmpValueByMode(mode) == SnmpValue)
			return mode;
	}
	return -1;
}

//virtual
BOOL CModem::NeedToUpdateScramblerMode(int mode, int Modulator)
{
	int current_mode = 0;
	GetScramblerMode(current_mode, Modulator);
	return (current_mode != mode);
}

// Spectrum modes

//virtual
const char *CModem::GetRSpectrumModeName(int mode)
{
	if (mode < 0 || mode >= GetRSpectrumModesCount())
		return "";
	return doGetRSpectrumModeName(mode);
}

MC_ErrorCode CModem::GetRSpectrumMode(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetRSpectrumMode(mode, demodulator);
}

MC_ErrorCode CModem::SetRSpectrumMode(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSpectrumMode(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetRSpectrumMode(mode, demodulator);
	GetRSpectrumMode(mode, demodulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateRSpectrumMode(int mode, int demodulator)
{
	int current_mode = 0;
	GetRSpectrumMode(current_mode, demodulator);
	return (current_mode != mode);
}

//virtual
const char *CModem::GetTSpectrumModeName(int mode)
{
	if (mode < 0 || mode >= GetTSpectrumModesCount())
		return "";
	return doGetTSpectrumModeName(mode);
}

MC_ErrorCode CModem::GetTSpectrumMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetTSpectrumMode(mode, modulator);
}

MC_ErrorCode CModem::SetTSpectrumMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTSpectrumMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTSpectrumMode(mode, modulator);
	GetTSpectrumMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTSpectrumMode(int mode, int modulator)
{
	int current_mode = 0;
	GetTSpectrumMode(current_mode, modulator);
	return (current_mode != mode);
}

// Burst mode

//virtual
const char *CModem::GetTBurstModeName(int mode)
{
	if (mode < 0 || mode >= GetTBurstModesCount())
		return "";
	return doGetTBurstModeName(mode);
}

MC_ErrorCode CModem::GetTBurstMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetTBurstMode(mode, modulator);
}

MC_ErrorCode CModem::SetTBurstMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTBurstMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTBurstMode(mode, modulator);
	GetTBurstMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTBurstMode(int mode, int modulator)
{
	int current_mode = 0;
	GetTBurstMode(current_mode, modulator);
	return (current_mode != mode);
}

MC_ErrorCode CModem::getTBurstPreambleLength(int &length, int modulator)
{
	length = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetTBurstPreambleLength(length, modulator);
}

MC_ErrorCode CModem::setTBurstPreambleLength(int &length, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTBurstPreambleLength(length, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTBurstPreambleLength(length, modulator);
	getTBurstPreambleLength(length, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTBurstPreambleLength(int length, int modulator)
{
	int currentLength = 0;
	getTBurstPreambleLength(currentLength, modulator);
	return (currentLength != length);
}

// AUPC mode

//virtual
const char *CModem::GetTAupcModeName(int mode)
{
	if (mode < 0 || mode >= GetTAupcModesCount())
		return "";
	return doGetTAupcModeName(mode);
}

MC_ErrorCode CModem::getTAupcMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetTAupcMode(mode, modulator);
}

MC_ErrorCode CModem::setTAupcMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTAupcMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTAupcMode(mode, modulator);
	getTAupcMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTAupcMode(int mode, int modulator)
{
	int currentMode = 0;
	getTAupcMode(currentMode, modulator);
	return (currentMode != mode);
}

// Cxr mute mode

//virtual
const char *CModem::GetTCxrMuteModeName(int mode)
{
	if (mode < 0 || mode >= GetTCxrMuteModesCount())
		return "";
	return doGetTCxrMuteModeName(mode);
}

MC_ErrorCode CModem::getTCxrMuteMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetTCxrMuteMode(mode, modulator);
}

MC_ErrorCode CModem::setTCxrMuteMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTCxrMuteMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTCxrMuteMode(mode, modulator);
	getTCxrMuteMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTCxrMuteMode(int mode, int modulator)
{
	int currentMode = 0;
	getTCxrMuteMode(currentMode, modulator);
	return (currentMode != mode);
}

// IF Impedance

//virtual
const char *CModem::GetRInputImpedanceModeName(int mode)
{
	if (mode < 0 || mode >= GetRInputImpedanceModesCount())
		return "";
	return doGetRInputImpedanceModeName(mode);
}

MC_ErrorCode CModem::GetRInputImpedanceMode(int &mode, int demodulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetRInputImpedanceMode(mode, demodulator);
}

MC_ErrorCode CModem::SetRInputImpedanceMode(int &mode, int demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRInputImpedanceMode(mode, demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetRInputImpedanceMode(mode, demodulator);
	GetRInputImpedanceMode(mode, demodulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateRInputImpedanceMode(int mode, int demodulator)
{
	int current_mode = 0;
	GetRInputImpedanceMode(current_mode, demodulator);
	return (current_mode != mode);
}

//virtual
const char *CModem::GetTOutputImpedanceModeName(int mode)
{
	if (mode < 0 || mode >= GetTOutputImpedanceModesCount())
		return "";
	return doGetTOutputImpedanceModeName(mode);
}

MC_ErrorCode CModem::GetTOutputImpedanceMode(int &mode, int modulator)
{
	mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	return doGetTOutputImpedanceMode(mode, modulator);
}

MC_ErrorCode CModem::SetTOutputImpedanceMode(int &mode, int modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTOutputImpedanceMode(mode, modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = doSetTOutputImpedanceMode(mode, modulator);
	GetTOutputImpedanceMode(mode, modulator);

	return EC;
}

//virtual
BOOL CModem::NeedToUpdateTOutputImpedanceMode(int mode, int modulator)
{
	int current_mode = 0;
	GetTOutputImpedanceMode(current_mode, modulator);
	return (current_mode != mode);
}

// Data inversion

//virtual
BOOL CModem::NeedToUpdateRDataInvEnabled(BOOL bEnabled, int Demodulator)
{
	BOOL bCurrentValue = FALSE;
	IsRDataInvEnabled(bCurrentValue, Demodulator);
	return bCurrentValue != bEnabled;
}

//virtual
BOOL CModem::NeedToUpdateTDataInvEnabled(BOOL bEnabled, int Modulator)
{
	BOOL bCurrentValue = FALSE;
	IsTDataInvEnabled(bCurrentValue, Modulator);
	return bCurrentValue != bEnabled;
}
