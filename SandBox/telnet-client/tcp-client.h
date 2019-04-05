#pragma once

#include "tcp.h"
#include <string>

class TcpClientSocket : public TcpSocket
{
    typedef TcpSocket inherited;
public:
    TcpClientSocket(int af);
    virtual ~TcpClientSocket();

public:
    bool connect(IPADDRESS_TYPE IP, IPPORT portNo);
    bool connect(const std::string &IpAddress, IPPORT portNo);
    bool isConnected() const { return bConnected_; }
    const std::string &getConnectionName() const { return connectionName_; }
    void disconnect();

protected:
    virtual void onConnected() {}
    virtual void onDisconnected() {}

    bool bConnected_;
    std::string connectionName_;
};

