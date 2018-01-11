// Class CSnifferSocket implementation

#include "sniffer.h"
#include <net/if.h>
#include <string.h>
#include <stdio.h>

#ifndef SIO_RCVALL
#define SIO_RCVALL 0x98000001
#endif


namespace common {

namespace network {

SnifferSocket::SnifferSocket()
{
    open();
}

//virtual
SnifferSocket::~SnifferSocket()
{
}

bool SnifferSocket::open()
{
#ifdef SOCKETS_WSA
    return create(AF_PACKET, SOCK_RAW, IPPROTO_IP);
#endif
#ifdef SOCKETS_BSD
   return create(AF_PACKET, SOCK_RAW, htons(0x0003));
#endif
}

bool SnifferSocket::bind(IPADDRESS_TYPE InterfaceIpAddress)
{
//    return true;
    return inherited::bind(0, InterfaceIpAddress);
}

bool SnifferSocket::enablePromiscMode()
{
    unsigned long flag = 1;  // flag PROMISC ON/OFF

#ifdef SOCKETS_WSA
    ioctlsocket(m_Socket, SIO_RCVALL, &flag);
#endif
#ifdef SOCKETS_BSD
//    ioctl(m_Socket, SIO_RCVALL, &flag);

    struct ifreq eth;

    strcpy(eth.ifr_name, "enp0s3");

    ioctl(m_Socket, SIOCGIFFLAGS, &eth);

    eth.ifr_flags |= IFF_PROMISC;

    ioctl(m_Socket, SIOCSIFFLAGS, &eth);

#endif
    return true;
}

bool SnifferSocket::disablePromiscMode()
{
    unsigned long	flag = 0;  // flag PROMISC ON/OFF
#ifdef SOCKETS_WSA
    ioctlsocket(m_Socket, SIO_RCVALL, &flag);
#endif
#ifdef SOCKETS_BSD
    ioctl(m_Socket, SIO_RCVALL, &flag);
#endif

    return true;
}

bool SnifferSocket::waitForPacket()
{
    struct sockaddr Src;
    int nReadBytes = 0;
    bool bSuccess = ReadFrom(m_szBufferForPackets, sizeof(m_szBufferForPackets), nReadBytes, &Src);
	if (!bSuccess)
        return false;
	
	SIpHeader *pIpHeader = (SIpHeader *)m_szBufferForPackets;
	unsigned short	nIpHdrLen = pIpHeader->h_len * 4;
	unsigned char *pUserData = m_szBufferForPackets + nIpHdrLen;
	unsigned int nUserDataLength = nReadBytes - nIpHdrLen;
    bool bProcessed = OnIpPacket(pIpHeader, pUserData, nUserDataLength);
	if (bProcessed)
        return true; // no more processing required

	SIcmpHeader *pIcmpHeader;
	SIgmpHeader *pIgmpHeader;
	STcpHeader *pTcpHeader;
	SUdpHeader *pUdpHeader;
	
	switch (pIpHeader->proto)
	{
	case IPPROTO_TCP:
		pTcpHeader = (STcpHeader *)pUserData;
		pUserData += sizeof(STcpHeader);
		nUserDataLength -= sizeof(STcpHeader);
		OnTcpPacket(pIpHeader, pTcpHeader, pUserData, nUserDataLength);
		break;
	case IPPROTO_UDP:
		pUdpHeader = (SUdpHeader *)pUserData;
		pUserData += sizeof(SUdpHeader);
		nUserDataLength -= sizeof(SUdpHeader);
		OnUdpPacket(pIpHeader, pUdpHeader, pUserData, nUserDataLength);
		break;
	case IPPROTO_ICMP:
		pIcmpHeader = (SIcmpHeader *)pUserData;
		pUserData += sizeof(SIcmpHeader);
		nUserDataLength -= sizeof(SIcmpHeader);
		OnIcmpPacket(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case IPPROTO_IGMP:
		pIgmpHeader = (SIgmpHeader *)pUserData;
		pUserData += sizeof(SIgmpHeader);
		nUserDataLength -= sizeof(SIgmpHeader);
		OnIgmpPacket(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	default:
		OnUnknownProtoPacket(pIpHeader, pUserData, nUserDataLength);
		break;
	} // end of switch (pIpHeader->proto)

    return true; // successfully read and processed
}

}
}
