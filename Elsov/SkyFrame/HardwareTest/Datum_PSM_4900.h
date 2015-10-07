#ifndef __DATUM_PSM_4900_H_INCLUDED__
#define __DATUM_PSM_4900_H_INCLUDED__

#include "DatumConventionalModem.h"

class CDatumPsm4900 : public CDatumConventionalModem
{
public:
// Construction-destruction
	CDatumPsm4900();
	virtual ~CDatumPsm4900();
	
// Public methods
// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified);
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetUnitName(char *&pszString);
	virtual MC_ErrorCode SetUnitName(char *pszString);
	virtual MC_ErrorCode GetModelName(std::string &strModel);
	virtual MC_ErrorCode GetSerialNo(unsigned int &serialNo);
	virtual MC_ErrorCode GetSoftwareVersion(std::string &strVersion);
	virtual unsigned char GetModeField(unsigned char Mode) { return (unsigned char)0x01; }
	virtual void OnMcConnected();

// Non Volatile RAM

// 10 MHz reference type
	virtual MC_ErrorCode IsR10MHzSupplierEnabled(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode EnableR10MHzSupplier(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode IsT10MHzSupplierEnabled(BOOL &bEnable, int Modulator);
	virtual MC_ErrorCode EnableT10MHzSupplier(BOOL &bEnable, int Modulator);

// DC suppliers
	virtual int GetRPowerSupplyModesCount();
	virtual const char *GetRPowerSupplyModeName(int Mode);
	virtual MC_ErrorCode GetRPowerSupplyMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRPowerSupplyMode(int &Mode, int Demodulator);

	virtual int GetTPowerSupplyModesCount();
	virtual const char *GetTPowerSupplyModeName(int Mode);
	virtual MC_ErrorCode GetTPowerSupplyMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTPowerSupplyMode(int &Mode, int Modulator);

// IF params and status
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);

// Data params
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);
	virtual BOOL CanGetRDataCarrierStatus()  { return TRUE; }

// Frequency (carrier)
	virtual MC_ErrorCode GetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual MC_ErrorCode SetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual MC_ErrorCode GetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode SetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode GetSearchRange(unsigned int &SearchRange, int Demodulator);
	virtual MC_ErrorCode SetSearchRange(unsigned int &SearchRange, int Demodulator);
	
	virtual int GetRSweepModesCount();
	virtual const char *GetRSweepModeName(int Mode);
	virtual MC_ErrorCode GetRSweepMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRSweepMode(int &Mode, int Demodulator);
	
	virtual MC_ErrorCode GetInputLevel(double &InputLevel, int Demodulator);
	virtual MC_ErrorCode GetEbNo(double &EbNo, int Demodulator);
	virtual MC_ErrorCode IsRCarrier(BOOL &bValid, int Demodulator);
	virtual MC_ErrorCode GetDemodulatorOffset(int &Offset, int Demodulator);
	virtual MC_ErrorCode GetModulatorShift(int &Shift, int Modulator);
	virtual MC_ErrorCode SetModulatorShift(int &Shift, int Modulator);
	virtual MC_ErrorCode IsContiniousWaveOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode TurnContiniousWaveOn(BOOL &bOn, int Modulator);

// Demodulator reference generator fine tune
	virtual BOOL CanDemodulatorRefFineTune() { return TRUE; }
	virtual int GetDemodulatorRefFineTuneMinValue() { return -127;}
	virtual int GetDemodulatorRefFineTuneMaxValue() { return +127;}
	virtual MC_ErrorCode GetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator);
	virtual MC_ErrorCode SetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator);

// Doppler Buffer Size
	virtual MC_ErrorCode GetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator);
	virtual MC_ErrorCode SetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator);

// Modulation type
	virtual int GetRModulationTypeCount();
	virtual const char *GetRModulationTypeName(int Type);
	virtual MC_ErrorCode GetRModulationType(int &Type, int Demodulator);
	virtual MC_ErrorCode SetRModulationType(int &Type, int Demodulator);

	virtual int GetTModulationTypeCount();
	virtual const char *GetTModulationTypeName(int Type);
	virtual MC_ErrorCode GetTModulationType(int &Type, int Modulator);
	virtual MC_ErrorCode SetTModulationType(int &Type, int Modulator);
	
// Data clock source
	virtual int GetRDataClockSourcesCount();
	virtual const char *GetRDataClockSourceName(int source);
	virtual int GetTDataClockSourcesCount();
	virtual const char *GetTDataClockSourceName(int source);

	virtual MC_ErrorCode GetTDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetTDataClockSource(int &Source, int Modulator);
	virtual BOOL CanTDataClockInv() { return FALSE; }
	virtual MC_ErrorCode GetRDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetRDataClockSource(int &Source, int Modulator);
	virtual BOOL CanRDataClockInv() { return FALSE; }

// Output
	virtual double GetMinOutputLevel() { return -35.; }
	virtual double GetMaxOutputLevel() { return +5.; }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode GetOutputLevel(double &Level, int Modulator);
	virtual MC_ErrorCode SetOutputLevel(double &Level, int Modulator);
	
// Data rate
	virtual int GetMaxDataRateBPSK() { return 1650000; }
	virtual int GetMinDataRateBPSK() { return 9600; }
	virtual int GetMaxDataRateQPSK() { return 3850000; }
	virtual int GetMinDataRateQPSK() { return 19200; }
	virtual MC_ErrorCode GetRDataRate(unsigned int &DataRate, int Demodulator);
	virtual MC_ErrorCode SetRDataRate(unsigned int &DataRate, int Demodulator);
	virtual MC_ErrorCode GetTDataRate(unsigned int &DataRate, int Modulator);
	virtual MC_ErrorCode SetTDataRate(unsigned int &DataRate, int Modulator);

// BER
	virtual MC_ErrorCode GetBER(double &BER, int Demodulator);
	virtual MC_ErrorCode GetSER(double &BER, int Demodulator);

// BER-test
	virtual BOOL CanBerTest(int Power);
	virtual MC_ErrorCode SetTBerTest(int &Power, int Modulator);
	virtual MC_ErrorCode GetTBerTest(int &Power, int Modulator);
	virtual MC_ErrorCode SetRBerTest(int &Power, int Demodulator);
	virtual MC_ErrorCode GetRBerTest(int &Power, int Demodulator);
	virtual MC_ErrorCode GetBerTestStatus(double &BER, unsigned int &SyncLoss, double &Errors, double &TotalBits, double &ErrorFreeSecsPercentage, unsigned int &ErredSecs, unsigned int &TotalTestDuration);
	virtual MC_ErrorCode RestartBerTest(int Demodulator);
	virtual BOOL CanInsert1ErrorForBerTest() { return TRUE; }
	virtual BOOL Insert1ErrorForBerTest();

// FEC
	virtual int GetRFecModesCount();
	virtual const char *GetRFecModeName(int Mode);
	virtual MC_ErrorCode GetRFecMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRFecMode(int &Mode, int Demodulator);

	virtual int GetTFecModesCount();
	virtual const char *GetTFecModeName(int Mode);
	virtual MC_ErrorCode GetTFecMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTFecMode(int &Mode, int Modulator);

// Reed-Solomon
	virtual const char *GetReedSolomonModeName(int Mode);
	virtual int GetReedSolomonModesCount();

	virtual MC_ErrorCode GetRReedSolomonMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetRReedSolomonMode(int &Mode, int Demodulator) { return MC_COMMAND_NOT_SUPPORTED; }

	virtual MC_ErrorCode GetTReedSolomonMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode SetTReedSolomonMode(int &Mode, int Modulator) { return MC_COMMAND_NOT_SUPPORTED; }

// Differential encoding/decoding
	virtual int GetDiffDecoderModeCount();
	virtual const char *GetDiffDecoderModeName(int Mode);
	virtual MC_ErrorCode GetDiffDecoderMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetDiffDecoderMode(int &Mode, int Demodulator);

	virtual int GetDiffEncoderModeCount();
	virtual const char *GetDiffEncoderModeName(int Mode);
	virtual MC_ErrorCode GetDiffEncoderMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetDiffEncoderMode(int &Mode, int Modulator);

// Scrambling/Descrambling
	virtual int GetScramblerModesCount();
	virtual const char *doGetScramblerModeName(int Mode);
	virtual MC_ErrorCode doGetScramblerMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetScramblerMode(int &mode, int modulator);

	virtual int GetDescramblerModesCount();
	virtual const char *doGetDescramblerModeName(int mode);
	virtual MC_ErrorCode doGetDescramblerMode(int &mode, int demodulator);
	virtual MC_ErrorCode doSetDescramblerMode(int &mode, int demodulator);

// Spectral inversion
	virtual MC_ErrorCode IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode EnableRSpectralInv(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator);
	virtual MC_ErrorCode EnableTSpectralInv(BOOL &bEnabled, int Modulator);

// Data inversion
	virtual BOOL CanRDataInv() { return FALSE; }
//	virtual MC_ErrorCode IsRDataInvEnabled(BOOL &bEnabled, int Demodulator);
//	virtual MC_ErrorCode EnableRDataInv(BOOL &bEnabled, int Demodulator);
	virtual BOOL CanTDataInv() { return FALSE; }
//	virtual MC_ErrorCode IsTDataInvEnabled(BOOL &bEnabled, int Modulator);
//	virtual MC_ErrorCode EnableTDataInv(BOOL &bEnabled, int Modulator);

// LNB status
	virtual MC_ErrorCode GetLnbStatus(CLnbStatus &Status, int Demodulator);
// BUC status
	virtual MC_ErrorCode GetBucStatus(CBucStatus &Status, int Modulator);

// Public members
public:

// Protected methods
protected:

// Protected methods
	unsigned char m_WriteData[168];
};

class CDatumPsm4900_70MHz : public CDatumPsm4900
{
public:
	// Construction-destruction
	CDatumPsm4900_70MHz();
	virtual ~CDatumPsm4900_70MHz();
	
// Public methods

// Overridable methods
// Common device commands
	virtual char *GetName() { return m_pszName; }

// Frequency (carrier)
	virtual unsigned int GetMinRFrequency() { return 50000; } //  70 MHz band
	virtual unsigned int GetMaxRFrequency() { return 90000; } //  70 MHz band
	virtual unsigned int GetMinTFrequency() { return 50000; } //  70 MHz band
	virtual unsigned int GetMaxTFrequency() { return 90000; } //  70 MHz band
	virtual double GetMinComfortLevel() { return -70; }
	virtual double GetMaxComfortLevel() { return -50; }

	static char *m_pszName;
	
// Protected methods
protected:
};

class CDatumPsm4900_140MHz : public CDatumPsm4900
{
public:
	// Construction-destruction
	CDatumPsm4900_140MHz();
	virtual ~CDatumPsm4900_140MHz();
	
// Public methods

// Overridable methods
// Common device commands
	virtual char *GetName() { return m_pszName; }

// Frequency (carrier)
	virtual unsigned int GetMinRFrequency() { return 100000; } //  140 MHz band
	virtual unsigned int GetMaxRFrequency() { return 180000; } //  140 MHz band
	virtual unsigned int GetMinTFrequency() { return 100000; } //  140 MHz band
	virtual unsigned int GetMaxTFrequency() { return 180000; } //  140 MHz band
	virtual double GetMinComfortLevel() { return -70; }
	virtual double GetMaxComfortLevel() { return -50; }

	static char *m_pszName;
	
// Protected methods
protected:
};

class CDatumPsm4900_Hybrid : public CDatumPsm4900
{
public:
	// Construction-destruction
	CDatumPsm4900_Hybrid();
	virtual ~CDatumPsm4900_Hybrid();
	
// Public methods

// Overridable methods
// Common device commands
	virtual char *GetName() { return m_pszName; }

// 10 MHz reference
	virtual BOOL CanR10MHzSupply() { return TRUE; }

// LNB status
	virtual BOOL CanGetLnbStatus() { return TRUE; }

// Frequency (carrier)
	virtual unsigned int GetMinRFrequency() { return 950000; } //  140 MHz band
	virtual unsigned int GetMaxRFrequency() { return 1900000; } //  140 MHz band
	virtual unsigned int GetMinTFrequency() { return 50000; } //  140 MHz band
	virtual unsigned int GetMaxTFrequency() { return 90000; } //  140 MHz band
	virtual double GetMinComfortLevel() { return -70; }
	virtual double GetMaxComfortLevel() { return -50; }

	static char *m_pszName;
	
// Protected methods
protected:
};

class CDatumPsm4900_LBand : public CDatumPsm4900
{
public:
	// Construction-destruction
	CDatumPsm4900_LBand();
	virtual ~CDatumPsm4900_LBand();
	
// Public methods

// Overridable methods
// Common device commands
	virtual char *GetName() { return m_pszName; }

// 10 MHz reference suppliers
	virtual BOOL CanR10MHzSupply() { return TRUE; }
	virtual BOOL CanT10MHzSupply() { return TRUE; }

// IF params
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);

// LNB status
	virtual BOOL CanGetLnbStatus() { return TRUE; }

// BUC status
	virtual BOOL CanGetBucStatus() { return TRUE; }

// Frequency (carrier)
	virtual unsigned int GetMinRFrequency() { return 950000; } //  L-band
	virtual unsigned int GetMaxRFrequency() { return 1750000; } //  L-band
	virtual unsigned int GetMinTFrequency() { return 950000; } //  L-band
	virtual unsigned int GetMaxTFrequency() { return 1750000; } //  L-band
	virtual double GetMinComfortLevel() { return -80; }
	virtual double GetMaxComfortLevel() { return -30; }

	static char *m_pszName;
	
// Protected methods
protected:
};


#endif //__DATUM_PSM_4900_H_INCLUDED__