// Class SnifferSocket implementation

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


SnifferSocket::SnifferSocket()
{
#ifdef SOCKETS_WSA
    m_Socket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
#endif
#ifdef SOCKETS_BSD
    m_Socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
#endif
    printf("Socket = %d\n", m_Socket);
    if (m_Socket == INVALID_SOCKET)
        perror("socket");
}

//virtual
SnifferSocket::~SnifferSocket()
{
}

void SnifferSocket::destroy()
{
    if (isDestroyed())
        return; // do nothing

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
}

#if (SOCKETS_WSA)
bool SnifferSocket::promiscModeOn(IPADDRESS_TYPE ifaceIP)
{
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = ifaceIP;
    local.sin_port = 0;
    if (::bind(m_Socket, (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
    {
        perror("bind");
        return false;
    }

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
    socklen_t fromlen = sizeof(src);
    int nPacketSize = recvfrom(m_Socket, bufferForPackets_, sizeof(bufferForPackets_), 0, &src, &fromlen);
    if (nPacketSize == SOCKET_ERROR)
    {
        perror("recvfrom");
        return false;
    }

#if (SOCKETS_WSA)
    SIpHeader *pIpHeader = (SIpHeader *)bufferForPackets_;
#elif (SOCKETS_BSD)
    struct ethhdr *pEthernetHeader = (struct ethhdr *)bufferForPackets_;
    SIpHeader *pIpHeader = (SIpHeader *)(pEthernetHeader+1);
    nPacketSize -= sizeof(struct ethhdr);
#endif
    unsigned short nIpHdrLen = pIpHeader->getHeaderLen();
    int nPayloadLen = nPacketSize - nIpHdrLen;
    unsigned char *pPayload = (unsigned char *)pIpHeader + nIpHdrLen;
    ipPacketCaptured(pIpHeader, pPayload, nPayloadLen);

    switch (pIpHeader->proto)
	{
    case IPPROTO_TCP: {
        STcpHeader *pTcpHeader = (STcpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)pTcpHeader+pTcpHeader->getHeaderLen();
        nPayloadLen -= pTcpHeader->getHeaderLen();
        tcpPacketCaptured(pIpHeader, pTcpHeader, pPayload, nPayloadLen);
        break; }
    case IPPROTO_UDP: {
        SUdpHeader *pUdpHeader = (SUdpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)(pUdpHeader+1);
        nPayloadLen -= sizeof(SUdpHeader);
        udpPacketCaptured(pIpHeader, pUdpHeader, pPayload, nPayloadLen);
        break; }
    case IPPROTO_ICMP: {
        SIcmpHeader *pIcmpHeader = (SIcmpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)(pIcmpHeader+1);
        nPayloadLen -= sizeof(SIcmpHeader);
        icmpPacketCaptured(pIpHeader, pIcmpHeader, pPayload, nPayloadLen);
        break; }
    case IPPROTO_IGMP: {
        SIgmpHeader *pIgmpHeader = (SIgmpHeader *)(pIpHeader+1);
        pPayload = (unsigned char *)(pIgmpHeader+1);
        nPayloadLen -= sizeof(SIgmpHeader);
        igmpPacketCaptured(pIpHeader, pIgmpHeader, pPayload, nPayloadLen);
        break; }
	default:
        unknownProtoPacketCaptured(pIpHeader, pPayload, nPayloadLen);
		break;
	} // end of switch (pIpHeader->proto)

    return true; // successfully read and processed
}
