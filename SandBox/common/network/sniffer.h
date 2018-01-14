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
    virtual bool OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength) { return false; } // returns TRUE if packet processing ended and no need to process lower-protocol data (UDP, TCP, ICMP etc.)
    virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
    virtual void OnIgmpPacket(SIpHeader *pIpHeader, SIgmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
    virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
    virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
    virtual void OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}

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
