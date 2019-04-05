// Class TcpSocket implementation

#include "tcp.h"
#include "misc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TcpSocket

TcpSocket::TcpSocket(sa_family_t af) : af_(af)
{
    socket_ = socket(af, SOCK_STREAM, 0);
}

//virtual
TcpSocket::~TcpSocket()
{
    close(socket_);
}

bool TcpSocket::recv(void *pBufferToFill, size_t nBufferSize, ssize_t &nReadBytes)
{
    if (!isCreated())
		return false;
    nReadBytes = ::recv(socket_, (char *)pBufferToFill, nBufferSize, 0);
	if (nReadBytes == SOCKET_ERROR || nReadBytes == 0)
	{
        perror("recv");
        return false;
	}
	return true;
}

bool TcpSocket::send(const void *pBufferToSend, size_t nBytesToSend, ssize_t &nSentBytes)
{
    if (!isCreated())
		return false;
    nSentBytes = ::send(socket_, (const char *)pBufferToSend, nBytesToSend, 0);
	if (nSentBytes == SOCKET_ERROR)
	{
        perror("send");
		return false;
	}
	return true;
}

bool TcpSocket::setOption(int level, int optionName, void *pcValue, socklen_t optionLength)
{
    if ((setsockopt(socket_, level, optionName, pcValue, optionLength)) == SOCKET_ERROR)
    {
        perror("setsockopt");
        return false;
    }
    return true;
}

bool TcpSocket::getOption(int level, int optionName, void *pcValue, socklen_t &optionLength)
{
    if ((getsockopt(socket_, level, optionName, pcValue, &optionLength)) == SOCKET_ERROR)
    {
        //perror("getsockopt");
        return false;
    }
    return true;
}

bool TcpSocket::enableKeepAlive(bool bEnable)
{
    int value = bEnable ? 1 : 0;
    return setOption(SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(value));
}

bool TcpSocket::isKeepAliveEnabled(bool &bEnabled)
{
    int value = 0;
    socklen_t len;
    bool bSuccess = getOption(SOL_SOCKET, SO_KEEPALIVE, &value, len);
    if (bSuccess)
        bEnabled = (value == 1) ? true : false;
    return bSuccess;
}

bool TcpSocket::setKeepAliveTime(int t)
{
#ifdef TCP_KEEPIDLE // unavailable on some platforms say SunOS/Solaris
    return setOption(SOL_TCP, TCP_KEEPIDLE, &t, sizeof(t));
#else
	return false;
#endif
}

bool TcpSocket::getKeepAliveTime(int &t)
{
#ifdef TCP_KEEPIDLE // unavailable on some platforms say SunOS/Solaris
    socklen_t len;
    return getOption(SOL_TCP, TCP_KEEPIDLE, &t, len);
#else
	return false;
#endif
}



