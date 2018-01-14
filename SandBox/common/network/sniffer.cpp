// Class CSnifferSocket implementation

#include "sniffer.h"

#if (SOCKETS_WSA)
#ifndef SIO_RCVALL
#define SIO_RCVALL 0x98000001
#endif
#elif (SOCKETS_BSD)
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

#if (SOCKETS_WSA)
bool SnifferSocket::promiscModeOn(IPADDRESS_TYPE ifaceIP)
{
    inherited::bind(0, ifaceIP);
    unsigned long flag = 1;  // flag PROMISC ON/OFF
    ioctlsocket(m_Socket, SIO_RCVALL, &flag);
    return true;
}
#elif (SOCKETS_BSD)
bool SnifferSocket::promiscModeOn(const char *pszIfaceName)
{
    memset(&ifaceDesc_, 0, sizeof(ifaceDesc_));
    strcpy(ifaceDesc_.ifr_name, pszIfaceName);
    ioctl(m_Socket, SIOCGIFFLAGS, &ifaceDesc_);
    ifaceDesc_.ifr_flags |= IFF_PROMISC;
    ioctl(m_Socket, SIOCSIFFLAGS, &ifaceDesc_);
    return true;
}
#endif

bool SnifferSocket::promiscModeOff()
{
#if (SOCKETS_WSA)
    unsigned long flag = 0;  // flag PROMISC ON/OFF
    ioctlsocket(m_Socket, SIO_RCVALL, &flag);
    return true;
#elif (SOCKETS_BSD)
    ioctl(m_Socket, SIOCGIFFLAGS, &ifaceDesc_);
    ifaceDesc_.ifr_flags &= ~IFF_PROMISC;
    ioctl(m_Socket, SIOCSIFFLAGS, &ifaceDesc_);
    return true;
#endif
}
bool SnifferSocket::waitForPacket()
{
    struct sockaddr Src;
    int nReadBytes = 0;
    bool bSuccess = ReadFrom(bufferForPackets_, sizeof(bufferForPackets_), nReadBytes, &Src);
	if (!bSuccess)
        return false;
	
#if (SOCKETS_WSA)
    SIpHeader *pIpHeader = (SIpHeader *)m_szBufferForPackets;
#elif (SOCKETS_BSD)
    struct ethhdr *pEthernetHeader = (struct ethhdr *)bufferForPackets_;
    SIpHeader *pIpHeader = (SIpHeader *)(pEthernetHeader+1);
#endif
    unsigned short	nIpHdrLen = pIpHeader->getHeaderLength();
    unsigned char *pUserData = bufferForPackets_ + nIpHdrLen;
	unsigned int nUserDataLength = nReadBytes - nIpHdrLen;
    bool bProcessed = OnIpPacket(pIpHeader, pUserData, nUserDataLength);
	if (bProcessed)
        return true; // no more processing required

    switch (pIpHeader->proto)
	{
    case IPPROTO_TCP: {
        STcpHeader *pTcpHeader = (STcpHeader *)pUserData;
		pUserData += sizeof(STcpHeader);
		nUserDataLength -= sizeof(STcpHeader);
		OnTcpPacket(pIpHeader, pTcpHeader, pUserData, nUserDataLength);
        break; }
    case IPPROTO_UDP: {
        SUdpHeader *pUdpHeader = (SUdpHeader *)pUserData;
		pUserData += sizeof(SUdpHeader);
		nUserDataLength -= sizeof(SUdpHeader);
		OnUdpPacket(pIpHeader, pUdpHeader, pUserData, nUserDataLength);
        break; }
    case IPPROTO_ICMP: {
        SIcmpHeader *pIcmpHeader = (SIcmpHeader *)pUserData;
		pUserData += sizeof(SIcmpHeader);
		nUserDataLength -= sizeof(SIcmpHeader);
		OnIcmpPacket(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
        break; }
    case IPPROTO_IGMP: {
        SIgmpHeader *pIgmpHeader = (SIgmpHeader *)pUserData;
		pUserData += sizeof(SIgmpHeader);
		nUserDataLength -= sizeof(SIgmpHeader);
		OnIgmpPacket(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
        break; }
	default:
		OnUnknownProtoPacket(pIpHeader, pUserData, nUserDataLength);
		break;
	} // end of switch (pIpHeader->proto)

    return true; // successfully read and processed
}

} //namespace network
} //namespace common
