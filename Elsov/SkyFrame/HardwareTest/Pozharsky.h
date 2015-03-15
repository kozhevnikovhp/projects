/*	Pozharsky.h - header file for Pozharsky's Up/Down converters interfaces

	Author: Evgeny A. Kozhevnikov, Zelenograd, Moscow, Russia evgeny@kozhevnikov.org

Revision history:
	May 16 2003 - initial creation
	Aug 22 2003 - remote Tcp/Ip control added
	Oct 13 2004 - newer version (October2004) added
*/

#ifndef __POZHARSKY_H_INCLUDED__
#define __POZHARSKY_H_INCLUDED__

#include "SerialPort.h"
#include "UdpBasedTerminal.h"
#include "UpDownConverter.h"

// Forward declarations

class CSerialPort_Pozharsky : public CSerialPort
{
	virtual BOOL CanBeShared() { return TRUE; }
	virtual char *GetControlString() { return "9600,n,8,2"; }
	virtual unsigned int GetReadTimeout() { return 200; }
	virtual unsigned int GetWriteTimeout() { return 200; }
};


///////////////////////////////////////////////////////////////////////////////////////////////
// CPozharskyClientSocket class declaration

class CPozharskyClientSocket : public CUdpBasedTerminal
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
	virtual unsigned short GetDefaultPort() { return 8889; }
// Protected members
protected:
};


class CPozharskyUpDownConverter : public CUpDownConverter
{
public:
	// Construction-destruction
	CPozharskyUpDownConverter();
	virtual ~CPozharskyUpDownConverter();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return ""; }
	
// Common device commands
	virtual CTerminal *GetTerminal();
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanBeControlledByTcpIp() { return TRUE; }
	virtual void OnMcConnected();
	virtual BOOL HasMCBusAddress() { return TRUE; }
	virtual void OnBeforeSendMC(char *pszBufferToSend, unsigned int nBytesToSend); // stores info from command
	virtual unsigned int GetMCBusAddress() { return m_cMCNodeAddress; }
	virtual void SetMCBusAddress(unsigned int Address) { m_cMCNodeAddress = (unsigned char)Address; }
	virtual unsigned int GetMinMCBusAddressScanning() { return 0x01; }
	virtual unsigned int GetMaxMCBusAddressScanning() { return 0xFE; }
	virtual BOOL CheckMCBusAddress(unsigned int Address);
	virtual BOOL CanWriteConfiguration() { return TRUE; }
	virtual MC_ErrorCode WriteConfiguration();
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return m_cReply; }
	virtual unsigned int GetMCReplyDataLength() { return m_MCReplyLength; }
	virtual MC_ErrorCode ParseMCReply() { return MC_OK; } // must be overridden

// Up/down converter methods
// Frequency
	virtual MC_ErrorCode SetUpOutputFrequency(unsigned int &Frequency);
	virtual MC_ErrorCode GetUpOutputFrequency(unsigned int &Frequency);
	virtual MC_ErrorCode SetDownInputFrequency(unsigned int &Frequency);
	virtual MC_ErrorCode GetDownInputFrequency(unsigned int &Frequency);
	virtual unsigned int GetMinUpOutputFrequency() { return 0; }
	virtual unsigned int GetMaxUpOutputFrequency() { return 0; }
	virtual unsigned int GetMinDownInputFrequency() { return 0; }
	virtual unsigned int GetMaxDownInputFrequency() { return 0; }
	
// On/off
	virtual MC_ErrorCode IsTurnedOn(BOOL &bOn);
	virtual MC_ErrorCode TurnOn(BOOL &bOn);
	virtual CanUpOnOff() { return TRUE; }
	virtual MC_ErrorCode IsUpTurnedOn(BOOL &bOn);
	virtual MC_ErrorCode TurnUpOn(BOOL &bOn);
	virtual BOOL CanDownOnOff() { return TRUE; }
	virtual MC_ErrorCode IsDownTurnedOn(BOOL &bOn);
	virtual MC_ErrorCode TurnDownOn(BOOL &bOn);
// Frequency
	virtual MC_ErrorCode SetFrequency(unsigned int &Frequency);
	virtual MC_ErrorCode GetFrequency(unsigned int &Frequency);
// Input attenuator
// Output attenuator
// Gain/loss
// Temperature
	virtual MC_ErrorCode GetTemperature(double &T);
	virtual MC_ErrorCode GetUpTemperature(double &T);
	virtual MC_ErrorCode GetDownTemperature(double &T);
// Misc
	MC_ErrorCode ReadCommand(unsigned char Port, unsigned char &cDataByte);
	MC_ErrorCode ReadCommand(unsigned char Port, unsigned short &sDataWord);
	MC_ErrorCode WriteCommand(unsigned char Port, unsigned char &cDataByte);
	MC_ErrorCode WriteCommand(unsigned char Port, unsigned short &sDataWord);
	MC_ErrorCode Command(unsigned char CommandCode, unsigned char Port, unsigned char &cDataByte);
	MC_ErrorCode Command(unsigned char CommandCode, unsigned char Port, unsigned short &sDataWord);
	MC_ErrorCode Command(unsigned char CommandCode, unsigned char Port, unsigned char *pcDataBytes, unsigned char DataLength);

// Public members
public:
	CSerialPort_Pozharsky m_SerialPort;
	CPozharskyClientSocket m_TcpIpSocket;

// Protected members
protected:
	unsigned char m_cCommand[300];
	unsigned char m_cReply[300];
	unsigned char m_cMCNodeAddress;
	unsigned char *m_pReplyData;
	unsigned int m_MCReplyLength;
	unsigned char m_Port;
	unsigned char m_CommandCode;

// Private members
private:
};

class CPozharskyUpDownConverterVer2002 : public CPozharskyUpDownConverter
{
public:
	// Construction-destruction
	CPozharskyUpDownConverterVer2002();
	virtual ~CPozharskyUpDownConverterVer2002();
	
// Public methods

// Overridable methods
	
// Common device commands
	virtual MC_ErrorCode ParseMCReply();

// Up/down converter methods
	virtual unsigned int GetMinUpOutputFrequency() { return 950; }
	virtual unsigned int GetMaxUpOutputFrequency() { return 1450; }
	virtual unsigned int GetMinDownInputFrequency() { return 950; }
	virtual unsigned int GetMaxDownInputFrequency() { return 1750; }
	
// Public members
public:

// Protected methods
protected:
// On/off
// Frequency
// Input attenuator
	virtual MC_ErrorCode SetInputAtt(double &AttValue);
	virtual MC_ErrorCode GetInputAtt(double &AttValue);
// Output attenuator
	virtual MC_ErrorCode SetOutputAtt(double &AttValue);
	virtual MC_ErrorCode GetOutputAtt(double &AttValue);
// Gain/loss
	virtual MC_ErrorCode SetGainLoss(double &AttValue);
	virtual MC_ErrorCode GetGainLoss(double &AttValue);
// Temperature
// Misc
// Private members
private:
};

class CPozharskyUpDownConverterVer2004 : public CPozharskyUpDownConverter
{
public:
	// Construction-destruction
	CPozharskyUpDownConverterVer2004();
	virtual ~CPozharskyUpDownConverterVer2004();
	
// Public methods

// Overridable methods
	
// Common device commands
	virtual MC_ErrorCode ParseMCReply();

// Up/down converter methods
	virtual unsigned int GetMinUpOutputFrequency() { return 950; }
	virtual unsigned int GetMaxUpOutputFrequency() { return 1450; }
	virtual unsigned int GetMinDownInputFrequency() { return 950; }
	virtual unsigned int GetMaxDownInputFrequency() { return 1750; }
	
// Public members
public:

// Protected methods
protected:
// On/off
// Frequency
// Input attenuator
	virtual MC_ErrorCode SetInputAtt(double &AttValue);
	virtual MC_ErrorCode GetInputAtt(double &AttValue);
// Output attenuator
// Gain/loss
// Temperature
// Misc
// Private members
private:
};

class CPozharskyUpDownConverterVer2011 : public CPozharskyUpDownConverter
{
public:
	// Construction-destruction
	CPozharskyUpDownConverterVer2011();
	virtual ~CPozharskyUpDownConverterVer2011();
	
// Public methods

// Overridable methods
	
// Common device commands
	virtual MC_ErrorCode ParseMCReply();

// Up/down converter methods
	virtual unsigned int GetMinUpOutputFrequency() { return 950; }
	virtual unsigned int GetMaxUpOutputFrequency() { return 1450; }
	virtual unsigned int GetMinDownInputFrequency() { return 950; }
	virtual unsigned int GetMaxDownInputFrequency() { return 1750; }
	
// Public members
public:

// Protected methods
protected:
// On/off
// Frequency
// Input attenuator
	virtual MC_ErrorCode SetInputAtt(double &AttValue);
	virtual MC_ErrorCode GetInputAtt(double &AttValue);
// Output attenuator
// Gain/loss
// Temperature
// Misc
// Private members
private:
};

class CPozharskyUpConverterKvv140Ver2002 : public CPozharskyUpDownConverterVer2002
{
public:
	// Construction-destruction
	CPozharskyUpConverterKvv140Ver2002();
	virtual ~CPozharskyUpConverterKvv140Ver2002();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
	
// Common device commands
	virtual BOOL IsUpConverter() { return TRUE; }

// Up converter methods
// Output
// Frequency
	virtual BOOL CanSetUpOutputFrequency() { return TRUE; }
	virtual BOOL CanGetUpOutputFrequency() { return TRUE; }
// Input attenuator
	virtual CanSetUpInputAtt() { return TRUE; }
	virtual MC_ErrorCode SetUpInputAtt(double &AttValue);
	virtual CanGetUpInputAtt() { return TRUE; }
	virtual MC_ErrorCode GetUpInputAtt(double &AttValue);
// Output attenuator
	virtual CanSetUpOutputAtt() { return TRUE; }
	virtual MC_ErrorCode SetUpOutputAtt(double &AttValue);
	virtual CanGetUpOutputAtt() { return TRUE; }
	virtual MC_ErrorCode GetUpOutputAtt(double &AttValue);
// Gain/loss
	virtual CanSetUpGainLoss() { return TRUE; }
	virtual MC_ErrorCode SetUpGainLoss(double &AttValue);
	virtual CanGetUpOGainLoss() { return TRUE; }
	virtual MC_ErrorCode GetUpGainLoss(double &AttValue);
// Temperature
	virtual BOOL CanGetUpTemperature() { return TRUE; }

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
// Protected members
};

class CPozharskyUpConverterKvv140Ver2004 : public CPozharskyUpDownConverterVer2004
{
public:
	// Construction-destruction
	CPozharskyUpConverterKvv140Ver2004();
	virtual ~CPozharskyUpConverterKvv140Ver2004();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
	
// Common device commands
	virtual BOOL IsUpConverter() { return TRUE; }

// Up converter methods
// Output
// Frequency
	virtual BOOL CanSetUpOutputFrequency() { return TRUE; }
	virtual BOOL CanGetUpOutputFrequency() { return TRUE; }
// Input attenuator
	virtual CanSetUpInputAtt() { return TRUE; }
	virtual MC_ErrorCode SetUpInputAtt(double &AttValue);
	virtual CanGetUpInputAtt() { return TRUE; }
	virtual MC_ErrorCode GetUpInputAtt(double &AttValue);
// Temperature
	virtual BOOL CanGetUpTemperature() { return TRUE; }

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
// Protected members
};

class CPozharskyUpConverterKvv140Ver2011 : public CPozharskyUpDownConverterVer2011
{
public:
	// Construction-destruction
	CPozharskyUpConverterKvv140Ver2011();
	virtual ~CPozharskyUpConverterKvv140Ver2011();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
	
// Common device commands
	virtual BOOL IsUpConverter() { return TRUE; }

// Up converter methods
// Output
// Frequency
	virtual BOOL CanSetUpOutputFrequency() { return TRUE; }
	virtual BOOL CanGetUpOutputFrequency() { return TRUE; }
// Input attenuator
	virtual CanSetUpInputAtt() { return TRUE; }
	virtual MC_ErrorCode SetUpInputAtt(double &AttValue);
	virtual CanGetUpInputAtt() { return TRUE; }
	virtual MC_ErrorCode GetUpInputAtt(double &AttValue);

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
// Protected members
};

class CPozharskyDownConverterKvn140Ver2002 : public CPozharskyUpDownConverterVer2002
{
public:
	// Construction-destruction
	CPozharskyDownConverterKvn140Ver2002();
	virtual ~CPozharskyDownConverterKvn140Ver2002();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
	
// Common device commands
	virtual BOOL IsDownConverter() { return TRUE; }

// Down converter methods
// Receiver
// Frequency
	virtual BOOL CanSetDownInputFrequency() { return TRUE; }
	virtual BOOL CanGetDownInputFrequency() { return TRUE; }
// Input attenuator
	virtual CanSetDownInputAtt() { return TRUE; }
	virtual MC_ErrorCode SetDownInputAtt(double &AttValue);
	virtual CanGetDownInputAtt() { return TRUE; }
	virtual MC_ErrorCode GetDownInputAtt(double &AttValue);
// Output attenuator
	virtual CanSetDownOutputAtt() { return TRUE; }
	virtual MC_ErrorCode SetDownOutputAtt(double &AttValue);
	virtual CanGetDownOutputAtt() { return TRUE; }
	virtual MC_ErrorCode GetDownOutputAtt(double &AttValue);
// Gain/loss
	virtual CanGetUpOGainLoss() { return TRUE; }
	virtual MC_ErrorCode GetDownGainLoss(double &AttValue);

// Spectrum inversion
	virtual BOOL CanDownSpectralInv() { return TRUE; }
	virtual MC_ErrorCode EnableDownSpectralInv(BOOL &bEnabled);
	virtual MC_ErrorCode IsDownSpectralInvEnabled(BOOL &bEnabled);
// Temperature
	virtual BOOL CanGetDownTemperature() { return TRUE; }

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
// Protected members
};

class CPozharskyDownConverterKvn140Ver2004 : public CPozharskyUpDownConverterVer2004
{
public:
	// Construction-destruction
	CPozharskyDownConverterKvn140Ver2004();
	virtual ~CPozharskyDownConverterKvn140Ver2004();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
	
// Common device commands
	virtual BOOL IsDownConverter() { return TRUE; }

// Down converter methods
// Frequency
	virtual BOOL CanSetDownInputFrequency() { return TRUE; }
	virtual BOOL CanGetDownInputFrequency() { return TRUE; }
// Input attenuator
	virtual CanSetDownInputAtt() { return TRUE; }
	virtual MC_ErrorCode SetDownInputAtt(double &AttValue);
	virtual CanGetDownInputAtt() { return TRUE; }
	virtual MC_ErrorCode GetDownInputAtt(double &AttValue);

// Spectrum inversion
//	virtual BOOL CanDownSpectralInv() { return TRUE; }
//	virtual MC_ErrorCode EnableDownSpectralInv(BOOL &bEnabled);
//	virtual MC_ErrorCode IsDownSpectralInvEnabled(BOOL &bEnabled);
// Temperature
	virtual BOOL CanGetDownTemperature() { return TRUE; }

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
// Protected members
};

class CPozharskyDownConverterKvn140Ver2011 : public CPozharskyUpDownConverterVer2011
{
public:
	// Construction-destruction
	CPozharskyDownConverterKvn140Ver2011();
	virtual ~CPozharskyDownConverterKvn140Ver2011();
	
// Public methods

// Overridable methods
	virtual char *GetName() { return m_pszName; }
	
// Common device commands
	virtual BOOL IsDownConverter() { return TRUE; }

// Down converter methods
// Frequency
	virtual BOOL CanSetDownInputFrequency() { return TRUE; }
	virtual BOOL CanGetDownInputFrequency() { return TRUE; }
// Input attenuator
	virtual CanSetDownInputAtt() { return TRUE; }
	virtual MC_ErrorCode SetDownInputAtt(double &AttValue);
	virtual CanGetDownInputAtt() { return TRUE; }
	virtual MC_ErrorCode GetDownInputAtt(double &AttValue);

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
// Protected members
};

#endif //__POZHARSKY_H_INCLUDED__