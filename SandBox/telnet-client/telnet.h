#pragma once

#include "tcp-client.h"

class TelnetSocket : public TcpClientSocket
{
    typedef TcpClientSocket inherited;
public:
    TelnetSocket(int af);
    virtual ~TelnetSocket();

public:

protected:
};

