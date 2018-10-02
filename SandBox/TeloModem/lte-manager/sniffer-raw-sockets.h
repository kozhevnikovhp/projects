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

    virtual bool promiscModeOn();
    virtual bool promiscModeOff();
    virtual int getSelectableFd() { return socket_; }
    virtual bool doWaitForPacket(struct ethhdr *&pEthernetHeader, void *&pPayload, unsigned int &nPayloadLen);

protected:

    int socket_;
    char bufferForPackets_[0xFFFF];
};

