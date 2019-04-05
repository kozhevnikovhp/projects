// Class TcpClientSocket implementation

#include "tcp-client.h"
#include "misc.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TcpClientSocket

TcpClientSocket::TcpClientSocket(int af) : inherited(af), bConnected_(false)
{
}

//virtual
TcpClientSocket::~TcpClientSocket()
{
    disconnect();
}

bool TcpClientSocket::connect(const std::string &IpAddress, IPPORT portNo)
{
    connectionName_ = IpAddress;
    IPADDRESS_TYPE IP = dotNotationToAddress(IpAddress.c_str());
    if (0 == IP)
        return false;
    return connect(IP, portNo);
}

bool TcpClientSocket::connect(IPADDRESS_TYPE IP, IPPORT portNo)
{
    if (bConnected_)
        return true; // Do nothing
    bConnected_ = false;
    struct sockaddr_in socketParams;
    socketParams.sin_family = af_;
    socketParams.sin_port = htons(portNo);
    socketParams.sin_addr.s_addr = IP;
    if (::connect(socket_, (sockaddr *)&socketParams, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        perror("connect");
        return false;
    }
    bConnected_ = true;
    onConnected();
    return true;
}

void TcpClientSocket::disconnect()
{
    if (isConnected())
    {
        onDisconnected();
        bConnected_ = false;
    }
}

