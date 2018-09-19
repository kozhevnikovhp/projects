/*
 *  sniffer.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

// Class Sniffer implementation

#include "sniffer.h"

#include <net/if.h>
#include <linux/if_ether.h>

#include <string.h>
#include <stdio.h>


Sniffer::Sniffer(const std::string &ifaceName)
    : ifaceName_(ifaceName), pHandle_(nullptr), bHasEthernetHeader_(false)
{
}

//virtual
Sniffer::~Sniffer()
{
    destroy();
}

void Sniffer::destroy()
{
    promiscModeOff();
}

bool Sniffer::promiscModeOn()
{
    pHandle_ = pcap_open_live(ifaceName_.c_str(), BUFSIZ, 1, 1000, error_buffer_);
    if (pHandle_ != nullptr)
    {
        bHasEthernetHeader_ = false;
        if (pcap_datalink(pHandle_) == DLT_EN10MB)
            bHasEthernetHeader_ = true;
    }
    else
        fprintf(stderr, "pcap_open_live %s\n", error_buffer_);
    return (pHandle_ != nullptr);
}

bool Sniffer::promiscModeOff()
{
    if (pHandle_ != nullptr)
        pcap_close(pHandle_);
    return true;
}

bool Sniffer::waitForPacket()
{
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
