#pragma once

#include <netinet/tcp.h>

#include "ip.h"
#include <string>

// TcpSocket - stream sockets classes will be derived from it

class TcpSocket
{
public:
    TcpSocket(int af);
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
    int af_; // address family
};

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

class TcpServerSocket : public TcpSocket
{
    typedef TcpSocket inherited;
public:
    TcpServerSocket(int af);
    virtual ~TcpServerSocket();

public:

protected:
};
