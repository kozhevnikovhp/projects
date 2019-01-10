/*
 *  sniffer-raw-sockets.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

// Class Sniffer implementation

#include <string.h>
#include "sniffer-raw-sockets.h"


SnifferRawSockets::SnifferRawSockets(const std::string &ifaceName)
    : Sniffer(ifaceName)
{
    socket_ = -1;
}

//virtual
SnifferRawSockets::~SnifferRawSockets()
{
    if (socket_ > 0)
    {
        promiscModeOff();
    }
}

// virtual
bool SnifferRawSockets::promiscModeOn(bool bLog)
{
    socket_ = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (socket_ <= 0)
        perror("Sniffer socket creation"); // TODO: to LOG!

    if (socket_ <= 0)
        return false;

    struct ifreq iface;
    strncpy(iface.ifr_name, ifaceName_.c_str(), IFNAMSIZ-1);

    int ec = ioctl(socket_, SIOCGIFFLAGS, &iface);
    if (ec < 0)
    {
        if (bLog)
            perror("ioctl SIOCGIFFLAGS"); // TODO: to LOG!
        return false;
    }
    iface.ifr_flags |= IFF_PROMISC;
    ec = ioctl(socket_, SIOCSIFFLAGS, &iface);
    if (ec < 0)
        if (bLog)
            perror("ioctl SIOCSIFFLAGS"); // TODO: to LOG!
    return (ec >= 0);
}

// virtual
bool SnifferRawSockets::promiscModeOff()
{
    if (socket_ <= 0)
        return false;

    struct ifreq iface;
    strncpy(iface.ifr_name, ifaceName_.c_str(), IFNAMSIZ-1);

    int ec = ioctl(socket_, SIOCGIFFLAGS, &iface);
    if (ec < 0)
    {
        perror("ioctl SIOCGIFFLAGS");
        return false;
    }
    iface.ifr_flags &= ~IFF_PROMISC;
    ec = ioctl(socket_, SIOCSIFFLAGS, &iface);
    if (ec < 0)
        perror("ioctl SIOCSIFFLAGS");

    ::shutdown(socket_, SHUT_RDWR);
    ::close(socket_);
    socket_ = -1;

    return (ec >= 0);
}

//virtual
bool SnifferRawSockets::doWaitForPacket(ethhdr *&pEthernetHeader, void *&pPayload, unsigned int &nPayloadLen)
{
    pEthernetHeader = nullptr;
    pPayload = nullptr;
    nPayloadLen = 0;

    if (socket_ <= 0)
        return false;

    struct sockaddr src;
    socklen_t fromlen = sizeof(src);
    ssize_t nPacketSize = recvfrom(socket_, bufferForPackets_, sizeof(bufferForPackets_), 0, &src, &fromlen);

    if (nPacketSize == SOCKET_ERROR)
    {
        perror("recvfrom");
        return false;
    }

    pEthernetHeader = (struct ethhdr *)bufferForPackets_;
    pPayload = (pEthernetHeader + 1);
    nPayloadLen = (unsigned int)nPacketSize - sizeof(struct ethhdr);

    return true; // successfully read and processed
}
