#include <string.h>
#include <stdio.h>

#include "misc.h"

#ifdef SOCKETS_WSA
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "IpHelper.h"
#elif (SOCKETS_BSD)
#endif


////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE getIP(const sockaddr *pSockAddr)
{
#ifdef SOCKETS_WSA
    return ((sockaddr_in *)pSockAddr)->sin_addr.S_un.S_addr;
#endif
#ifdef SOCKETS_BSD
    return ((sockaddr_in *)pSockAddr)->sin_addr.s_addr;
#endif
}

IPADDRESS_TYPE getIP(const in_addr *pAddr)
{
#ifdef SOCKETS_WSA
	return pAddr->S_un.S_addr;
#endif
#ifdef SOCKETS_BSD
	return pAddr->s_addr;
#endif
}

void setIP(sockaddr *pSockAddr, IPADDRESS_TYPE IP)
{
#ifdef SOCKETS_WSA
    ((sockaddr_in *)pSockAddr)->sin_addr.S_un.S_addr = IP;
#endif
#ifdef SOCKETS_BSD
    ((sockaddr_in *)pSockAddr)->sin_addr.s_addr = IP;
#endif
}

void setIP(sockaddr_in *pSockAddr, IPADDRESS_TYPE IP)
{
#ifdef SOCKETS_WSA
    pSockAddr->sin_addr.S_un.S_addr = IP;
#endif
#ifdef SOCKETS_BSD
    pSockAddr->sin_addr.s_addr = IP;
#endif
}

void setIP(in_addr *pAddr, IPADDRESS_TYPE IP)
{
#ifdef SOCKETS_WSA
	pAddr->S_un.S_addr = IP;
#endif
#ifdef SOCKETS_BSD
	pAddr->s_addr = IP;
#endif
}

IPADDRESS_TYPE dotNotationToAddress(const std::string &str)
{
    return dotNotationToAddress(str.c_str());
}

// TODO: make it better
// PROBLEM: gethostbyname(), inet_*() are declared UNSAFE and DEPRECATED in LINUX, but in Windows is OK
// At the same time inet_pton cannot be compiled on WINDOWS, have no idea why
IPADDRESS_TYPE dotNotationToAddress(const char *pszStr)
{
    IPADDRESS_TYPE IP = inet_addr(pszStr);
    return IP;
/*#ifdef SOCKETS_WSA
    IPADDRESS_TYPE IP = inet_addr(pszStr);
    return IP;
#endif
#ifdef SOCKETS_BSD
    sockaddr_in sa;
    inet_pton(AF_INET, pszStr, &(sa.sin_addr));
    return sa.sin_addr.s_addr;
#endif*/
}

// TODO: make it better
// PROBLEM: inet_*() is declared UNSAFE and DEPRECATED in LINUX, but in Windows is OK
// At the same time inet_pton cannot be compiled on WINDOWS
std::string addressToDotNotation(IPADDRESS_TYPE IP)
{
#ifdef SOCKETS_WSA
    struct in_addr inaddr;
    setIP(&inaddr, IP);
    std::string hostName(inet_ntoa(inaddr));
    return hostName;
#endif
#ifdef SOCKETS_BSD
    sockaddr_in sa;
    sa.sin_addr.s_addr = IP;
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sa.sin_addr), buffer, sizeof(buffer));
    std::string hostName(buffer);
    return hostName;
#endif
}

std::string addressToHostName(IPADDRESS_TYPE IP)
{
    std::string hostName;
    sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    setIP(&addr, IP);
    addr.sa_family = AF_INET;
    char hbuf[NI_MAXHOST];
    memset(hbuf, 0, sizeof(hbuf));

    bool bSuccess = (getnameinfo(&addr, addrlen,
                                 hbuf, sizeof(hbuf),
                                 NULL, 0,
                                 0) == 0); // CRASHES HERE under LINUX!!! IN CASE OF STATIC LINK
    if (bSuccess)
        hostName.append(hbuf);

    return hostName;
}

std::string getServiceName(IPADDRESS_TYPE IP, IPPORT portNo, bool bUDP)
{
    std::string serviceName;
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    setIP(&addr, IP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNo);
    char sbuf[NI_MAXSERV];
    memset(sbuf, 0, sizeof(sbuf));

    int flags = 0;
    if (bUDP)
        flags |= NI_DGRAM;
    bool bSuccess = (getnameinfo((sockaddr *)&addr, addrlen,
                                 NULL, 0,
                                 sbuf, sizeof(sbuf),
                                 flags) == 0); // CRASHES HERE under LINUX!!! IN CASE OF STATIC LINK
    if (bSuccess)
        serviceName.append(sbuf);

    return serviceName;
}

std::string getFullName(IPADDRESS_TYPE IP)
{
    std::string hostName = addressToDotNotation(IP);
    std::string strSymbolicName = addressToHostName(IP);
    if (!strSymbolicName.empty())
    {
        hostName += '(';
        hostName += strSymbolicName;
        hostName += ')';
    }
    return hostName;
}

bool isTheSameSubnet(IPADDRESS_TYPE a1, IPADDRESS_TYPE a2, IPADDRESS_TYPE subnetMask)
{
    return ((a1 & subnetMask) == (a2 & subnetMask));
}

#ifdef SOCKETS_BSD
bool isItInterfaceName(const std::string &ifaceName)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }
    bool b = isItInterfaceName(ifaceName, sock);
    close(sock);
    return b;
}

bool isItInterfaceName(const std::string &ifaceName, int sock)
{
    // http://forum.sources.ru/index.php?showtopic=78789
    char buf[2048];
    struct ifconf ifc;
    // Query available interfaces.
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        perror("ioctl(SIOCGIFCONF)");
        return false;
    }

    // Iterate through the list of interfaces.
    int nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
    for (int i = 0; i < nInterfaces; i++)
    {
        struct ifreq *pInterface = &ifc.ifc_req[i];

        // device name
        if (!ifaceName.compare(pInterface->ifr_name))
            return true;
    }
    return false;
}
#endif

bool findBestInterface(IPADDRESS_TYPE IP, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask, std::string &ifaceName)
{
    ifaceIP = ifaceMask = 0;
    // check all interfaces to find better pair Address/Mask to be in the same net as target IP
#if (SOCKETS_WSA)
    //printf("This workstation has the following IP-addresses:\n");
    IpHelper helper;
    unsigned int i;
    for (i = 0; i < helper.GetIpAddressesCount(); i++)
    {
        IPADDRESS_TYPE thisIP = helper.GetIpAddress(i);
        IPADDRESS_TYPE thisMask = helper.GetIpSubnetMask(i);
        //std::string str = helper.GetIfaceDesc(i);
        //printf("IP-Address %d : %s/%s\n", i, addressToDotNotation(thisIP).c_str(), addressToDotNotation(thisMask).c_str());
        if (isTheSameSubnet(IP, thisIP, thisMask))
        {
            ifaceIP = thisIP;
            ifaceMask = thisMask;
            return true;
        }
    }
    return false;
#elif (SOCKETS_BSD)
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }
    bool b = findBestInterface(IP, ifaceIP, ifaceMask, ifaceName, sock);
    close(sock);
    return b;
#endif
}

#ifdef SOCKETS_BSD
bool findBestInterface(IPADDRESS_TYPE IP, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask, std::string &ifaceName, int sock)
{
    char buf[2048];
    struct ifconf ifc;
    // Query available interfaces.
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        perror("ioctl(SIOCGIFCONF)");
        return false;
    }

    // Iterate through the list of interfaces.
    int nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
    for (int i = 0; i < nInterfaces; i++)
    {
        struct ifreq *pInterface = &ifc.ifc_req[i];
        // device name
        ifaceName = pInterface->ifr_name;
        // IP address
        ifaceIP = getIP(&pInterface->ifr_addr);
        //printf("%s: IP %s", pInterface->ifr_name, addressToDotNotation(ifaceIP).c_str());

        // SubnetMask
        if(ioctl(sock, SIOCGIFNETMASK, pInterface) < 0)
        {
            perror("ioctl SIOCGIFNETMASK");
            return false;
        }
        ifaceMask = getIP(&pInterface->ifr_netmask);
        //printf(", MASK %s", addressToDotNotation(ifaceMask).c_str());
        if (isTheSameSubnet(IP, ifaceIP, ifaceMask))
            return true;

        // Get the broadcast address
        //if (ioctl(sock, SIOCGIFBRDADDR, pInterface) >= 0)
        //{
        //    IPADDRESS_TYPE broadcastIP = getIP(&pInterface->ifr_broadaddr);
        //    printf(", BROADCAST %s", addressToDotNotation(broadcastIP).c_str());
        //}

        // Get the MAC address
        //if(ioctl(sock, SIOCGIFHWADDR, pInterface) >= 0)
        //{
        //}

        //printf("\n");
    }
     return false; // not found
}
#endif


bool getInterfaceAddressAndMask(const std::string &ifaceName, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask)
{
    ifaceIP = ifaceMask = 0;
    // check all interfaces to find better pair Address/Mask to be in the same net as target IP
#if (SOCKETS_WSA)
    //printf("This workstation has the following IP-addresses:\n");
/*    IpHelper helper;
    unsigned int i;
    for (i = 0; i < helper.GetIpAddressesCount(); i++)
    {
        IPADDRESS_TYPE thisIP = helper.GetIpAddress(i);
        IPADDRESS_TYPE thisMask = helper.GetIpSubnetMask(i);
        //std::string str = helper.GetIfaceDesc(i);
        //printf("IP-Address %d : %s/%s\n", i, addressToDotNotation(thisIP).c_str(), addressToDotNotation(thisMask).c_str());
        if (isTheSameSubnet(IP, thisIP, thisMask))
        {
            ifaceIP = thisIP;
            ifaceMask = thisMask;
            return true;
        }
    }*/
    return false;
#elif (SOCKETS_BSD)
    // http://forum.sources.ru/index.php?showtopic=78789
    char buf[2048];
    struct ifconf ifc;
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        perror("socket");
        return false;
    }

    // Query available interfaces.
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(s, SIOCGIFCONF, &ifc) < 0)
    {
        perror("ioctl(SIOCGIFCONF)");
        close(s);
        return false;
    }

    // Iterate through the list of interfaces.
    int nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
    for (int i = 0; i < nInterfaces; i++)
    {
        struct ifreq *pInterface = &ifc.ifc_req[i];
        //printf("Iface %d = %s\n", i, pInterface->ifr_name);

        // device name
        if (ifaceName.compare(pInterface->ifr_name))
            continue;
        // IP address
        ifaceIP = getIP(&pInterface->ifr_addr);
        //printf("%s: IP %s", pInterface->ifr_name, addressToDotNotation(ifaceIP).c_str());

        // SubnetMask
        if(ioctl(s, SIOCGIFNETMASK, pInterface) < 0)
        {
            perror("ioctl SIOCGIFNETMASK");
            close(s);
            return false;
        }
        ifaceMask = getIP(&pInterface->ifr_netmask);
        //printf(", MASK %s", addressToDotNotation(ifaceMask).c_str());
        break;
    }
    close(s);
    return (ifaceIP != 0 && ifaceMask != 0);
#endif
}

int count1s(IPADDRESS_TYPE Address)
{
    // count 1s
    int n1Count = 0;
    for (int i = 0; i < 32; i++)
    {
        if (Address & (1 << i))
           ++n1Count;
    }
    return n1Count;
}

IPADDRESS_TYPE getSubnetMaskByLength(int nMaskLength)
{
    IPADDRESS_TYPE uMasks[] = {
        0x00000000, // "0.0.0.0"  , /0
        0x00000080, // "128.0.0.0", /1
        0x000000C0, // "192.0.0.0", /2
        0x000000E0, // "224.0.0.0", /3
        0x000000F0, // "240.0.0.0", /4
        0x000000F8, // "248.0.0.0", /5
        0x000000FC, // "252.0.0.0", /6
        0x000000FE, // "254.0.0.0", /7
        0x000000FF, // "255.0.0.0", /8
        0x000080FF, // "255.128.0.0", /9
        0x0000C0FF, // "255.192.0.0", /10
        0x0000E0FF, // "255.224.0.0", /11
        0x0000F0FF, // "255.240.0.0", /12
        0x0000F8FF, // "255.248.0.0", /13
        0x0000FCFF, // "255.252.0.0", /14
        0x0000FEFF, // "255.254.0.0", /15
        0x0000FFFF, // "255.255.0.0", /16
        0x0080FFFF, // "255.255.128.0", /17
        0x00C0FFFF, // "255.255.192.0", /18
        0x00E0FFFF, // "255.255.224.0", /19
        0x00F0FFFF, // "255.255.240.0", /20
        0x00F8FFFF, // "255.255.248.0", /21
        0x00FCFFFF, // "255.255.252.0", /22
        0x00FEFFFF, // "255.255.254.0", /23
        0x00FFFFFF, // "255.255.255.0", /24
        0x80FFFFFF, // "255.255.255.128", /25
        0xC0FFFFFF, // "255.255.255.192", /26
        0xE0FFFFFF, // "255.255.255.224", /27
        0xF0FFFFFF, // "255.255.255.240", /28
        0xF8FFFFFF, // "255.255.255.248", /29
        0xFCFFFFFF, // "255.255.255.252", /30
        0xFEFFFFFF, // "255.255.255.254", /31
        0xFFFFFFFF // "255.255.255.255", /32
    };

    if (nMaskLength <= 0 || nMaskLength > 32)
        return 0;
    return uMasks[nMaskLength];
}

// including 0th and last broadcast addresses, just 2**(32-MaskLength)
int calcMaxPossibleHostsInSubnet(IPADDRESS_TYPE subnetMask)
{
    return calcMaxPossibleHostsFromMaskLen(count1s(subnetMask));
}

int calcMaxPossibleHostsFromMaskLen(int nMaskLength)
{
    static unsigned int nHosts[] = {
        0x00000001,	0x00000002,	0x00000004,	0x00000008,	0x00000010,	0x00000020,	0x00000040,	0x00000080,
        0x00000100,	0x00000200,	0x00000400,	0x00000800,	0x00001000,	0x00002000,	0x00004000,	0x00008000,
        0x00010000,	0x00020000,	0x00040000,	0x00080000,	0x00100000,	0x00200000,	0x00400000,	0x00800000,
        0x01000000, 0x02000000, 0x04000000,	0x08000000,	0x10000000, 0x20000000,	0x40000000, 0x80000000
    };
    
    if (nMaskLength <= 0 || nMaskLength > 32)
        return 0;
    return nHosts[32-nMaskLength];
}

void printIpHeader(const SIpHeader *pHeader)
{
    printf(" IP HEADER:\n");
    printf(" |-IP Version : %d\n", pHeader->version);
    printf(" |-IP Header Length : %d octets\n", pHeader->getHeaderLen());
    printf(" |-Type Of Service : %d\n", pHeader->tos);
    printf(" |-IP Total Length : %d octets\n", pHeader->getPacketLen());
    printf(" |-Identification : %d\n", ntohs(pHeader->ident));
    printf(" |-Reserved ZERO Field : %d\n", pHeader->reserved_zero);
    printf(" |-Dont Fragment Field : %d\n", pHeader->dont_fragment);
    printf(" |-More Fragment Field : %d\n", pHeader->more_fragment);
    printf(" |-TTL : %d\n", pHeader->ttl);
    printf(" |-Protocol : %d\n", pHeader->proto);
    printf(" |-Checksum : %d\n", ntohs(pHeader->checksum));
    printf(" |-SRC IP : %s\n", addressToDotNotation(pHeader->sourceIP).c_str());
    printf(" |-DST IP : %s\n", addressToDotNotation(pHeader->destIP).c_str());
    printf(" ============================\n");
}

void printIcmpHeader(const SIcmpHeader *pHeader)
{
    printf("ICMP HEADER:\n");
    printf(" |-Type : %d\n", pHeader->type);
    printf(" |-Code : %d\n", pHeader->code);
    printf(" |-Checksum : %d\n", ntohs(pHeader->checksum));
    printf(" |-ID : %d\n", ntohs(pHeader->id));
    printf(" |-Sequence : %d\n", ntohs(pHeader->seq));
    printf(" ============================\n");
}

void printTcpHeader(const STcpHeader *pHeader)
{
    printf(" TCP HEADER:\n");
    printf(" |-SRC Port : %u\n", pHeader->getSrcPortNo());
    printf(" |-DST Port : %u\n", pHeader->getDstPortNo());
    printf(" |-Seq Number : %u\n", pHeader->getSeqNum());
    printf(" |-Ack Number : %u\n", pHeader->getAckNum());
    printf(" |-Header Length : %d octets\n", pHeader->getHeaderLen());
    printf(" |-FIN : %d\n", pHeader->fin);
    printf(" |-SYN : %d\n", pHeader->syn);
    printf(" |-RST : %d\n", pHeader->rst);
    printf(" |-PSH : %d\n", pHeader->psh);
    printf(" |-ACK : %d\n", pHeader->ack);
    printf(" |-URG : %d\n", pHeader->urg);
    printf(" |-CWR : %d\n", pHeader->cwr);
    printf(" |-ECN : %d\n", pHeader->ecn);
    printf(" |-Window : %d\n", ntohs(pHeader->window));
    printf(" |-Checksum : %d\n", ntohs(pHeader->checksum));
    printf(" |-Urgent Pointer : %d\n", pHeader->urgent_pointer);
    printf(" ============================\n");
}

void printUdpHeader(const SUdpHeader *pHeader)
{
    printf("UDP HEADER:\n");
    printf(" |-SRC Port : %d\n", pHeader->getSrcPortNo());
    printf(" |-DST Port : %d\n", pHeader->getDstPortNo());
    printf(" |-UDP Length : %d\n", ntohs(pHeader->length));
    printf(" |-UDP Checksum : %d\n", ntohs(pHeader->checksum));
    printf(" ============================\n");
}

void printPacketPayload(char *data , int size)
{
    char a , line[17] , c;
    int j;

    //loop over each character and print
    for (int i=0 ; i < size ; i++)
    {
        c = data[i];

        //Print the hex value for every character , with a space. Important to make unsigned
        printf(" %.2x", (unsigned char) c);

        //Add the character to data line. Important to make unsigned
        a = ( c >=32 && c <=128) ? (unsigned char) c : '.';

        line[i%16] = a;

        //if last character of a line , then print the line - 16 characters in 1 line
        if( (i!=0 && (i+1)%16==0) || i == size - 1)
        {
            line[i%16 + 1] = '\0';

            //print a big gap of 10 characters between hex and characters
            printf("          ");

            //Print additional spaces for last lines which might be less than 16 characters in length
            for ( j = strlen(line) ; j < 16; j++)
            {
                printf("   ");
            }

            printf("%s \n" , line);
        }
    }
}
