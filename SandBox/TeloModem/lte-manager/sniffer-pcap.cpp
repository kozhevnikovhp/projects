/*
 *  sniffer-pcap.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

// Class Sniffer implementation

#include "sniffer-pcap.h"

#include <net/if.h>
#include <linux/if_ether.h>

#include <string.h>
#include <stdio.h>
#include "log.h"

SnifferPcap::SnifferPcap(const std::string &ifaceName)
    : Sniffer(ifaceName), pHandle_(nullptr), bHasEthernetHeader_(false)
{
}

//virtual
SnifferPcap::~SnifferPcap()
{
    promiscModeOff();
}

// virtual
bool SnifferPcap::promiscModeOn()
{
    pHandle_ = pcap_open_live(ifaceName_.c_str(), BUFSIZ, 1, 1000, error_buffer_);
    if (pHandle_ != nullptr)
    {
        bHasEthernetHeader_ = false;
        if (pcap_datalink(pHandle_) == DLT_EN10MB)
            bHasEthernetHeader_ = true;
    }
    else
        log_error("pcap_open_live %s\n", error_buffer_);
    return (pHandle_ != nullptr);
}

// virtual
bool SnifferPcap::promiscModeOff()
{
    if (pHandle_ != nullptr)
        pcap_close(pHandle_);
    pHandle_ = nullptr;
    return true;
}

bool SnifferPcap::doWaitForPacket(ethhdr *&pEthernetHeader, void *&pPayload, unsigned int &nPayloadLen)
{
    pEthernetHeader = nullptr;
    pPayload = nullptr;
    nPayloadLen = 0;
    bool bSuccess = false;

    struct pcap_pkthdr *pHeader;	// The header that pcap gives us
    const u_char *pPacket;		// The actual packet

    int ec = pcap_next_ex(getHandle(), &pHeader, &pPacket);
    //printf("ec = %d\n");
    if (ec == 1)
    {
        nPayloadLen = pHeader->len;
        if (bHasEthernetHeader_)
        {
            pEthernetHeader = (struct ethhdr *)pPacket;
            pPayload = (void *)(pEthernetHeader+1);
            nPayloadLen -= sizeof(struct ethhdr);
        }
        else
        {
            pPayload = (void *)pPacket;
        }
        bSuccess = true;
    }

    return bSuccess;
}
