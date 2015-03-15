/*	Radyne.h - header file for Radyne modems interfaces

	Author: Evgeny A. Kozhevnikov, Zelenograd, Moscow, Russia evgeny@kozhevnikov.org

Revision history:
	20/Aug/2003 - initial creation
	22/Aug/2003 - remote Tcp/Ip control added
	22/Sep/2006	- M&C bus address can be changed (plus scanning capability)
*/

#ifndef __RADYNE_H_INCLUDED__
#define __RADYNE_H_INCLUDED__

#include "SerialPort.h"
#include "UdpBasedTerminal.h"
#include "Modem.h"

// Forward declarations

class CSerialPort_Radyne : public CSerialPort
{
	virtual char *GetControlString() { return "19200,n,8,1"; }
	virtual unsigned int GetReadTimeout() { return 1000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};


///////////////////////////////////////////////////////////////////////////////////////////////
// CRadyneClientSocket class declaration

class CRadyneClientSocket : public CUdpBasedTerminal
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
	virtual unsigned short GetDefaultPort() { return 8888; }
// Protected members
protected:
};


///////////////////////////////////////////////////////////////////////////////////////////////
// CRadyneModem class declaration

class CRadyneModem : public CModem
{
public:
	// Construction-destruction
	CRadyneModem();
	virtual ~CRadyneModem();
	
// Public methods
	void WriteLog(unsigned char *pszString, int length);
	MC_ErrorCode Command(int CommandLength);
	
// Overridable methods
	virtual int GetModulatorsNumber() { return 1; }
	virtual int GetDemodulatorsNumber() { return 1; }

// Common device commands
	virtual CTerminal *GetTerminal();
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanBeControlledByTcpIp() { return TRUE; }
	virtual void OnMcConnected();
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return m_pszReply; }
	virtual unsigned int GetMCReplyDataLength() { return m_MCReplyLength; }
	virtual BOOL HasMCBusAddress() { return TRUE; }
	virtual unsigned int GetMCBusAddress() { return m_ModemAddress; }
	virtual void SetMCBusAddress(unsigned int Address);
	virtual unsigned int GetMinMCBusAddressScanning() { return 0x20; }
	virtual unsigned int GetMaxMCBusAddressScanning() { return 0xFF; }
	virtual BOOL CheckMCBusAddress(unsigned int Address);


// Non Volatile RAM

// 10 MHz reference type
// Frequency (carrier)
// Modulation type
// Data clock source
// Output
// Data rate
// BER
// FEC
// Differential encoding/decoding
// Scrambling/Descrambling
// Spectral inversion
// Data inversion

// Protected methods
protected:
	int FillCommandBuffer(unsigned short CommandCode, unsigned char *pcDataBytes, unsigned short nDataBytes);
	unsigned char CheckSum(unsigned char *pData, int Count);
	unsigned int RawDataToInt(unsigned char *pszRawData);
	short RawDataToSignedShort(unsigned char *pszRawData);
	void IntToRawData(unsigned int IntValue, unsigned char *pszRawData);
	void SignedShortToRawData(short ShortValue, unsigned char *pszRawData);

// Protected members
	CSerialPort_Radyne m_SerialPort;
	CRadyneClientSocket m_TcpIpSocket;
	unsigned char m_ModemAddress; // address of modem on control line. 255 - broadcast 
	unsigned char m_ControllerAddress; // address of controller (computer) on control line (0...254) 
	char m_szVersion[128];
	char m_szUnitName[16];
	unsigned int m_MCReplyLength;
	unsigned char m_pszReply[256];
	unsigned char m_pszCommand[256];
	unsigned char m_pszIntValue[4];
	unsigned char m_pszShortValue[2];
	unsigned char m_FSN; // frame sequence number
};

class CRadyneDMD2401 : public CRadyneModem
{
public:
// Construction-destruction
	CRadyneDMD2401();
	virtual ~CRadyneDMD2401();
	
// Public methods
// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified);
	virtual char *GetName() { return m_pszName; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { return GetDeviceVersionString(pszString); }
	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { return GetDeviceVersionString(pszString); }

// Non Volatile RAM
	virtual BOOL CanControlParamsSavingInNvRamRightAfterSetting() { return TRUE; }

// 10 MHz reference type

// IF params and status
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);

// Data params
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);

// Frequency (carrier)
	virtual BOOL CanSetRFrequency() { return TRUE; }
	virtual unsigned int GetMinRFrequency() { return 100000; } //  140 MHz band
	virtual unsigned int GetMaxRFrequency() { return 180000; } //  140 MHz band
	virtual MC_ErrorCode GetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual MC_ErrorCode SetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual BOOL CanSetTFrequency() { return TRUE; }
	virtual unsigned int GetMinTFrequency() { return 100000; } //  140 MHz band
	virtual unsigned int GetMaxTFrequency() { return 180000; } //  140 MHz band
	virtual MC_ErrorCode GetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode SetTFrequency(unsigned int &Frequency, int Modulator);
	virtual BOOL CanSetSearchRange() { return TRUE; }
	virtual MC_ErrorCode GetSearchRange(unsigned int &SearchRange, int Demodulator);
	virtual MC_ErrorCode SetSearchRange(unsigned int &SearchRange, int Demodulator);
	virtual MC_ErrorCode GetEbNo(double &EbNo, int Demodulator);
	virtual MC_ErrorCode IsRCarrier(BOOL &bValid, int Demodulator);
	virtual MC_ErrorCode GetDemodulatorOffset(int &Offset, int Demodulator);
	virtual MC_ErrorCode SetDemodulatorOffset(int &Offset, int Demodulator);
	virtual BOOL CanGetInputLevel() { return TRUE; }
	virtual MC_ErrorCode GetInputLevel(double &InputLevel, int Demodulator);
	virtual double GetMinComfortLevel() { return -60; }
	virtual double GetMaxComfortLevel() { return -20; }
	virtual BOOL CanContiniousWave() { return TRUE; }
	virtual MC_ErrorCode IsContiniousWaveOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode TurnContiniousWaveOn(BOOL &bOn, int Modulator);

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
	virtual BOOL CanSetRDataClockSource() { return TRUE; }
	virtual BOOL CanSetTDataClockSource() { return TRUE; }
	virtual int GetRDataClockSourcesCount();
	virtual const char *GetRDataClockSourceName(int source);
	virtual int GetTDataClockSourcesCount();
	virtual const char *GetTDataClockSourceName(int source);

	virtual MC_ErrorCode GetTDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetTDataClockSource(int &Source, int Modulator);
	virtual BOOL CanTDataClockInv() { return TRUE; }
	virtual MC_ErrorCode IsTDataClockInvEnabled(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode EnableTDataClockInv(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode GetRDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetRDataClockSource(int &Source, int Modulator);
	virtual BOOL CanRDataClockInv() { return TRUE; }
	virtual MC_ErrorCode IsRDataClockInvEnabled(BOOL &bOn, int Demodulator);
	virtual MC_ErrorCode EnableRDataClockInv(BOOL &bOn, int Demodulator);

// Output
	virtual BOOL CanOutputOnOff() { return TRUE; }
	virtual BOOL CanOutputLevel() { return TRUE; }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn, int Modulator);
	virtual double GetMinOutputLevel() { return -30.; }
	virtual double GetMaxOutputLevel() { return -5.; }
	virtual MC_ErrorCode GetOutputLevel(double &Level, int Modulator);
	virtual MC_ErrorCode SetOutputLevel(double &Level, int Modulator);
	
// Data rate
	virtual BOOL CanDataRate() { return TRUE; }
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

// BER-test
	virtual BOOL CanBerTest(int Power);
	virtual MC_ErrorCode SetTBerTest(int &Power, int Modulator);
	virtual MC_ErrorCode GetTBerTest(int &Power, int Modulator);
	virtual MC_ErrorCode SetRBerTest(int &Power, int Demodulator);
	virtual MC_ErrorCode GetRBerTest(int &Power, int Demodulator);
	virtual MC_ErrorCode GetBerTestStatus(double &BER, unsigned int &SyncLoss, double &Errors, double &TotalBits, double &ErrorFreeSecsPercentage, unsigned int &ErredSecs, unsigned int &TotalTestDuration);
	virtual MC_ErrorCode RestartBerTest(int Demodulator);
	virtual BOOL CanInsert1ErrorForBerTest() { return FALSE; }
// FEC
	virtual int GetRFecModesCount();
	virtual const char *GetRFecModeName(int Mode);
	virtual MC_ErrorCode GetRFecMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRFecMode(int &Mode, int Demodulator);

	virtual int GetTFecModesCount();
	virtual const char *GetTFecModeName(int Mode);
	virtual MC_ErrorCode GetTFecMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTFecMode(int &Mode, int Modulator);

// Differential encoding/decoding
	virtual BOOL CanDiffDecoder() { return TRUE; }
	virtual int GetDiffDecoderModeCount();
	virtual const char *GetDiffDecoderModeName(int Mode);
	virtual MC_ErrorCode GetDiffDecoderMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetDiffDecoderMode(int &Mode, int Demodulator);

	virtual BOOL CanDiffEncoder() { return TRUE; }
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
	virtual BOOL CanRSpectralInv() { return TRUE; }
	virtual MC_ErrorCode IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode EnableRSpectralInv(BOOL &bEnabled, int Demodulator);
	virtual BOOL CanTSpectralInv() { return TRUE; }
	virtual MC_ErrorCode IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator);
	virtual MC_ErrorCode EnableTSpectralInv(BOOL &bEnabled, int Modulator);

// Data inversion
	virtual BOOL CanRDataInv() { return TRUE; }
	virtual MC_ErrorCode IsRDataInvEnabled(BOOL &bEnabled, int Demodulator);
	virtual MC_ErrorCode EnableRDataInv(BOOL &bEnabled, int Demodulator);
	virtual BOOL CanTDataInv() { return TRUE; }
	virtual MC_ErrorCode IsTDataInvEnabled(BOOL &bEnabled, int Modulator);
	virtual MC_ErrorCode EnableTDataInv(BOOL &bEnabled, int Modulator);

// Public members
public:
	static char *m_pszName;

	// Protected methods
protected:
	int CalcCombinedTClockSource(unsigned char cControl, unsigned char cSource);
	int CalcCombinedRClockSource(unsigned char cControl);
};

#endif // __RADYNE_H_INCLUDED__