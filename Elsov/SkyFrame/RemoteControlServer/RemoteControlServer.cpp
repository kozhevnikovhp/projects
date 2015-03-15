// RemoteControlServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include "UdpThreadSocket.h"
#include "..\HardwareTest\Radyne.h"
#include "..\HardwareTest\Pozharsky.h"
#include "..\HardwareTest\Datum.h"

// Forward declarations
class CRemoteControlServer;

///////////////////////////////////////////////////////////////////////////////////////////////
// CServerSocket class declaration

class CServerSocket : public CUdpThreadSocket
{
//Attributes
public:
	CServerSocket()
	{
		m_pServer = NULL;
		m_bRunning = FALSE;
	}
	virtual ~CServerSocket() {}

// Public methods
public:
	void SetServer(CRemoteControlServer *pServer) { m_pServer = pServer; }
	BOOL IsRunning() { return m_bRunning; }
	void Stop() { Close(); m_bRunning = FALSE; }

// Public overridables
public:
// Common socket methods
// CThreadObject interface
	virtual void OnThreadStarted();
	virtual BOOL ThreadBody();
	virtual void OnThreadEnded();

// Public members
public:

// Protected methods
protected:
// Protected overridables
protected:
// Protected members
protected:
	CRemoteControlServer *m_pServer;
	BOOL m_bRunning;
};

class CServerSocket_Radyne : public CServerSocket
{
	virtual unsigned short GetDefaultPort() { return 8888; }
};


class CServerSocket_Pozharsky : public CServerSocket
{
	virtual unsigned short GetDefaultPort() { return 8889; }
};


class CServerSocket_Datum : public CServerSocket
{
	virtual unsigned short GetDefaultPort() { return 8890; }
};


/////////////////////////////////////////////////////////////////////////////////////////
// CRadyneServer class declaration

class CRemoteControlServer
{
// Construction/destruction
public:
	CRemoteControlServer();
	virtual ~CRemoteControlServer();
// Public methods
public:
	BOOL Start();
	BOOL Stop();
	BOOL OpenComPort(unsigned int nComPort) {	return GetDevice()->Connect(nComPort); }

// Public overridables
public:
	virtual CDevice *GetDevice() = 0;
	virtual CServerSocket *GetSocket() = 0;

// Protected methods

// Protected overridables
protected:

// Protected members
protected:
	BOOL m_bRunning;
};

class CRadyneRemoteControlServer : public CRemoteControlServer
{
// Construction/destruction
public:

// Public overridables
public:
	virtual CDevice *GetDevice() { return &m_Device; }
	virtual CServerSocket *GetSocket() { return &m_TcpIpSocket; }

// Protected overridables
protected:

// Protected members
protected:
	CRadyneDMD2401 m_Device;
	CServerSocket_Radyne m_TcpIpSocket;
};


class CPozharskyRemoteControlServer : public CRemoteControlServer
{
// Construction/destruction
public:

// Public overridables
public:
	virtual CDevice *GetDevice() { return &m_Device; }
	virtual CServerSocket *GetSocket() { return &m_TcpIpSocket; }

// Protected overridables
protected:

// Protected members
protected:
	CPozharskyUpDownConverter m_Device;
	CServerSocket_Pozharsky m_TcpIpSocket;
};


class CDatumRemoteControlServer : public CRemoteControlServer
{
// Construction/destruction
public:

// Public overridables
public:
	virtual CDevice *GetDevice() { return &m_Device; }
	virtual CServerSocket *GetSocket() { return &m_TcpIpSocket; }

// Protected overridables
protected:

// Protected members
protected:
	CDatumPsm4900_LBand m_Device;
	CServerSocket_Datum m_TcpIpSocket;
};

//virtual
void CServerSocket::OnThreadStarted()
{
	TRACE("CServerSocket::ThreadProcedure started\n");
	ASSERT(m_pServer);
	if (!Bind(GetDefaultPort()))
	{
		TRACE("ERROR! Socket not bind!\n");
	}
}

//virtual
BOOL CServerSocket::ThreadBody()
{
	unsigned char cByte = 0;
	unsigned int nWrittenBytes = 0;
	unsigned int nCommandBytes, nReplyBytes;
	unsigned char pcCommandBuffer[256];
	static int nCounter = 0;
	struct sockaddr_in Src;

	if (!NeedContinueThread())
		return FALSE; // terminate thread
	if (ReadFrom((char *)&pcCommandBuffer, sizeof(pcCommandBuffer), nCommandBytes, &Src))
	{
		printf("%d:\t\t%d bytes from %s:%d to serial port\n", ++nCounter, nCommandBytes, inet_ntoa(Src.sin_addr), Src.sin_port);
		m_pServer->GetDevice()->SendMC((char *)pcCommandBuffer, nCommandBytes);
		MC_ErrorCode EC = m_pServer->GetDevice()->ReadReplyUntilPrompt();
		if (EC == MC_DEVICE_NOT_RESPONDING)
			printf("\t\t - no reply from serial port\n");
		else
		{
			nReplyBytes = m_pServer->GetDevice()->GetMCReplyDataLength();
			WriteTo((char *)m_pServer->GetDevice()->GetMCReplyData(), nReplyBytes, nWrittenBytes, &Src);
			printf("%d:\t\t%d bytes from serial port to %s:%d\n", ++nCounter, nReplyBytes, inet_ntoa(Src.sin_addr), Src.sin_port);
		}
		return TRUE;
	}
	return FALSE;
}

//virtual
void CServerSocket::OnThreadEnded()
{
	TRACE("CServerSocket::ThreadProcedure ended\n");
}


CRemoteControlServer::CRemoteControlServer()
{
	m_bRunning = FALSE;
}

//virtual
CRemoteControlServer::~CRemoteControlServer()
{
	Stop();
}

BOOL CRemoteControlServer::Start()
{
	if (m_bRunning)
		return TRUE;
	
	GetSocket()->SetServer(this);
	m_bRunning = GetSocket()->Run();
	return m_bRunning;
}

BOOL CRemoteControlServer::Stop()
{
	if (!m_bRunning)
		return FALSE;

	m_bRunning = FALSE;	

	GetSocket()->Stop();
	return TRUE;
}


int main(int argc, char* argv[])
{
	SYSTEMTIME time;
	CTime CurrentTime;

	GetLocalTime(&time);
	printf("%02d:%02d:%02d\tProgram started as ",
				time.wHour,	time.wMinute, time.wSecond); 
	for (int i = 1; i<argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");

	CRemoteControlServer *pServer;
	unsigned int nComPort;

	for (i = 1; i<argc; i++)
	{
		pServer = NULL;
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case 'R':
		case 'r': // Radyne modem
			pServer = new CRadyneRemoteControlServer();
			printf("Radyne modem:");
			break;
		case 'P':
		case 'p': // Up/Down converter
			pServer = new CPozharskyRemoteControlServer();
			printf("Up/down converter:");
			break;
		case 'D':
		case 'd': // Datum modem
			pServer = new CDatumRemoteControlServer();
			printf("Datum modem:");
			break;
		default:
			printf("Error: - Unrecognized command-line option '%c'\n", *(pszArg+1));
			break;
		}
		if (!pServer)
			continue;
		if (sscanf(argv[i]+2, "%d", &nComPort) == 1)
		{
			printf("\n\tSerial port COM%d: ", nComPort);
			if (pServer->OpenComPort(nComPort))
				printf("was opened successfully.");
			else
				printf("was not opened due to error %d.", GetLastError());
			printf("\n\tServer ");
			if (pServer->Start())
				printf("started.");
			else
				printf("not started.");
			printf("\n");
		}
		else
			printf("Error: - Unrecognized command-line parameter(s) '%c'\n", *(pszArg+1));
	}

	getch();
	return 0;
}
