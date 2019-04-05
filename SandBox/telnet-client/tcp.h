#pragma once

#include <netinet/tcp.h>

#include "ip.h"

class TcpSocket
{
public:
    TcpSocket(sa_family_t af);
    virtual ~TcpSocket();

public:
    bool isCreated() const { return (socket_ >= 0); }

    // Common socket methods
    bool setOption(int level, int optionName, void *pcValue, socklen_t optionLength);
    bool getOption(int level, int optionName, void *pcValue, socklen_t &optionLength);

    bool enableKeepAlive(bool bEnable);
    bool isKeepAliveEnabled(bool &bEnabled);
    bool setKeepAliveTime(int t);
    bool getKeepAliveTime(int &t);

    bool recv(void *pBufferToFill, size_t nBufferSize, ssize_t &nReadBytes);
    bool send(const void *pBufferToSend, size_t nBytesToSend, ssize_t &nSentBytes);

protected:
    int socket_;
    sa_family_t af_; // address family
};


