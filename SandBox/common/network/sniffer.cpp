// Class CSnifferSocket implementation

#include "sniffer.h"

#if (WIN32)
#ifndef SIO_RCVALL
#define SIO_RCVALL 0x98000001
#endif
#elif (UNIX)
#include <net/if.h>
#include <linux/if_ether.h>
#endif

#include <string.h>
#include <stdio.h>



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
    return create(AF_INET, SOCK_RAW, IPPROTO_IP);
#endif
#ifdef SOCKETS_BSD
   return create(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
#endif
}

#if (WIN32)
bool SnifferSocket::promiscModeOn(IPADDRESS_TYPE ifaceIP)
{
    inherited::bind(0, ifaceIP);
    unsigned long flag = 1;  // flag PROMISC ON/OFF
    ioctlsocket(m_Socket, SIO_RCVALL, &flag);
    return true;
}
#elif (UNIX)
bool SnifferSocket::promiscModeOn(const char *pszIfaceName)
{
    struct ifreq interface;
    strcpy(interface.ifr_name, pszIfaceName);
    ioctl(m_Socket, SIOCGIFFLAGS, &interface);
    interface.ifr_flags |= IFF_PROMISC;
    ioctl(m_Socket, SIOCSIFFLAGS, &eth);
    return true;
}
#endif

bool SnifferSocket::promiscModeOff()
{
#if (WIN32)
    unsigned long flag = 0;  // flag PROMISC ON/OFF
    ioctlsocket(m_Socket, SIO_RCVALL, &flag);
    return true;
#elif (UNIX)
    struct ifreq interface;
    strcpy(interface.ifr_name, pszIfaceName);
    ioctl(m_Socket, SIOCGIFFLAGS, &interface);
    interface.ifr_flags &= ~IFF_PROMISC;
    ioctl(m_Socket, SIOCSIFFLAGS, &eth);
    return true;

#endif
}
bool SnifferSocket::waitForPacket()
{
    struct sockaddr Src;
    int nReadBytes = 0;
    bool bSuccess = ReadFrom(m_szBufferForPackets, sizeof(m_szBufferForPackets), nReadBytes, &Src);
	if (!bSuccess)
        return false;
	
    //struct ethhdr* eth = (struct ethhdr*)m_szBufferForPackets;
    SIpHeader *pIpHeader = (SIpHeader *)/*(eth+1)*/(m_szBufferForPackets);
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
