/*	AgilentPowerMeter.h: Monitoring and Control of Agilent power meters

	Author: Evgeny A. Kozhevnikov Moscow, Russia  evgeny@kozhevnikov.org
	
	Revision history:
		05 November 2003 - initial creation
*/

#ifndef __AGILENT_POWER_METER_H_INCLUDED__
#define __AGILENT_POWER_METER_H_INCLUDED__

#include "PowerMeter.h"
#include "FrequencyCounter.h"
#include "SerialPort.h"


/////////////////////////////////////////////////////////////////////////////////
class CSerialPort_AgilentPowerMeter : public CSerialPort
{
	virtual char *GetControlString() { return "9600,n,8,1,x"; }
	virtual unsigned int GetReadTimeout() { return 10000; }
	virtual unsigned int GetWriteTimeout() { return 2000; }
};


/////////////////////////////////////////////////////////////////////////////////
class CAgilentPowerMeter : public CPowerMeter
{
// Construction/destruction
public:
	CAgilentPowerMeter();
	virtual ~CAgilentPowerMeter();
// Public methods
	MC_ErrorCode Command(int CommandLength);

// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified);
	virtual CTerminal *GetTerminal() { return &m_SerialPort; }
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanAsciiControl() { return TRUE; }
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return (unsigned char *)m_pszReply; }
	virtual unsigned int GetMCReplyDataLength() { return m_MCReplyLength; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual void OnMcConnected();
	virtual void OnMcDisconnected();

// Non Volatile RAM
// Power meter methods
	virtual MC_ErrorCode ResetMeasurements();
	virtual MC_ErrorCode MeasurePower(double &Power, int Channel);
	
// Protected methods
protected:

// Protected members
protected:
	CSerialPort_AgilentPowerMeter m_SerialPort;
	char m_pszReply[128];
	char m_pszCommand[128];
	unsigned int m_MCReplyLength;
};


/////////////////////////////////////////////////////////////////////////////////
class CSerialPort_AgilentFrequencyCounter : public CSerialPort
{
	virtual char *GetControlString() { return "9600,n,8,1,x"; }
	virtual unsigned int GetReadTimeout() { return 10000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};


/////////////////////////////////////////////////////////////////////////////////
class CAgilentFrequencyCounter : public CFrequencyCounter
{
// Construction/destruction
public:
	CAgilentFrequencyCounter();
	virtual ~CAgilentFrequencyCounter();
// Public methods
	MC_ErrorCode Command(char *pszCommand);
	MC_ErrorCode Command(int CommandLength);

// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified);
	virtual CTerminal *GetTerminal() { return &m_SerialPort; }
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanAsciiControl() { return TRUE; }
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return (unsigned char *)m_pszReply; }
	virtual unsigned int GetMCReplyDataLength() { return m_MCReplyLength; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual void OnMcConnected();
	virtual void OnMcDisconnected();

// Non Volatile RAM
// Power meter methods
	virtual MC_ErrorCode ResetMeasurements();
	virtual MC_ErrorCode MeasureFrequency(double &Frequency, int Channel);
	
// Protected methods
protected:

// Protected members
protected:
	CSerialPort_AgilentFrequencyCounter m_SerialPort;
	char m_pszReply[128];
	char m_pszCommand[128];
	char m_szVersion[256];
	unsigned int m_MCReplyLength;
	int m_CurrentChannel;
};


class CAgilentE4418B : public CAgilentPowerMeter
{
// Construction/destruction
public:
	CAgilentE4418B();
	virtual ~CAgilentE4418B();
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }

// Common device methods
// Non Volatile RAM
// Power meter methods
	virtual int GetChannelsNumber() { return 1; }

// Protected methods
protected:
// Public members
public:
	static char *m_pszName;
protected:
// Protected members
};


class CAgilent53150A : public CAgilentFrequencyCounter
{
// Construction/destruction
public:
	CAgilent53150A();
	virtual ~CAgilent53150A();
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }

// Common device methods
// Non Volatile RAM
// Power meter methods
	virtual int GetChannelsNumber() { return 2; }

// Protected methods
protected:
// Public members
public:
	static char *m_pszName;
protected:
// Protected members
};



#endif // __AGILENT_POWER_METER_H_INCLUDED__