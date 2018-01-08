#include "stdafx.h"
#include "Udp.h"

/////////////////////////////////////////////////////////////////////
// Class CUdpSocket implementation

CUdpSocket::CUdpSocket()
{
	CreateAndInit();
}

CUdpSocket::CUdpSocket(unsigned short uPort)
{
	CreateAndInit();
	Bind(uPort);
}

BOOL CUdpSocket::CreateAndInit()
{
	m_pDatagramBuffer = NULL;
	BOOL bSuccess = Create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_pDatagramBuffer = new char[GetMaxDatagramSize()];
	return bSuccess;
}

//virtual
CUdpSocket::~CUdpSocket()
{
	delete[] m_pDatagramBuffer;
}

//virtual
BOOL CUdpSocket::Bind(unsigned short uPort)
{
	int optVal=1;
	setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optVal, sizeof(optVal));

	// Binding
	struct sockaddr_in SocketParams;
	SocketParams.sin_family = AF_INET;
	SocketParams.sin_port = htons(uPort);
	SocketParams.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(GetSocket(), (LPSOCKADDR)&SocketParams, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpSocket::SendEchoRequest
(
	unsigned long IpAddress,
	unsigned short nPacketSize
)
{
	if (nPacketSize > GetMaxDatagramSize())
		return FALSE;
	memset(m_pDatagramBuffer, 0, GetMaxDatagramSize());
	int nUserDataSize = nPacketSize - sizeof(SIpHeader) - sizeof(SUdpHeader);

	// Fill user data
	if (nUserDataSize < 2)
		nUserDataSize = 2;
	char *pUserData = m_pDatagramBuffer; // user data follows ICMP-header
	//FillDataBuffer(pUserData, nUserDataSize);

	// Send packet
	unsigned int nSentBytes;
	BOOL bWritten = WriteTo(m_pDatagramBuffer, nUserDataSize, nSentBytes, IpAddress, IPPORT_ECHO);
	return bWritten;
}

BOOL CUdpSocket::SendEchoRequest(const char *pszIpAddress, unsigned short nPacketSize)
{
	unsigned long IpAddress = inet_addr(pszIpAddress);
	return SendEchoRequest(IpAddress, nPacketSize);
}

BOOL CUdpSocket::SendBroadcastEchoRequest(unsigned short nPacketSize)
{
	return SendEchoRequest(INADDR_BROADCAST, nPacketSize);
}


