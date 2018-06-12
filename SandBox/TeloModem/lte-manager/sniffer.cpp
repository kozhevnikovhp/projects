// Class SnifferSocket implementation

#include "sniffer.h"

#include <net/if.h>
#include <linux/if_ether.h>

#include <string.h>
#include <stdio.h>


Sniffer::Sniffer(const std::string &ifaceName)
{
    ifaceName_ = ifaceName;
    pHandle_ = pcap_open_live(ifaceName.c_str(), BUFSIZ, 1, 1000, error_buffer_);
    if (pHandle_ == NULL)
    {
        fprintf(stderr, "pcap_open_live %s\n", error_buffer_);
        return;
    }

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

bool Sniffer::promiscModeOn(const char *pszIfaceName)
{
/*    memset(&ifaceDesc_, 0, sizeof(ifaceDesc_));
    strcpy(ifaceDesc_.ifr_name, pszIfaceName);
    ioctl(socket_, SIOCGIFFLAGS, &ifaceDesc_);
    ifaceDesc_.ifr_flags |= IFF_PROMISC;
    ioctl(socket_, SIOCSIFFLAGS, &ifaceDesc_);*/
    return true;
}

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
        //printf("%s: packet with length of %d\n", ifaceName_.c_str(), nPacketSize);
        if (bHasEthernetHeader_)
        {
            struct ethhdr *pEthernetHeader = (struct ethhdr *)pPacket;
            //printf("Ethernet proto = 0x%04X\n", pEthernetHeader->h_proto);
            switch (ntohs(pEthernetHeader->h_proto))
            {
                case ETH_P_IP:
                    pIpHeader = (SIpHeader *)(pEthernetHeader+1);
                    nPacketSize -= sizeof(struct ethhdr);
                    //printf("\t%s: IP-Packet %d bytes\n", ifaceName_.c_str(), nPacketSize);
                    break;
                default:
                    //printf("\t%s: unknown ethernet proto = 0x%04X, refer to file linux/if_ether.h and add implementation if needed\n", ifaceName_.c_str(), ntohs(pEthernetHeader->h_proto));
                    break;
            } // switch
         } //bHasEthernetHeader_
         else
            pIpHeader = (SIpHeader *)pPacket;
    }
    if (!pIpHeader)
        return true; // APR, RARP etc, but not IP

    //printIpHeader(pIpHeader);

    unsigned short nIpHdrLen = pIpHeader->getHeaderLen();
    int nPayloadLen = nPacketSize - nIpHdrLen;
    unsigned char *pPayload = (unsigned char *)pIpHeader + nIpHdrLen;
    ipPacketCaptured(pIpHeader, pPayload, nPayloadLen);

    return true; // successfully read and processed
}
