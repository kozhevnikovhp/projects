/*
 *  traffic-counter.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include "sniffer-pcap.h"
#include "sniffer-raw-sockets.h"
#include "misc.h"

class TrafficStatistics
{
public:
    TrafficStatistics();

    void update(unsigned int nPacketSize, bool bInput);
    void clear()
    {
        nInputPackets_ = nOutputPackets_ = nInputBytes_ = nOutputBytes_ = 0;
    }
    int getInputBytes() const { return nInputBytes_; }
    int getOutputBytes() const { return nOutputBytes_; }

protected:
    int nInputPackets_;
    int nOutputPackets_;
    int nInputBytes_;
    int nOutputBytes_;
};

#ifdef USE_PCAP
#define BaseSniffer SnifferPcap
#else
#define BaseSniffer SnifferRawSockets
#endif

class InterfaceTrafficCounter : public BaseSniffer
{
public:
    InterfaceTrafficCounter(const std::string &ifaceName);

    bool startListening();
    bool stopListening();
    int getUserInputBytes() const { return UserStat_.getInputBytes(); }
    int getUserOutputBytes() const { return UserStat_.getOutputBytes(); }
    int getTeloInputBytes() const { return TeloStat_.getInputBytes(); }
    int getTeloOutputBytes() const { return TeloStat_.getOutputBytes(); }
    void clearStatistics();
    bool checkForInterface() const;

protected:
    bool isMyPacket(const SIpHeader *pIpHeader) const { return (isPacketFromMe(pIpHeader) || isPacketToMe(pIpHeader)); }
    bool isLanPacket(const SIpHeader *pIpHeader) const { return isTheSameSubnet(pIpHeader->sourceIP, pIpHeader->destIP, subnetMask_); }
    bool isPacketToMe(const SIpHeader *pIpHeader) const { return (getIP() == pIpHeader->destIP); }
    bool isPacketFromMe(const SIpHeader *pIpHeader) const { return (getIP() == pIpHeader->sourceIP); }
    IPADDRESS_TYPE getIP() const { return teloIP_; }

    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const void *pPayload,  unsigned int nPayloadLen);

    IPADDRESS_TYPE subnetMask_, teloIP_;
    TrafficStatistics TeloStat_, UserStat_;
    int nInterfaceDoesntExistReported_;
    int nCouldntListenReported_;
};

class TrafficCounter
{
public:
    TrafficCounter();

    void addInterface(const char *pszInterfaceName);
    bool hasJob() const { return !interfaces_.empty(); }
    bool startListening();
    bool stopListening();
    int doJob();

    int getUserInputBytes() const;
    int getUserOutputBytes() const;
    int getTeloInputBytes() const;
    int getTeloOutputBytes() const;
    void clearStatistics();

protected:
    std::vector<InterfaceTrafficCounter> interfaces_;
};

