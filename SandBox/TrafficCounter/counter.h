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

    void report(const std::string &ifaceName, const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime, bool bFirstTime) const;

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

class InterfaceTrafficCounter : public Sniffer
{
public:
    InterfaceTrafficCounter(const std::string &ifaceName, IPADDRESS_TYPE IP = 0);

    bool listen();
    void reportStatistics(bool bFirstTime, unsigned int deltaTime);
    void reportOverallStat(FILE *pFile, unsigned int totalTime);
    void fillTopTalkers(std::vector<Talker> &topTalkers, bool bLAN);

    std::map<ServiceApp, ServiceStat> &getStatistics(bool bLAN);

// Protected methods
protected:
    bool isMyPacket(const SIpHeader *pIpHeader) const { return (isPacketFromMe(pIpHeader) || isPacketToMe(pIpHeader)); }
    bool isLanPacket(const SIpHeader *pIpHeader) const { return isTheSameSubnet(pIpHeader->sourceIP, pIpHeader->destIP, subnetMask_); }
    bool isPacketToMe(const SIpHeader *pIpHeader) const { return (getIP() == pIpHeader->destIP); }
    bool isPacketFromMe(const SIpHeader *pIpHeader) const { return (getIP() == pIpHeader->sourceIP); }
    IPADDRESS_TYPE getIP() const;

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen);
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen);
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload, int nPayloadLen);

    INODE getInode(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader);
    void updateTopTalkers(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader, bool bLAN);
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

    std::map<ServiceApp, ServiceStat> servicesStatWAN_;
    std::map<ServiceApp, ServiceStat> servicesStatLAN_;

    InodeToAppCache inodeToAppCache_;
    ServiceToInodeCache serviceToInodeCache_;
    IPADDRESS_TYPE enforcedIP_;
};

class TrafficCounter
{
public:
    TrafficCounter();

    void addInterface(const char *pszInterfaceName);
    void setDuration(int duration);
    bool hasJob() const { return !interfaces_.empty(); }
    int doJob();

protected:
    bool reportStatistics(bool bFirstTime);  // returns true if work is done
    void reportTopTalkers(FILE *pFile, double totalTime, bool bLAN);

    std::vector<InterfaceTrafficCounter> interfaces_;
    unsigned int startTime_;
    unsigned int lastStatTime_;
    std::vector<Talker> topTalkers_;
    int duration_;
};

#endif // COUNTER_H
