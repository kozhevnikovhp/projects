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


Sniffer::Sniffer(const std::string &ifaceName)
{
    ifaceName_ = ifaceName;
    pHandle_ = pcap_open_live(ifaceName.c_str(), BUFSIZ, 1, 1000, error_buffer_);
    if (pHandle_ == NULL)
        fprintf(stderr, "Couldn't open device %s: %s\n", ifaceName.c_str(), error_buffer_);

    bHasEthernetHeader_ = false;
    if (pcap_datalink(pHandle_) == DLT_EN10MB)
    {
        bHasEthernetHeader_ = true;
    }

/*#ifdef SOCKETS_WSA
    socket_ = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
#endif
#ifdef SOCKETS_BSD
    socket_ = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
#endif
    //printf("Socket = %d\n", socket_);
    if (!isCreated())
        perror("Sniffer socket creation");*/
}

//virtual
Sniffer::~Sniffer()
{
}

void Sniffer::destroy()
{
    if (isDestroyed())
        return; // do nothing

/*#ifdef SOCKETS_WSA
    ::shutdown(socket_, SD_BOTH);
    ::closesocket(socket_);
#endif

#ifdef SOCKETS_BSD
    ::shutdown(socket_, SHUT_RDWR);
    if (::close(socket_) != 0) // success = 0, fail = -1
    {
        perror("close");
    }
#endif
*/
}

#if (SOCKETS_WSA)
bool SnifferSocket::promiscModeOn(IPADDRESS_TYPE ifaceIP)
{
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = ifaceIP;
    local.sin_port = 0;
    if (::bind(socket_, (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
    {
        perror("bind");
        return false;
    }

    unsigned long flag = 1;  // flag PROMISC ON/OFF
    ioctlsocket(socket_, SIO_RCVALL, &flag);
    return true;
}
#elif (SOCKETS_BSD)
bool Sniffer::promiscModeOn(const char *pszIfaceName)
{
/*    memset(&ifaceDesc_, 0, sizeof(ifaceDesc_));
    strcpy(ifaceDesc_.ifr_name, pszIfaceName);
    ioctl(socket_, SIOCGIFFLAGS, &ifaceDesc_);
    ifaceDesc_.ifr_flags |= IFF_PROMISC;
    ioctl(socket_, SIOCSIFFLAGS, &ifaceDesc_);*/
    return true;
}
#endif

bool Sniffer::promiscModeOff()
{
/*#if (SOCKETS_WSA)
    unsigned long flag = 0;  // flag PROMISC ON/OFF
    ioctlsocket(socket_, SIO_RCVALL, &flag);
    return true;
#elif (SOCKETS_BSD)
    ioctl(socket_, SIOCGIFFLAGS, &ifaceDesc_);
    ifaceDesc_.ifr_flags &= ~IFF_PROMISC;
    ioctl(socket_, SIOCSIFFLAGS, &ifaceDesc_);
    return true;
#endif
*/
    return true;
}

bool Sniffer::waitForPacket()
{
/*    struct sockaddr src;
    socklen_t fromlen = sizeof(src);
    int nPacketSize = recvfrom(socket_, bufferForPackets_, sizeof(bufferForPackets_), 0, &src, &fromlen);

    if (nPacketSize == SOCKET_ERROR)
    {
        perror("recvfrom");
        return false;
    }
    //printf("Packet %d bytes\n", nPacketSize);*/

    SIpHeader *pIpHeader = NULL;
    struct pcap_pkthdr *pHeader;	/* The header that pcap gives us */
    const u_char *pPacket;		/* The actual packet */

    int nPacketSize = 0;
    int ec = pcap_next_ex(getHandle(), &pHeader, &pPacket);
    if (ec == 1)
    {
        nPacketSize = pHeader->len;
        printf("%s: packet with length of %d\n", ifaceName_.c_str(), nPacketSize);
        if (bHasEthernetHeader_)
        {
            struct ethhdr *pEthernetHeader = (struct ethhdr *)pPacket;
            //printf("Ethernet proto = 0x%04X\n", pEthernetHeader->h_proto);
            if (pEthernetHeader->h_proto == htons(ETH_P_IP))
            {
                pIpHeader = (SIpHeader *)(pEthernetHeader+1);
                nPacketSize -= sizeof(struct ethhdr);
                printf("\tIP-Packet %d bytes\n", nPacketSize);
            }
            else if (pEthernetHeader->h_proto == htons(ETH_P_ARP))
            {
                printf("ARP\n");
            }
            else if (pEthernetHeader->h_proto == htons(ETH_P_RARP))
            {
                printf("RARP\n");
            }
            else
            {
                printf("Ethernet proto = %d\n", ntohs(pEthernetHeader->h_proto));
            }
         }
         else
            pIpHeader = (SIpHeader *)pPacket;
    }
#if (SOCKETS_WSA)
    IpHeader = (SIpHeader *)bufferForPackets_;
#elif (SOCKETS_BSD)
    struct ethhdr *pEthernetHeader = (struct ethhdr *)bufferForPackets_;
    //printf("Ethernet proto = 0x%04X\n", pEthernetHeader->h_proto);
    if (pEthernetHeader->h_proto == htons(ETH_P_IP))
    {
        pIpHeader = (SIpHeader *)(pEthernetHeader+1);
        nPacketSize -= sizeof(struct ethhdr);
        //printf("\tIP-Packet %d bytes\n", nPacketSize);
    }
    else if (pEthernetHeader->h_proto == htons(ETH_P_ARP))
    {
        //printf("ARP\n");
    }
    else if (pEthernetHeader->h_proto == htons(ETH_P_RARP))
    {
        //printf("RARP\n");
    }
#endif
    if (!pIpHeader)
        return true; // APR, RARP etc, but not IP

    //printIpHeader(pIpHeader);

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
