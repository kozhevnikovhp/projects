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

    void setIfaceName(const std::string &ifaceName) { ifaceName_ = ifaceName; }
    void update(int nPacketSize);

    std::string &hostName(IPADDRESS_TYPE IP);

    std::string &serviceName(IPADDRESS_TYPE IP, IPPORT portNo, bool bUDP);

    int getPackets() const { return nPackets_; }
    int getBytes() const { return nBytes_; }

protected:
    int nPackets_;
    int nBytes_;
    bool bTriedToResolveHostName_; // try only once, can be time consuming
    bool bTriedToResolveServiceName_; // try only once, can be time consuming
    std::string ifaceName_;
    std::string hostName_;
    std::string serviceName_;
};

class ProtocolStat
{
public:
    ProtocolStat();

    void report(const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime, bool bFirstTime) const;

    void update(unsigned int nPacketSize, bool bInput);

    int getInputBytes() const { return nInputBytes_; }
    int getOutputBytes() const { return nOutputBytes_; }

protected:
    int nInputPackets_;
    int nOutputPackets_;
    int nInputBytes_;
    int nOutputBytes_;
};

typedef unsigned long INODE;
typedef std::map<INODE, std::string> InodeToAppCache;

typedef std::pair<IPADDRESS_TYPE, IPPORT> Service;
typedef std::map<Service, INODE> ServiceToInodeCache;

typedef std::tuple<std::string, IPADDRESS_TYPE, IPPORT, unsigned char, std::string> ServiceApp;
typedef std::pair<const ServiceApp *, ServiceStat *> Talker;

class TrafficCounter : public SnifferSocket
{
public:
    TrafficCounter(const std::string &ifaceName);

    bool listen();
    void reportStatistics(bool bFirstTime);

// Protected methods
protected:
    bool isTeloPacket(const SIpHeader *pIpHeader) const;
    bool isLanPacket(const SIpHeader *pIpHeader) const;

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen);
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload, int nPayloadLen);

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
    ProtocolStat LanStatTotal_, LanStatLast_;
    std::string ifaceName_;

    std::map<ServiceApp, ServiceStat> servicesStat_;
    std::vector<Talker> topTalkers_;

    InodeToAppCache inodeToAppCache_;
    ServiceToInodeCache serviceToInodeCache_;

    unsigned int lastStatTime_;
    unsigned int startTime_;
};

#endif // COUNTER_H
