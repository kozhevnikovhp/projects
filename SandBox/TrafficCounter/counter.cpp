#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <string.h>

#include <algorithm>

#include "counter.h"
#include "portable.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ServiceStat

ServiceStat::ServiceStat()
    : nPackets_(0), nBytes_(0), bTriedToResolveHostName_(false), bTriedToResolveServiceName_(false)
{
}

void ServiceStat::update(int nPacketSize)
{
    ++nPackets_;
    nBytes_ += nPacketSize;
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
    : nInputPackets_(0), nOutputPackets_(0), nInputBytes_(0), nOutputBytes_(0)
{
}

void ProtocolStat::report(const std::string &ifaceName, const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime, bool bFirstTime) const
{
    const char *pszOpenMode = "a";
    if (bFirstTime)
        pszOpenMode = "w";
    std::string fullFileName = ifaceName + "_";
    fullFileName += pszFileName;
    FILE *pFile = fopen(fullFileName.c_str(), pszOpenMode);
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
        fprintf(pFile, "%s\t\t%d\t%d\t%d\t%d", strTime, nInputPackets_, nInputBytes_, nOutputPackets_, nOutputBytes_);
        //fprintf(stdout, "%s\t\t%d\t%d\t%d\t%d", strTime, nInputPackets_, nInputBytes_, nOutputPackets_, nOutputBytes_);
        // Difference with previous one
        fprintf(pFile, "\t\t%d\t%d\t%d\t%d",
                nInputPackets_-prev.nInputPackets_, nInputBytes_-prev.nInputBytes_,
                nOutputPackets_-prev.nOutputPackets_, nOutputBytes_-prev.nOutputBytes_);
        // Speed (/sec)
        fprintf(pFile, "\t\t%.2f\t%.2f\t%.2f\t%.2f",
                (nInputPackets_-prev.nInputPackets_)/double(deltaTime),
                (nInputBytes_-prev.nInputBytes_)/double(deltaTime),
                (nOutputPackets_-prev.nOutputPackets_)/double(deltaTime),
                (nOutputBytes_-prev.nOutputBytes_)/double(deltaTime));

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
        nInputBytes_ += nPacketSize;
    }
    else
    {
        ++nOutputPackets_;
        nOutputBytes_ += nPacketSize;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
// TrafficCounter

InterfaceTrafficCounter::InterfaceTrafficCounter(const std::string &ifaceName, IPADDRESS_TYPE IP)
    : Sniffer(ifaceName)
{
    teloIP_ = 0;
    subnetMask_ = 0;
    enforcedIP_ = IP;
    updateInodeAppCache();
}

bool InterfaceTrafficCounter::listen()
{
    bool bSuccess = getInterfaceAddressAndMask(ifaceName_, teloIP_, subnetMask_);
    if (bSuccess)
    {
        printf("Listening local interface %s to figure out traffic of %s...\n",
               ifaceName_.c_str(),
               addressToDotNotation(getIP()).c_str());
        promiscModeOn(ifaceName_.c_str());
    }
    else
    {
        printf("ERROR: interface %s does not exist\n", ifaceName_.c_str());
        destroy();
    }

    return bSuccess;
}

void InterfaceTrafficCounter::reportStatistics(bool bFirstTime, unsigned int deltaTime)
{
    IpStatTotal_.report(ifaceName_, "ip.txt", IpStatLast_, deltaTime, bFirstTime);
    IcmpStatTotal_.report(ifaceName_, "icmp.txt", IcmpStatLast_, deltaTime, bFirstTime);
    IgmpStatTotal_.report(ifaceName_, "igmp.txt", IgmpStatLast_, deltaTime, bFirstTime);
    TcpStatTotal_.report(ifaceName_, "tcp.txt", TcpStatLast_, deltaTime, bFirstTime);
    UdpStatTotal_.report(ifaceName_, "udp.txt", UdpStatLast_, deltaTime, bFirstTime);
    LanStatTotal_.report(ifaceName_, "lan.txt", LanStatLast_, deltaTime, bFirstTime);

    memcpy(&IpStatLast_, &IpStatTotal_, sizeof(ProtocolStat));
    memcpy(&IcmpStatLast_, &IcmpStatTotal_, sizeof(ProtocolStat));
    memcpy(&IgmpStatLast_, &IgmpStatTotal_, sizeof(ProtocolStat));
    memcpy(&TcpStatLast_, &TcpStatTotal_, sizeof(ProtocolStat));
    memcpy(&UdpStatLast_, &UdpStatTotal_, sizeof(ProtocolStat));
    memcpy(&LanStatLast_, &LanStatTotal_, sizeof(ProtocolStat));
}

void InterfaceTrafficCounter::reportOverallStat(FILE *pFile, unsigned int totalTime)
{
    fprintf(pFile, "Interface '%s':\n", ifaceName_.c_str());
    fprintf(pFile, "Averaged inbound WAN traffic: %.3f KB/hour, %.3f MB/day, %.3f GB/month\n",
            IpStatTotal_.getInputBytes()/1.E3*3600./totalTime,
            IpStatTotal_.getInputBytes()/1.E6*3600.*24./totalTime,
            IpStatTotal_.getInputBytes()/1.E9*3600.*24.*30./totalTime);
    fprintf(pFile, "Averaged outbound WAN traffic: %.3f KB/hour, %.3f MB/day, %.3f GB/month\n",
            IpStatTotal_.getOutputBytes()/1.E3*3600./totalTime,
            IpStatTotal_.getOutputBytes()/1.E6*3600.*24./totalTime,
            IpStatTotal_.getOutputBytes()/1.E9*3600.*24.*30./totalTime);
    fprintf(pFile, "Averaged inbound LAN traffic: %.3f KB/hour, %.3f MB/day, %.3f GB/month\n",
            LanStatTotal_.getInputBytes()/1.E3*3600./totalTime,
            LanStatTotal_.getInputBytes()/1.E6*3600.*24./totalTime,
            LanStatTotal_.getInputBytes()/1.E9*3600.*24.*30./totalTime);
    fprintf(pFile, "Averaged outbound LAN traffic: %.3f KB/hour, %.3f MB/day, %.3f GB/month\n\n",
            LanStatTotal_.getOutputBytes()/1.E3*3600./totalTime,
            LanStatTotal_.getOutputBytes()/1.E6*3600.*24./totalTime,
            LanStatTotal_.getOutputBytes()/1.E9*3600.*24.*30./totalTime);
}

void InterfaceTrafficCounter::fillTopTalkers(std::vector<Talker> &topTalkers)
{
    for (auto &serviceStat : servicesStat_)
    {
        const ServiceApp &serviceApp = serviceStat.first;
        ServiceStat &stat = serviceStat.second;
        Talker talker(&serviceApp, &stat);
        topTalkers.emplace_back(talker);
    }
}

bool InterfaceTrafficCounter::isTeloPacket(const SIpHeader *pIpHeader) const
{
    return (pIpHeader->sourceIP == getIP()) || (pIpHeader->destIP == getIP());
}

bool InterfaceTrafficCounter::isLanPacket(const SIpHeader *pIpHeader) const
{
    return isTheSameSubnet(pIpHeader->sourceIP, pIpHeader->destIP, subnetMask_);
}

IPADDRESS_TYPE InterfaceTrafficCounter::getIP() const
{
    if (enforcedIP_)
        return enforcedIP_;
    return teloIP_;
}

//virtual
void InterfaceTrafficCounter::ipPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload,  int nPayloadLen)
{
    if (!isTeloPacket(pIpHeader))
        return;
    if (isLanPacket(pIpHeader))
        LanStatTotal_.update(pIpHeader->getPacketLen(), pIpHeader->destIP == getIP());
    else
        IpStatTotal_.update(pIpHeader->getPacketLen(), pIpHeader->destIP == getIP());
    //printIpHeader(pIpHeader);
}

//virtual
void InterfaceTrafficCounter::icmpPacketCaptured(const SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!isTeloPacket(pIpHeader))
        return;
    if (isLanPacket(pIpHeader))
        return;
    /*printf("%s: ICMP packet %s -> %s\tlength = %d\n", ifaceName_.c_str(),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str(),
            pIpHeader->getPacketLen());*/
    IcmpStatTotal_.update(pIpHeader->getPacketLen(), pIpHeader->destIP == getIP());
    //printIcmpHeader(pIcmpHeader);
    IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
    if (pIpHeader->destIP == getIP())
        serviceIP = pIpHeader->sourceIP;
    IPPORT servicePort = pIcmpHeader->type;
    updateTopTalkers(serviceIP, servicePort, pIpHeader);
}

//virtual
void InterfaceTrafficCounter::igmpPacketCaptured(const SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!isTeloPacket(pIpHeader))
        return;
    if (isLanPacket(pIpHeader))
        return;
    /*printf("%s: IGMP packet %s -> %s\tlength = %d\n", ifaceName_.c_str(),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str(),
            pIpHeader->getPacketLen());*/
    IgmpStatTotal_.update(pIpHeader->getPacketLen(), pIpHeader->destIP == getIP());
}

//virtual
void InterfaceTrafficCounter::tcpPacketCaptured(const SIpHeader *pIpHeader, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!isTeloPacket(pIpHeader))
        return;
    if (isLanPacket(pIpHeader))
        return;
    /*printf("%s: TCP packet %s:%d -> %s:%d\tlength = %d \n", ifaceName_.c_str(),
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
    TcpStatTotal_.update(pIpHeader->getPacketLen(), pIpHeader->destIP == getIP());
    IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
    IPPORT servicePort = pTcpHeader->getDstPortNo();
    if (pIpHeader->destIP == getIP())
    {
        serviceIP = pIpHeader->sourceIP;
        servicePort = pTcpHeader->getSrcPortNo();
    }
    updateTopTalkers(serviceIP, servicePort, pIpHeader);
}

//virtual
void InterfaceTrafficCounter::udpPacketCaptured(const SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!isTeloPacket(pIpHeader))
        return;
    if (isLanPacket(pIpHeader))
        return;
    /*printf("%s: UDP packet %s:%d -> %s:%d\tlength = %d \n", ifaceName_.c_str(),
            addressToDotNotation(pIpHeader->sourceIP).c_str(), pUdpHeader->getSrcPortNo(),
            addressToDotNotation(pIpHeader->destIP).c_str(), pUdpHeader->getDstPortNo(),
            pIpHeader->getPacketLen());*/
    UdpStatTotal_.update(pIpHeader->getPacketLen(), pIpHeader->destIP == getIP());
    //printUdpHeader(pUdpHeader);

    IPADDRESS_TYPE serviceIP = pIpHeader->destIP;
    IPPORT servicePort = pUdpHeader->getDstPortNo();
    if (pIpHeader->destIP == getIP())
    {
        serviceIP = pIpHeader->sourceIP;
        servicePort = pUdpHeader->getSrcPortNo();
    }
    updateTopTalkers(serviceIP, servicePort, pIpHeader);
}

//virtual
void InterfaceTrafficCounter::unknownProtoPacketCaptured(const SIpHeader *pIpHeader, const unsigned char *pPayload, int nPayloadLen)
{
    if (!isTeloPacket(pIpHeader))
        return;
    if (isLanPacket(pIpHeader))
        return;
    /*printf("%s UNKNOWN packet: PROTO = %d, %s -> %s\n", ifaceName_.c_str(),
            pIpHeader->proto,
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());*/
}

INODE InterfaceTrafficCounter::getInode(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader)
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

void InterfaceTrafficCounter::updateTopTalkers(IPADDRESS_TYPE serviceIP, IPPORT servicePort, const SIpHeader *pIpHeader)
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

    ServiceApp serviceApp(ifaceName_, serviceIP, servicePort, pIpHeader->proto, appName);
    auto serviceIt = servicesStat_.find(serviceApp);
    if (servicesStat_.end() == serviceIt)
    { // not found, create new entry
        ServiceStat stat;
        stat.setIfaceName(ifaceName_);
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
void InterfaceTrafficCounter::updateInodeAppCache()
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
        procfdlen = snprintf(line, sizeof(line), PATH_PROC_X_FD, direproc->d_name);
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
            inodeToAppCache_[inode] = std::string(cmdlp);
       }
        closedir(dirfd);
        dirfd = NULL;
    }
    if (dirproc)
        closedir(dirproc);
    if (dirfd)
        closedir(dirfd);
}

///////////////////////////////////////////////////////
// TrafficCounter

TrafficCounter::TrafficCounter()
{
    startTime_ = portableGetCurrentTimeSec();
    lastStatTime_ = portableGetCurrentTimeSec() - 60*60; // an hour ago
}

void TrafficCounter::addInterface(const char *pszInterfaceName)
{

    if (strchr(pszInterfaceName, '[') && strchr(pszInterfaceName, '['))
    {
        char *pszDup = strdup(pszInterfaceName);
        char *pcBackBracket = strchr(pszDup, ']');
        *pcBackBracket = 0;
        char *pcFrontBracket = strchr(pszDup, '[');
        *pcFrontBracket = 0;
        if (!isItInterfaceName(pszDup))
        {
            printf("'%s' is not a valid interface name\n", pszDup);
            return;
        }

        char *pszIpAddress = pcFrontBracket+1;
        IPADDRESS_TYPE IP = dotNotationToAddress(pszIpAddress);
        if (!IP)
        {
            printf("'%s' is not a valid IP-address\n", pszIpAddress);
            return;
        }

        InterfaceTrafficCounter iface(pszDup, IP);
        interfaces_.push_back(iface);
        free(pszDup);
    }
    else
    {
        if (!isItInterfaceName(pszInterfaceName))
        {
            printf("'%s' is not a valid interface name\n", pszInterfaceName);
            return;
        }

        InterfaceTrafficCounter iface(pszInterfaceName);
        interfaces_.push_back(iface);
    }
}

int TrafficCounter::doJob()
{
    const int OK = 0;
    const int NotOK = 1;
    // initial check
    if (interfaces_.empty())
        return NotOK;
    for (auto &iface : interfaces_)
    {
        if (!iface.isCreated())
            return NotOK;
        if (!iface.listen())
            return NotOK;
    }

    struct pollfd fds[256];
    int nfds = 0;
    memset(fds, 0, sizeof(fds));

    for (auto &iface : interfaces_)
    {
        fds[nfds].fd = pcap_get_selectable_fd(iface.getHandle());//iface.getHandle()->fd;
        fds[nfds].events = POLLIN;
        ++nfds;
    }
    int timeout = (0.5 * 1000); // half-second

    reportStatistics(true);
    while (1)
    {
        int rc = poll(fds, nfds, timeout); // 0 means "timeout expired -> do nothing, but probably, report statistics if there is no packet there
        if (rc > 0)
        {
            for (int i = 0; i < nfds; ++i)
            {
                if (fds[i].revents == POLLIN)
                    interfaces_[i].waitForPacket();
                fds[i].revents = 0;
            }
        }
        else if (rc < 0)
        {
          perror("  poll() failed");
          break;
        }
        reportStatistics(false);
    }
    return OK;
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
    unsigned int currentTime = portableGetCurrentTimeSec();
    unsigned int deltaTime = currentTime - lastStatTime_;
    if (deltaTime < 60*1) // every 1 min
        return; // too early to do something
    for (auto it = interfaces_.begin(); it != interfaces_.end(); ++it)
    {
        it->reportStatistics(bFirstTime, deltaTime);
    }

    double totalTime = currentTime - startTime_;
    lastStatTime_ = currentTime;
    const char *pszRank = "Rank";                   int iRankW = strlen(pszRank);
    const char *pszInterface = "Interface";         int iInterfaceW = strlen(pszInterface);
    const char *pszApplication = "Application";     int iApplicationW = strlen(pszApplication);
    const char *pszService = "Service";             int iServiceW = strlen(pszService);
    const char *pszPackets = "Packets";             int iPacketsW = strlen(pszPackets);
    const char *pszBytes = "Bytes";                 int iBytesW = strlen(pszBytes);
    const char *pszIpAddress = "IP-address";        int iIpAddressW = strlen(pszIpAddress);
    const char *pszProto = "Proto";                 int iProtoW = strlen(pszProto);
    const char *pszPort = "Port";                   int iPortW = strlen(pszPort);
    const char *pszUrl = "URL";                     int iUrlW = strlen(pszUrl);
    const char *pszKBytesPerHour = "KB/hour";       int iKBytesPerHourW = strlen(pszKBytesPerHour);
    const char *pszMBytesPerDay = "MB/day";         int iMBytesPerDayW = strlen(pszMBytesPerDay);
    const char *pszGBytesPerMonth = "GB/month";     int iGBytesPerMonthW = strlen(pszGBytesPerMonth);
    const int nSpacesBetweenColumns = 2;

    // print out top-talkers to us
    const int N_MAX_REPORTED_TALKERS = 20;
    FILE *pFile = fopen("top_talkers.txt", "w");
    if (pFile)
    {
        fprintf(pFile, "Total measurement time = %.0f seconds\n\n", totalTime);
        if (totalTime != 0)
        {
            for (auto &iface : interfaces_)
                iface.reportOverallStat(pFile, totalTime);
        }

        topTalkers_.clear();
        for (auto &iface : interfaces_)
            iface.fillTopTalkers(topTalkers_);
        std::sort(topTalkers_.begin(), topTalkers_.end(), [](const Talker &t1, const Talker &t2){ return t1.second->getBytes() > t2.second->getBytes(); });

        // header
        if (topTalkers_.size() > N_MAX_REPORTED_TALKERS)
            fprintf(pFile, "%d top WAN-talkers (of totally %d talkers):\n", N_MAX_REPORTED_TALKERS, (int)topTalkers_.size());
        else
            fprintf(pFile, "Top WAN-talkers:\n");

        int nSpaces;
        for (int iPass = 1; iPass <= 2; ++iPass)
        {
            // a little bit stupid but..... :-)
            if (iPass == 2)
            {
                nSpaces = iRankW            - fprintf(pFile, "%s", pszRank);           putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iInterfaceW       - fprintf(pFile, "%s", pszInterface);      putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iApplicationW     - fprintf(pFile, "%s", pszApplication);    putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iServiceW         - fprintf(pFile, "%s", pszService);        putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iPacketsW         - fprintf(pFile, "%s", pszPackets);        putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iBytesW           - fprintf(pFile, "%s", pszBytes);          putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iIpAddressW       - fprintf(pFile, "%s", pszIpAddress);      putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iProtoW           - fprintf(pFile, "%s", pszProto);          putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iPortW            - fprintf(pFile, "%s", pszPort);           putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iUrlW             - fprintf(pFile, "%s", pszUrl);            putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iKBytesPerHourW   - fprintf(pFile, "%s", pszKBytesPerHour);  putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iMBytesPerDayW    - fprintf(pFile, "%s", pszMBytesPerDay);   putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                nSpaces = iGBytesPerMonthW  - fprintf(pFile, "%s", pszGBytesPerMonth); //putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                fprintf(pFile, "\n");
            }
            int nReported = 0;
            for (auto &talker : topTalkers_)
            {
                if (nReported >= N_MAX_REPORTED_TALKERS)
                    break;

                const ServiceApp *pServiceApp = talker.first;
                ServiceStat *pStat = talker.second;

                const std::string &ifaceName = std::get<0>(*pServiceApp);
                IPADDRESS_TYPE IP            = std::get<1>(*pServiceApp);
                IPPORT portNo                = std::get<2>(*pServiceApp);
                unsigned char proto          = std::get<3>(*pServiceApp);
                const std::string &appName   = std::get<4>(*pServiceApp);

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
                else if (proto == IPPROTO_ICMP)
                {
                    if (portNo == 0)
                        service = "EchoReply";
                    else if (portNo == 3)
                        service = "DestUnreach";
                    else if (portNo == 4)
                        service = "SrcQuench";
                    else if (portNo == 8)
                        service = "EchoRequest";
                    else if (portNo == 9)
                        service = "RouterAdv";
                    else if (portNo == 10)
                        service = "RouterSolic";
                    else if (portNo == 11)
                        service = "TimeExceed";
                }

                ++nReported;

                // a little bit stupid but..... :-)
                if (iPass == 1)
                {
                    char szTmp[512];
                    iRankW            = std::max(iRankW, sprintf(szTmp, "%d.", nReported));
                    iInterfaceW       = std::max(iInterfaceW, sprintf(szTmp, "%s", ifaceName.c_str()));
                    iApplicationW     = std::max(iApplicationW, sprintf(szTmp, "%s", appName.c_str()));
                    iServiceW         = std::max(iServiceW, sprintf(szTmp, "%s", service.c_str()));
                    iPacketsW         = std::max(iPacketsW, sprintf(szTmp, "%d", pStat->getPackets()));
                    iBytesW           = std::max(iBytesW, sprintf(szTmp, "%d", pStat->getBytes()));
                    iIpAddressW       = std::max(iIpAddressW, sprintf(szTmp, "%s", hostAddress.c_str()));
                    iProtoW           = std::max(iProtoW, sprintf(szTmp, "%s", pszProtoName));
                    iPortW            = std::max(iPortW, sprintf(szTmp, "%d", portNo));
                    iUrlW             = std::max(iUrlW, sprintf(szTmp, "%s", URL.c_str()));
                    iKBytesPerHourW   = std::max(iKBytesPerHourW, sprintf(szTmp, "%.3f", pStat->getBytes()/1.E3*3600./totalTime));
                    iMBytesPerDayW    = std::max(iMBytesPerDayW, sprintf(szTmp, "%.3f", pStat->getBytes()/1.E6*3600.*24./totalTime));
                    iGBytesPerMonthW  = std::max(iGBytesPerMonthW, sprintf(szTmp, "%.3f", pStat->getBytes()/1.E9*3600.*24.*30./totalTime));
                }
                else
                {
                    nSpaces = iRankW - fprintf(pFile, "%d.", nReported);                                                 putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iInterfaceW - fprintf(pFile, "%s", ifaceName.c_str());                                     putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iApplicationW - fprintf(pFile, "%s", appName.c_str());                                     putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iServiceW - fprintf(pFile, "%s", service.c_str());                                         putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iPacketsW - fprintf(pFile, "%d", pStat->getPackets());                                     putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iBytesW - fprintf(pFile, "%d", pStat->getBytes());                                         putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iIpAddressW - fprintf(pFile, "%s", hostAddress.c_str());                                   putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iProtoW - fprintf(pFile, "%s", pszProtoName);                                              putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iPortW - fprintf(pFile, "%d", portNo);                                                     putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iUrlW - fprintf(pFile, "%s", URL.c_str());                                                 putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iKBytesPerHourW - fprintf(pFile, "%.3f", pStat->getBytes()/1.E3*3600./totalTime);          putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iMBytesPerDayW - fprintf(pFile, "%.3f", pStat->getBytes()/1.E6*3600.*24./totalTime);       putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    nSpaces = iGBytesPerMonthW - fprintf(pFile, "%.3f", pStat->getBytes()/1.E9*3600.*24.*30./totalTime); //putSpaces(pFile, nSpacesBetweenColumns + nSpaces);
                    fprintf(pFile, "\n");
                }
            }
        }
        fclose(pFile);
    }
}

