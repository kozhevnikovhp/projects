/*
 *  sniffer.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <linux/if_ether.h>

#include "ip.h"

class Sniffer
{
public:
    Sniffer(const std::string &ifaceName);
    virtual ~Sniffer();

    bool waitForPacket();

    virtual bool promiscModeOn(bool bLog) = 0;
    virtual bool promiscModeOff() = 0;
    virtual int getSelectableFd() = 0;
    virtual bool doWaitForPacket(struct ethhdr *&pEthernetHeader, void *&pPayload, unsigned int &nPayloadLen) = 0;

// Protected methods
protected:

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const void *pIpPayload,  unsigned int nIpPayloadLen) {}
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const void *pIcmpPayload, unsigned int nIcmpPayloadLen) {}
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const void *pIgmpPayload, unsigned int nIgmpPayloadLen) {}
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const void *pTcpPayload, unsigned int nTcpPayloadLen) {}
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const void *pUdpPayload, unsigned int nUdpPayloadLen) {}
    virtual void ipUnknownProtoPacketCaptured(const SIpHeader *pIpHeader, const void *pIpPayload, unsigned int nIpPayloadLen) {}

// Protected members
protected:
    std::string ifaceName_;
};

