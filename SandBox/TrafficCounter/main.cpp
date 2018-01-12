#include <stdio.h>
#include <string.h>
#include <time.h>
#include "network/sniffer.h"
#include "network/misc.h"
#include "portable/portable.h"

using namespace common::network;

class ProtocolStat
{
public:
    ProtocolStat()
        : nInputPackets_(0), nOutputPackets_(0), nInputOctets_(0), nOutputOctets_()
    { }

    void report(const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime) const
    {
        FILE *pFile = fopen(pszFileName, "a");
        if (pFile)
        {
            time_t rawtime;
            char strTime[80];

            time(&rawtime);
            struct tm *pTimeInfo = localtime(&rawtime);

            strftime(strTime, sizeof(strTime), "%d.%m.%Y %H:%M:%S", pTimeInfo);
            // Absolute numbers
            fprintf(pFile, "%s\t\t%lld\t%lld\t%lld\t%lld", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
            fprintf(stdout, "%s\t\t%lld\t%lld\t%lld\t%lld", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
            // Difference with previous one
            fprintf(pFile, "\t\t%lld\t%lld\t%lld\t%lld", nInputPackets_-prev.nInputPackets_, nInputOctets_-prev.nInputOctets_, nOutputPackets_-prev.nOutputPackets_, nOutputOctets_-prev.nOutputOctets_);
            // Speed (/sec)
            fprintf(pFile, "\t\t%.2f\t%.2f\t%.2f\t%.2f", (nInputPackets_-prev.nInputPackets_)/double(deltaTime), (nInputOctets_-prev.nInputOctets_)/double(deltaTime), (nOutputPackets_-prev.nOutputPackets_)/double(deltaTime), (nOutputOctets_-prev.nOutputOctets_)/double(deltaTime));

            fprintf(pFile, "\n");
            fprintf(stdout, "\n");
        }
        fclose(pFile);
    }

    void update(unsigned int nPacketSize, bool bInput)
    {
        if (bInput)
        {
            ++nInputPackets_;
            nInputOctets_ += nPacketSize;
        }
        else
        {
            ++nOutputPackets_;
            nOutputOctets_ += nPacketSize;
        }
    }
protected:
    long long nInputPackets_;
    long long nOutputPackets_;
    long long nInputOctets_;
    long long nOutputOctets_;
};

class ListenerSocket : public SnifferSocket
{
//Attributes
public:
#if (WIN32)
    ListenerSocket(IPADDRESS_TYPE ifaceIP, IPADDRESS_TYPE ifaceMask, IPADDRESS_TYPE teloIP)
    {
        subnetMask_ = ifaceMask;
        teloIP_ = teloIP;
        lastStatTime_ = 0;
        printf("Listening %s...\n", addressToDotNotation(ifaceIP).c_str());
        promiscModeOn(ifaceIP);
    }
#elif (UNIX)
    ListenerSocket(IPADDRESS_TYPE teloIP)
    {
        //subnetMask_ = ifaceMask;
        teloIP_ = teloIP;
        lastStatTime_ = 0;
        //printf("Listening %s...\n", pszIfaceName);
        //promiscModeOn(pszIfaceName);
    }

#endif

// Public methods
// Public overridables
// Public members

// Protected methods
protected:
    bool isPacketOfInterest(const SIpHeader *pIpHeader) const
    {
        if ((pIpHeader->sourceIP != teloIP_) && (pIpHeader->destIP != teloIP_))
            return false; // Telo traffic only
        if (isTheSameSubnet(pIpHeader->sourceIP, pIpHeader->destIP, subnetMask_))
            return false; // Skip LAN traffic
        return true;
    }
    void reportStatistics()
    {
        unsigned int currentTime = common::portable::portableGetCurrentTimeSec();
        unsigned int deltaTime = currentTime - lastStatTime_;
        if (deltaTime < 60*1) // every 1 min
            return; // too early to do something
        IpStatTotal_.report("ip.txt", IpStatLast_, deltaTime);
        IcmpStatTotal_.report("icmp.txt", IcmpStatLast_, deltaTime);
        TcpStatTotal_.report("tcp.txt", TcpStatLast_, deltaTime);
        UdpStatTotal_.report("udp.txt", UdpStatLast_, deltaTime);

        lastStatTime_ = currentTime;
        memcpy(&IpStatLast_, &IpStatTotal_, sizeof(ProtocolStat));
        memcpy(&IcmpStatLast_, &IcmpStatTotal_, sizeof(ProtocolStat));
        memcpy(&TcpStatLast_, &TcpStatTotal_, sizeof(ProtocolStat));
        memcpy(&UdpStatLast_, &UdpStatTotal_, sizeof(ProtocolStat));
    }

// Protected overridables
protected:
    virtual bool OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        reportStatistics();
        if (!isPacketOfInterest(pIpHeader))
                return true; // true means "processed", no any other processing needed
        bool bInput = (pIpHeader->destIP == teloIP_);
        unsigned int nPacketSize = pIpHeader->h_len*4+nUserDataLength; // recalculate it :-(
        IpStatTotal_.update(nPacketSize, bInput);
        return false;
    }
    virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        printf("ICMP\tlen = %5d (from %s\t to %s)\n", ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        bool bInput = (pIpHeader->destIP == teloIP_);
        unsigned int nPacketSize = pIpHeader->h_len*4+nUserDataLength; // recalculate it :-(
        IcmpStatTotal_.update(nPacketSize, bInput);
    }
    virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);
        printf("TCP:%5d/%5d len = %5d (from %s\t to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        bool bInput = (pIpHeader->destIP == teloIP_);
        unsigned int nPacketSize = pIpHeader->h_len*4+nUserDataLength; // recalculate it :-(
        TcpStatTotal_.update(nPacketSize, bInput);
    }
    virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        unsigned short SrcPortNo = ntohs(pUdpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pUdpHeader->DstPortNo);
        printf("UDP:%5d/%5d len = %5d (from %s\t to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        bool bInput = (pIpHeader->destIP == teloIP_);
        unsigned int nPacketSize = pIpHeader->h_len*4+nUserDataLength; // recalculate it :-(
        UdpStatTotal_.update(nPacketSize, bInput);
    }
    virtual void OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        printf("UNKNOWN: PROTO=%d, %s -> %s\n", pIpHeader->proto,
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
    }

// Protected members
protected:
    IPADDRESS_TYPE subnetMask_, teloIP_;
    ProtocolStat IpStatTotal_, IpStatLast_;
    ProtocolStat UdpStatTotal_, UdpStatLast_;
    ProtocolStat TcpStatTotal_, TcpStatLast_;
    ProtocolStat IcmpStatTotal_, IcmpStatLast_;
    unsigned int lastStatTime_;
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Not enough arguments\nUsage: TrafficCounter TeloIP\n");
        return 0;
    }
#if (WIN32)
    IpSocket::InitSockets();

    IPADDRESS_TYPE ifaceIP = stringToAddress(argv[1]);
    if (!ifaceIP)
    {
        printf("Cannot resolve %s to IP-address\n", argv[1]);
        return 0;
    }

    IPADDRESS_TYPE subnetMask = stringToAddress(argv[2]);
    if (!subnetMask)
    {
        printf("Cannot resolve subnet mask %s\n", argv[2]);
        return 0;
    }

    IPADDRESS_TYPE teloIP = stringToAddress(argv[3]);
    if (!teloIP)
    {
        printf("Cannot resolve %s to IP-address\n", argv[3]);
        return 0;
    }

    ListenerSocket sniffer(ifaceIP, subnetMask, teloIP);
#elif (UNIX)

#endif
    if (!sniffer.isCreated())
    {
        printf("Abnormal termination!\n");
        return 1;
    }

    while (1)
    {
        if (sniffer.waitForPacket())
        {
        }
    }

    return 0;
}
