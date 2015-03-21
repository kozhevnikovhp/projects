// Class cTcpSocket implementation

#ifdef SOCKETS_BSD
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include <stdio.h>
#include <string.h>
#include "tcp.h"
#include "misc.h"

namespace common {

namespace network {

cTcpSocket::cTcpSocket()
{
	m_bConnected = false;
    strcpy(m_pszConnectionName, "0.0.0.0");
    open();
}

//virtual
cTcpSocket::~cTcpSocket()
{
    destroy();
}

bool cTcpSocket::connect(const char *pszIpAddress, IPPORT portNo)
{
	strcpy(m_pszConnectionName, pszIpAddress);
	IPADDRESS_TYPE IpAddress = StringToAddress(pszIpAddress);
	if (0 == IpAddress)
		return false;
	return connect(IpAddress, portNo);
}

bool cTcpSocket::connect(const char *pszIpAddress)
{
	return connect(pszIpAddress, getDefaultPort());
}

bool cTcpSocket::connect(IPADDRESS_TYPE IpAddress)
{
	return connect(IpAddress, getDefaultPort());
}

bool cTcpSocket::connect(IPADDRESS_TYPE IpAddress, IPPORT portNo)
{
	if (m_bConnected)
		return true; // Do nothing
	if (!isCreated())
		open();
	if (!isCreated())
		return false;
	m_bConnected = false;
	struct sockaddr_in SocketParams;
	SocketParams.sin_family = AF_INET;
	SocketParams.sin_port = htons(portNo);
	SocketParams.sin_addr.s_addr = IpAddress;
	if (::connect(GetSocket(), (sockaddr *)&SocketParams, sizeof(sockaddr)) == SOCKET_ERROR)
	{
        //perror("connect");
		storeLastErrorCode();
		return false;
	}
	m_bConnected = true;
	onConnected();
	return true;
}


static const char *pszConnected = "opened";
static const char *pszNotConnected = "not opened";

//virtual
char *cTcpSocket::getConnectionStatus()
{
	const char *pszStatus = pszNotConnected;
	if (isConnected())
		pszStatus = pszConnected;
	sprintf(m_pszConnectionStatus, "TCP connection %s is %s", getConnectionName(), pszStatus);
	return m_pszConnectionStatus;
}

void cTcpSocket::disconnect()
{
	if (isConnected())
	{
		onDisconnected();
		m_bConnected = false;
		destroy();
	}
}

bool cTcpSocket::Read(void *pBufferToFill, size_t nBufferSize, int &nReadBytes)
{
	if (!isCreated() || !isConnected())
		return false;
	nReadBytes = ::recv(GetSocket(), (char *)pBufferToFill, nBufferSize, 0);
	if (nReadBytes == SOCKET_ERROR || nReadBytes == 0)
	{
		storeLastErrorCode();
		return false;
	}
	return true;
}

bool cTcpSocket::Write(const void *pBufferToSend, size_t nBytesToSend, int &nSentBytes)
{
	if (!isCreated() || !isConnected())
		return false;
	nSentBytes = ::send(GetSocket(), (char *)pBufferToSend, nBytesToSend, 0);
	if (nSentBytes == SOCKET_ERROR)
	{
        perror("send");
		storeLastErrorCode();
		return false;
	}
	return true;
}

void cTcpSocket::clearAllBuffers()
{
	int nReadSymbols = 0;
	do 
	{
		nReadSymbols = 0;
		PeekInputSymbols(NULL, 0, nReadSymbols); //???
	} while (nReadSymbols != 0);
}

bool cTcpSocket::enableKeepAlive(bool bEnable)
{
    int value;
    if (bEnable)
        value = 1;
    else
        value = 0;
    return SetOption(SOL_SOCKET, SO_KEEPALIVE, (char *)&value, sizeof(value));
}

bool cTcpSocket::isKeepAliveEnabled(bool &bEnabled)
{
    int value = 0;
    socklen_t len;
	bool bSuccess = GetOption(SOL_SOCKET, SO_KEEPALIVE, (char *)&value, len);
    if (bSuccess)
    {
        if (value == 1)
			bEnabled = true;
        else
			bEnabled = false;
    }
    return bSuccess;
}

bool cTcpSocket::setKeepAliveTime(int t)
{
#ifdef TCP_KEEPIDLE // unavailable on some platforms say SunOS/Solaris
    return SetOption(SOL_TCP, TCP_KEEPIDLE, (char *)&t, sizeof(t));
#else
	return false;
#endif
}

bool cTcpSocket::getKeepAliveTime(int &t)
{
#ifdef TCP_KEEPIDLE // unavailable on some platforms say SunOS/Solaris
    socklen_t len;
    return GetOption(SOL_TCP, TCP_KEEPIDLE, (char *)&t, len);
#else
	return false;
#endif
}

bool cTcpSocket::PeekInputSymbols(void *pBufferToFill, size_t nBufferSize, int &nReadBytes)
{
	nReadBytes = 0;
	int n;
	char c;
	if (!isCreated() || !isConnected())
		return false;
	char *pcCharBuffer = (char *)pBufferToFill;

	fd_set set;
	FD_ZERO(&set);
	FD_SET(GetSocket(), &set);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;
	while (1)
	{
		int nReady = ::select(GetSocket()+1, &set, NULL, NULL, &tv);
		if (nReady == SOCKET_ERROR)
		{
            storeLastErrorCode();
			return false;
		}
		if (nReady == 0)
			return (nReadBytes > 0);
		if (!Read(&c, 1, n))
		{
            storeLastErrorCode();
			if (n == 0 && getLastErrorCode() != ETIMEDOUT)
			{
				destroy(); // Close itself because another peer broken the connection
				return false;
			}
			return true;
		}

		if (pcCharBuffer != NULL)
			pcCharBuffer[nReadBytes] = c;
		if (n > 0)
			nReadBytes++;
	}
	return true;
}

bool cTcpSocket::open()
{
	// Socket creation
    return create(AF_INET, SOCK_STREAM, 0);
}

//virtual
void cTcpSocket::onBeforeClosing()
{
	m_bConnected = false;
}

bool cTcpSocket::listen(IPPORT portNo, IPADDRESS_TYPE InterfaceIpAddress/* = INADDR_ANY*/)
{
	if (!bind(portNo, InterfaceIpAddress))
		return false;

	if (::listen(GetSocket(), SOMAXCONN) == SOCKET_ERROR)
	{
        perror("listen");
        storeLastErrorCode();
		return false;
	}
	return true;
}

cTcpSocket *cTcpSocket::accept()
{
	sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	SOCKET NewSocket = ::accept(GetSocket(), (struct sockaddr*)&from, &fromlen);
	if (NewSocket == INVALID_SOCKET)
	{
        perror("accept");
        storeLastErrorCode();
		return NULL;
	}
	cTcpSocket *pNewSocket = createAcceptSocket();
	if (pNewSocket)
	{
		pNewSocket->m_Socket = NewSocket;
		pNewSocket->m_bConnected = true;
		strcpy(m_pszConnectionStatus, inet_ntoa(from.sin_addr));
	}
	return pNewSocket;
}

bool cTcpSocket::getSockName(IPADDRESS_TYPE &IpAddress, IPPORT &portNo)
{
	sockaddr_in local;
	socklen_t size = sizeof(local);
	local.sin_family = AF_INET;
	if (getsockname(GetSocket(), (sockaddr *)&local, &size) == SOCKET_ERROR)
	{
        perror("getsockname");
        storeLastErrorCode();
		return false;
	}
	IpAddress = local.sin_addr.s_addr;
	portNo = ntohs(local.sin_port);
	return true;
}

bool cTcpSocket::getSockName(char *pszIpAddress, IPPORT &portNo)
{
	sockaddr_in local;
	pszIpAddress[0] = 0;
	socklen_t size = sizeof(local);
	local.sin_family = AF_INET;
	if (getsockname(GetSocket(), (sockaddr *)&local, &size) == SOCKET_ERROR)
	{
        perror("getsockname");
        storeLastErrorCode();
		return false;
	}

	strcpy(pszIpAddress, inet_ntoa(local.sin_addr));
	portNo = ntohs(local.sin_port);
	return true;
}

bool cTcpSocket::getPeerName(IPADDRESS_TYPE &IpAddress, IPPORT &portNo)
{
	sockaddr_in local;
	socklen_t size = sizeof(local);
	local.sin_family = AF_INET;
	if (getpeername(GetSocket(), (sockaddr *)&local, &size) == SOCKET_ERROR)
	{
        perror("getpeername");
        storeLastErrorCode();
		return false;
	}
	IpAddress = local.sin_addr.s_addr;
	portNo = ntohs(local.sin_port);
	return true;
}

bool cTcpSocket::getPeerName(char *pszIpAddress, IPPORT &portNo)
{
	sockaddr_in local;
	pszIpAddress[0] = 0;
	socklen_t size = sizeof(local);
	local.sin_family = AF_INET;
	if (getpeername(GetSocket(), (sockaddr *)&local, &size) == SOCKET_ERROR)
	{
        perror("getpeername");
        storeLastErrorCode();
		return false;
	}

	strcpy(pszIpAddress, inet_ntoa(local.sin_addr));
	portNo = ntohs(local.sin_port);
	return true;
}

} // namespace network

} // namespace common
