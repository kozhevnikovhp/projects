#ifndef __SNIFFER_H_INCLUDED__
#define __SNIFFER_H_INCLUDED__

#include "misc.h"
#include "ip.h"

class SnifferSocket
{
public:
    SnifferSocket();
    virtual ~SnifferSocket();

    bool isCreated() const { return (m_Socket != INVALID_SOCKET); }
    bool isDestroyed() const { return (m_Socket == INVALID_SOCKET); }
    void destroy();

// Public members
#if (SOCKETS_WSA)
    bool promiscModeOn(IPADDRESS_TYPE ifaceIP);
#elif (SOCKETS_BSD)
    bool promiscModeOn(const char *pszIfaceName);
#endif
    bool promiscModeOff();
    bool waitForPacket();

// Protected methods
protected:

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen) { }
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload, int nPayloadLen) {}

// Protected members
protected:
    SOCKET m_Socket;
    char bufferForPackets_[0xFFFF];
#if (SOCKETS_BSD)
    struct ifreq ifaceDesc_;
#endif
};

#endif // __SNIFFER_H_INCLUDED__
