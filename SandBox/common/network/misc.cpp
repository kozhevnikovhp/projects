#include <string.h>
#include <stdio.h>

#include "misc.h"

#ifdef SOCKETS_WSA
#include <Winsock2.h>
#endif

namespace common {

namespace network {


////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE getIP(const sockaddr *pSockAddr)
{
	struct sockaddr_in *p = (struct sockaddr_in *)pSockAddr;
#ifdef SOCKETS_WSA
	return p->sin_addr.S_un.S_addr;
#endif
#ifdef SOCKETS_BSD
	return p->sin_addr.s_addr;
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
	struct sockaddr_in *p = (struct sockaddr_in *)pSockAddr;
#ifdef SOCKETS_WSA
	p->sin_addr.S_un.S_addr = IP;
#endif
#ifdef SOCKETS_BSD
	p->sin_addr.s_addr = IP;
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

IPADDRESS_TYPE stringToAddress(const std::string &str)
{
    return stringToAddress(str.c_str());
}

IPADDRESS_TYPE stringToAddress(const char *pszStr)
{
    struct hostent *pHost = gethostbyname(pszStr);
    if (!pHost)
    {
        perror("gethostbyname");
        return 0;
    }
    char *p = pHost->h_addr_list[0];
    IPADDRESS_TYPE *pIpAddress = (IPADDRESS_TYPE *)p;
    IPADDRESS_TYPE ip = *pIpAddress;

    return ip;
}

std::string addressToDotNotation(IPADDRESS_TYPE Address)
{
    struct in_addr inaddr;
    setIP(&inaddr, Address);
    std::string str(inet_ntoa(inaddr));
    return str;
}

std::string addressToHostName(IPADDRESS_TYPE ip)
{
 	struct in_addr inaddr;
    setIP(&inaddr, ip);

    struct hostent *pHost = gethostbyaddr((const char *)&inaddr,
                                         sizeof inaddr, AF_INET);
    if (!pHost)
    {
        perror("gethostbyname");
        return "";
    }
    std::string strName(pHost->h_name);
    return strName;
}

std::string getFullName(IPADDRESS_TYPE ip)
{
    std::string str = addressToDotNotation(ip);
    std::string strSymbolicName = addressToHostName(ip);
    if (!strSymbolicName.empty())
    {
        str += '(';
        str += strSymbolicName;
        str += ')';
    }
    return str;
}

bool isTheSameSubnet(IPADDRESS_TYPE a1, IPADDRESS_TYPE a2, IPADDRESS_TYPE subnetMask)
{
    return ((a1 & subnetMask) == (a2 & subnetMask));
}

bool findBestInterface(IPADDRESS_TYPE IP, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask, std::string &ifaceName)
{
    //
    // check all interfaces to find better pair Address/Mask to be in the same net as target IP
#if (WIN32)

#elif (UNIX)
    // http://forum.sources.ru/index.php?showtopic=78789
    // find IP address and mask of the interface
    char szHostName[128] = "";
    gethostname(szHostName, sizeof(szHostName));
    printf("%s\n", szHostName);
    struct hostent *pHost = gethostbyname(szHostName);
    if (!pHost)
    {
        perror("gethostbyname");
    }
    char *p = pHost->h_addr_list[0];
    for (int i = 0; pHost->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, pHost
               ->h_addr_list[i], sizeof(struct in_addr));
        printf("Address %d:%s\n", i, inet_ntoa(addr));
    }
#endif
}

int Count1s(IPADDRESS_TYPE Address)
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

IPADDRESS_TYPE GetSubnetMaskByLength(int nMaskLength)
{
    static IPADDRESS_TYPE uMasks[] = {
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
        0xFFFFFFFF, // "255.255.255.255", /32
    };

    if (nMaskLength <= 0 || nMaskLength > 32)
        return 0;
    return uMasks[nMaskLength];
}

// including 0th and last broadcast addresses, just 2**(32-MaskLength)
int CalcMaxPossibleHosts(IPADDRESS_TYPE subnetMask)
{
    return CalcMaxPossibleHosts(Count1s(subnetMask));
}

int CalcMaxPossibleHosts(int nMaskLength)
{
    static int nHosts[] = {
        0x00000001,	0x00000002,	0x00000004,	0x00000008,	0x00000010,	0x00000020,	0x00000040,	0x00000080,
        0x00000100,	0x00000200,	0x00000400,	0x00000800,	0x00001000,	0x00002000,	0x00004000,	0x00008000,
        0x00010000,	0x00020000,	0x00040000,	0x00080000,	0x00100000,	0x00200000,	0x00400000,	0x00800000,
        0x01000000, 0x02000000, 0x04000000,	0x08000000,	0x10000000, 0x20000000,	0x40000000, 0x80000000
    };
    
    if (nMaskLength <= 0 || nMaskLength > 32)
        return 0;
    return nHosts[32-nMaskLength];
}


} // namespace network

} // namespace common

