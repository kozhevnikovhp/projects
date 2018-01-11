#ifndef __SNIFFER_SOCKETS_H_INCLUDED__
#define __SNIFFER_SOCKETS_H_INCLUDED__

#include "raw.h"
//#include "icmp.h"

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
    bool bind(IPADDRESS_TYPE = INADDR_ANY);
    bool enablePromiscMode();
    bool disablePromiscMode();
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
    unsigned char m_szBufferForPackets[0xFFFF];
};

}
}

#endif // __SNIFFER_SOCKETS_H_INCLUDED__
