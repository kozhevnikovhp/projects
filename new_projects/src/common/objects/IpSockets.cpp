/* File IpSockets.cpp
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org
 
Revision history:
	11 Dec 2001 - initial creation
	25 Jul 2008 - Raw sockets: pseudo checksums for UDP/TCP, SRC IpAddress in IP-header etc
*/
#include "stdafx.h"
#include "IpSockets.h"
#include <stdio.h>
#include <ws2tcpip.h>
 
// methods to initialize and deinitialize socket subsystem
int InitSockets()
{
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
	return 1; // success
} //  InitSockets()


int FinitSockets()
{
	WSACleanup();
	return 1; // success
} //  FinitSockets()


///////////////////////////////////////////////////////////////////////////
// Class CIpSocket implementation

unsigned int CIpSocket::m_SocketCount = 0;
unsigned int CIpSocket::m_MaxSocketSeqNumber = 0;

CIpSocket::CIpSocket()
{
	if (!m_SocketCount)
		InitSockets();
	++m_SocketCount;
	++m_MaxSocketSeqNumber;
	m_SocketSeqNumber = m_MaxSocketSeqNumber;
	m_Socket = INVALID_SOCKET;
	m_dwLastErrorCode = 0;

	m_cTOS = 0;
	m_cTTL = 128;
}

//virtual
CIpSocket::~CIpSocket()
{
	Close();
	--m_SocketCount;
	if (!m_SocketCount)
		FinitSockets();
}

LOGICAL CIpSocket::Create(int af, int type, int protocol)
{
	m_Socket = socket(af, type, protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

LOGICAL CIpSocket::Bind(unsigned short uPort, IPADDRESS_TYPE InterfaceIpAddress/* = INADDR_ANY */)
{
	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = InterfaceIpAddress;
	local.sin_port = htons(uPort);
	if (bind(GetSocket(), (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

LOGICAL CIpSocket::SetOption(int level, int OptionName, char *pcValue, int OptionLength)
{
	if ((setsockopt(m_Socket, level, OptionName, pcValue, OptionLength)) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

LOGICAL CIpSocket::GetOption(int level, int OptionName, char *pcValue, int &OptionLength)
{
	if ((getsockopt(m_Socket, level, OptionName, pcValue, &OptionLength)) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

unsigned long Sum16bit(unsigned short *pData, int nDataSize)
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

unsigned short CIpSocket::CalcCheckSum(unsigned short *pData, unsigned short nDataSize)
{
	unsigned long checksum = Sum16bit(pData, nDataSize);

	checksum =  (checksum >> 16) + (checksum & 0xFFFF);
	checksum += (checksum >> 16);

	return (unsigned short)(~checksum);
}

unsigned short CIpSocket::CalcPseudoCheckSum(SIpHeader *pIpHeader, unsigned short *pData, unsigned short nDataSize)
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

LOGICAL CIpSocket::SetReadTimeout(unsigned long a_dwTimeout)
{
	return SetOption(SOL_SOCKET, SO_RCVTIMEO, (char *)&a_dwTimeout, sizeof(a_dwTimeout));
}

//virtual
LOGICAL CIpSocket::ReadFrom
(
	void *pBufferToFill,
	unsigned int nBufferSize,
	unsigned int &nReadBytes,
	struct sockaddr_in *a_pSrc
)
{
	int fromlen = sizeof(*a_pSrc);
	nReadBytes = recvfrom(GetSocket(), (char *)pBufferToFill, nBufferSize, 0, (struct sockaddr *)a_pSrc, &fromlen);
	if (nReadBytes == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CIpSocket::ReadFrom
(
	void *pBufferToFill,
	unsigned int nBufferSize,
	unsigned int &nReadBytes,
	IPADDRESS_TYPE &IpAddress,
	unsigned short &PortNo
)
{
	struct sockaddr_in from;
	memset(&from, 0, sizeof(from));
	from.sin_family = AF_INET;
	from.sin_addr.S_un.S_addr = ADDR_ANY;
	LOGICAL ret = ReadFrom(pBufferToFill, nBufferSize, nReadBytes, &from);
	IpAddress = from.sin_addr.S_un.S_addr;
	PortNo = ntohs(from.sin_port);
	return ret;
}


LOGICAL CIpSocket::ReadOverlapped
(
	char *pBufferToFill,
	int nBufferSize,
	unsigned short PortNo,
	void *pData,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	WSAOVERLAPPED Overlapped;
	memset(&Overlapped, 0, sizeof(Overlapped));
	Overlapped.hEvent = pData;

	struct sockaddr_in SrcSocket;
	memset(&SrcSocket, 0, sizeof(SrcSocket));
	SrcSocket.sin_family = AF_INET;
	SrcSocket.sin_port = htons(PortNo);
	SrcSocket.sin_addr.S_un.S_addr = ADDR_ANY;

	unsigned long BytesReceived = 0, Flags = 0;
	int fromlen = sizeof(SrcSocket);
	WSABUF wsa;
	wsa.buf = pBufferToFill;
	wsa.len = nBufferSize;
	int retcode = WSARecvFrom(GetSocket(), &wsa, 1, &BytesReceived, &Flags, (LPSOCKADDR)&SrcSocket, &fromlen, &Overlapped, lpCompletionRoutine);
	if (retcode != 0)
	{
		m_dwLastErrorCode = WSAGetLastError();
	}
	SleepEx(0, TRUE);
	return (retcode == 0 || retcode == WSA_IO_PENDING);
}

LOGICAL CIpSocket::SetWriteTimeout(unsigned long a_dwTimeout)
{
	if ((setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&a_dwTimeout,
						sizeof(a_dwTimeout))) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CIpSocket::WriteTo
(
	void *pBufferToSend,
	unsigned int nBytesToSend,
	unsigned int &nSentBytes,
	struct sockaddr_in *a_pDest
)
{
	nSentBytes = sendto(m_Socket, (char *)pBufferToSend, nBytesToSend, 0, (struct sockaddr *)a_pDest,	sizeof(*a_pDest));
	if (nSentBytes == SOCKET_ERROR || nSentBytes != nBytesToSend)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CIpSocket::WriteTo
(
	void *pBufferToSend,
	unsigned int nBytesToSend,
	unsigned int &nSentBytes,
	char *pszIpAddress,
	unsigned short PortNo
)
{
	IPADDRESS_TYPE IpAddress = StringToAddress(pszIpAddress);
	return WriteTo(pBufferToSend, nBytesToSend, nSentBytes, IpAddress, PortNo);
}

//virtual
LOGICAL CIpSocket::WriteTo
(
	void *pBufferToSend,
	unsigned int nBytesToSend,
	unsigned int &nSentBytes,
	IPADDRESS_TYPE IpAddress,
	unsigned short PortNo
)
{
	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(PortNo);
	dest.sin_addr.S_un.S_addr = IpAddress;
	return WriteTo(pBufferToSend, nBytesToSend, nSentBytes, &dest);
}

//virtual
void CIpSocket::Close()
{
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
}

LOGICAL CIpSocket::SetSendTTL(unsigned char ttl)
{
	m_cTTL = ttl;
	int dwValue = ttl;
	return SetOption(IPPROTO_IP, IP_TTL, (char *)&dwValue, sizeof(dwValue));
}

LOGICAL CIpSocket::SetSendTOS(unsigned char tos)
{
	m_cTOS = tos;
  	int dwValue = tos;
	return SetOption(IPPROTO_IP, IP_TOS, (char *)&dwValue, sizeof(dwValue));
}

LOGICAL CIpSocket::EnableBroadcasting(LOGICAL bEnable /*= LOGICAL_TRUE */)
{
  	int dwValue = bEnable;
	return SetOption(SOL_SOCKET, SO_BROADCAST, (char *)&dwValue, sizeof(dwValue));
}

LOGICAL CIpSocket::WriteBroadcast(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes, unsigned short PortNo)
{
	return WriteTo(pBufferToSend, nBytesToSend, nSentBytes, INADDR_BROADCAST, PortNo);
}

///////////////////////////////////////////////////////////////////////////
// Class CUserSocket implementation

CUserSocket::CUserSocket()
{
}

//virtual
CUserSocket::~CUserSocket()
{
}


///////////////////////////////////////////////////////////////////////////
// Class CIpRawSocket implementation

CIpRawSocket::CIpRawSocket()
{
	ClearIpHeader();
	m_uSrcIP = 0; // unknown yet
}

//virtual
CIpRawSocket::~CIpRawSocket()
{
}

//virtual
LOGICAL CIpRawSocket::ReadFrom
(
	void *pBufferToFill,
	unsigned int nBufferSize,
	unsigned int &nReadBytes,
	struct sockaddr_in *a_pSrc
)
{
	ClearIpHeader();
	LOGICAL bSuccess = CIpSocket::ReadFrom(pBufferToFill, nBufferSize, nReadBytes, a_pSrc);
	if (bSuccess)
		ExtractIPHeader(pBufferToFill);
	return bSuccess;
}

//virtual
LOGICAL CIpRawSocket::ReadFrom
(
	void *pBufferToFill,
	unsigned int nBufferSize,
	unsigned int &nReadBytes,
	IPADDRESS_TYPE &IpAddress,
	unsigned short &PortNo
)
{
	ClearIpHeader();
	LOGICAL bSuccess = CIpSocket::ReadFrom(pBufferToFill, nBufferSize, nReadBytes, IpAddress, PortNo);
	if (bSuccess)
		ExtractIPHeader(pBufferToFill);
	return bSuccess;
}

void CIpRawSocket::ClearIpHeader()
{
	memset(&m_IPHeader, 0, sizeof(m_IPHeader));
}

void CIpRawSocket::ExtractIPHeader(void *pBuffer)
{
	ClearIpHeader();
	SIpHeader *pSrcIpHeader = (SIpHeader *)pBuffer;
	memcpy(&m_IPHeader, pSrcIpHeader, sizeof(m_IPHeader));
}

LOGICAL CIpRawSocket::SetIpHeaderIncl(LOGICAL bIncludeIpHeader)
{
	int dwValue = bIncludeIpHeader;
	return SetOption(IPPROTO_IP, IP_HDRINCL, (char *)&dwValue, sizeof(dwValue));
}

// Fills IP-header of the packet.
// Called for sockets which have their own IP-header.
// Returns length of IP-header + options field length in octets
int CIpRawSocket::FillIpHeader(SIpHeader *pIpHeader, int nPacketSize, unsigned long uSrcIP, unsigned long uDstIP)
{
	if (!HasOwnIpHeader())
	{
//		TRACE("CIpRawSocket::FillIpHeader is called for the socket having no its own IP-header!\n");
		return 0;
	}
	pIpHeader->version = 4; //IPv4
	int nIpHeaderLength = (sizeof(SIpHeader) + FillIpHeaderOptions(pIpHeader)); // in octets
	pIpHeader->h_len = nIpHeaderLength /4; // in 32-bits words
	pIpHeader->tos = IpHeader_TOS();
	pIpHeader->total_len = htons(nPacketSize);
	pIpHeader->ident = 0;
	pIpHeader->frag_and_flags = 0;
	pIpHeader->ttl = IpHeader_TTL();
	pIpHeader->proto = IpHeader_Proto();
	pIpHeader->checksum = 0;
	pIpHeader->destIP = uDstIP;
	
	// look for Src IP-address
	if (uSrcIP == 0)
		uSrcIP = m_uSrcIP;
	if (uSrcIP == 0)
	{
		char szName[255];
		gethostname(szName, sizeof(szName));
		struct hostent *pHostent = gethostbyname(szName);
		char **p = pHostent->h_addr_list;
		m_uSrcIP = uSrcIP = *(IPADDRESS_TYPE *)*p;
	}
	pIpHeader->sourceIP =  uSrcIP;
	pIpHeader->checksum = CalcCheckSum((unsigned short *)pIpHeader, nIpHeaderLength);
	return nIpHeaderLength;
}

////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE StringToAddress(const char *pszAddress)
{
	char *pszByte = (char *)pszAddress;
	IPADDRESS_TYPE b1=0, b2 = 0, b3 = 0, b4 = 0;
	IPADDRESS_TYPE Address = 0;

	b1 = atoi(pszByte);
	Address += b1;

	pszByte = strchr(pszByte, '.');
	if (!pszByte)
		return 0;
	pszByte++;
	b2 = atoi(pszByte);
	Address += (b2 << 8);

	pszByte = strchr(pszByte, '.');
	if (!pszByte)
		return 0;
	pszByte++;
	b3 = atoi(pszByte);
	Address += (b3 << 16);

	pszByte = strchr(pszByte, '.');
	if (!pszByte)
		return 0;
	pszByte++;
	b4 = atoi(pszByte);
	Address += b4 << 24;

	return Address;
}

char *AddressToString(char *pszAddress, IPADDRESS_TYPE Address)
{
	sprintf(pszAddress, "%d.%d.%d.%d", LOBYTE(LOWORD(Address)), HIBYTE(LOWORD(Address)),
		LOBYTE(HIWORD(Address)), HIBYTE(HIWORD(Address)));
	return pszAddress;
}

int Count1s(IPADDRESS_TYPE Address)
{
	// count 1s
	int n1Count = 0;
	for (int i = 0; i < 32; i++)
	{
		if (Address & (1 << i))
			++n1Count;
	}
	return n1Count;
}

IPADDRESS_TYPE GetSubnetMaskByLength(int nMaskLength)
{
	static IPADDRESS_TYPE uMasks[] = {
				0x00000000, // "0.0.0.0"  , /0
				0x00000080, // "128.0.0.0", /1
				0x000000C0, // "192.0.0.0", /2
				0x000000E0, // "224.0.0.0", /3
				0x000000F0, // "240.0.0.0", /4
				0x000000F8, // "248.0.0.0", /5
				0x000000FC, // "252.0.0.0", /6
				0x000000FE, // "254.0.0.0", /7
				0x000000FF, // "255.0.0.0", /8
				0x000080FF, // "255.128.0.0", /9
				0x0000C0FF, // "255.192.0.0", /10
				0x0000E0FF, // "255.224.0.0", /11
				0x0000F0FF, // "255.240.0.0", /12
				0x0000F8FF, // "255.248.0.0", /13
				0x0000FCFF, // "255.252.0.0", /14
				0x0000FEFF, // "255.254.0.0", /15
				0x0000FFFF, // "255.255.0.0", /16
				0x0080FFFF, // "255.255.128.0", /17
				0x00C0FFFF, // "255.255.192.0", /18
				0x00E0FFFF, // "255.255.224.0", /19
				0x00F0FFFF, // "255.255.240.0", /20
				0x00F8FFFF, // "255.255.248.0", /21
				0x00FCFFFF, // "255.255.252.0", /22
				0x00FEFFFF, // "255.255.254.0", /23
				0x00FFFFFF, // "255.255.255.0", /24
				0x80FFFFFF, // "255.255.255.128", /25
				0xC0FFFFFF, // "255.255.255.192", /26
				0xE0FFFFFF, // "255.255.255.224", /27
				0xF0FFFFFF, // "255.255.255.240", /28
				0xF8FFFFFF, // "255.255.255.248", /29
				0xFCFFFFFF, // "255.255.255.252", /30
				0xFEFFFFFF, // "255.255.255.254", /31
				0xFFFFFFFF, // "255.255.255.255", /32
	};
	if (nMaskLength <= 0 || nMaskLength > 32)
		return 0;
	return uMasks[nMaskLength];
}

// including 0th and last broadcast addresses, just 2**(32-MaskLength)
int CalcMaxPossibleHosts(IPADDRESS_TYPE uSubnetMask)
{
	return CalcMaxPossibleHosts(Count1s(uSubnetMask));
}

int CalcMaxPossibleHosts(int nMaskLength)
{
	static int nHosts[] = {
		0x00000001,	0x00000002,	0x00000004,	0x00000008,	0x00000010,	0x00000020,	0x00000040,	0x00000080,
		0x00000100,	0x00000200,	0x00000400,	0x00000800,	0x00001000,	0x00002000,	0x00004000,	0x00008000,
		0x00010000,	0x00020000,	0x00040000,	0x00080000,	0x00100000,	0x00200000,	0x00400000,	0x00800000,
		0x01000000, 0x02000000, 0x04000000,	0x08000000,	0x10000000, 0x20000000,	0x40000000, 0x80000000
	};
	if (nMaskLength <= 0 || nMaskLength > 32)
		return 0;
	return nHosts[32-nMaskLength];
}