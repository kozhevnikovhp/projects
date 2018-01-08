// Class CTcpSocket implementation

#include "stdafx.h"
#include "stdio.h"
#include "TcpSockets.h"
#include <ws2tcpip.h>

CTcpSocket::CTcpSocket()
{
	m_bConnected = LOGICAL_FALSE;
	strcpy(m_pszConnectionName, "0.0.0.0");
}

//virtual
CTcpSocket::~CTcpSocket()
{
}

//virtual
LOGICAL CTcpSocket::Connect(const char *pszIpAddress, unsigned short PortNo)
{
	strcpy(m_pszConnectionName, pszIpAddress);
	unsigned long IpAddress = StringToAddress(pszIpAddress);
	if (0 == IpAddress)
		return LOGICAL_FALSE;
	return Connect(IpAddress, PortNo);
}

//virtual
LOGICAL CTcpSocket::Connect(const char *pszIpAddress)
{
	return Connect(pszIpAddress, GetDefaultPort());
}

//virtual
LOGICAL CTcpSocket::Connect(unsigned long IpAddress)
{
	return Connect(IpAddress, GetDefaultPort());
}

LOGICAL CTcpSocket::Connect(unsigned long IpAddress, unsigned short PortNo)
{
	if (m_bConnected)
		return LOGICAL_TRUE; // Do nothing
	if (!IsOpened())
		Open();
	if (!IsOpened())
		return LOGICAL_FALSE;
	m_bConnected = LOGICAL_FALSE;
	struct sockaddr_in SocketParams;
	SocketParams.sin_family = AF_INET;
	SocketParams.sin_port = htons(PortNo);
	SocketParams.sin_addr.s_addr = IpAddress;
	if (connect(GetSocket(), (sockaddr *)&SocketParams, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	m_bConnected = LOGICAL_TRUE;
	OnConnected();
	return LOGICAL_TRUE;
}


static char *pszConnected = "opened";
static char *pszNotConnected = "not opened";
//virtual
char *CTcpSocket::GetConnectionStatus()
{
	char *pszStatus = pszNotConnected;
	if (IsConnected())
		pszStatus = pszConnected;
	sprintf(m_pszConnectionStatus, "TCP connection %s is %s", GetConnectionName(), pszStatus);
	return m_pszConnectionStatus;
}

void CTcpSocket::Disconnect()
{
	if (IsConnected())
	{
		OnDisconnected();
		m_bConnected = LOGICAL_FALSE;
		Shutdown();
		Close();
	}
}

LOGICAL CTcpSocket::Read(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	if (!IsOpened() || !IsConnected())
		return LOGICAL_FALSE;
	nReadBytes = recv(GetSocket(), (char *)pBufferToFill, nBufferSize, 0);
	if (nReadBytes == SOCKET_ERROR || nReadBytes == 0)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
/*	TRACE("recv: %d bytes of %d received\n", nReadBytes, nBufferSize);
	for (unsigned int i=0; i<nReadBytes; i++)
	{
		TRACE("%d\n", pBufferToFill[i]);
	}*/
	// End of patch
	return LOGICAL_TRUE;
}

LOGICAL CTcpSocket::Write(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes)
{
	if (!IsOpened() || !IsConnected())
		return LOGICAL_FALSE;
	nSentBytes = send(GetSocket(), (char *)pBufferToSend, nBytesToSend, 0);
	if (nSentBytes == SOCKET_ERROR || nSentBytes != nBytesToSend)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
#ifdef _DEBUG
/*	TRACE("CTcpSocket::Write to port : ");
	for (int i = 0; i<nBytesToSend; i++)
	{
		TRACE(" %d", (unsigned char)pBufferToSend[i]);
	}
	TRACE("\n");*/
#endif
	return LOGICAL_TRUE;
}

//virtual
void CTcpSocket::ClearAllBuffers()
{
	unsigned int nReadSymbols = 0;
	do 
	{
		nReadSymbols = 0;
		PeekInputSymbols(NULL, 0, nReadSymbols); //???
	} while (nReadSymbols != 0);
}

//virtual
LOGICAL CTcpSocket::PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	nReadBytes = 0;
	unsigned int n;
	char c;
	if (!IsOpened() || !IsConnected())
		return LOGICAL_FALSE;
	char *pcCharBuffer = (char *)pBufferToFill;

	fd_set set;
	FD_ZERO(&set);
	FD_SET(GetSocket(), &set);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;
	while (1)
	{
		int nReady = select(GetSocket()+1, &set, NULL, NULL, &tv);
		if (nReady == SOCKET_ERROR)
		{
			m_dwLastErrorCode = WSAGetLastError();
			return LOGICAL_FALSE;
		}
		if (nReady == 0)
			return (nReadBytes > 0);
		if (!Read(&c, 1, n))
		{
			m_dwLastErrorCode = WSAGetLastError();
			if (n == 0 && m_dwLastErrorCode != WSAETIMEDOUT)
			{
				Close(); // Close itself because another peer broke the connection
				return LOGICAL_FALSE;
			}
			return LOGICAL_TRUE;
		}

		if (pcCharBuffer != NULL)
			pcCharBuffer[nReadBytes] = c;
		if (n > 0)
			nReadBytes++;
	}
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CTcpSocket::Open()
{
	// Socket creation
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}

	return LOGICAL_TRUE;
}

//virtual
void CTcpSocket::Close()
{
	Disconnect();
	m_bConnected = LOGICAL_FALSE;
	CIpSocket::Close();
}

void CTcpSocket::Shutdown()
{
	shutdown(GetSocket(), SD_BOTH);
}

LOGICAL CTcpSocket::Listen(unsigned short uPort, unsigned long InterfaceIpAddress/* = INADDR_ANY*/)
{
	if (!Bind(uPort, InterfaceIpAddress))
		return LOGICAL_FALSE;

	if (listen(GetSocket(), SOMAXCONN) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

CTcpSocket *CTcpSocket::Accept()
{
	sockaddr_in from;
	int fromlen=sizeof(from);
	SOCKET NewSocket = accept(GetSocket(), (struct sockaddr*)&from, &fromlen);
	if (NewSocket == INVALID_SOCKET)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return NULL;
	}
	CTcpSocket *pNewSocket = CreateAcceptSocket();
	if (pNewSocket)
	{
		pNewSocket->m_Socket = NewSocket;
		pNewSocket->m_bConnected = LOGICAL_TRUE;
		strcpy(m_pszConnectionStatus, inet_ntoa(from.sin_addr));
	}
	return pNewSocket;
}

LOGICAL CTcpSocket::GetSockName(unsigned long &IpAddress, unsigned short &PortNo)
{
	sockaddr_in local;
	int size = sizeof(local);
	local.sin_family = AF_INET;
	if (getsockname(GetSocket(), (sockaddr *)&local, &size) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	IpAddress = local.sin_addr.s_addr;
	PortNo = ntohs(local.sin_port);
	return LOGICAL_TRUE;
}

LOGICAL CTcpSocket::GetSockName(char *pszIpAddress, unsigned short &PortNo)
{
	sockaddr_in local;
	pszIpAddress[0] = 0;
	int size = sizeof(local);
	local.sin_family = AF_INET;
	if (getsockname(GetSocket(), (sockaddr *)&local, &size) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}

	strcpy(pszIpAddress, inet_ntoa(local.sin_addr));
	PortNo = ntohs(local.sin_port);
	return LOGICAL_TRUE;
}
