#ifndef __DATUM_H_INCLUDED__
#define __DATUM_H_INCLUDED__

#include "SerialPort.h"
#include "UdpBasedTerminal.h"
#include "Modem.h"

const unsigned char modeRead = 0x00;
const unsigned char modeExecute = 0x0F;
const unsigned char modeExecuteAndWrite = 0xFF;

const unsigned char PAD = 0xFF; // pad byte
const unsigned char COMMAND_OPEN_FLAG = 0xA5; // open flag for command
const unsigned char REPLY_OPEN_FLAG = 0x5A; // open flag for response
const unsigned char CLOSE_FLAG = 0x96; // close flag for bot command and response


class CSerialPort_Datum : public CSerialPort
{
	virtual char *GetControlString() { return "9600,n,8,1"; }
	virtual unsigned int GetReadTimeout() { return 1000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};


///////////////////////////////////////////////////////////////////////////////////////////////
// CDatumClientSocket class declaration

class CDatumClientSocket : public CUdpBasedTerminal
{
// Public methods
public:

// Public overridables
public:
// Common socket methods

// Public members
public:

// Protected methods
protected:
// Protected overridables
protected:
	virtual unsigned short GetDefaultPort() { return 8890; }
// Protected members
protected:
};


class CDatumModem : public CModem
{
public:
	// Construction-destruction
	CDatumModem();
	virtual ~CDatumModem();
	
// Public methods
	virtual CSerialPort *GetSerialPort() { return &m_SerialPort; }
	void WriteLog(unsigned char *pszString, int length);
	MC_ErrorCode Command(int CommandLength, BOOL bWaitForReply = TRUE);
	virtual unsigned char GetModeField(unsigned char Mode) = 0; // pure virtual - must be overriden
	
// Overridable methods
	virtual int GetModulatorsNumber() { return 1; }
	virtual int GetDemodulatorsNumber() { return 1; }

// Common device commands
	virtual CTerminal *GetTerminal();
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanBeControlledByTcpIp() { return TRUE; }
	virtual BOOL CanGetUnitName() { return TRUE; }
	virtual BOOL CanSetUnitName() { return TRUE; }
	virtual void OnMcConnected();
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return m_pRawReply; }
	virtual unsigned int GetMCReplyDataLength() { return m_RawReplyLength; }
	virtual BOOL HasMCBusAddress() { return TRUE; }
	virtual unsigned int GetMCBusAddress() { return m_ModemAddress; }
	virtual void SetMCBusAddress(unsigned int Address);
	virtual unsigned int GetMinMCBusAddressScanning() { return 0x00; }
	virtual unsigned int GetMaxMCBusAddressScanning() { return 0xFF; }
	virtual BOOL CheckMCBusAddress(unsigned int Address);

// Non Volatile RAM

// 10 MHz reference type
// Frequency (carrier)
	virtual BOOL CanSetRFrequency() { return TRUE; }
	virtual BOOL CanSetTFrequency() { return TRUE; }
	virtual BOOL CanSetModulatorShift() { return TRUE; }
	virtual BOOL CanSetSearchRange() { return TRUE; }
	virtual BOOL CanGetInputLevel() { return TRUE; }
	virtual BOOL CanContiniousWave() { return TRUE; }

// Modulation type
// Data clock source
	virtual BOOL CanSetRDataClockSource() { return TRUE; }
	virtual BOOL CanSetTDataClockSource() { return TRUE; }

// Output
	virtual BOOL CanOutputOnOff() { return TRUE; }
	virtual BOOL CanOutputLevel() { return TRUE; }

// Data rate
	virtual BOOL CanDataRate() { return TRUE; }

// BER
	double ParseBER();
	double ParseSER();

// Differential encoding/decoding
	virtual BOOL CanDiffDecoder() { return TRUE; }
	virtual BOOL CanDiffEncoder() { return TRUE; }

// Scrambling/Descrambling
	virtual BOOL CanDescramble() { return TRUE; }
	virtual BOOL CanScramble() { return TRUE; }

// Spectral inversion
	virtual BOOL CanRSpectralInv() { return TRUE; }
	virtual BOOL CanTSpectralInv() { return TRUE; }

// Data inversion

// Alarms
	
// Protected methods
protected:
	int FillCommandBuffer(unsigned char CommandCode, unsigned char Mode, unsigned char *pcDataBytes, unsigned char nDataBytes);
	unsigned char CheckSum(int Start, int Stop);
	unsigned char CheckSum(unsigned char *pcData, int Start, int Stop);
	unsigned int RawDataToInt(unsigned char *pszRawData);
	short RawDataToSignedShort(unsigned char *pszRawData);
	void IntToRawData(unsigned int IntValue, unsigned char *pszRawData);
	void SignedShortToRawData(short ShortValue, unsigned char *pszRawData);

// Protected members
	CSerialPort_Datum m_SerialPort;
	CDatumClientSocket m_TcpIpSocket;
	unsigned char m_ModemAddress; // address of modem on control line. 255 - broadcast 
	unsigned char m_ControllerAddress; // address of controller (computer) on control line (0...254) 
	unsigned char m_cStatusByte;
	unsigned char m_cErrorByte;
	unsigned char m_cDataBytes;
	unsigned int m_ReturnedDataLength;
	unsigned int m_RawReplyLength;
	char m_szVersion[128];
	char m_szUnitName[16];
	unsigned char m_pDataBytes[256];
	unsigned char m_pRawReply[256];
	unsigned char m_pszCommand[256];
	unsigned char m_pszIntValue[4];
};

class CDatumPsm4900 : public CDatumModem
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
	virtual const char *GetScramblerModeName(int Mode);
	virtual MC_ErrorCode GetScramblerMode(int &mode, int Modulator);
	virtual MC_ErrorCode SetScramblerMode(int &mode, int Modulator);

	virtual int GetDescramblerModesCount();
	virtual const char *GetDescramblerModeName(int Mode);
	virtual MC_ErrorCode GetDescramblerMode(int &mode, int Demodulator);
	virtual MC_ErrorCode SetDescramblerMode(int &mode, int Demodulator);

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

enum {
	DATUM500_UNIT_REFERENCE_ALARM,
	DATUM500_UNIT_OCXO_OVEN_ALARM,
	DATUM500_UNIT_TEST_ACTIVE_ALARM,
	DATUM500_UNIT_HARDWARE_ALARM,
	DATUM500_UNIT_BEEPER_ALARM,
	DATUM500_UNIT_ALARM_COUNT
};
enum {
	DATUM500_INTERFACE_TEST_ALARM,
	DATUM500_INTERFACE_SDMS_ALARM,
	DATUM500_INTERFACE_BER_ALARM,
	DATUM500_INTERFACE_ALARM_COUNT
};
enum {
	DATUM500_MODULATOR_ALARM_COUNT
};
enum {
	DATUM500_DEMODULATOR_CXR_ALARM,
	DATUM500_DEMODULATOR_LVL_ALARM,
	DATUM500_DEMODULATOR_EBNO_ALARM,
	DATUM500_DEMODULATOR_TSTACT_ALARM,
	DATUM500_DEMODULATOR_HARD_ALARM,
	DATUM500_DEMODULATOR_BCK_ALARM,
	DATUM500_DEMODULATOR_LNB_ALARM,
	DATUM500_DEMODULATOR_ALARM_COUNT
};

class CDatumPsm500 : public CDatumModem
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
	virtual MC_ErrorCode IsR10MHzSupplierEnabled(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode EnableR10MHzSupplier(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode IsT10MHzSupplierEnabled(BOOL &bEnable, int Modulator);
	virtual MC_ErrorCode EnableT10MHzSupplier(BOOL &bEnable, int Modulator);

// IF params and status
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);

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
	
// Spectral inversion
	virtual MC_ErrorCode IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode EnableRSpectralInv(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator);
	virtual MC_ErrorCode EnableTSpectralInv(BOOL &bEnabled, int Modulator);

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
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);
	virtual BOOL CanGetRDataCarrierStatus()  { return TRUE; }

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
	virtual int GetUnitAlarmCount() const { return DATUM500_UNIT_ALARM_COUNT; }
	virtual MC_ErrorCode GetInterfaceAlarms(unsigned int *&pAlarms);
	virtual int GetInterfaceAlarmCount() const { return DATUM500_INTERFACE_ALARM_COUNT; }
	virtual MC_ErrorCode GetModulatorAlarms(unsigned int *&pAlarms);
	virtual int GetModulatorAlarmCount() const { return DATUM500_MODULATOR_ALARM_COUNT; }
	virtual MC_ErrorCode GetDemodulatorAlarms(unsigned int *&pAlarms);
	virtual int GetDemodulatorAlarmCount() const { return DATUM500_DEMODULATOR_ALARM_COUNT; }

// LNB status
	virtual MC_ErrorCode GetLnbStatus(CLnbStatus &Status, int Demodulator);
// BUC status
	virtual MC_ErrorCode GetBucStatus(CBucStatus &Status, int Modulator);

protected:
	int ParseReplyForFecMode() const;
	void FillWriteDataWithFecMode(int Mode);

// Protected methods
protected:
	unsigned char m_WriteData[168];
	unsigned int m_UnitAlarms[DATUM500_UNIT_ALARM_COUNT];
	unsigned int m_InterfaceAlarms[DATUM500_INTERFACE_ALARM_COUNT];
	unsigned int m_DemodulatorAlarms[DATUM500_DEMODULATOR_ALARM_COUNT];
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

/*enum {
	DATUM_M7_UNIT_REFERENCE_ALARM,
	DATUM500_UNIT_OCXO_OVEN_ALARM,
	DATUM500_UNIT_TEST_ACTIVE_ALARM,
	DATUM500_UNIT_HARDWARE_ALARM,
	DATUM500_UNIT_BEEPER_ALARM,
	DATUM500_UNIT_ALARM_COUNT
};
enum {
	DATUM500_INTERFACE_TEST_ALARM,
	DATUM500_INTERFACE_SDMS_ALARM,
	DATUM500_INTERFACE_BER_ALARM,
	DATUM500_INTERFACE_ALARM_COUNT
};
enum {
	DATUM500_MODULATOR_ALARM_COUNT
};
enum {
	DATUM500_DEMODULATOR_CXR_ALARM,
	DATUM500_DEMODULATOR_LVL_ALARM,
	DATUM500_DEMODULATOR_EBNO_ALARM,
	DATUM500_DEMODULATOR_TSTACT_ALARM,
	DATUM500_DEMODULATOR_HARD_ALARM,
	DATUM500_DEMODULATOR_BCK_ALARM,
	DATUM500_DEMODULATOR_LNB_ALARM,
	DATUM500_DEMODULATOR_ALARM_COUNT
};*/

class CDatum_M7 : public CDatumModem
{
public:
	// Construction-destruction
	CDatum_M7();
	virtual ~CDatum_M7();
	
// Public methods

// Overridable methods
/*	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetUnitName(char *&pszString);
	virtual MC_ErrorCode SetUnitName(char *pszString);
	virtual MC_ErrorCode GetModelName(std::string &strModel);
	virtual MC_ErrorCode GetSerialNo(unsigned int &serialNo);
	virtual MC_ErrorCode GetSoftwareVersion(std::string &strVersion);*/
	virtual unsigned char GetModeField(unsigned char Mode) { return (unsigned char)0x05; }

// Non Volatile RAM

// 10 MHz reference type
/*	virtual MC_ErrorCode IsR10MHzSupplierEnabled(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode EnableR10MHzSupplier(BOOL &bEnable, int Demodulator);
	virtual MC_ErrorCode IsT10MHzSupplierEnabled(BOOL &bEnable, int Modulator);
	virtual MC_ErrorCode EnableT10MHzSupplier(BOOL &bEnable, int Modulator);
*/
// IF params and status
/*	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);
*/
// Frequency (carrier)
/*	virtual MC_ErrorCode GetRFrequency(unsigned int &Frequency, int Demodulator);
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
/*	virtual MC_ErrorCode GetRDataRate(unsigned int &DataRate, int Demodulator);
	virtual MC_ErrorCode SetRDataRate(unsigned int &DataRate, int Demodulator);
	virtual MC_ErrorCode GetTDataRate(unsigned int &DataRate, int Modulator);
	virtual MC_ErrorCode SetTDataRate(unsigned int &DataRate, int Modulator);
*/
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

// Protected methods
protected:
	unsigned char m_WriteData[168];
//	unsigned int m_UnitAlarms[DATUM500_UNIT_ALARM_COUNT];
//	unsigned int m_InterfaceAlarms[DATUM500_INTERFACE_ALARM_COUNT];
//	unsigned int m_DemodulatorAlarms[DATUM500_DEMODULATOR_ALARM_COUNT];
};

class CDatum_M7_LBand : public CDatum_M7
{
public:
	CDatum_M7_LBand();
	virtual ~CDatum_M7_LBand();

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