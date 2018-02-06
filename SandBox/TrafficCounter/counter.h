#ifndef __COUNTER_H_INCLUDED__
#define __COUNTER_H_INCLUDED__

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include "sniffer.h"

class ServiceStat
{
public:
    ServiceStat();

    void update(int nPacketSize);

    std::string &hostName(IPADDRESS_TYPE IP);

    std::string &serviceName(IPADDRESS_TYPE IP, IPPORT portNo, bool bUDP);

    long long getPackets() const { return nPackets_; }
    long long getOctets() const { return nOctets_; }

protected:
    long long nPackets_;
    long long nOctets_;
    bool bTriedToResolveHostName_; // try only once, can be time consuming
    bool bTriedToResolveServiceName_; // try only once, can be time consuming
    std::string hostName_;
    std::string serviceName_;
};

class ProtocolStat
{
public:
    ProtocolStat();

    void report(const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime, bool bFirstTime) const;

    void update(unsigned int nPacketSize, bool bInput);

protected:
    long long nInputPackets_;
    long long nOutputPackets_;
    long long nInputOctets_;
    long long nOutputOctets_;
};

typedef unsigned long INODE;
typedef std::map<INODE, std::string> InodeToAppCache;

typedef std::pair<IPADDRESS_TYPE, IPPORT> Service;
typedef std::map<Service, INODE> ServiceToInodeCache;

typedef std::tuple<IPADDRESS_TYPE, IPPORT, unsigned char, std::string> ServiceApp;
typedef std::pair<const ServiceApp *, ServiceStat *> Talker;

class TrafficCounter : public SnifferSocket
{
public:
    TrafficCounter();

    bool listenTo(const std::string &ifaceName);
    bool listenTo(IPADDRESS_TYPE teloIP);

// Protected methods
protected:
    bool isPacketOfInterest(const SIpHeader *pIpHeader) const;
    void reportStatistics(bool bFirstTime);

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen);
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload, int nPayloadLen);

    INODE getInode(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader);

    void updateTopTalkers(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader);

    void updateInodeAppCache();

// Protected members
protected:
    IPADDRESS_TYPE subnetMask_, teloIP_;
    ProtocolStat IpStatTotal_, IpStatLast_;
    ProtocolStat UdpStatTotal_, UdpStatLast_;
    ProtocolStat TcpStatTotal_, TcpStatLast_;
    ProtocolStat IcmpStatTotal_, IcmpStatLast_;
    ProtocolStat IgmpStatTotal_, IgmpStatLast_;

    std::map<ServiceApp, ServiceStat> servicesStat_;
    std::vector<Talker> topTalkers_;

    InodeToAppCache inodeToAppCache_;
    ServiceToInodeCache serviceToInodeCache_;

    unsigned int lastStatTime_;
    bool bPacketOfInterest_;
    bool bInputPacket_;
};

#endif // COUNTER_H
