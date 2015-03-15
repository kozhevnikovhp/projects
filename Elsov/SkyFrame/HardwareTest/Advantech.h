#ifndef __ADVANTECH_H_INCLUDED__
#define __ADVANTECH_H_INCLUDED__

#include "SerialPort.h"
#include "Tranceiver.h"

const unsigned char REQ = 0x01;		// request status
const unsigned char ON_OFF = 0x02;		// transmit on/off
const unsigned char CHG_ADDR = 0x03;	// change M&C address
const unsigned char FREQ = 0x04;	// Carrier frequency


class CSerialPort_Advantech : public CSerialPort
{
	virtual char *GetControlString() { return "19200,n,8,1,x"; }
	virtual unsigned int GetReadTimeout() { return 1000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};


class CAdvantechTranceiver : public CTranceiver
{
// Construction/destruction
public:
	CAdvantechTranceiver();
	virtual ~CAdvantechTranceiver();
// Public methods
	MC_ErrorCode Command();

// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified);
	virtual CTerminal *GetTerminal() { return &m_SerialPort; }
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned int GetMCReplyDataLength() { return m_MCReplyLength; }
	virtual unsigned char *GetMCReplyData() { return m_pszReply; }

// Non Volatile RAM

// Temperature
	virtual BOOL CanGetTemperature() { return TRUE; }
	virtual MC_ErrorCode IsTemperatureAlarm(BOOL &bAlarm);
	virtual MC_ErrorCode GetTemperature(double &T);
// Output power and mute
	virtual BOOL CanOutputOnOff() { return TRUE; }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn);
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn);
	virtual double GetMinPower() { return 0.; }
	virtual double GetMaxPower() { return 100.; }
	virtual BOOL CanGetPower() { return TRUE; }
	virtual MC_ErrorCode GetPower(double &Power);
// Frequency
	virtual BOOL CanGetFrequency() { return TRUE; }
	virtual MC_ErrorCode GetFrequency(unsigned int &Frequency);
	virtual BOOL CanSetFrequency() { return TRUE; }
	virtual MC_ErrorCode SetFrequency(unsigned int &Frequency);
	
// Protected methods
protected:
	void FillCommandBuffer(unsigned char CommandCode, unsigned char *pcDataBytes);
	unsigned char CheckSum(int Start, int Stop);
	short RawValueToShort(unsigned char *pszRawValue);
// Protected members
	CSerialPort_Advantech m_SerialPort;
	unsigned char m_pszReply[7];
	unsigned char m_pszCommand[7];
	unsigned int m_Frequency;
	unsigned int m_MCReplyLength;
};

class CAdvantechWty1 : public CAdvantechTranceiver
{
// Construction/destruction
public:
	CAdvantechWty1();
	virtual ~CAdvantechWty1();
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }

// Common device commands
// Non Volatile RAM
// Frequency (carrier)

// Protected methods
protected:
// Public members
public:
	static char *m_pszName;
protected:
// Protected members
};

#endif // __ADVANTECH_H_INCLUDED__