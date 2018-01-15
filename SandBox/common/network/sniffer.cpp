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
   return create(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
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
    struct sockaddr src;
    int nPacketSize = 0;
    bool bSuccess = ReadFrom(bufferForPackets_, sizeof(bufferForPackets_), nPacketSize, &src);
	if (!bSuccess)
        return false;
	
#if (SOCKETS_WSA)
    SIpHeader *pIpHeader = (SIpHeader *)bufferForPackets_;
#elif (SOCKETS_BSD)
    struct ethhdr *pEthernetHeader = (struct ethhdr *)bufferForPackets_;
    SIpHeader *pIpHeader = (SIpHeader *)(pEthernetHeader+1);
    nPacketSize -= sizeof(struct ethhdr);
#endif
    unsigned short nIpHdrLen = pIpHeader->getHeaderLength();
    int nPayloadLen = nPacketSize - nIpHdrLen;
    unsigned char *pPayload = (unsigned char *)pIpHeader + nIpHdrLen;
    ipPacketCaptured(pIpHeader, nPacketSize, pPayload, nPayloadLen);

    switch (pIpHeader->proto)
	{
    case IPPROTO_TCP: {
        STcpHeader *pTcpHeader = (STcpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)pTcpHeader+pTcpHeader->getHeaderLength();
        nPayloadLen -= pTcpHeader->getHeaderLength();
        tcpPacketCaptured(pIpHeader, nPacketSize, pTcpHeader, pPayload, nPayloadLen);
        break; }
    case IPPROTO_UDP: {
        SUdpHeader *pUdpHeader = (SUdpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)(pUdpHeader+1);
        nPayloadLen -= sizeof(SUdpHeader);
        udpPacketCaptured(pIpHeader, nPacketSize, pUdpHeader, pPayload, nPayloadLen);
        break; }
    case IPPROTO_ICMP: {
        SIcmpHeader *pIcmpHeader = (SIcmpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)(pIcmpHeader+1);
        nPayloadLen -= sizeof(SIcmpHeader);
        icmpPacketCaptured(pIpHeader, nPacketSize, pIcmpHeader, pPayload, nPayloadLen);
        break; }
    case IPPROTO_IGMP: {
        SIgmpHeader *pIgmpHeader = (SIgmpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)(pIgmpHeader+1);
        nPayloadLen -= sizeof(SIgmpHeader);
        igmpPacketCaptured(pIpHeader, nPacketSize, pIgmpHeader, pPayload, nPayloadLen);
        break; }
	default:
        unknownProtoPacketCaptured(pIpHeader, nPacketSize, pPayload, nPayloadLen);
		break;
	} // end of switch (pIpHeader->proto)

    return true; // successfully read and processed
}

} //namespace network
} //namespace common
