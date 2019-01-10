/*
 *  sniffer-pcap.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include "sniffer.h"
#include <pcap.h>

class SnifferPcap : public Sniffer
{
public:
    SnifferPcap(const std::string &ifaceName);
    virtual ~SnifferPcap();

    pcap_t *getHandle() const { return pHandle_; }

    virtual bool promiscModeOn(bool bLog);
    virtual bool promiscModeOff();
    virtual bool isListening() const { return (pHandle_ != nullptr); }
    virtual int getSelectableFd() const { return pcap_get_selectable_fd(pHandle_); }
    virtual bool doWaitForPacket(struct ethhdr *&pEthernetHeader, void *&pPayload, unsigned int &nPayloadLen);

protected:
    pcap_t *pHandle_;
    bool bHasEthernetHeader_;
    char error_buffer_[PCAP_ERRBUF_SIZE];
};

