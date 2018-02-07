#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <algorithm>
#include "counter.h"
#include "portable.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ServiceStat

ServiceStat::ServiceStat()
    : nPackets_(0), nOctets_(0), bTriedToResolveHostName_(false), bTriedToResolveServiceName_(false)
{
}

void ServiceStat::update(int nPacketSize)
{
    ++nPackets_;
    nOctets_ += nPacketSize;
}

std::string &ServiceStat::hostName(IPADDRESS_TYPE IP)
{
    if (!bTriedToResolveHostName_)
    {
        hostName_ = addressToHostName(IP);
        bTriedToResolveHostName_ = true;
    }
    return hostName_;
}

std::string &ServiceStat::serviceName(IPADDRESS_TYPE IP, IPPORT portNo, bool bUDP)
{
    if (!bTriedToResolveServiceName_)
    {
        serviceName_ = ::getServiceName(IP, portNo, bUDP);
        bTriedToResolveServiceName_ = true;
    }
    return serviceName_;
}


////////////////////////////////////////////////////////////////////////////////////////////
// ProtocolStat

ProtocolStat::ProtocolStat()
    : nInputPackets_(0), nOutputPackets_(0), nInputOctets_(0), nOutputOctets_()
{
}

void ProtocolStat::report(const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime, bool bFirstTime) const
{
    const char *pszOpenMode = "a";
    if (bFirstTime)
        pszOpenMode = "w";
    FILE *pFile = fopen(pszFileName, pszOpenMode);
    if (pFile)
    {
        if (bFirstTime)
        { // print header
            fprintf(pFile, "Date time");
            fprintf(pFile, "\t\tInPkts\tInBytes\tOutPkts\tOutBytes");
            fprintf(pFile, "\t\tdInPkts\tdInBytes\tdOutPkts\tdOutBytes");
            fprintf(pFile, "\t\tInPkts/s\tInBytes/s\tOutPkts/s\tOutBytes/s");
            fprintf(pFile, "\n");
        }
        time_t rawtime;
        char strTime[80];

        time(&rawtime);
        struct tm *pTimeInfo = localtime(&rawtime);

        strftime(strTime, sizeof(strTime), "%d.%m.%Y %H:%M:%S", pTimeInfo);
        // Absolute numbers
        fprintf(pFile, "%s\t\t%d\t%d\t%d\t%d", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
        //fprintf(stdout, "%s\t\t%d\t%d\t%d\t%d", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
        // Difference with previous one
        fprintf(pFile, "\t\t%d\t%d\t%d\t%d", nInputPackets_-prev.nInputPackets_, nInputOctets_-prev.nInputOctets_, nOutputPackets_-prev.nOutputPackets_, nOutputOctets_-prev.nOutputOctets_);
        // Speed (/sec)
        fprintf(pFile, "\t\t%.2f\t%.2f\t%.2f\t%.2f", (nInputPackets_-prev.nInputPackets_)/double(deltaTime), (nInputOctets_-prev.nInputOctets_)/double(deltaTime), (nOutputPackets_-prev.nOutputPackets_)/double(deltaTime), (nOutputOctets_-prev.nOutputOctets_)/double(deltaTime));

        fprintf(pFile, "\n");
        //fprintf(stdout, "\n");
        fclose(pFile);
    }
}

void ProtocolStat::update(unsigned int nPacketSize, bool bInput)
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


////////////////////////////////////////////////////////////////////////////////////////////
// TrafficCounter

TrafficCounter::TrafficCounter()
{
    bInputPacket_ = false;
    teloIP_ = 0;
    subnetMask_ = 0;
    lastStatTime_ = portableGetCurrentTimeSec() - 60*60; // an hour ago
    updateInodeAppCache();
}

bool TrafficCounter::listenTo(const std::string &ifaceName)
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
    reportStatistics(true);
    return bSuccess;
}

bool TrafficCounter::listenTo(IPADDRESS_TYPE teloIP)
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
    reportStatistics(true);
    return bSuccess;
}

bool TrafficCounter::isPacketOfInterest(const SIpHeader *pIpHeader) const
{
    if ((pIpHeader->sourceIP != teloIP_) && (pIpHeader->destIP != teloIP_))
        return false; // Telo traffic only
    if (isTheSameSubnet(pIpHeader->sourceIP, pIpHeader->destIP, subnetMask_))
        return false; // Skip LAN traffic
    return true;
}

static bool compareTalkers(const Talker &t1, const Talker &t2)
{
    return t1.second->getOctets() > t2.second->getOctets();
}

static void putSpaces(FILE *pFile, int nSpaces)
{
    if (!pFile)
        return;
    for (int i = 0; i < nSpaces; ++i)
        fprintf(pFile, " ");
}

void TrafficCounter::reportStatistics(bool bFirstTime)
{
    const char *pszRank = "Rank";                int iRankW = strlen(pszRank);
    const char *pszApplication = "Application";  int iApplicationW = strlen(pszApplication);
    const char *pszService = "Service";          int iServiceW = strlen(pszService);
    const char *pszPackets = "Packets";          int iPacketsW = strlen(pszPackets);
    const char *pszBytes = "Bytes";              int iBytesW = strlen(pszBytes);
    const char *pszIpAddress = "IP-address";     int iIpAddressW = strlen(pszIpAddress);
    const char *pszProto = "Proto";              int iProtoW = strlen(pszProto);
    const char *pszPort = "Port";                int iPortW = strlen(pszPort);
    const char *pszUrl = "URL";                  int iUrlW = strlen(pszUrl);
    const int nSpacesBetweenColumns = 2;

    unsigned int currentTime = portableGetCurrentTimeSec();
    unsigned int deltaTime = currentTime - lastStatTime_;
    if (deltaTime < 60*1) // every 1 min
        return; // too early to do something
    IpStatTotal_.report("ip.txt", IpStatLast_, deltaTime, bFirstTime);
    IcmpStatTotal_.report("icmp.txt", IcmpStatLast_, deltaTime, bFirstTime);
    IgmpStatTotal_.report("igmp.txt", IgmpStatLast_, deltaTime, bFirstTime);
    TcpStatTotal_.report("tcp.txt", TcpStatLast_, deltaTime, bFirstTime);
    UdpStatTotal_.report("udp.txt", UdpStatLast_, deltaTime, bFirstTime);

    lastStatTime_ = currentTime;
    memcpy(&IpStatLast_, &IpStatTotal_, sizeof(ProtocolStat));
    memcpy(&IcmpStatLast_, &IcmpStatTotal_, sizeof(ProtocolStat));
    memcpy(&IgmpStatLast_, &IgmpStatTotal_, sizeof(ProtocolStat));
    memcpy(&TcpStatLast_, &TcpStatTotal_, sizeof(ProtocolStat));
    memcpy(&UdpStatLast_, &UdpStatTotal_, sizeof(ProtocolStat));

    // print out top-talkers to us
    const int N_MAX_REPORTED_TALKERS = 20;
    FILE *pFile = fopen("top_talkers.txt", "w");
    if (pFile)
    {
        // header
        if (servicesStat_.size() > N_MAX_REPORTED_TALKERS)
            fprintf(pFile, "%d top talkers to TELO (of totally %d talkers)\n", N_MAX_REPORTED_TALKERS, (int)servicesStat_.size());
        else
            fprintf(pFile, "Top talkers to TELO\n");

        topTalkers_.clear();
        topTalkers_.reserve(servicesStat_.size());
        for (auto it = servicesStat_.begin(); it != servicesStat_.end(); ++it)
        {
            const ServiceApp &serviceApp = it->first;
            ServiceStat &stat = it->second;
            Talker talker(&serviceApp, &stat);
            topTalkers_.emplace_back(talker);
        }
        std::sort(topTalkers_.begin(), topTalkers_.end(), compareTalkers);

        int nSpaces;
        for (int iPass = 1; iPass <= 2; ++iPass)
        {
            // a little bit stupid but..... :-)
            if (iPass == 2)
            {
                nSpaces = iRankW        - fprintf(pFile, "%s", pszRank);        putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iApplicationW - fprintf(pFile, "%s", pszApplication); putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iServiceW     - fprintf(pFile, "%s", pszService);     putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iPacketsW     - fprintf(pFile, "%s", pszPackets);     putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iBytesW       - fprintf(pFile, "%s", pszBytes);       putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iIpAddressW   - fprintf(pFile, "%s", pszIpAddress);   putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iProtoW       - fprintf(pFile, "%s", pszProto);       putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iPortW        - fprintf(pFile, "%s", pszPort);        putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iUrlW         - fprintf(pFile, "%s", pszUrl);         //putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                fprintf(pFile, "\n");
            }
            int nReported = 0;
            for (std::vector<Talker>::iterator talkerIt = topTalkers_.begin(); talkerIt != topTalkers_.end(); ++talkerIt)
            {
                if (nReported >= N_MAX_REPORTED_TALKERS)
                    break;

                const ServiceApp *pServiceApp = talkerIt->first;
                ServiceStat *pStat = talkerIt->second;

                IPADDRESS_TYPE IP = std::get<0>(*pServiceApp);
                IPPORT portNo = std::get<1>(*pServiceApp);
                unsigned char proto = std::get<2>(*pServiceApp);
                const std::string &appName = std::get<3>(*pServiceApp);
                const char *pszProtoName = "UNKNOWN";
                if (proto == IPPROTO_UDP)
                    pszProtoName = "UDP";
                else if (proto == IPPROTO_TCP)
                    pszProtoName = "TCP";
                else if (proto == IPPROTO_ICMP)
                    pszProtoName = "ICMP";
                else if (proto == IPPROTO_IGMP)
                    pszProtoName = "IGMP";

                std::string hostAddress = addressToDotNotation(IP);
                std::string URL = pStat->hostName(IP);
                std::string service;
                if ((proto == IPPROTO_UDP) || (proto == IPPROTO_TCP))
                    service = pStat->serviceName(IP, portNo, (proto == IPPROTO_UDP));

                ++nReported;

                // a little bit stupid but..... :-)
                if (iPass == 1)
                {
                    char szTmp[512];
                    iRankW          = std::max(iRankW, sprintf(szTmp, "%d", nReported));
                    iApplicationW   = std::max(iApplicationW, sprintf(szTmp, "%s", appName.c_str()));
                    iServiceW       = std::max(iServiceW, sprintf(szTmp, "%s", service.c_str()));
                    iPacketsW       = std::max(iPacketsW, sprintf(szTmp, "%d", pStat->getPackets()));
                    iBytesW         = std::max(iBytesW, sprintf(szTmp, "%d", pStat->getOctets()));
                    iIpAddressW     = std::max(iIpAddressW, sprintf(szTmp, "%s", hostAddress.c_str()));
                    iProtoW         = std::max(iProtoW, sprintf(szTmp, "%s", pszProtoName));
                    iPortW          = std::max(iPortW, sprintf(szTmp, "%d", portNo));
                    iUrlW           = std::max(iUrlW, sprintf(szTmp, "%s", URL.c_str()));
                }
                else
                {
                    nSpaces = iRankW - fprintf(pFile, "%d", nReported);                 putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iApplicationW - fprintf(pFile, "%s", appName.c_str());    putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iServiceW - fprintf(pFile, "%s", service.c_str());        putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iPacketsW - fprintf(pFile, "%d", pStat->getPackets());  putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iBytesW - fprintf(pFile, "%d", pStat->getOctets());    putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iIpAddressW - fprintf(pFile, "%s", hostAddress.c_str());  putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iProtoW - fprintf(pFile, "%s", pszProtoName);             putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iPortW - fprintf(pFile, "%d", portNo);                    putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iUrlW - fprintf(pFile, "%s", URL.c_str());                //putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    fprintf(pFile, "\n");
                }
            }
        }
        fclose(pFile);
    }
}

//virtual
void TrafficCounter::ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen)
{
    reportStatistics(false);
    bPacketOfInterest_ = isPacketOfInterest(pIpHeader);
    if (!bPacketOfInterest_)
        return;
    bInputPacket_ = (pIpHeader->destIP == teloIP_);
    IpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
    //printIpHeader(pIpHeader);
}

//virtual
void TrafficCounter::icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!bPacketOfInterest_)
        return;
    /*printf("ICMP packet: %s -> %s\tlength = %d\n",
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str(),
            pIpHeader->getPacketLen());*/
    IcmpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
    //printIcmpHeader(pIcmpHeader);
    IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
    IPPORT servicePort = 0;
    if (bInputPacket_)
        serviceIP = pIpHeader->sourceIP;
     this->updateTopTalkers(serviceIP, servicePort, pIpHeader);
}

//virtual
void TrafficCounter::igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!bPacketOfInterest_)
        return;
    /*printf("IGMP packet: %s -> %s\tlength = %d\n",
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str(),
            pIpHeader->getPacketLen());*/
    IgmpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
}

//virtual
void TrafficCounter::tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!bPacketOfInterest_)
        return;
    /*printf("TCP packet: %s:%d -> %s:%d\tlength = %d \n",
            addressToDotNotation(pIpHeader->sourceIP).c_str(), pTcpHeader->getSrcPortNo(),
            addressToDotNotation(pIpHeader->destIP).c_str(), pTcpHeader->getDstPortNo(),
            pIpHeader->getPacketLen());*/
    //printTcpHeader(pTcpHeader);
    if (pTcpHeader->syn)
    {
        portableSwitchContext(); // let system refresh its files
        portableSwitchContext(); // let system refresh its files
        portableSwitchContext(); // let system refresh its files
        portableSwitchContext(); // let system refresh its files
        portableSwitchContext(); // let system refresh its files
    }
    TcpStatTotal_.update(pIpHeader->getPacketLen(), bInputPacket_);
    IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
    IPPORT servicePort = pTcpHeader->getDstPortNo();
    if (bInputPacket_)
    {
        serviceIP = pIpHeader->sourceIP;
        servicePort = pTcpHeader->getSrcPortNo();
    }
    this->updateTopTalkers(serviceIP, servicePort, pIpHeader);
}

//virtual
void TrafficCounter::udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!bPacketOfInterest_)
        return;
    /*printf("UDP packet: %s:%d -> %s:%d\tlength = %d \n",
            addressToDotNotation(pIpHeader->sourceIP).c_str(), pUdpHeader->getSrcPortNo(),
            addressToDotNotation(pIpHeader->destIP).c_str(), pUdpHeader->getDstPortNo(),
            pIpHeader->getPacketLen());*/
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

//virtual
void TrafficCounter::unknownProtoPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload, int nPayloadLen)
{
    if (!bPacketOfInterest_)
        return;
    /*printf("UNKNOWN packet: PROTO = %d, %s -> %s\n", pIpHeader->proto,
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());*/
}

INODE TrafficCounter::getInode(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader)
{
    const char *TCP_FILE_NAME = "/proc/net/tcp";
    const char *UDP_FILE_NAME = "/proc/net/udp";

    INODE resultINode = 0; // not found yet
    // try to find cached value to avoid iterating dirs and files, and sscanf-ing text strings many times for the same connection
    Service service(serviceIP, servicePort);
    auto inodeIt = serviceToInodeCache_.find(service);
    if (serviceToInodeCache_.end() != inodeIt)
    {
        resultINode = inodeIt->second;
    }
    else
    {
        const char *pszFileName = NULL;
        if (pIpHeader->proto == IPPROTO_TCP)
            pszFileName = TCP_FILE_NAME;
        else if (pIpHeader->proto == IPPROTO_UDP)
            pszFileName = UDP_FILE_NAME;

        if (pszFileName)
        {
            FILE *pFile = fopen(pszFileName, "r");
            if (pFile)
            {
                char buffer[8192];
                int no;
                IPADDRESS_TYPE localIP, remoteIP;
                IPPORT localPort, remotePort;
                INODE iNode = 0;
                unsigned long rxq, txq, time_len, retr;
                int state, uid, timer_run, timeout;
                while (fgets(buffer, sizeof(buffer), pFile))
                {
                    if (sscanf(buffer, "%d: %X:%hX %X:%hX %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n", &no, &localIP, &localPort, &remoteIP, &remotePort, &state,
                               &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &iNode) >= 12)
                    {
                        if (localIP == 0 || localPort == 0)
                            continue;
                        if (remoteIP == 0 || remotePort == 0)
                            continue;
                        if (iNode > 0)
                        {
                            Service service(remoteIP, remotePort);
                            serviceToInodeCache_[service] = iNode;
                            if (remoteIP == serviceIP && remotePort == servicePort)
                                resultINode = iNode; // memorize and continue iterating for future fun
                        }
                    }
                }
                fclose(pFile);
            }
            else
            {
                printf("cannot open file %s\n", pszFileName);
            }
        }
    }
    return resultINode;
}

void TrafficCounter::updateTopTalkers(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader)
{
    // try to figure out application name
    std::string appName;
    INODE iNode = getInode(serviceIP, servicePort, pIpHeader);
    if (iNode > 0)
    {
        auto appIt = inodeToAppCache_.find(iNode);
        if (inodeToAppCache_.end() == appIt)
        {
            this->updateInodeAppCache();
            appIt = inodeToAppCache_.find(iNode);
        }
        if (inodeToAppCache_.end() != appIt)
            appName = appIt->second.c_str();
    }
    else
    {
        //printf("inode not found\n");
    }

    ServiceApp serviceApp(serviceIP, servicePort, pIpHeader->proto, appName);
    auto serviceIt = servicesStat_.find(serviceApp);
    if (servicesStat_.end() == serviceIt)
    { // not found, create new entry
        ServiceStat stat;
        stat.update(pIpHeader->getPacketLen());
        servicesStat_[serviceApp] = stat;
    }
    else
    { // update statistics of the service
        ServiceStat &stat = serviceIt->second;
        stat.update(pIpHeader->getPacketLen());
    }
}

#define PRG_INODE	 "inode"
#define PRG_SOCKET_PFX    "socket:["
#define PRG_SOCKET_PFXl (strlen(PRG_SOCKET_PFX))
#define PRG_SOCKET_PFX2   "[0000]:"
#define PRG_SOCKET_PFX2l  (strlen(PRG_SOCKET_PFX2))
static int extract_type_1_socket_inode(const char lname[], INODE *pInode)
{
    /* If lname is of the form "socket:[12345]", extract the "12345"
       as inode.  Otherwise, return -1 as inode.
       */

    if (strlen(lname) < PRG_SOCKET_PFXl+3)
        return(-1);

    if (memcmp(lname, PRG_SOCKET_PFX, PRG_SOCKET_PFXl))
        return(-1);
    if (lname[strlen(lname)-1] != ']')
        return(-1);

    char inode_str[strlen(lname + 1)];  /* e.g. "12345" */
    const int inode_str_len = strlen(lname) - PRG_SOCKET_PFXl - 1;
    char *serr;

    strncpy(inode_str, lname+PRG_SOCKET_PFXl, inode_str_len);
    inode_str[inode_str_len] = '\0';
    *pInode = strtoul(inode_str, &serr, 0);
    if (!serr || *serr || *pInode == (INODE)(~0))
        return(-1);

    return(0);
}

static int extract_type_2_socket_inode(const char lname[], INODE *pInode)
{
    /* If lname is of the form "[0000]:12345", extract the "12345"
       as inode.  Otherwise, return -1 as inode.
       */

    if (strlen(lname) < PRG_SOCKET_PFX2l+1)
        return(-1);
    if (memcmp(lname, PRG_SOCKET_PFX2, PRG_SOCKET_PFX2l))
        return(-1);

    char *serr;
    *pInode = strtoul(lname + PRG_SOCKET_PFX2l, &serr, 0);
    if (!serr || *serr || *pInode == (INODE)(~0))
            return(-1);

    return(0);
}

#define PATH_PROC           "/proc"
#define PATH_FD_SUFF        "fd"
#define PATH_FD_SUFFl       strlen(PATH_FD_SUFF)
#define PATH_PROC_X_FD      PATH_PROC "/%s/" PATH_FD_SUFF
#define PATH_CMDLINE        "cmdline"
#define PATH_CMDLINEl       strlen(PATH_CMDLINE)
void TrafficCounter::updateInodeAppCache()
{
    //inodeToAppCache_.clear();

    int procfdlen, fd, cmdllen, lnamelen;
    char line[4096], lname[32], cmdlbuf[512];
    INODE inode;
    const char *cs, *cmdlp;
    DIR *dirproc = NULL, *dirfd = NULL;
    struct dirent *direproc, *direfd;

    cmdlbuf[sizeof(cmdlbuf) - 1] = '\0';
    if (!(dirproc=opendir(PATH_PROC)))
        return;
    while (errno = 0, direproc = readdir(dirproc))
    {
        for (cs = direproc->d_name; *cs; cs++)
            if (!isdigit(*cs))
            break;
        if (*cs)
            continue;
        procfdlen = snprintf(line, sizeof(line),PATH_PROC_X_FD,direproc->d_name);
        if (procfdlen <= 0 || procfdlen >= (int)sizeof(line) - 5)
            continue;
        errno = 0;
        dirfd = opendir(line);
        if (!dirfd)
            continue;

        line[procfdlen] = '/';
        cmdlp = NULL;
        while ((direfd = readdir(dirfd)))
        {
            /* Skip . and .. */
            if (!isdigit(direfd->d_name[0]))
                continue;
            if (procfdlen + 1 + strlen(direfd->d_name) + 1 > sizeof(line))
                continue;
            memcpy(line + procfdlen - PATH_FD_SUFFl, PATH_FD_SUFF "/", PATH_FD_SUFFl + 1);
            strncpy(line + procfdlen + 1, direfd->d_name, sizeof(line) - procfdlen - 1);
            lnamelen = readlink(line, lname, sizeof(lname) - 1);
            if (lnamelen == -1)
                continue;

            lname[lnamelen] = '\0';  // make it a null-terminated string

            if (extract_type_1_socket_inode(lname, &inode) < 0)
                if (extract_type_2_socket_inode(lname, &inode) < 0)
                    continue;

            if (!cmdlp)
            {
                if (procfdlen - PATH_FD_SUFFl + PATH_CMDLINEl >= sizeof(line) - 5)
                    continue;
                strncpy(line + procfdlen - PATH_FD_SUFFl, PATH_CMDLINE, sizeof(line) - procfdlen + PATH_FD_SUFFl);
                fd = open(line, O_RDONLY);
                if (fd < 0)
                    continue;
                cmdllen = read(fd, cmdlbuf, sizeof(cmdlbuf) - 1);
                if (close(fd))
                    continue;
                if (cmdllen == -1)
                    continue;
                if (cmdllen < (int)sizeof(cmdlbuf) - 1)
                    cmdlbuf[cmdllen]='\0';
                if (cmdlbuf[0] == '/' && (cmdlp = strrchr(cmdlbuf, '/')))
                    cmdlp++;
                else
                    cmdlp = cmdlbuf;
            }

            std::string name(cmdlp);
            inodeToAppCache_[inode] = name;
       }
        closedir(dirfd);
        dirfd = NULL;
    }
    if (dirproc)
        closedir(dirproc);
    if (dirfd)
        closedir(dirfd);
}

