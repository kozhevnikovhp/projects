/*
 *  sniffer.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

// Class Sniffer implementation

#include "sniffer.h"

Sniffer::Sniffer(const std::string &ifaceName)
    : ifaceName_(ifaceName)
{
}

//virtual
Sniffer::~Sniffer()
{
}

bool Sniffer::waitForPacket()
{
    struct ethhdr *pEthernetHeader = nullptr;
    void *pPayload = nullptr;
    unsigned int nPayloadLen = 0;

    if (!doWaitForPacket(pEthernetHeader, pPayload, nPayloadLen))
        return false;

    SIpHeader *pIpHeader = nullptr;
    if (pEthernetHeader)
    {
        unsigned short proto = ntohs(pEthernetHeader->h_proto);
        if (proto == ETH_P_IP)
        {
            pIpHeader = (SIpHeader *)pPayload;
        }
        /*else if (proto == ETH_P_IPV6)
        {
            //printf("\t%s: IPv6\n", ifaceName_.c_str());
        }
        else if (proto == ETH_P_ARP)
        {
            //printf("\t%s: ARP\n", ifaceName_.c_str());
        }
        else if (proto == ETH_P_RARP)
        {
            //printf("\t%s: RARP\n", ifaceName_.c_str());
        }
        else
        {
            //printf("\t%s: unknown ethernet proto = 0x%04X, refer to file linux/if_ether.h and add implementation if needed\n", ifaceName_.c_str(), proto));
        }*/
    }
    else
    {
        pIpHeader = (SIpHeader *)pPayload;
    }

    if (pIpHeader)
    {
        void *pIpPayload = (char *)pPayload + pIpHeader->getHeaderLen();
        unsigned int nIpPayloadLen = nPayloadLen - pIpHeader->getHeaderLen();
        ipPacketCaptured(pIpHeader, pIpPayload, nIpPayloadLen);

        /*switch (pIpHeader->proto)
        {
        case IPPROTO_TCP: {
            //printf("TCP\n");
            STcpHeader *pTcpHeader = (STcpHeader *)pIpPayload;
            tcpPacketCaptured(pIpHeader, pTcpHeader, (char *)pTcpHeader+pTcpHeader->getHeaderLen(), nIpPayloadLen - pTcpHeader->getHeaderLen());
            break; }
        case IPPROTO_UDP: {
            //printf("UDP\n");
            SUdpHeader *pUdpHeader = (SUdpHeader *)pIpPayload;
            udpPacketCaptured(pIpHeader, pUdpHeader, pUdpHeader+1, nIpPayloadLen - sizeof(SUdpHeader));
            break; }
        case IPPROTO_ICMP: {
            //printf("ICMP\n");
            SIcmpHeader *pIcmpHeader = (SIcmpHeader *)pIpPayload;
            icmpPacketCaptured(pIpHeader, pIcmpHeader, pIcmpHeader+1, nIpPayloadLen - sizeof(SIcmpHeader));
            break; }
        case IPPROTO_IGMP: {
            //printf("IGMP\n");
            SIgmpHeader *pIgmpHeader = (SIgmpHeader *)pIpPayload;
            igmpPacketCaptured(pIpHeader, pIgmpHeader, pIgmpHeader+1, nIpPayloadLen - sizeof(SIgmpHeader));
            break; }
        default:
            //printf("UNKNOWN protocol 0x%04X\n", pIpHeader->proto);
            ipUnknownProtoPacketCaptured(pIpHeader, pIpPayload, nIpPayloadLen);
            break;
        } // end of switch (pIpHeader->proto)
        */
    }

    return true;
}
