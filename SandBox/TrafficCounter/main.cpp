#include <stdio.h>
#include "network/sniffer.h"
#include "network/misc.h"

using namespace common::network;


class ListenerSocket : public SnifferSocket
{
//Attributes
public:
    ListenerSocket(IPADDRESS_TYPE myAddress)
    {
        myAddress_ = myAddress;
        m_nPackets = 0;
    }
    virtual ~ListenerSocket() {}

// Public methods
// Public overridables
// Public members

// Protected methods
protected:

// Protected overridables
protected:
    virtual bool OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        ++m_nPackets;
        return false;
    }
    virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        printf("%d ICMP\tlen = %5d (from %s\t to %s)\n", m_nPackets, ntohs(pIpHeader->total_len),
                    addressToDotNotation(pIpHeader->sourceIP).c_str(),
                    addressToDotNotation(pIpHeader->destIP).c_str());
    }
    virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        if (pIpHeader->sourceIP == myAddress_)
            return; // do nothing
        if (pIpHeader->destIP == myAddress_)
            return; // do nothing
        unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);
        printf("%d TCP:%5d/%5d len = %5d (from %s\t to %s)\n", m_nPackets, SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
    }
    virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        if (pIpHeader->sourceIP == myAddress_)
            return; // do nothing
        if (pIpHeader->destIP == myAddress_)
            return; // do nothing
        unsigned short SrcPortNo = ntohs(pUdpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pUdpHeader->DstPortNo);
        printf("%d UDP:%5d/%5d len = %5d (from %s\t to %s)\n", m_nPackets, SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
    }
    virtual void OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        printf("%d UNKNOWN: PROTO=%d, %s -> %s\n", m_nPackets, pIpHeader->proto,
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
    }

// Protected members
protected:
    unsigned int m_nPackets;
    char szFrom[64], szTo[64];
    IPADDRESS_TYPE myAddress_;
};

int main(int argc, char* argv[])
{
    printf("Hello\n");
    if (argc < 2)
    {
        printf("Not enough arguments\n");
        return 0;
    }

    IPADDRESS_TYPE ip = StringToAddress(argv[1]);
    if (!ip)
    {
        printf("Cannot resolve name %s to IP-address\n", argv[1]);
        return 0;
    }


    ListenerSocket sniffer(ip);
    if (!sniffer.isCreated())
    {
        printf("Abnormal termination!\n");
        return 1;
    }

    sniffer.bind(ip);
    sniffer.enablePromiscMode();

    printf("Listening %s...\n", addressToDotNotation(ip).c_str());
    while (1)
    {
        if (sniffer.waitForPacket())
        {
        }
    }

    return 0;
}
