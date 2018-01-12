#include "ip.h"
#include "misc.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

namespace common {

namespace network {

// methods to initialize and deinitialize socket subsystem
//static
int IpSocket::InitSockets()
{
    if (bSocketsInitialized_)
        return 1;
#ifdef SOCKETS_WSA
#define VERSION_MINOR 1
#define VERSION_MAJOR 1

	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD(VERSION_MINOR, VERSION_MAJOR);

	int err = WSAStartup(wVersionRequested, &WsaData);
	if (err != 0)
		return 0;

	if (LOBYTE(WsaData.wVersion) != VERSION_MAJOR ||
		HIBYTE(WsaData.wVersion) != VERSION_MINOR )
	{ // We could not find a usable WinSock DLL
		WSACleanup();
		return 0;
	}
#endif
    bSocketsInitialized_ = true;
    return 1; // success
} //  InitSockets()

//static
int IpSocket::FinitSockets()
{
#ifdef SOCKETS_WSA
	WSACleanup();
#endif
    bSocketsInitialized_ = false;
    return 1; // success
} //  FinitSockets()


///////////////////////////////////////////////////////////////////////////
// Class IpSocket implementation

unsigned int IpSocket::m_SocketCount = 0;
unsigned int IpSocket::m_MaxSocketSeqNumber = 0;
bool IpSocket::bSocketsInitialized_ = false;

IpSocket::IpSocket()
{
	m_SocketSeqNumber = m_MaxSocketSeqNumber;
	m_Socket = INVALID_SOCKET;

	m_cTOS = 0;
	m_cTTL = 128;
}

//virtual
IpSocket::~IpSocket()
{
	destroy();
}

void IpSocket::storeLastErrorCode()
{
#ifdef SOCKETS_WSA
	m_LastErrorCode = WSAGetLastError();
#endif
#ifdef SOCKETS_BSD
	m_LastErrorCode = errno;
#endif
}

bool IpSocket::create(int af, int type, int protocol)
{
    InitSockets();
    if (isCreated())
        return true; // opened already, do nothing
    
    ++m_SocketCount;
    ++m_MaxSocketSeqNumber;
    
    m_Socket = socket(af, type, protocol);
    if (m_Socket == INVALID_SOCKET)
    {
        storeLastErrorCode();
        perror("socket");
        return false;
    }
    
    return true;
}

void IpSocket::destroy()
{
    if (isDestroyed())
        return; // do nothing

    onBeforeClosing();
    
#ifdef SOCKETS_WSA
    ::shutdown(m_Socket, SD_BOTH);
    ::closesocket(m_Socket);
#endif
    
#ifdef SOCKETS_BSD
    ::shutdown(m_Socket, SHUT_RDWR);
    if (::close(m_Socket) != 0) // success = 0, fail = -1
    {
        perror("close");
    }
#endif
    
    m_Socket = INVALID_SOCKET;
    --m_SocketCount;
}

bool IpSocket::bind(IPPORT portNo, IPADDRESS_TYPE InterfaceIpAddress/* = INADDR_ANY */)
{
    sockaddr_in local;
    local.sin_family = AF_INET;
	local.sin_addr.s_addr = InterfaceIpAddress;
	local.sin_port = htons(portNo);
	if (::bind(GetSocket(), (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
	{
        perror("bind");
        storeLastErrorCode();
        return false;
	}
	return true;
}

bool IpSocket::SetOption(int level, int OptionName, char *pcValue, socklen_t OptionLength)
{
	if ((setsockopt(m_Socket, level, OptionName, pcValue, OptionLength)) == SOCKET_ERROR)
	{
        perror("setsockopt");
        storeLastErrorCode();
        return false;
	}
	return true;
}

bool IpSocket::GetOption(int level, int OptionName, char *pcValue, socklen_t &OptionLength)
{
	if ((getsockopt(m_Socket, level, OptionName, pcValue, &OptionLength)) == SOCKET_ERROR)
	{
        perror("getsockopt");
		storeLastErrorCode();
		return false;
	}
	return true;
}

unsigned long Sum16bit(unsigned short *pData, socklen_t nDataSize)
{
	unsigned long checksum = 0;

	while (nDataSize > 1)
	{
		checksum += *pData++;
		nDataSize  -= sizeof(unsigned short);
	}
	if (nDataSize)
		checksum += *(unsigned char *)pData;

	return checksum;
}

unsigned short IpSocket::CalcCheckSum(unsigned short *pData, unsigned short nDataSize)
{
	unsigned long checksum = Sum16bit(pData, nDataSize);

	checksum =  (checksum >> 16) + (checksum & 0xFFFF);
	checksum += (checksum >> 16);

	return (unsigned short)(~checksum);
}

unsigned short IpSocket::CalcPseudoCheckSum(SIpHeader *pIpHeader, unsigned short *pData, unsigned short nDataSize)
{
	SPseudoHeader pseudo;
	pseudo.src_addr = pIpHeader->sourceIP;
	pseudo.dst_addr = pIpHeader->destIP;
	pseudo.zero = 0;
	pseudo.proto = pIpHeader->proto;
	pseudo.length = htons(nDataSize);
	unsigned long checksum = Sum16bit((unsigned short *)&pseudo, sizeof(pseudo));
	checksum += Sum16bit(pData, nDataSize);

	checksum =  (checksum >> 16) + (checksum & 0xFFFF);
	checksum += (checksum >> 16);

	return (unsigned short)(~checksum);
}

bool IpSocket::SetReadTimeout(unsigned int timeout)
{
    struct timeval t;
    t.tv_sec = timeout/1000;
    t.tv_usec = (timeout % 1000)*1000;
    return SetOption(SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
}

//virtual
bool IpSocket::ReadFrom
(
	void *pBufferToFill,
	size_t nBufferSize,
	int &nReadBytes,
	struct sockaddr *pSrc
)
{
	socklen_t fromlen = sizeof(*pSrc);
	nReadBytes = recvfrom(GetSocket(), (char *)pBufferToFill, nBufferSize, 0, pSrc, &fromlen);
	if (nReadBytes == SOCKET_ERROR)
	{
        perror("recvfrom");
		storeLastErrorCode();
		return false;
	}
	return true;
}

//virtual
bool IpSocket::ReadFrom
(
    void *pBufferToFill,
    size_t nBufferSize,
    int &nReadBytes,
    IPADDRESS_TYPE &IpAddress,
    IPPORT &portNo
)
{
    sSockAddrIn from;
    memset(&from, 0, sizeof(from));
    from.addrin.sin_family = AF_INET;
	bool ret = ReadFrom(pBufferToFill, nBufferSize, nReadBytes, &from.addr);
    IpAddress = getIP(&from.addr);
    portNo = ntohs(from.addrin.sin_port);
    return ret;
}


bool IpSocket::SetWriteTimeout(unsigned int timeout)
{
    struct timeval t;
    t.tv_sec = timeout/1000;
    t.tv_usec = (timeout % 1000)*1000;
    return SetOption(SOL_SOCKET, SO_SNDTIMEO, (char *)&t, sizeof(t));
}

//virtual
bool IpSocket::WriteTo
(
    void *pBufferToSend,
    size_t nBytesToSend,
    int &nSentBytes,
    struct sockaddr *pDest
)
{
    nSentBytes = sendto(m_Socket, (char *)pBufferToSend, nBytesToSend, 0, pDest,	sizeof(*pDest));
    if (nSentBytes == SOCKET_ERROR)
    {
        perror("sendto");
        storeLastErrorCode();
		return false;
    }
	return true;
}

//virtual
bool IpSocket::WriteTo
(
    void *pBufferToSend,
    size_t nBytesToSend,
    int &nSentBytes,
    char *pszIpAddress,
    IPPORT portNo
)
{
    IPADDRESS_TYPE IpAddress = stringToAddress(pszIpAddress);
    return WriteTo(pBufferToSend, nBytesToSend, nSentBytes, IpAddress, portNo);
}

//virtual
bool IpSocket::WriteTo
(
    void *pBufferToSend,
    size_t nBytesToSend,
    int &nSentBytes,
    IPADDRESS_TYPE IpAddress,
    IPPORT portNo
)
{
    sSockAddrIn dest;
    memset(&dest, 0, sizeof(dest));
    dest.addrin.sin_family = AF_INET;
    dest.addrin.sin_port = htons(portNo);
    setIP(&dest.addr, IpAddress);
    return WriteTo(pBufferToSend, nBytesToSend, nSentBytes, &dest.addr);
}

bool IpSocket::SetSendTTL(unsigned char ttl)
{
    m_cTTL = ttl;
    int dwValue = ttl;
    return SetOption(IPPROTO_IP, IP_TTL, (char *)&dwValue, sizeof(dwValue));
}

bool IpSocket::SetSendTOS(unsigned char tos)
{
	m_cTOS = tos;
	int dwValue = tos;
	return SetOption(IPPROTO_IP, IP_TOS, (char *)&dwValue, sizeof(dwValue));
}

bool IpSocket::EnableBroadcasting(bool bEnable /*= true */)
{
    int dwValue = bEnable;
    return SetOption(SOL_SOCKET, SO_BROADCAST, (char *)&dwValue, sizeof(dwValue));
}

bool IpSocket::WriteBroadcast(void *pBufferToSend, size_t nBytesToSend, int &nSentBytes, IPPORT portNo)
{
    return WriteTo(pBufferToSend, nBytesToSend, nSentBytes, INADDR_BROADCAST, portNo);
}

///////////////////////////////////////////////////////////////////////////
// Class UserSocket implementation

UserSocket::UserSocket()
{
}

//virtual
UserSocket::~UserSocket()
{
}



} // namespace network

} // namespace common
