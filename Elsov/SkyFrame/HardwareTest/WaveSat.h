/* Wavesat.h: Monitoring and Control of WaveSat outdor tranceiver.
Author: Evgeny A. Kozhevnikov Moscow, Russia  ek538@mail.ru

  Revision history:
	01 September 2002 - initial creation
	02 September 2003 - M&C substitutor
*/

#ifndef __WAVESAT_H_INCLUDED__
#define __WAVESAT_H_INCLUDED__

#include "Tranceiver.h"
#include "DeviceSubstitutor.h"
#include "SerialPort.h"
#include "ThreadObject.h"

class CSerialPort_WaveSat : public CSerialPort
{
	virtual char *GetControlString() { return "19200,n,8,1"; }
	virtual unsigned int GetReadTimeout() { return 1000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};


class CWaveSatTranceiver : public CTranceiver
{
// Construction/destruction
public:
	CWaveSatTranceiver();
	virtual ~CWaveSatTranceiver();
// Public methods
	MC_ErrorCode Command(int CommandLength);

// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified);
	virtual CTerminal *GetTerminal() { return &m_SerialPort; }
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return m_pszReply; }
	virtual unsigned int GetMCReplyDataLength() { return m_MCReplyLength; }
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);

// Non Volatile RAM

// Temperature
	virtual BOOL CanGetTemperature() { return TRUE; }
	virtual MC_ErrorCode IsTemperatureAlarm(BOOL &bAlarm);
	virtual MC_ErrorCode GetTemperature(double &T);
// Output power and mute
	virtual BOOL CanOutputOnOff() { return TRUE; }
	virtual BOOL CanGetPower() { return TRUE; }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn);
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn);
	virtual MC_ErrorCode GetPower(double &Power);
// Frequency
	virtual unsigned int GetBandShift() { return 13050; }
	virtual BOOL CanGetFrequency() { return TRUE; }
	virtual MC_ErrorCode GetFrequency(unsigned int &Frequency);
	virtual BOOL CanSetFrequency() { return TRUE; }
	virtual MC_ErrorCode SetFrequency(unsigned int &Frequency);
// Gain
	virtual BOOL CanGetGain() { return TRUE; }
	virtual MC_ErrorCode GetGain(double &Gain);
	
// Protected methods
protected:
	int FillCommandBuffer(unsigned char CommandCode, unsigned char *pcDataBytes, unsigned char nDataBytes);
	short RawValueToShort(unsigned char *pszRawValue);
// Protected members
	CSerialPort_WaveSat m_SerialPort;
	unsigned char m_pszReply[32];
	unsigned char m_pszCommand[32];
	unsigned int m_MCReplyLength;
};


class CWaveSatWtr70 : public CWaveSatTranceiver
{
// Construction/destruction
public:
	CWaveSatWtr70();
	virtual ~CWaveSatWtr70();
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }

// Common device methods
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


class CWaveSatSubstitutor : public CDeviceSubstitutor, public CThreadObject
{
// Construction/destruction
public:
	CWaveSatSubstitutor();
	virtual ~CWaveSatSubstitutor();
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
// CThreadObject interface
	virtual void OnThreadStarted();
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();
// Monitor and Control
	virtual CTerminal *GetTerminal() { return &m_SerialPort; }
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual MC_ErrorCode ReadReplyUntilPrompt() { return MC_OK; }
	virtual unsigned char *GetMCReplyData() { return NULL; }
	virtual unsigned int GetMCReplyDataLength() { return 0; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
// Substitutor methods
	virtual BOOL Start();
	virtual BOOL Stop();
	virtual BOOL IsStarted() { return NeedContinueThread(); }

// Common device methods

// Protected methods
protected:
	BOOL ReadRequestSendReply();

// Public members
public:
	static char *m_pszName;
protected:
// Protected members
	CSerialPort_WaveSat m_SerialPort;
	unsigned char m_pcRequest[32];
	unsigned char m_pcReply[32];
	BOOL m_bAlarm;
};


#endif // __WAVESAT_H_INCLUDED__