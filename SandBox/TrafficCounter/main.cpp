#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include "sniffer.h"
#include "misc.h"
#include "portable.h"

class ServiceStat
{
public:
    ServiceStat()
        : nPackets_(0), nOctets_(0), bTriedToResolve_(false)
    {
    }

    void update(int nPacketSize)
    {
        ++nPackets_;
        nOctets_ += nPacketSize;
        //print();
    }

    std::string &hostName(IPADDRESS_TYPE IP)
    {
        if (!bTriedToResolve_)
        {
            hostName_ = addressToHostName(IP); //CRASH on LINUX!
            bTriedToResolve_ = true;
        }        
        return hostName_;
    }

    long long getPackets() const { return nPackets_; }
    long long getOctets() const { return nOctets_; }

    void print() const
    {
        printf(" %lld packets\t%lld octets\n", nPackets_);
    }

protected:
    long long nPackets_;
    long long nOctets_;
    bool bTriedToResolve_; // try only once, can be time consuming
    std::string hostName_;
};

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
            //fprintf(stdout, "%s\t\t%lld\t%lld\t%lld\t%lld", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
            // Difference with previous one
            fprintf(pFile, "\t\t%lld\t%lld\t%lld\t%lld", nInputPackets_-prev.nInputPackets_, nInputOctets_-prev.nInputOctets_, nOutputPackets_-prev.nOutputPackets_, nOutputOctets_-prev.nOutputOctets_);
            // Speed (/sec)
            fprintf(pFile, "\t\t%.2f\t%.2f\t%.2f\t%.2f", (nInputPackets_-prev.nInputPackets_)/double(deltaTime), (nInputOctets_-prev.nInputOctets_)/double(deltaTime), (nOutputPackets_-prev.nOutputPackets_)/double(deltaTime), (nOutputOctets_-prev.nOutputOctets_)/double(deltaTime));

            fprintf(pFile, "\n");
            //fprintf(stdout, "\n");
            fclose(pFile);
        }
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

typedef std::tuple<IPADDRESS_TYPE, IPPORT, unsigned char> ServiceType;
typedef std::pair<const ServiceType *, ServiceStat *> Talker;

bool compareTalkers(const Talker &t1, const Talker &t2)
{
    return t1.second->getOctets() > t2.second->getOctets();
}

class ListenerSocket : public SnifferSocket
{
//Attributes
public:
    ListenerSocket()
    {
        bInputPacket_ = false;
        teloIP_ = 0;
        subnetMask_ = 0;
        lastStatTime_ = 0;
    }

    bool listenTo(const std::string &ifaceName)
    {
        bool bSuccess = getInterfaceAddressAndMask(ifaceName, teloIP_, subnetMask_);
        if (bSuccess)
        {
#if (SOCKETS_WSA)
            /*printf("Listening local interface %s to figure out traffic of Telo %s...\n",
                   addressToDotNotation(ifaceIP).c_str(),
                   addressToDotNotation(teloIP_).c_str());
            promiscModeOn(teloIP_);*/
#elif (SOCKETS_BSD)
            printf("Listening local interface %s to figure out traffic of Telo %s...\n",
                   ifaceName.c_str(),
                   addressToDotNotation(teloIP_).c_str());
            promiscModeOn(ifaceName.c_str());
#endif
        }
        else
        {
            printf("ERROR: interface %s does not exist\n", ifaceName.c_str());
            destroy();
        }
        reportStatistics();

    }

    bool listenTo(IPADDRESS_TYPE teloIP)
    {
        teloIP_ = teloIP;
        IPADDRESS_TYPE ifaceIP;
        std::string ifaceName; // not used so far for Windows, Linux only
        bool bSuccess = findBestInterface(teloIP, ifaceIP, subnetMask_, ifaceName);
        if (bSuccess)
        {
#if (SOCKETS_WSA)
            printf("Listening local interface %s to figure out traffic of Telo %s...\n",
                   addressToDotNotation(ifaceIP).c_str(),
                   addressToDotNotation(teloIP_).c_str());
            promiscModeOn(ifaceIP);
#elif (SOCKETS_BSD)
            printf("Listening local interface %s to figure out traffic of Telo %s...\n",
                   ifaceName.c_str(),
                   addressToDotNotation(teloIP_).c_str());
            promiscModeOn(ifaceName.c_str());
#endif
        }
        else
        {
            printf("ERROR: could not find appropriate interface for listening %s\n", addressToDotNotation(teloIP_).c_str());
            destroy();
        }
        reportStatistics();
    }


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
        unsigned int currentTime = portableGetCurrentTimeSec();
        unsigned int deltaTime = currentTime - lastStatTime_;
        if (deltaTime < 60*1) // every 1 min
            return; // too early to do something
        IpStatTotal_.report("ip.txt", IpStatLast_, deltaTime);
        IcmpStatTotal_.report("icmp.txt", IcmpStatLast_, deltaTime);
        IgmpStatTotal_.report("igmp.txt", IgmpStatLast_, deltaTime);
        TcpStatTotal_.report("tcp.txt", TcpStatLast_, deltaTime);
        UdpStatTotal_.report("udp.txt", UdpStatLast_, deltaTime);

        lastStatTime_ = currentTime;
        memcpy(&IpStatLast_, &IpStatTotal_, sizeof(ProtocolStat));
        memcpy(&IcmpStatLast_, &IcmpStatTotal_, sizeof(ProtocolStat));
        memcpy(&IgmpStatLast_, &IgmpStatTotal_, sizeof(ProtocolStat));
        memcpy(&TcpStatLast_, &TcpStatTotal_, sizeof(ProtocolStat));
        memcpy(&UdpStatLast_, &UdpStatTotal_, sizeof(ProtocolStat));

        // print out top-talkers to us
        const unsigned int N_MAX_REPORTED_TALKERS = 20;
        FILE *pFile = fopen("top_talkers.txt", "w");
        if (pFile)
        {
            if (servicesStat_.size() > N_MAX_REPORTED_TALKERS)
                fprintf(pFile, "%d top talkers to TELO (of totally %d talkers)\n", N_MAX_REPORTED_TALKERS, servicesStat_.size());
            else
                fprintf(pFile, "Top talkers to TELO\n");

            topTalkers_.clear();
            topTalkers_.reserve(servicesStat_.size());
            for (auto it = servicesStat_.begin(); it != servicesStat_.end(); ++it)
            {
                const ServiceType &type = it->first;
                ServiceStat &stat = it->second;
                Talker talker(&type, &stat);
                topTalkers_.emplace_back(talker);
            }
            std::sort(topTalkers_.begin(), topTalkers_.end(), compareTalkers);
            int nReported = 0;
            for (std::vector<Talker>::iterator talkerIt = topTalkers_.begin(); talkerIt != topTalkers_.end(); ++talkerIt)
            {
                //printf("%d reported\n", nReported);
                if (nReported >= N_MAX_REPORTED_TALKERS)
                    break;

                const ServiceType *pType = talkerIt->first;
                ServiceStat *pStat = talkerIt->second;

                IPADDRESS_TYPE IP = std::get<0>(*pType);
                IPPORT portNo = std::get<1>(*pType);
                unsigned char proto = std::get<2>(*pType);
                const char *pszProtoName = "UNKNOWN";
                if (proto == IPPROTO_UDP)
                    pszProtoName = "UDP";
                else if (proto == IPPROTO_TCP)
                        pszProtoName = "TCP";
                else if (proto == IPPROTO_ICMP)
                        pszProtoName = "ICMP";

                std::string strHostAddress = addressToDotNotation(IP);
                //printf("%s ", strHostAddress.c_str());
                std::string strHostName = pStat->hostName(IP);
                //printf("%s\n", strHostName.c_str());

                ++nReported;
                fprintf(pFile, "%d.\t%s\t%d\t%s\t%8lld packets, %10lld octets\t%s\n",
                        nReported,
                        pszProtoName, portNo,
                        strHostAddress.c_str(),
                        pStat->getPackets(), pStat->getOctets(),
                        strHostName.c_str()
                        );
            }
            fclose(pFile);
        }
    }

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen)
    {
        reportStatistics();
        bPacketOfInterest_ = isPacketOfInterest(pIpHeader);
        if (!bPacketOfInterest_)
            return;
        bInputPacket_ = (pIpHeader->destIP == teloIP_);
        IpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
        //printIpHeader(pIpHeader);
    }
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("ICMP\tlen = %5d (from %s\t to %s)\n", pIpHeader->getPacketLen(),
                addressToDotNotation(pIpHeader->sourceIP).c_str(),
                addressToDotNotation(pIpHeader->destIP).c_str());
        IcmpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
        //printIcmpHeader(pIcmpHeader);
        IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
        IPPORT servicePort = 0;
        if (bInputPacket_)
            serviceIP = pIpHeader->sourceIP;
         this->updateTopTalkers(serviceIP, servicePort, pIpHeader);
    }
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("IGMP\tlen = %5d (from %s\t to %s)\n", pIpHeader->getPacketLen(),
               addressToDotNotation(pIpHeader->sourceIP).c_str(),
               addressToDotNotation(pIpHeader->destIP).c_str());
        IgmpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
    }
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("TCP:%5d/%5d len = %5d (from %s\t to %s)\n", pTcpHeader->getSrcPortNo(), pTcpHeader->getDstPortNo(), pIpHeader->getPacketLen(),
               addressToDotNotation(pIpHeader->sourceIP).c_str(),
               addressToDotNotation(pIpHeader->destIP).c_str());
        TcpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
        //printTcpHeader(pTcpHeader);
        IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
        IPPORT servicePort = pTcpHeader->getDstPortNo();
        if (bInputPacket_)
        {
            serviceIP = pIpHeader->sourceIP;
            servicePort = pTcpHeader->getSrcPortNo();
        }
        this->updateTopTalkers(serviceIP, servicePort, pIpHeader);
    }
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("UDP:%5d/%5d len = %5d (from %s\t to %s)\n", pUdpHeader->getSrcPortNo(), pUdpHeader->getDstPortNo(), pIpHeader->getPacketLen(),
               addressToDotNotation(pIpHeader->sourceIP).c_str(),
               addressToDotNotation(pIpHeader->destIP).c_str());
        UdpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
        //printUdpHeader(pUdpHeader);

        IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
        IPPORT servicePort = pUdpHeader->getDstPortNo();
        if (bInputPacket_)
        {
            serviceIP = pIpHeader->sourceIP;
            servicePort = pUdpHeader->getSrcPortNo();
        }
        this->updateTopTalkers(serviceIP, servicePort, pIpHeader);
    }
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("UNKNOWN: PROTO=%d, %s -> %s\n", pIpHeader->proto,
               addressToDotNotation(pIpHeader->sourceIP).c_str(),
               addressToDotNotation(pIpHeader->destIP).c_str());
    }

    void updateTopTalkers(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader)
    {
        ServiceType service(serviceIP, servicePort, pIpHeader->proto);
        auto it = servicesStat_.find(service);
        if (servicesStat_.end() == it)
        { // not found, create new entry
            ServiceStat stat;
            stat.update(pIpHeader->getPacketLen());
            servicesStat_[service] = stat;
        }
        else
        { // update statistics of the service
            ServiceStat &stat = it->second;
            stat.update(pIpHeader->getPacketLen());
        }
    }

// Protected members
protected:
    IPADDRESS_TYPE subnetMask_, teloIP_;
    ProtocolStat IpStatTotal_, IpStatLast_;
    ProtocolStat UdpStatTotal_, UdpStatLast_;
    ProtocolStat TcpStatTotal_, TcpStatLast_;
    ProtocolStat IcmpStatTotal_, IcmpStatLast_;
    ProtocolStat IgmpStatTotal_, IgmpStatLast_;

    std::map<ServiceType, ServiceStat> servicesStat_;
    std::vector<Talker> topTalkers_;

    unsigned int lastStatTime_;
    bool bPacketOfInterest_;
    bool bInputPacket_;
};

const char *TCP_FILE_NAME = "/proc/net/tcp";

int main(int argc, char* argv[])
{
#if 0
    char buffer[8192];
    unsigned long rxq, txq, time_len, retr, inode;
    int num, local_port, rem_port, d, state, uid, timer_run, timeout;
    char rem_addr[128], local_addr[128], timers[64];
    const struct aftype *ap;
    struct sockaddr_storage localsas, remsas;
    struct sockaddr_in *localaddr = (struct sockaddr_in *)&localsas;
    struct sockaddr_in *remaddr = (struct sockaddr_in *)&remsas;
#if HAVE_AFINET6
    char addr6[INET6_ADDRSTRLEN];
    struct in6_addr in6;
    extern struct aftype inet6_aftype;
#endif
    long clk_tck = ticks_per_second();

    if (lnr == 0)
    return;

    num = sscanf(line,
    "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
         &d, local_addr, &local_port, rem_addr, &rem_port, &state,
         &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);

    if (num < 11) {
    fprintf(stderr, _("warning, got bogus tcp line.\n"));
    return;
    }

    if (!flag_all && ((flag_lst && rem_port) || (!flag_lst && !rem_port)))
      return;

    if (strlen(local_addr) > 8) {
#if HAVE_AFINET6
    /* Demangle what the kernel gives us */
    sscanf(local_addr, "%08X%08X%08X%08X",
           &in6.s6_addr32[0], &in6.s6_addr32[1],
           &in6.s6_addr32[2], &in6.s6_addr32[3]);
    inet_ntop(AF_INET6, &in6, addr6, sizeof(addr6));
    inet6_aftype.input(1, addr6, &localsas);
    sscanf(rem_addr, "%08X%08X%08X%08X",
           &in6.s6_addr32[0], &in6.s6_addr32[1],
           &in6.s6_addr32[2], &in6.s6_addr32[3]);
    inet_ntop(AF_INET6, &in6, addr6, sizeof(addr6));
    inet6_aftype.input(1, addr6, &remsas);
    localsas.ss_family = AF_INET6;
    remsas.ss_family = AF_INET6;
#endif
    } else {
    sscanf(local_addr, "%X", &localaddr->sin_addr.s_addr);
    sscanf(rem_addr, "%X", &remaddr->sin_addr.s_addr);
    localsas.ss_family = AF_INET;
    remsas.ss_family = AF_INET;
    }

    if ((ap = get_afntype(localsas.ss_family)) == NULL) {
    fprintf(stderr, _("netstat: unsupported address family %d !\n"),
        localsas.ss_family);
    return;
    }

    addr_do_one(local_addr, sizeof(local_addr), 22, ap, &localsas, local_port, "tcp");
    addr_do_one(rem_addr, sizeof(rem_addr), 22, ap, &remsas, rem_port, "tcp");

    timers[0] = '\0';
    if (flag_opt)
        switch (timer_run) {
        case 0:
        snprintf(timers, sizeof(timers), _("off (0.00/%ld/%d)"), retr, timeout);
        break;

        case 1:
        snprintf(timers, sizeof(timers), _("on (%2.2f/%ld/%d)"),
             (double) time_len / clk_tck, retr, timeout);
        break;

        case 2:
        snprintf(timers, sizeof(timers), _("keepalive (%2.2f/%ld/%d)"),
             (double) time_len / clk_tck, retr, timeout);
        break;

        case 3:
        snprintf(timers, sizeof(timers), _("timewait (%2.2f/%ld/%d)"),
             (double) time_len / clk_tck, retr, timeout);
        break;

        case 4:
        snprintf(timers, sizeof(timers), _("probe (%2.2f/%ld/%d)"),
             (double) time_len / clk_tck, retr, timeout);
        break;

        default:
        snprintf(timers, sizeof(timers), _("unkn-%d (%2.2f/%ld/%d)"),
             timer_run, (double) time_len / clk_tck, retr, timeout);
        break;
        }

    printf("%-4s  %6ld %6ld %-*s %-*s %-11s",
           prot, rxq, txq, (int)netmax(23,strlen(local_addr)), local_addr, (int)netmax(23,strlen(rem_addr)), rem_addr, _(tcp_state[state]));

    finish_this_one(uid,inode,timers);
}

FILE *f = fopen(TCP_FILE_NAME, "r");
    if(f)
    {
        //print its contents
        while(fgets(buffer, sizeof(buffer), f))
            printf("%s", buffer);
        fclose(f);

    }
#endif
/*    procinfo = proc_fopen((TCP_FILE_NAME));			\
    if (procinfo == NULL) {				\
      if (errno != ENOENT && errno != EACCES) {		\
        perror((file));					\
        return -1;					\
      }							\
      if (!flag_noprot && (flag_arg || flag_ver))		\
        ESYSNOT("netstat", (name));			\
      if (!flag_noprot && flag_arg)			\
        rc = 1;						\
    } else {						\
      do {						\
        if (fgets(buffer, sizeof(buffer), procinfo))	\
          (proc)(lnr++, buffer,prot);			\
      } while (!feof(procinfo));				\
      fclose(procinfo);					\
    }

  */

   // IpSocket::InitSockets();
#ifdef SOCKETS_WSA
#define VERSION_MINOR 1
#define VERSION_MAJOR 1

    WSADATA WsaData;
    WORD wVersionRequested = MAKEWORD(VERSION_MINOR, VERSION_MAJOR);

    int err = WSAStartup(wVersionRequested, &WsaData);
    if (err != 0)
        return 0;

    if (LOBYTE(WsaData.wVersion) != VERSION_MAJOR ||
        HIBYTE(WsaData.wVersion) != VERSION_MINOR )
    { // We could not find a usable WinSock DLL
        WSACleanup();
        return 0;
    }
#endif

    IPADDRESS_TYPE teloIP = 0;
    ListenerSocket sniffer;

#ifdef SOCKETS_WSA
    teloIP = dotNotationToAddress(argv[1]);
    if (teloIP)
        sniffer.listenTo(teloIP);
#endif
#ifdef SOCKETS_BSD
    std::string ifaceName = "eth0";//"enp0s3";
    if (argc > 1)
    {
        ifaceName = argv[1];
        if (!isItInterfaceName(ifaceName))
        {
            teloIP = dotNotationToAddress(argv[1]);
        }
    }

    if (teloIP)
        sniffer.listenTo(teloIP);
    else
        sniffer.listenTo(ifaceName);
#endif //SOCKETS_BSD

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

#ifdef SOCKETS_WSA
    WSACleanup();
#endif

    return 0;
}

#if 0
void StartSniffing (SOCKET Sock); //This will sniff here and there

void ProcessPacket (char* , int); //This will decide how to digest
void PrintIpHeader (char*);
void PrintIcmpPacket (char* , int);


void PrintUdpPacket (char* , int);
void PrintTcpPacket (char* , int);
void ConvertToHex (char* , unsigned int);
void PrintData (char* , int);

typedef struct ip_hdr
{
    unsigned char ip_header_len:4; // 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
    unsigned char ip_version :4; // 4-bit IPv4 version
    unsigned char ip_tos; // IP type of service
    unsigned short ip_total_length; // Total length
    unsigned short ip_id; // Unique identifier

    unsigned char ip_frag_offset :5; // Fragment offset field

    unsigned char ip_more_fragment :1;
    unsigned char ip_dont_fragment :1;
    unsigned char ip_reserved_zero :1;

    unsigned char ip_frag_offset1; //fragment offset

    unsigned char ip_ttl; // Time to live
    unsigned char ip_protocol; // Protocol(TCP,UDP etc)
    unsigned short ip_checksum; // IP checksum
    unsigned int ip_srcaddr; // Source address
    unsigned int ip_destaddr; // Source address
} IPV4_HDR;

typedef struct udp_hdr
{
    unsigned short source_port; // Source port no.
    unsigned short dest_port; // Dest. port no.
    unsigned short udp_length; // Udp packet length
    unsigned short udp_checksum; // Udp checksum (optional)
} UDP_HDR;

// TCP header
typedef struct tcp_header
{
    unsigned short source_port; // source port
    unsigned short dest_port; // destination port
    unsigned int sequence; // sequence number - 32 bits
    unsigned int acknowledge; // acknowledgement number - 32 bits

    unsigned char ns :1; //Nonce Sum Flag Added in RFC 3540.
    unsigned char reserved_part1:3; //according to rfc
    unsigned char data_offset:4; /*The number of 32-bit words in the TCP header.
    This indicates where the data begins.
    The length of the TCP header is always a multiple
    of 32 bits.*/

    unsigned char fin :1; //Finish Flag
    unsigned char syn :1; //Synchronise Flag
    unsigned char rst :1; //Reset Flag
    unsigned char psh :1; //Push Flag
    unsigned char ack :1; //Acknowledgement Flag
    unsigned char urg :1; //Urgent Flag

    unsigned char ecn :1; //ECN-Echo Flag
    unsigned char cwr :1; //Congestion Window Reduced Flag

    ////////////////////////////////

    unsigned short window; // window
    unsigned short checksum; // checksum
    unsigned short urgent_pointer; // urgent pointer
} TCP_HDR;

typedef struct icmp_hdr
{
    BYTE type; // ICMP Error type
    BYTE code; // Type sub code
    USHORT checksum;
    USHORT id;
    USHORT seq;
} ICMP_HDR;

FILE *logfile;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;
struct sockaddr_in source,dest;
char hex[2];

//Its free!
IPV4_HDR *iphdr;
TCP_HDR *tcpheader;
UDP_HDR *udpheader;
ICMP_HDR *icmpheader;

int main()
{
    SOCKET sniffer;
    struct in_addr addr;
    int in;

    char hostname[100];
    struct hostent *local;
    WSADATA wsa;

    logfile=fopen("log.txt","w");
    if(logfile == NULL)
    {
        printf("Unable to create file.");
    }

    //Initialise Winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("WSAStartup() failed.\n");
        return 1;
    }
    printf("Initialised");

    //Create a RAW Socket
    printf("\nCreating RAW Socket...");
    sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (sniffer == INVALID_SOCKET)
    {
        printf("Failed to create raw socket.\n");
        return 1;
    }
    printf("Created.");

    //Retrive the local hostname
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    {
        printf("Error : %d",WSAGetLastError());
        return 1;
    }
    printf("\nHost name : %s \n",hostname);

    //Retrive the available IPs of the local host
    local = gethostbyname(hostname);
    printf("\nAvailable Network Interfaces : \n");
    if (local == NULL)
    {
        printf("Error : %d.\n",WSAGetLastError());
        return 1;
    }

    for (i = 0; local->h_addr_list[i] != 0; ++i)
    {
        memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
        printf("Interface Number : %d Address : %s\n",i,inet_ntoa(addr));
    }

    printf("Enter the interface number you would like to sniff : ");
    scanf("%d",&in);

    memset(&dest, 0, sizeof(dest));
    memcpy(&dest.sin_addr.s_addr,local->h_addr_list[in],sizeof(dest.sin_addr.s_addr));
    dest.sin_family = AF_INET;
    dest.sin_port = 0;

    printf("\nBinding socket to local system and port 0 ...");
    if (bind(sniffer,(struct sockaddr *)&dest,sizeof(dest)) == SOCKET_ERROR)
    {
        printf("bind(%s) failed.\n", inet_ntoa(addr));
        return 1;
    }
    printf("Binding successful");

    //Enable this socket with the power to sniff : SIO_RCVALL is the key Receive ALL ;)

    j=1;
    printf("\nSetting socket to sniff...");
    if (WSAIoctl(sniffer, SIO_RCVALL, &j, sizeof(j), 0, 0, (LPDWORD) &in , 0 , 0) == SOCKET_ERROR)
    {
        printf("WSAIoctl() failed.\n");
        return 1;
    }
    printf("Socket set.");

    //Begin
    printf("\nStarted Sniffing\n");
    printf("Packet Capture Statistics...\n");
    StartSniffing(sniffer); //Happy Sniffing

    //End
    closesocket(sniffer);
    WSACleanup();

    return 0;
}

void StartSniffing(SOCKET sniffer)
{
    char *Buffer = (char *)malloc(65536); //Its Big!
    int mangobyte;

    if (Buffer == NULL)
    {
        printf("malloc() failed.\n");
        return;
    }

    do
    {
        mangobyte = recvfrom(sniffer , Buffer , 65536 , 0 , 0 , 0); //Eat as much as u can

        if(mangobyte > 0)
        {
            ProcessPacket(Buffer, mangobyte);
        }
        else
        {
            printf( "recvfrom() failed.\n");
        }
    }
    while (mangobyte > 0);

    free(Buffer);
}

void ProcessPacket(char* Buffer, int Size)
{
    iphdr = (IPV4_HDR *)Buffer;
    ++total;

    switch (iphdr->ip_protocol) //Check the Protocol and do accordingly...
    {
        case 1: //ICMP Protocol
        ++icmp;
        PrintIcmpPacket(Buffer,Size);
        break;

        case 2: //IGMP Protocol
        ++igmp;
        break;

        case 6: //TCP Protocol
        ++tcp;
        PrintTcpPacket(Buffer,Size);
        break;

        case 17: //UDP Protocol
        ++udp;
        PrintUdpPacket(Buffer,Size);
        break;

        default: //Some Other Protocol like ARP etc.
        ++others;
        break;
    }
    printf("TCP : %d UDP : %d ICMP : %d IGMP : %d Others : %d Total : %d\r",tcp,udp,icmp,igmp,others,total);
}

void PrintTcpPacket(char* Buffer, int Size)
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    tcpheader=(TCP_HDR*)(Buffer+iphdrlen);

    fprintf(logfile,"\n\n***********************TCP Packet*************************\n");

    PrintIpHeader( Buffer );

    fprintf(logfile,"\n");
    fprintf(logfile,"TCP Header\n");
    fprintf(logfile," |-Source Port : %u\n",ntohs(tcpheader->source_port));
    fprintf(logfile," |-Destination Port : %u\n",ntohs(tcpheader->dest_port));
    fprintf(logfile," |-Sequence Number : %u\n",ntohl(tcpheader->sequence));
    fprintf(logfile," |-Acknowledge Number : %u\n",ntohl(tcpheader->acknowledge));
    fprintf(logfile," |-Header Length : %d DWORDS or %d BYTES\n"
    ,(unsigned int)tcpheader->data_offset,(unsigned int)tcpheader->data_offset*4);
    fprintf(logfile," |-CWR Flag : %d\n",(unsigned int)tcpheader->cwr);
    fprintf(logfile," |-ECN Flag : %d\n",(unsigned int)tcpheader->ecn);
    fprintf(logfile," |-Urgent Flag : %d\n",(unsigned int)tcpheader->urg);
    fprintf(logfile," |-Acknowledgement Flag : %d\n",(unsigned int)tcpheader->ack);
    fprintf(logfile," |-Push Flag : %d\n",(unsigned int)tcpheader->psh);
    fprintf(logfile," |-Reset Flag : %d\n",(unsigned int)tcpheader->rst);
    fprintf(logfile," |-Synchronise Flag : %d\n",(unsigned int)tcpheader->syn);
    fprintf(logfile," |-Finish Flag : %d\n",(unsigned int)tcpheader->fin);
    fprintf(logfile," |-Window : %d\n",ntohs(tcpheader->window));
    fprintf(logfile," |-Checksum : %d\n",ntohs(tcpheader->checksum));
    fprintf(logfile," |-Urgent Pointer : %d\n",tcpheader->urgent_pointer);
    fprintf(logfile,"\n");
    fprintf(logfile," DATA Dump ");
    fprintf(logfile,"\n");

    fprintf(logfile,"IP Header\n");
    PrintData(Buffer,iphdrlen);

    fprintf(logfile,"TCP Header\n");
    PrintData(Buffer+iphdrlen,tcpheader->data_offset*4);

    fprintf(logfile,"Data Payload\n");
    PrintData(Buffer+iphdrlen+tcpheader->data_offset*4
    ,(Size-tcpheader->data_offset*4-iphdr->ip_header_len*4));

    fprintf(logfile,"\n###########################################################");
}

void PrintUdpPacket(char *Buffer,int Size)
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    udpheader = (UDP_HDR *)(Buffer + iphdrlen);

    fprintf(logfile,"\n\n***********************UDP Packet*************************\n");

    PrintIpHeader(Buffer);

    fprintf(logfile,"\nUDP Header\n");
    fprintf(logfile," |-Source Port : %d\n",ntohs(udpheader->source_port));
    fprintf(logfile," |-Destination Port : %d\n",ntohs(udpheader->dest_port));
    fprintf(logfile," |-UDP Length : %d\n",ntohs(udpheader->udp_length));
    fprintf(logfile," |-UDP Checksum : %d\n",ntohs(udpheader->udp_checksum));

    fprintf(logfile,"\n");
    fprintf(logfile,"IP Header\n");

    PrintData(Buffer,iphdrlen);

    fprintf(logfile,"UDP Header\n");

    PrintData(Buffer+iphdrlen,sizeof(UDP_HDR));

    fprintf(logfile,"Data Payload\n");

    PrintData(Buffer+iphdrlen+sizeof(UDP_HDR) ,(Size - sizeof(UDP_HDR) - iphdr->ip_header_len*4));

    fprintf(logfile,"\n###########################################################");
}

void PrintIcmpPacket(char* Buffer , int Size)
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    icmpheader=(ICMP_HDR*)(Buffer+iphdrlen);

    fprintf(logfile,"\n\n***********************ICMP Packet*************************\n");
    PrintIpHeader(Buffer);

    fprintf(logfile,"\n");

    fprintf(logfile,"ICMP Header\n");
    fprintf(logfile," |-Type : %d",(unsigned int)(icmpheader->type));

    if((unsigned int)(icmpheader->type)==11)
    {
        fprintf(logfile," (TTL Expired)\n");
    }
    else if((unsigned int)(icmpheader->type)==0)
    {
        fprintf(logfile," (ICMP Echo Reply)\n");
    }

    fprintf(logfile," |-Code : %d\n",(unsigned int)(icmpheader->code));
    fprintf(logfile," |-Checksum : %d\n",ntohs(icmpheader->checksum));
    fprintf(logfile," |-ID : %d\n",ntohs(icmpheader->id));
    fprintf(logfile," |-Sequence : %d\n",ntohs(icmpheader->seq));
    fprintf(logfile,"\n");

    fprintf(logfile,"IP Header\n");
    PrintData(Buffer,iphdrlen);

    fprintf(logfile,"UDP Header\n");
    PrintData(Buffer+iphdrlen,sizeof(ICMP_HDR));

    fprintf(logfile,"Data Payload\n");
    PrintData(Buffer+iphdrlen+sizeof(ICMP_HDR) , (Size - sizeof(ICMP_HDR) - iphdr->ip_header_len*4));

    fprintf(logfile,"\n###########################################################");
}

/*
    Print the hex values of the data
*/
void PrintData (char* data , int Size)
{
    char a , line[17] , c;
    int j;

    //loop over each character and print
    for(i=0 ; i < Size ; i++)
    {
        c = data[i];

        //Print the hex value for every character , with a space. Important to make unsigned
        fprintf(logfile," %.2x", (unsigned char) c);

        //Add the character to data line. Important to make unsigned
        a = ( c >=32 && c <=128) ? (unsigned char) c : '.';

        line[i%16] = a;

        //if last character of a line , then print the line - 16 characters in 1 line
        if( (i!=0 && (i+1)%16==0) || i == Size - 1)
        {
            line[i%16 + 1] = '\0';

            //print a big gap of 10 characters between hex and characters
            fprintf(logfile ,"          ");

            //Print additional spaces for last lines which might be less than 16 characters in length
            for( j = strlen(line) ; j < 16; j++)
            {
                fprintf(logfile , "   ");
            }

            fprintf(logfile , "%s \n" , line);
        }
    }

    fprintf(logfile , "\nlocal_branch21_working");
}
#endif
