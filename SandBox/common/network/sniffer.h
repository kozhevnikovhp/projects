#ifndef __SNIFFER_H_INCLUDED__
#define __SNIFFER_H_INCLUDED__

#include "raw.h"

#if (SOCKETS_BSD)
#include <net/if.h>
#endif

namespace common {

namespace network {

class SnifferSocket : public IpRawSocket
{
    typedef IpRawSocket inherited;
//Attributes
public:
    SnifferSocket();
    virtual ~SnifferSocket();
// Public methods
// Public overridables
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
    bool  open();

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload,  int nPayloadLen) { }
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen) {}
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload, int nPayloadLen) {}

// Protected members
protected:
    unsigned char bufferForPackets_[0xFFFF];
#if (SOCKETS_BSD)
    struct ifreq ifaceDesc_;
#endif
};

}
}

#endif // __SNIFFER_H_INCLUDED__
