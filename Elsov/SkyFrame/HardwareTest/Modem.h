#ifndef __MODEM_H_INCLUDED__
#define __MODEM_H_INCLUDED__

#include "RadioDevice.h"

const int DCS_Unknown = -1; // Data clock source unknown
const int DCS_Internal = 0;
const int DCS_External = 1;

class CIfParams
{
// Construction/destruction
public:
	CIfParams();
	virtual ~CIfParams();
// public members
	unsigned int m_Frequency;
	int m_ModulationType;
	int m_PowerSupplyMode;
	BOOL m_bSpectrumInversion;
	BOOL m_b10MHzSupplyEnabled;
};


class CDemIfParams : public CIfParams
{
// Construction/destruction
public:
	CDemIfParams();
	virtual ~CDemIfParams();
// public members
	unsigned int m_SearchRange;
	unsigned int m_SweepTime;
	double m_LowLevel;
	double m_LowEbno;
};


class CModIfParams : public CIfParams
{
// Construction/destruction
public:
	CModIfParams();
	virtual ~CModIfParams();
// public members
	int m_Offset;
	BOOL m_bOutputEnabled;
	double m_OutputLevel;
	BOOL m_bContiniousWave;
};

class CDemIfStatus
{
// Construction/destruction
public:
	CDemIfStatus();
	virtual ~CDemIfStatus();
// public members
	BOOL m_bCarrier; // RF carrier lock
	BOOL m_bDemLock; // Demodulator (data?) lock
	int m_Offset;
	double m_SN;
	double m_InputLevel;
	double m_BER;
	unsigned int m_Buffer;
};

class CModemDataParams
{
// Construction/destruction
public:
	CModemDataParams();
	virtual ~CModemDataParams();
// public members
	unsigned int m_DataRate;
	int m_ClockSource; // interpreted by each device type itself
	BOOL m_bClockInverted;
	int m_FecMode;
	int m_ReedSolomonMode;

	BOOL m_bDataInverted;
	int m_DifferentialCodingMode;
	int m_ScramblingMode;
	int m_DopplerBufferDelay;
	int m_DopplerBufferSize;
};

class CLnbStatus
{
// Construction/destruction
public:
	CLnbStatus();
	virtual ~CLnbStatus();
// public members
	double m_Current;
	double m_CurrentMin, m_CurrentMax;
	double m_Voltage;
	unsigned int m_LowFrequency;
};

class CBucStatus
{
// Construction/destruction
public:
	CBucStatus();
	virtual ~CBucStatus();
// public members
	double m_Current;
	double m_CurrentMin, m_CurrentMax;
	double m_Voltage;
};


class CAutoSN
{
public:
	CAutoSN()
	{
		ResetAutomatics();
		m_MinSN = 7;
		m_MaxSN = 8;
	}
	virtual ~CAutoSN() {}

// public methods
	void ResetAutomatics()
	{
		memset(m_SN, 0, sizeof(m_SN));
		m_nMeasure = 0;
	}
	void NewMeasure(double SN)
	{
		if (m_nMeasure > 1)
		{
			m_SN[0] = m_SN[1];
			m_SN[1] = SN;
		}
		else
			m_SN[m_nMeasure] = SN;
		++m_nMeasure;
	}
	void SetControlParams(double MinSN, double MaxSN)
	{
		m_MinSN = MinSN;
		m_MaxSN = MaxSN;
	}
	BOOL NeedControl(double &DeltaLevel)
	{
		BOOL bNeed = FALSE;
		DeltaLevel = 0.;
		//if (m_nMeasure <= 1)
		//	return FALSE;
		if (m_SN[0] < m_MinSN /*&& m_SN[1] < m_MinSN */||
			m_SN[0] > m_MaxSN /*&& m_SN[1] > m_MaxSN */)
		{
			bNeed = TRUE;
			DeltaLevel = (m_MinSN+m_MaxSN)/2. - m_SN[0];
			if (DeltaLevel > 3)
				DeltaLevel = 3;
			if (DeltaLevel < -1)
				DeltaLevel = -1;
		}
		else
			ResetAutomatics();
		return bNeed;
	}

// protected members
	double m_SN[2];
	double m_MinSN, m_MaxSN;
	int m_nMeasure;
};

class CModem : public CRadioDevice, public CAutoSN
{
public:
	// Construction-destruction
	CModem();
	virtual ~CModem();
	
	// Public methods

	// Overridable methods
	
// Common device commands
	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { pszString = ""; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { pszString = ""; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL IsModem() { return TRUE; }

	virtual int GetModulatorsNumber() = 0;			// pure virtual method - must be overriden
	virtual int GetDemodulatorsNumber() = 0;		// pure virtual method - must be overriden

// 10 MHz reference suppliers
	virtual BOOL CanR10MHzSupply() { return FALSE; }
	virtual MC_ErrorCode IsR10MHzSupplierEnabled(BOOL &bEnabled, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableR10MHzSupplier(BOOL &bEnabled, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }

	virtual BOOL CanT10MHzSupply() { return FALSE; }
	virtual MC_ErrorCode IsT10MHzSupplierEnabled(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableT10MHzSupplier(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }

// DC suppliers
	virtual int GetRPowerSupplyModesCount()  { return 1; }
	virtual const char *GetRPowerSupplyModeName(int Mode) { return "None"; }
	virtual MC_ErrorCode GetRPowerSupplyMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRPowerSupplyMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRPowerSupplyMode(int Mode, int Demodulator);

	virtual int GetTPowerSupplyModesCount()  { return 1; }
	virtual const char *GetTPowerSupplyModeName(int Mode) { return "None"; }
	virtual MC_ErrorCode GetTPowerSupplyMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTPowerSupplyMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTPowerSupplyMode(int Mode, int Modulator);

// IF params and status
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);

// Data params
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);

// LNB status
	virtual BOOL CanGetLnbStatus() { return FALSE; }
	virtual MC_ErrorCode GetLnbStatus(CLnbStatus &Status, int Demodulator)  { return MC_COMMAND_NOT_SUPPORTED; }

// BUC status
	virtual BOOL CanGetBucStatus() { return FALSE; }
	virtual MC_ErrorCode GetBucStatus(CBucStatus &Status, int Modulator)  { return MC_COMMAND_NOT_SUPPORTED; }

// Frequency (carrier)
	virtual BOOL CanSetRFrequency() { return FALSE; }
	virtual unsigned int GetMinRFrequency() { return 0; }
	virtual unsigned int GetMaxRFrequency() { return 0; }
	virtual MC_ErrorCode GetRFrequency(unsigned int &Frequency, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRFrequency(unsigned int &Frequency, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRFrequency(unsigned int Frequency, int Demodulator);

	virtual BOOL CanSetSearchRange() { return FALSE; }
	virtual MC_ErrorCode GetSearchRange(unsigned int  &SearchRange, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetSearchRange(unsigned int  &SearchRange, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateSearchRange(unsigned int SearchRange, int Demodulator);

	virtual int GetRSweepModesCount()  { return 1; }
	virtual const char *GetRSweepModeName(int Mode) { return "None"; }
	virtual MC_ErrorCode GetRSweepMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRSweepMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRSweepMode(int Mode, int Demodulator);

	virtual BOOL CanGetInputLevel() { return FALSE; }
	virtual MC_ErrorCode GetInputLevel(double &InputLevel, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual double GetMinComfortLevel() = 0; // pure virtual - must be overriden
	virtual double GetMaxComfortLevel() = 0; // pure virtual - must be overriden
	virtual MC_ErrorCode GetEbNo(double &EbNo, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode IsRCarrier(BOOL &bValid, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetDemodulatorOffset(int &Offset, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }

	// Demodulator shift (systematic shift, not current offset value)
	virtual BOOL CanSetDemodulatorShift() { return FALSE; }
	virtual MC_ErrorCode GetDemodulatorShift(int &Shift, int Demodulator) { Shift = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetDemodulatorShift(int &Shift, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateDemodulatorShift(int Shift, int Demodulator);
	// Demodulator reference generator fine tune
	virtual BOOL CanDemodulatorRefFineTune() { return FALSE; }
	virtual int GetDemodulatorRefFineTuneMinValue() { return 0;}
	virtual int GetDemodulatorRefFineTuneMaxValue() { return 0;}
	virtual MC_ErrorCode GetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator) { FineTuneValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateDemodulatorRefFineTune(int FineTuneValue, int Demodulator);
	
	// Modulator shift
	virtual BOOL CanSetModulatorShift() { return FALSE; }
	virtual MC_ErrorCode GetModulatorShift(int &Shift, int Modulator) { Shift = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetModulatorShift(int &Shift, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateModulatorShift(int Shift, int Modulator);
	// Modulator reference generator fine tune
	virtual BOOL CanModulatorRefFineTune() { return FALSE; }
	virtual int GetModulatorRefFineTuneMinValue() { return 0;}
	virtual int GetModulatorRefFineTuneMaxValue() { return 0;}
	virtual MC_ErrorCode GetModulatorRefFineTune(int &FineTuneValue, int Modulator) { FineTuneValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetModulatorRefFineTune(int &FineTuneValue, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateModulatorRefFineTune(int FineTuneValue, int Modulator);

// Doppler Buffer Size
	virtual MC_ErrorCode GetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; } 
	virtual MC_ErrorCode SetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; } 
	virtual BOOL NeedToUpdateDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator);

	virtual BOOL CanSetTFrequency() { return FALSE; }
	virtual unsigned int GetMinTFrequency() { return 0; }
	virtual unsigned int GetMaxTFrequency() { return 0; }
	virtual MC_ErrorCode GetTFrequency(unsigned int &Frequency, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTFrequency(unsigned int &Frequency, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTFrequency(unsigned int Frequency, int Modulator);
	virtual BOOL CanContiniousWave() { return FALSE; }
	virtual MC_ErrorCode IsContiniousWaveOn(BOOL &bOn, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode TurnContiniousWaveOn(BOOL &bOn, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }

// Modulation type
	virtual int GetRModulationTypeCount() = 0;
	virtual const char *GetRModulationTypeName(int Type) = 0;
	virtual MC_ErrorCode GetRModulationType(int &Type, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRModulationType(int &Type, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRModulationType(int ModulationType, int Demodulator);

	virtual int GetTModulationTypeCount() = 0;
	virtual const char *GetTModulationTypeName(int Type) = 0;
	virtual MC_ErrorCode GetTModulationType(int &Type, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTModulationType(int &Type, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTModulationType(int ModulationType, int Modulator);
	
// Data clock
	virtual BOOL CanSetRDataClockSource() { return FALSE; }
	virtual BOOL CanSetTDataClockSource() { return FALSE; }
	virtual int GetRDataClockSourcesCount() { return 2; } // Internal & External
	virtual const char *GetRDataClockSourceName(int source);
	virtual MC_ErrorCode GetRDataClockSource(int &Source, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRDataClockSource(int &Source, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRDataClockSource(int &Source, int Demodulator);
	virtual BOOL CanRDataClockInv() { return FALSE; }
	virtual MC_ErrorCode IsRDataClockInvEnabled(BOOL &bOn, int Demodulator)  { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableRDataClockInv(BOOL &bOn, int Demodulator)  { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRDataClockInv(BOOL bOn, int Demodulator);

	virtual int GetTDataClockSourcesCount() { return 2; } // Internal & External
	virtual const char *GetTDataClockSourceName(int source);
	virtual MC_ErrorCode GetTDataClockSource(int &Source, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTDataClockSource(int &Source, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTDataClockSource(int &Source, int Modulator);
	virtual BOOL CanTDataClockInv() { return FALSE; }
	virtual MC_ErrorCode IsTDataClockInvEnabled(BOOL &bOn, int Modulator)  { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableTDataClockInv(BOOL &bOn, int Modulator)  { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTDataClockInv(BOOL bOn, int Modulator);

// Output
	virtual BOOL CanOutputOnOff() { return FALSE; }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateOutputOn(BOOL &bOn, int Modulator);
	virtual BOOL CanOutputLevel() { return FALSE; }
	virtual double GetMinOutputLevel() = 0;		// pure virtual method - must be overriden
	virtual double GetMaxOutputLevel() = 0;		// pure virtual method - must be overriden
	virtual MC_ErrorCode GetOutputLevel(double &Level, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetOutputLevel(double &Level, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateOutputLevel(double Level, int Modulator);
	MC_ErrorCode ChangeOutputLevel(double DeltaLevel, int Modulator);
	
//Data rate
	virtual BOOL CanDataRate() { return FALSE; }
	virtual int GetMaxDataRateBPSK() = 0;		// pure virtual method - must be overriden
	virtual int GetMinDataRateBPSK() = 0;		// pure virtual method - must be overriden
	virtual int GetMaxDataRateQPSK() = 0;		// pure virtual method - must be overriden
	virtual int GetMinDataRateQPSK() = 0;		// pure virtual method - must be overriden
	virtual MC_ErrorCode GetRDataRate(unsigned int &DataRate, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRDataRate(unsigned int &DataRate, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRDataRate(unsigned int DataRate, int Demodulator);
	virtual MC_ErrorCode GetTDataRate(unsigned int &DataRate, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTDataRate(unsigned int &DataRate, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTDataRate(unsigned int DataRate, int Modulator);

// BER
	virtual MC_ErrorCode GetBER(double &BER, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetSER(double &SER, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }

// BER-test
	virtual BOOL CanBerTest(int Power) { return FALSE; }
	virtual MC_ErrorCode SetTBerTest(int &Power, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetTBerTest(int &Power, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRBerTest(int &Power, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetRBerTest(int &Power, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetBerTestStatus(double &BER, unsigned int &SyncLoss, double &Errors, double &TotalBits, double &ErrorFreeSecsPercentage, unsigned int &ErredSecs, unsigned int &TotalTestDuration) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode RestartBerTest(int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanInsert1ErrorForBerTest() { return FALSE; }
	virtual BOOL Insert1ErrorForBerTest() { return MC_COMMAND_NOT_SUPPORTED; }
	
// FEC
	virtual int GetRFecModesCount() = 0;
	virtual const char *GetRFecModeName(int Mode) = 0;
	virtual MC_ErrorCode GetRFecMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRFecMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRFecMode(int Mode, int Demodulator);

	virtual int GetTFecModesCount() = 0;
	virtual const char *GetTFecModeName(int Mode) = 0;
	virtual MC_ErrorCode GetTFecMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTFecMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTFecMode(int Mode, int Modulator);

// Reed-Solomon
	virtual const char *GetReedSolomonModeName(int Mode) = 0;
	virtual int GetReedSolomonModesCount() = 0;

	virtual MC_ErrorCode GetRReedSolomonMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRReedSolomonMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRReedSolomonMode(int Mode, int Demodulator);

	virtual MC_ErrorCode GetTReedSolomonMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTReedSolomonMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTReedSolomonMode(int Mode, int Modulator);

// Differential encoding/decoding
	virtual BOOL CanDiffDecoder() { return FALSE; }
	virtual int GetDiffDecoderModeCount() { return 0;}
	virtual const char *GetDiffDecoderModeName(int Mode) { return ""; }
	virtual MC_ErrorCode GetDiffDecoderMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetDiffDecoderMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateDiffDecoderMode(int Mode, int Demodulator);

	virtual BOOL CanDiffEncoder() { return FALSE; }
	virtual int GetDiffEncoderModeCount() { return 0;}
	virtual const char *GetDiffEncoderModeName(int Mode) { return ""; }
	virtual MC_ErrorCode GetDiffEncoderMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetDiffEncoderMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateDiffEncoderMode(int Mode, int Modulator);

// Scrambling/Descrambling
	const char *GetDescramblerModeName(int mode);
	MC_ErrorCode GetDescramblerMode(int &mode, int demodulator);
	MC_ErrorCode SetDescramblerMode(int &mode, int demodulator);
	virtual BOOL NeedToUpdateDescramblerMode(int mode, int demodulator);
	virtual BOOL CanDescramble() { return FALSE; }
	virtual int GetDescramblerModesCount() = 0;
	virtual const char *doGetDescramblerModeName(int Mode) = 0;
	virtual MC_ErrorCode doGetDescramblerMode(int &mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode doSetDescramblerMode(int &mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	
	const char *GetScramblerModeName(int mode);
	MC_ErrorCode GetScramblerMode(int &mode, int modulator);
	MC_ErrorCode SetScramblerMode(int &mode, int Modulator);
	virtual BOOL NeedToUpdateScramblerMode(int mode, int Modulator);
	virtual BOOL CanScramble() { return FALSE; }
	virtual int GetScramblerModesCount() = 0;
	virtual const char *doGetScramblerModeName(int mode) = 0;
	virtual MC_ErrorCode doGetScramblerMode(int &mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode doSetScramblerMode(int &mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }

// Spectral inversion
	virtual BOOL CanRSpectralInv() { return FALSE; }
	virtual MC_ErrorCode IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableRSpectralInv(BOOL &bEnabled, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRSpectralInvEnabled(BOOL bEnabled, int Demodulator);
	virtual BOOL CanTSpectralInv() { return FALSE; }
	virtual MC_ErrorCode IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableTSpectralInv(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTSpectralInvEnabled(BOOL bEnabled, int Modulator);

// Data inversion
	virtual BOOL CanRDataInv() { return FALSE; }
	virtual MC_ErrorCode IsRDataInvEnabled(BOOL &bEnabled, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableRDataInv(BOOL &bEnabled, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateRDataInvEnabled(BOOL bEnabled, int Demodulator);
	virtual BOOL CanTDataInv() { return FALSE; }
	virtual MC_ErrorCode IsTDataInvEnabled(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableTDataInv(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL NeedToUpdateTDataInvEnabled(BOOL bEnabled, int Modulator);

// Data interface control (CTS, DTR etc. signals)
	virtual BOOL CanTCts() { return FALSE; }
	virtual MC_ErrorCode IsTCtsEnabled(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableTCts(BOOL &bEnabled, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }

// Alarms
	virtual MC_ErrorCode GetUnitAlarms(unsigned int *&pAlarms) { pAlarms = NULL; return MC_COMMAND_NOT_SUPPORTED; }
	virtual int GetUnitAlarmCount() const { return 0; }
	virtual MC_ErrorCode GetInterfaceAlarms(unsigned int *&pAlarms) { pAlarms = NULL; return MC_COMMAND_NOT_SUPPORTED; }
	virtual int GetInterfaceAlarmCount() const { return 0; }
	virtual MC_ErrorCode GetModulatorAlarms(unsigned int *&pAlarms) { pAlarms = NULL; return MC_COMMAND_NOT_SUPPORTED; }
	virtual int GetModulatorAlarmCount() const { return 0; }
	virtual MC_ErrorCode GetDemodulatorAlarms(unsigned int *&pAlarms) { pAlarms = NULL; return MC_COMMAND_NOT_SUPPORTED; }
	virtual int GetDemodulatorAlarmCount() const { return 0; }

// Protected methods
protected:
};

#endif //__MODEM_H_INCLUDED__