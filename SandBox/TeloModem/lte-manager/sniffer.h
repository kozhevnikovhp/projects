#ifndef __SNIFFER_H_INCLUDED__
#define __SNIFFER_H_INCLUDED__

//#include "misc.h"
#include "ip.h"

#include <pcap.h>
#include <string>

class Sniffer
{
public:
    Sniffer(const std::string &ifaceName);
    virtual ~Sniffer();

    pcap_t *getHandle() const { return pHandle_; }
    bool isCreated() const { return (pHandle_ != NULL); }
    bool isDestroyed() const { return (pHandle_ == NULL); }
    void destroy();

// Public members
   bool promiscModeOn(const char *pszIfaceName);
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
    std::string ifaceName_;
    pcap_t *pHandle_;
    bool bHasEthernetHeader_;
    char bufferForPackets_[0xFFFF];
    char error_buffer_[PCAP_ERRBUF_SIZE];
};

#endif // __SNIFFER_H_INCLUDED__
