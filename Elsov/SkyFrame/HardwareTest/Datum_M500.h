#ifndef __DATUM_M500_H_INCLUDED__
#define __DATUM_M500_H_INCLUDED__

#include "DatumConventionalModem.h"

class CDatumPsm500 : public CDatumConventionalModem
{
public:
	// Construction-destruction
	CDatumPsm500();
	virtual ~CDatumPsm500();
	
// Public methods

// Overridable methods
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetUnitName(char *&pszString);
	virtual MC_ErrorCode SetUnitName(char *pszString);
	virtual MC_ErrorCode GetModelName(std::string &strModel);
	virtual MC_ErrorCode GetSerialNo(unsigned int &serialNo);
	virtual MC_ErrorCode GetSoftwareVersion(std::string &strVersion);
	virtual unsigned char GetModeField(unsigned char Mode) { return (unsigned char)0x05; }

// Non Volatile RAM

// 10 MHz reference type
	virtual int GetR10MHzModesCount();
	virtual const char *doGetR10MHzModeName(int mode);
	virtual MC_ErrorCode doGetR10MHzMode(int &mode, int demodulator);
	virtual MC_ErrorCode doSetR10MHzMode(int &mode, int demodulator);

	virtual int GetT10MHzModesCount();
	virtual const char *doGetT10MHzModeName(int mode);
	virtual MC_ErrorCode doGetT10MHzMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetT10MHzMode(int &mode, int modulator);

// IF params and status
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);

// Frequency (carrier)
	virtual BOOL CanSetModulatorShift() { return TRUE; }

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

// DC Power Supply
	virtual int GetRPowerSupplyModesCount();
	virtual const char *GetRPowerSupplyModeName(int Mode);
	virtual MC_ErrorCode GetRPowerSupplyMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRPowerSupplyMode(int &Mode, int Demodulator);

	virtual int GetTPowerSupplyModesCount();
	virtual const char *GetTPowerSupplyModeName(int Mode);
	virtual MC_ErrorCode GetTPowerSupplyMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTPowerSupplyMode(int &Mode, int Modulator);

// Modulation type
	virtual int GetRModulationTypeCount();
	virtual const char *GetRModulationTypeName(int Type);
	virtual MC_ErrorCode GetRModulationType(int &Type, int Demodulator);
	virtual MC_ErrorCode SetRModulationType(int &Type, int Demodulator);

	virtual int GetTModulationTypeCount();
	virtual const char *GetTModulationTypeName(int Type);
	virtual MC_ErrorCode GetTModulationType(int &Type, int Modulator);
	virtual MC_ErrorCode SetTModulationType(int &Type, int Modulator);
	
// Spectrum mode
	virtual BOOL CanRSpectrumMode() { return TRUE; }
	virtual int GetRSpectrumModesCount();
	virtual const char *doGetRSpectrumModeName(int mode);
	virtual MC_ErrorCode doGetRSpectrumMode(int &mode, int demodulator);
	virtual MC_ErrorCode doSetRSpectrumMode(int &mode, int demodulator);

	virtual BOOL CanTSpectrumMode() { return TRUE; }
	virtual int GetTSpectrumModesCount();
	virtual const char *doGetTSpectrumModeName(int mode);
	virtual MC_ErrorCode doGetTSpectrumMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetTSpectrumMode(int &mode, int modulator);

// Burst mode
	virtual BOOL CanTBurstMode() { return TRUE; }
	virtual int GetTBurstModesCount();
	virtual const char *doGetTBurstModeName(int mode);
	virtual MC_ErrorCode doGetTBurstMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetTBurstMode(int &mode, int modulator);

	virtual MC_ErrorCode doGetTBurstPreambleLength(int &length, int modulator);
	virtual MC_ErrorCode doSetTBurstPreambleLength(int &length, int modulator);

// AUPC mode
	virtual BOOL CanTAupcMode() { return TRUE; }
	virtual int GetTAupcModesCount();
	virtual const char *doGetTAupcModeName(int mode);
	virtual MC_ErrorCode doGetTAupcMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetTAupcMode(int &mode, int modulator);

// Cxr mute mode
	virtual BOOL CanTCxrMuteMode() { return TRUE; }
	virtual int GetTCxrMuteModesCount();
	virtual const char *doGetTCxrMuteModeName(int mode);
	virtual MC_ErrorCode doGetTCxrMuteMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetTCxrMuteMode(int &mode, int modulator);

// IF impedance
	virtual BOOL CanRInputImpedanceMode() { return TRUE; }
	virtual int GetRInputImpedanceModesCount();
	virtual const char *doGetRInputImpedanceModeName(int mode);
	virtual MC_ErrorCode doGetRInputImpedanceMode(int &mode, int demodulator);
	virtual MC_ErrorCode doSetRInputImpedanceMode(int &mode, int demodulator);

	virtual BOOL CanTOutputImpedanceMode() { return TRUE; }
	virtual int GetTOutputImpedanceModesCount();
	virtual const char *doGetTOutputImpedanceModeName(int mode);
	virtual MC_ErrorCode doGetTOutputImpedanceMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetTOutputImpedanceMode(int &mode, int modulator);

// FEC mode
	virtual BOOL CanRFecMode() { return TRUE; }
	virtual int GetRFecModeCount();
	virtual const char *doGetRFecModeName(int mode);
	virtual MC_ErrorCode doGetRFecMode(int &mode, int demodulator);
	virtual MC_ErrorCode doSetRFecMode(int &mode, int demodulator);
	virtual int doGetRFecSnmpValueByMode(int mode);

	virtual BOOL CanTFecMode() { return TRUE; }
	virtual int GetTFecModeCount();
	virtual const char *doGetTFecModeName(int mode);
	virtual MC_ErrorCode doGetTFecMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetTFecMode(int &mode, int modulator);
	virtual int doGetTFecSnmpValueByMode(int mode);

// FEC option
	virtual BOOL CanRFecOption() { return TRUE; }
	virtual int GetRFecOptionCount();
	virtual const char *doGetRFecOptionName(int option);
	virtual MC_ErrorCode doGetRFecOption(int &option, int demodulator);
	virtual MC_ErrorCode doSetRFecOption(int &option, int demodulator);

	virtual BOOL CanTFecOption() { return TRUE; }
	virtual int GetTFecOptionCount();
	virtual const char *doGetTFecOptionName(int option);
	virtual MC_ErrorCode doGetTFecOption(int &option, int modulator);
	virtual MC_ErrorCode doSetTFecOption(int &option, int modulator);

// FEC code rate
	virtual BOOL CanRFecCodeRate() { return TRUE; }
	virtual int GetRFecCodeRateCount();
	virtual const char *doGetRFecCodeRateName(int mode);
	virtual MC_ErrorCode doGetRFecCodeRate(int &mode, int demodulator);
	virtual MC_ErrorCode doSetRFecCodeRate(int &mode, int demodulator);

	virtual BOOL CanTFecCodeRate() { return TRUE; }
	virtual int GetTFecCodeRateCount();
	virtual const char *doGetTFecCodeRateName(int mode);
	virtual MC_ErrorCode doGetTFecCodeRate(int &mode, int modulator);
	virtual MC_ErrorCode doSetTFecCodeRate(int &mode, int modulator);

	// Reed-Solomon
	virtual const char *GetReedSolomonModeName(int Mode);
	virtual int GetReedSolomonModesCount();

	virtual MC_ErrorCode GetRReedSolomonMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRReedSolomonMode(int &Mode, int Demodulator);

	virtual MC_ErrorCode GetTReedSolomonMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTReedSolomonMode(int &Mode, int Modulator);

// Data params
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);
	virtual BOOL CanGetRDataCarrierStatus()  { return TRUE; }

// Differential encoding/decoding
	virtual int GetDiffDecoderModesCount();
	virtual const char *doGetDiffDecoderModeName(int mode);
	virtual MC_ErrorCode doGetDiffDecoderMode(int &mode, int demodulator);
	virtual MC_ErrorCode doSetDiffDecoderMode(int &mode, int demodulator);

	virtual int GetDiffEncoderModesCount();
	virtual const char *doGetDiffEncoderModeName(int mode);
	virtual MC_ErrorCode doGetDiffEncoderMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetDiffEncoderMode(int &mode, int modulator);

// Scrambling/Descrambling
	virtual int GetScramblerModesCount();
	virtual const char *doGetScramblerModeName(int mode);
	virtual MC_ErrorCode doGetScramblerMode(int &mode, int modulator);
	virtual MC_ErrorCode doSetScramblerMode(int &mode, int modulator);
	virtual int doGetScramblerSnmpValueByMode(int mode);

	virtual int GetDescramblerModesCount();
	virtual const char *doGetDescramblerModeName(int Mode);
	virtual MC_ErrorCode doGetDescramblerMode(int &mode, int Demodulator);
	virtual MC_ErrorCode doSetDescramblerMode(int &mode, int Demodulator);
	virtual int doGetDescramblerSnmpValueByMode(int mode);

// Data Rate
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

// Alarms
	virtual MC_ErrorCode GetUnitAlarms(unsigned int *&pAlarms);
	virtual MC_ErrorCode GetInterfaceAlarms(unsigned int *&pAlarms);
	virtual MC_ErrorCode GetModulatorAlarms(unsigned int *&pAlarms);
	virtual MC_ErrorCode GetDemodulatorAlarms(unsigned int *&pAlarms);

// LNB status
	virtual MC_ErrorCode GetLnbStatus(CLnbStatus &Status, int Demodulator);
// BUC status
	virtual MC_ErrorCode GetBucStatus(CBucStatus &Status, int Modulator);

protected:
	int ParseReplyForFecMode() const;
	void FillWriteDataWithFecMode(int mode);

// Protected methods
protected:
	unsigned char m_WriteData[168];
};

class CDatumPsm500_LBand : public CDatumPsm500
{
public:
	CDatumPsm500_LBand();
	virtual ~CDatumPsm500_LBand();

// Common device commands
	virtual char *GetName() { return m_pszName; }

// 10 MHz reference suppliers
	virtual BOOL CanR10MHzSupply() { return TRUE; }
	virtual BOOL CanT10MHzSupply() { return TRUE; }

// Frequency (carrier)
	virtual unsigned int GetMinRFrequency() { return 950000; } //  L-band
	virtual unsigned int GetMaxRFrequency() { return 1900000; } //  L-band
	virtual unsigned int GetMinTFrequency() { return 950000; } //  L-band
	virtual unsigned int GetMaxTFrequency() { return 1750000; } //  L-band
	virtual double GetMinComfortLevel() { return -80; }
	virtual double GetMaxComfortLevel() { return -30; }

// LNB status
	virtual BOOL CanGetLnbStatus() { return TRUE; }

// BUC status
	virtual BOOL CanGetBucStatus() { return TRUE; }

	static char *m_pszName;
};



#endif //__DATUM_M500_H_INCLUDED__