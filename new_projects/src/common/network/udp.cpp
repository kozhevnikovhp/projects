#include "udp.h"
#include <string.h>

namespace common {

namespace network {

/////////////////////////////////////////////////////////////////////
// Class cUdpSocket implementation

cUdpSocket::cUdpSocket()
{
	CreateAndInit();
}

cUdpSocket::cUdpSocket(IPPORT portNo)
{
	CreateAndInit();
	bind(portNo);
}

bool cUdpSocket::CreateAndInit()
{
	m_pDatagramBuffer = NULL;
	bool bSuccess = create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_pDatagramBuffer = new char[GetMaxDatagramSize()];
	return bSuccess;
}

//virtual
cUdpSocket::~cUdpSocket()
{
	delete[] m_pDatagramBuffer;
}

//virtual
bool cUdpSocket::bind(IPPORT portNo)
{
	int optVal=1;
	setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optVal, sizeof(optVal));

	// Binding
	struct sockaddr_in SocketParams;
	SocketParams.sin_family = AF_INET;
	SocketParams.sin_port = htons(portNo);
	SocketParams.sin_addr.s_addr = htonl(INADDR_ANY);
	if (::bind(m_Socket, (const sockaddr *)&SocketParams, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		storeLastErrorCode();
		return false;
	}

	return true;
}

bool cUdpSocket::SendEchoRequest(IPADDRESS_TYPE IpAddress, unsigned short nPacketSize)
{
	if (nPacketSize > GetMaxDatagramSize())
		return false;
	memset(m_pDatagramBuffer, 0, GetMaxDatagramSize());
	int nUserDataSize = nPacketSize - sizeof(SIpHeader) - sizeof(SUdpHeader);

	// Fill user data
	if (nUserDataSize < 2)
		nUserDataSize = 2;
	//char *pUserData = m_pDatagramBuffer; // user data follows ICMP-header
	//FillDataBuffer(pUserData, nUserDataSize);

	// Send packet
	int nSentBytes;
	bool bWritten = WriteTo(m_pDatagramBuffer, nUserDataSize, nSentBytes, IpAddress, IPPORT_ECHO);
	return bWritten;
}

bool cUdpSocket::SendEchoRequest(const char *pszIpAddress, unsigned short nPacketSize)
{
	IPADDRESS_TYPE IpAddress = inet_addr(pszIpAddress);
	return SendEchoRequest(IpAddress, nPacketSize);
}

bool cUdpSocket::SendBroadcastEchoRequest(unsigned short nPacketSize)
{
	return SendEchoRequest(INADDR_BROADCAST, nPacketSize);
}


} // namespace network

} // namespace common

