#pragma once

#include "tcp.h"

class TcpServerSocket : public TcpSocket
{
    typedef TcpSocket inherited;
public:
    TcpServerSocket(int af);
    virtual ~TcpServerSocket();

public:

protected:
};
