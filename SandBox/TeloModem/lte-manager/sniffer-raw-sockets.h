/*
 *  sniffer-raw-sockets.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include "sniffer.h"

class SnifferRawSockets : public Sniffer
{
public:
    SnifferRawSockets(const std::string &ifaceName);
    virtual ~SnifferRawSockets();

    virtual bool promiscModeOn(bool bLog);
    virtual bool promiscModeOff();
    virtual bool isListening() const { return (socket_ > 0); }
    virtual int getSelectableFd() const { return socket_; }
    virtual bool doWaitForPacket(struct ethhdr *&pEthernetHeader, void *&pPayload, unsigned int &nPayloadLen);

protected:
    int socket_;
    char bufferForPackets_[0xFFFF];
};

