#include "stdafx.h"
#include "Device.h"
#include "SerialPort.h"

static char CR=0x0D;
static char LF=0x0A;

FILE *CDevice::m_pLogFile=NULL;

CDevice::CDevice()
{
	m_ControlType = MC_UNKNOWN_TYPE;
	m_bControllable = FALSE;
	m_bParamSavingInNvRamRightAfterSettingEnabled = FALSE;
	m_ConnectionTime = 0xFFFFFFFF;
	m_bLogFileEnabled = FALSE;
}

CDevice::~CDevice()
{
}

BOOL CDevice::Connect(const char *pszName)
{
	Disconnect();
	unsigned long StartTime = GetTickCount();
	m_bControllable = GetTerminal()->Connect(pszName);
	unsigned long ConnectionTime = GetTickCount()-StartTime;
	SetLocality(ConnectionTime);
	if (m_bControllable)
		OnMcConnected();
	return m_bControllable;
}

BOOL CDevice::Connect()
{
	Disconnect();
	unsigned long StartTime = GetTickCount();
	m_bControllable = GetTerminal()->Connect();
	unsigned long ConnectionTime = GetTickCount()-StartTime;
	SetLocality(ConnectionTime);
	if (m_bControllable)
		OnMcConnected();
	return m_bControllable;
}

BOOL CDevice::Connect(unsigned int nPort)
{
	static char pszPortName[16];
	sprintf(pszPortName, "COM%d", nPort);
	return Connect(pszPortName);
}

BOOL CDevice::Disconnect()
{
	if (m_bControllable)
		OnMcDisconnected();
	GetTerminal()->Disconnect();
	m_bControllable = FALSE;
	return TRUE;
}

// Monitor and Control methods

unsigned int CDevice::PumpMC(char *pszBuffer, unsigned int nBufferSize)
{
	pszBuffer[0] = 0;
	unsigned int nReadBytes = 0;
	GetTerminal()->PeekInputSymbols(pszBuffer, nBufferSize, nReadBytes);
	if (nReadBytes)
		pszBuffer[nReadBytes] = 0; // to make zero-terminating string
	return nReadBytes;
}

BOOL CDevice::SendMC(char *pszBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes)
{
	nSentBytes = 0;
	GetTerminal()->ClearAllBuffers();
	OnBeforeSendMC(pszBufferToSend, nBytesToSend);
	return GetTerminal()->Write(pszBufferToSend, nBytesToSend, nSentBytes);
}

BOOL CDevice::SendMC(char *pszBufferToSend, unsigned int nBytesToSend)
{
	unsigned int nSentBytes;
	return SendMC(pszBufferToSend, nBytesToSend, nSentBytes);
}

//virtual
BOOL CDevice::IsLocallyControlled()
{
	if (!IsControllable())
		return FALSE;
	return (GetControlType() == MC_SERIAL_PORT	||
			GetControlType() == MC_GPIB			||
			m_ConnectionTime < GetMaxLocalConnectionTime());
}

//virtual
void CDevice::SetLocality(unsigned long ConnectionTime)
{
	m_ConnectionTime = ConnectionTime;
}
