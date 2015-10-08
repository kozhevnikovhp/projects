#ifndef __DATUM_H_INCLUDED__
#define __DATUM_H_INCLUDED__

#include "SerialPort.h"
#include "UdpBasedTerminal.h"
#include "Modem.h"


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

enum {
	DATUM_UNIT_REFERENCE_ALARM,
	DATUM_UNIT_OCXO_OVEN_ALARM,
	DATUM_UNIT_TEST_ACTIVE_ALARM,
	DATUM_UNIT_HARDWARE_ALARM,
	DATUM_UNIT_BEEPER_ALARM,
	DATUM_UNIT_ALARM_COUNT
};

enum {
	DATUM_INTERFACE_TEST_ALARM,
	DATUM_INTERFACE_SDMS_ALARM,
	DATUM_INTERFACE_BER_ALARM,
	DATUM_INTERFACE_ALARM_COUNT
};
enum {
	DATUM_MODULATOR_ALARM_COUNT
};
enum {
	DATUM_DEMODULATOR_CXR_ALARM,
	DATUM_DEMODULATOR_LVL_ALARM,
	DATUM_DEMODULATOR_EBNO_ALARM,
	DATUM_DEMODULATOR_TSTACT_ALARM,
	DATUM_DEMODULATOR_HARD_ALARM,
	DATUM_DEMODULATOR_BCK_ALARM,
	DATUM_DEMODULATOR_LNB_ALARM,
	DATUM_DEMODULATOR_ALARM_COUNT
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
	
// Overridable methods
// Common device commands
	virtual CTerminal *GetTerminal();
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanBeControlledByTcpIp() { return TRUE; }
	virtual BOOL CanGetUnitName() { return TRUE; }
	virtual BOOL CanSetUnitName() { return TRUE; }
	virtual void OnMcConnected();
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
	virtual int GetIUnitAlarmCount() const { return DATUM_UNIT_ALARM_COUNT; }
	virtual int GetInterfaceAlarmCount() const { return DATUM_INTERFACE_ALARM_COUNT; }
	virtual int GetModulatorAlarmCount() const { return DATUM_MODULATOR_ALARM_COUNT; }
	virtual int GetDemodulatorAlarmCount() const { return DATUM_DEMODULATOR_ALARM_COUNT; }
	
// Protected methods
protected:

// Protected members
	CSerialPort_Datum m_SerialPort;
	CDatumClientSocket m_TcpIpSocket;

	unsigned char m_ModemAddress; // address of modem on control line. 255 - broadcast 
	unsigned char m_ControllerAddress; // address of controller (computer) on control line (0...254) 
	unsigned int m_ReturnedDataLength;
	unsigned int m_RawReplyLength;
	char m_szVersion[128];
	char m_szUnitName[16];
	unsigned char m_pDataBytes[256];
	unsigned char m_pRawReply[256];
	unsigned char m_pszCommand[256];
	unsigned char m_pszIntValue[4];

	unsigned int m_UnitAlarms[DATUM_UNIT_ALARM_COUNT];
	unsigned int m_InterfaceAlarms[DATUM_INTERFACE_ALARM_COUNT];
	unsigned int m_DemodulatorAlarms[DATUM_DEMODULATOR_ALARM_COUNT];

};

#endif //__DATUM_H_INCLUDED__