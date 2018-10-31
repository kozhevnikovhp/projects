/*
 *  traffic-counter.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/poll.h>

#include <algorithm>

#include "traffic-counter.h"
#include "log.h"

/////////////////////////////////////////////////////////////////////////
/// TrafficStatistics::TrafficStatistics
///
TrafficStatistics::TrafficStatistics()
    : nInputPackets_(0), nOutputPackets_(0), nInputBytes_(0), nOutputBytes_(0)
{
}

void TrafficStatistics::update(unsigned int nPacketSize, bool bInput)
{
    if (bInput)
    {
        ++nInputPackets_;
        nInputBytes_ += nPacketSize;
    }
    else
    {
        ++nOutputPackets_;
        nOutputBytes_ += nPacketSize;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// InterfaceTrafficCounter

InterfaceTrafficCounter::InterfaceTrafficCounter(const std::string &ifaceName)
    : BaseSniffer(ifaceName)
{
    teloIP_ = 0;
    subnetMask_ = 0;
    nInterfaceDoesntExistReported_ = nCouldntListenReported_ = 0;
}

bool InterfaceTrafficCounter::startListening()
{
    if (!isItInterfaceName(ifaceName_))
    {
        if (nInterfaceDoesntExistReported_ < 5)
            log_error("Interface '%s' does not exist\n", ifaceName_.c_str());
        ++nInterfaceDoesntExistReported_;
        return false;
    }
    nInterfaceDoesntExistReported_ = 0;
    bool bSuccess = getInterfaceAddressAndMask(ifaceName_, teloIP_, subnetMask_);
    if (bSuccess)
        bSuccess = promiscModeOn(nCouldntListenReported_ < 5);
    if (bSuccess)
    {
        log_info("Start listening to interface %s", ifaceName_.c_str());
        nCouldntListenReported_ = 0;
    }
    else
    {
        if (nCouldntListenReported_ < 5)
            log_error("Could not listen to interface %s\n", ifaceName_.c_str());
        ++nCouldntListenReported_;
    }

    return bSuccess;
}

bool InterfaceTrafficCounter::stopListening()
{
    log_info("Stop listening to interface %s", ifaceName_.c_str());
    promiscModeOff();
    return true;
}

//virtual
void InterfaceTrafficCounter::ipPacketCaptured(const SIpHeader *pIpHeader, const void *pIpPayload,  unsigned int nIpPayloadLen)
{
    if (isMyPacket(pIpHeader))
        TeloStat_.update(pIpHeader->getPacketLen(), isPacketToMe(pIpHeader));
    else
        UserStat_.update(pIpHeader->getPacketLen(), isPacketToMe(pIpHeader));
}

void InterfaceTrafficCounter::clearStatistics()
{
    TeloStat_.clear();
    UserStat_.clear();
}

///////////////////////////////////////////////////////
// TrafficCounter

TrafficCounter::TrafficCounter()
    : bListening_(false)
{
}

void TrafficCounter::addInterface(const char *pszInterfaceName)
{
    interfaces_.emplace_back(pszInterfaceName);
}

bool TrafficCounter::startListening()
{
    for (auto &iface : interfaces_)
    {
        if (!iface.startListening())
            return false;
    }
    bListening_ = true;
    return true;
}

bool TrafficCounter::stopListening()
{
    for (auto &iface : interfaces_)
        iface.stopListening();
    bListening_ = false;
    return true;
}

int TrafficCounter::doJob()
{
    const int OK = 0;
    const int NotOK = 1;
    // initial check
    if (!hasJob())
        return NotOK;

    struct pollfd fds[256];
    nfds_t nfds = 0;
    memset(fds, 0, sizeof(fds));

    for (auto &iface : interfaces_)
    {
        fds[nfds].fd = iface.getSelectableFd();
        fds[nfds].events = POLLIN;
        ++nfds;
    }
    const int timeout = 10; // 10 msec

    bool bContinue = true;
    while (bContinue)
    {
        int rc = poll(fds, nfds, timeout);
        if (rc > 0)
        {
            for (nfds_t i = 0; i < nfds; ++i)
            {
                if (fds[i].revents == POLLIN)
                    interfaces_[i].waitForPacket();
                fds[i].revents = 0;
            }
            bContinue = true; // is anybody else in there?
        }
        else if (rc < 0)
        {
            perror("  poll() failed");
            bContinue = false;
        }
        else
            bContinue = false; // 0 means "timeout expired -> do nothing and exit
    }
    return OK;
}

int TrafficCounter::getUserInputBytes() const
{
    int nBytes = 0;
    for (auto &iface : interfaces_)
        nBytes += iface.getUserInputBytes();
    return nBytes;
}

int TrafficCounter::getUserOutputBytes() const
{
    int nBytes = 0;
    for (auto &iface : interfaces_)
        nBytes += iface.getUserOutputBytes();
    return nBytes;
}

int TrafficCounter::getTeloInputBytes() const
{
    int nBytes = 0;
    for (auto &iface : interfaces_)
        nBytes += iface.getTeloInputBytes();
    return nBytes;
}

int TrafficCounter::getTeloOutputBytes() const
{
    int nBytes = 0;
    for (auto &iface : interfaces_)
        nBytes += iface.getTeloOutputBytes();
    return nBytes;
}

void TrafficCounter::clearStatistics()
{
    for (auto &iface : interfaces_)
        iface.clearStatistics();
}


