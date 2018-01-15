#include <string.h>
#include <stdio.h>

#include "misc.h"

#ifdef SOCKETS_WSA
#include <Winsock2.h>
#include "IpHelper.h"
#elif (SOCKETS_BSD)
#include <net/if.h>
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

        // device name
        ifaceName = pInterface->ifr_name;
        // IP address
        ifaceIP = getIP(&pInterface->ifr_addr);
        //printf("%s: IP %s", pInterface->ifr_name, addressToDotNotation(ifaceIP).c_str());

        // SubnetMask
        if(ioctl(s, SIOCGIFNETMASK, pInterface) < 0)
        {
            perror("ioctl SIOCGIFNETMASK");
            return false;
        }
        ifaceMask = getIP(&pInterface->ifr_netmask);
        //printf(", MASK %s", addressToDotNotation(ifaceMask).c_str());
        if (isTheSameSubnet(IP, ifaceIP, ifaceMask))
        {
            close(s);
            return true;
        }

        // Get the broadcast address
        //if (ioctl(s, SIOCGIFBRDADDR, pInterface) >= 0)
        //{
        //    IPADDRESS_TYPE broadcastIP = getIP(&pInterface->ifr_broadaddr);
        //    printf(", BROADCAST %s", addressToDotNotation(broadcastIP).c_str());
        //}

        // Get the MAC address
        //if(ioctl(s, SIOCGIFHWADDR, pInterface) >= 0)
        //{
        //}

        //printf("\n");
    }
    close(s);
    return false; // not found
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
int calcMaxPossibleHostsInSubnet(IPADDRESS_TYPE subnetMask)
{
    return calcMaxPossibleHostsFromMaskLen(count1s(subnetMask));
}

int calcMaxPossibleHostsFromMaskLen(int nMaskLength)
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

