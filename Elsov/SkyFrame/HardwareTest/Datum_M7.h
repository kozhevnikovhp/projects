#ifndef __DATUM_M7_H_INCLUDED__
#define __DATUM_M7_H_INCLUDED__

#include "Datum.h"

class CDatum_M7 : public CDatumModem
{
public:
	// Construction-destruction
	CDatum_M7();
	virtual ~CDatum_M7();
	
// Public methods

// Overridable methods
public:
	virtual MC_ErrorCode ReadReplyUntilPrompt();
/*	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetUnitName(char *&pszString);
	virtual MC_ErrorCode SetUnitName(char *pszString);
	virtual MC_ErrorCode GetModelName(std::string &strModel);
	virtual MC_ErrorCode GetSerialNo(unsigned int &serialNo);
	virtual MC_ErrorCode GetSoftwareVersion(std::string &strVersion);*/

// Non Volatile RAM

// 10 MHz reference type
/*	virtual MC_ErrorCode IsR10MHzSupplierEnabled(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode EnableR10MHzSupplier(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode IsT10MHzSupplierEnabled(BOOL &bEnable, int Modulator);
	virtual MC_ErrorCode EnableT10MHzSupplier(BOOL &bEnable, int Modulator);
*/
// IF params and status
	virtual unsigned char getModulatorSlotNumber(int nModulator) const = 0;
	virtual unsigned char getDemodulatorSlotNumber(int nDemodulator) const = 0;

/*	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);
*/
// Frequency (carrier)
	virtual MC_ErrorCode GetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual MC_ErrorCode SetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual MC_ErrorCode GetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode SetTFrequency(unsigned int &Frequency, int Modulator);

	virtual MC_ErrorCode GetSearchRange(unsigned int &SearchRange, int Demodulator);
	virtual MC_ErrorCode SetSearchRange(unsigned int &SearchRange, int Demodulator);
/*
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
*/

// Demodulator reference generator fine tune
/*	virtual BOOL CanDemodulatorRefFineTune() { return TRUE; }
	virtual int GetDemodulatorRefFineTuneMinValue() { return -127;}
	virtual int GetDemodulatorRefFineTuneMaxValue() { return +127;}
	virtual MC_ErrorCode GetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator);
	virtual MC_ErrorCode SetDemodulatorRefFineTune(int &FineTuneValue, int Demodulator);
*/
// Doppler Buffer Size
//	virtual MC_ErrorCode GetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator);
//	virtual MC_ErrorCode SetDemodulatorDopplerBufferSize(unsigned int &BufferSize, int Demodulator);

// DC Power Supply
/*	virtual int GetRPowerSupplyModesCount();
	virtual const char *GetRPowerSupplyModeName(int Mode);
	virtual MC_ErrorCode GetRPowerSupplyMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRPowerSupplyMode(int &Mode, int Demodulator);

	virtual int GetTPowerSupplyModesCount();
	virtual const char *GetTPowerSupplyModeName(int Mode);
	virtual MC_ErrorCode GetTPowerSupplyMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTPowerSupplyMode(int &Mode, int Modulator);
*/
// Modulation type
	virtual int GetRModulationTypeCount();
	virtual const char *GetRModulationTypeName(int Type);
	virtual MC_ErrorCode GetRModulationType(int &Type, int Demodulator);
	virtual MC_ErrorCode SetRModulationType(int &Type, int Demodulator);

	virtual int GetTModulationTypeCount();
	virtual const char *GetTModulationTypeName(int Type);
	virtual MC_ErrorCode GetTModulationType(int &Type, int Modulator);
	virtual MC_ErrorCode SetTModulationType(int &Type, int Modulator);
	
// Spectral inversion
/*	virtual MC_ErrorCode IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode EnableRSpectralInv(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator);
	virtual MC_ErrorCode EnableTSpectralInv(BOOL &bEnabled, int Modulator);
*/
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

	virtual MC_ErrorCode GetRReedSolomonMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRReedSolomonMode(int &Mode, int Demodulator);

	virtual MC_ErrorCode GetTReedSolomonMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTReedSolomonMode(int &Mode, int Modulator);

// Data params
/*	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);
	virtual BOOL CanGetRDataCarrierStatus()  { return TRUE; }
*/
// Differential encoding/decoding
/*	virtual int GetDiffDecoderModeCount();
	virtual const char *GetDiffDecoderModeName(int Mode);
	virtual MC_ErrorCode GetDiffDecoderMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetDiffDecoderMode(int &Mode, int Demodulator);

	virtual int GetDiffEncoderModeCount();
	virtual const char *GetDiffEncoderModeName(int Mode);
	virtual MC_ErrorCode GetDiffEncoderMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetDiffEncoderMode(int &Mode, int Modulator);
*/
// Scrambling/Descrambling
	virtual int GetScramblerModesCount();
	virtual const char *GetScramblerModeName(int Mode);
	virtual MC_ErrorCode GetScramblerMode(int &mode, int Modulator);
	virtual MC_ErrorCode SetScramblerMode(int &mode, int Modulator);

	virtual int GetDescramblerModesCount();
	virtual const char *GetDescramblerModeName(int Mode);
	virtual MC_ErrorCode GetDescramblerMode(int &mode, int Demodulator);
	virtual MC_ErrorCode SetDescramblerMode(int &mode, int Demodulator);

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
//	virtual MC_ErrorCode GetBER(double &BER, int Demodulator);
//	virtual MC_ErrorCode GetSER(double &BER, int Demodulator);

// Data clock source
/*	virtual int GetRDataClockSourcesCount();
	virtual const char *GetRDataClockSourceName(int source);
	virtual int GetTDataClockSourcesCount();
	virtual const char *GetTDataClockSourceName(int source);

	virtual MC_ErrorCode GetTDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetTDataClockSource(int &Source, int Modulator);
	virtual BOOL CanTDataClockInv() { return FALSE; }
	virtual MC_ErrorCode GetRDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetRDataClockSource(int &Source, int Modulator);
	virtual BOOL CanRDataClockInv() { return FALSE; }
*/
// Output
	virtual double GetMinOutputLevel() { return -35.; }
	virtual double GetMaxOutputLevel() { return +5.; }
/*	virtual MC_ErrorCode IsOutputOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode GetOutputLevel(double &Level, int Modulator);
	virtual MC_ErrorCode SetOutputLevel(double &Level, int Modulator);
*/
// BER-test
/*	virtual BOOL CanBerTest(int Power);
	virtual MC_ErrorCode SetTBerTest(int &Power, int Modulator);
	virtual MC_ErrorCode GetTBerTest(int &Power, int Modulator);
	virtual MC_ErrorCode SetRBerTest(int &Power, int Demodulator);
	virtual MC_ErrorCode GetRBerTest(int &Power, int Demodulator);
	virtual MC_ErrorCode GetBerTestStatus(double &BER, unsigned int &SyncLoss, double &Errors, double &TotalBits, double &ErrorFreeSecsPercentage, unsigned int &ErredSecs, unsigned int &TotalTestDuration);
	virtual MC_ErrorCode RestartBerTest(int Demodulator);
	virtual BOOL CanInsert1ErrorForBerTest() { return TRUE; }
	virtual BOOL Insert1ErrorForBerTest();
*/
// Alarms
/*	virtual MC_ErrorCode GetUnitAlarms(unsigned int *&pAlarms);
	virtual int GetUnitAlarmCount() const { return DATUM500_UNIT_ALARM_COUNT; }
	virtual MC_ErrorCode GetInterfaceAlarms(unsigned int *&pAlarms);
	virtual int GetInterfaceAlarmCount() const { return DATUM500_INTERFACE_ALARM_COUNT; }
	virtual MC_ErrorCode GetModulatorAlarms(unsigned int *&pAlarms);
	virtual int GetModulatorAlarmCount() const { return DATUM500_MODULATOR_ALARM_COUNT; }
	virtual MC_ErrorCode GetDemodulatorAlarms(unsigned int *&pAlarms);
	virtual int GetDemodulatorAlarmCount() const { return DATUM500_DEMODULATOR_ALARM_COUNT; }
*/
// LNB status
//	virtual MC_ErrorCode GetLnbStatus(CLnbStatus &Status, int Demodulator);
// BUC status
//	virtual MC_ErrorCode GetBucStatus(CBucStatus &Status, int Modulator);

protected:
	MC_ErrorCode getUnsignedInt32Param(unsigned char slot, unsigned char param, unsigned int &value);
	MC_ErrorCode setUnsignedInt32Param(unsigned char slot, unsigned char param, unsigned int value);
	MC_ErrorCode getSignedInt32Param(unsigned char slot, unsigned char param, int &value);
	MC_ErrorCode setSignedInt32Param(unsigned char slot, unsigned char param, int value);

	MC_ErrorCode getUnsignedInt8Param(unsigned char slot, unsigned char param, unsigned int &value);
	MC_ErrorCode setUnsignedInt8Param(unsigned char slot, unsigned char param, unsigned int value);
	MC_ErrorCode getSignedInt8Param(unsigned char slot, unsigned char param, int &value);
	MC_ErrorCode setSignedInt8Param(unsigned char slot, unsigned char param, int value);

	unsigned int ModemDataToUnsignedInt(const unsigned char *pModemData);
	unsigned short ModemDataToUnsignedShort(const unsigned char *pModemData);
	unsigned char ModemDataToUnsignedChar(const unsigned char *pModemData);

// Protected methods
protected:
	unsigned char m_WriteData[168];
//	unsigned int m_UnitAlarms[DATUM500_UNIT_ALARM_COUNT];
//	unsigned int m_InterfaceAlarms[DATUM500_INTERFACE_ALARM_COUNT];
//	unsigned int m_DemodulatorAlarms[DATUM500_DEMODULATOR_ALARM_COUNT];
};

class CDatum_M7_LMod_LDem : public CDatum_M7
{
public:
	CDatum_M7_LMod_LDem();
	virtual ~CDatum_M7_LMod_LDem();

	virtual int GetModulatorsNumber() { return 1; }
	virtual int GetDemodulatorsNumber() { return 1; }

	virtual unsigned char getModulatorSlotNumber(int nModulator) const;
	virtual unsigned char getDemodulatorSlotNumber(int nDemodulator) const;

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

#endif //__DATUM_H_INCLUDED__