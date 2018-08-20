/*
 *  misc.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <stdio.h>

#include "misc.h"


////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE getIP(const sockaddr *pSockAddr)
{
    return ((sockaddr_in *)pSockAddr)->sin_addr.s_addr;
}

IPADDRESS_TYPE getIP(const in_addr *pAddr)
{
	return pAddr->s_addr;
}

void setIP(sockaddr *pSockAddr, IPADDRESS_TYPE IP)
{
    ((sockaddr_in *)pSockAddr)->sin_addr.s_addr = IP;
}

void setIP(sockaddr_in *pSockAddr, IPADDRESS_TYPE IP)
{
    pSockAddr->sin_addr.s_addr = IP;
}

void setIP(in_addr *pAddr, IPADDRESS_TYPE IP)
{
	pAddr->s_addr = IP;
}


// TODO: make it better
// PROBLEM: gethostbyname(), inet_*() are declared UNSAFE and DEPRECATED in LINUX, but in Windows is OK
// At the same time inet_pton cannot be compiled on WINDOWS, have no idea why
IPADDRESS_TYPE dotNotationToAddress(const char *pszStr)
{
    IPADDRESS_TYPE IP = inet_addr(pszStr);
    return IP;
}

// TODO: make it better
// PROBLEM: inet_*() is declared UNSAFE and DEPRECATED in LINUX, but in Windows is OK
// At the same time inet_pton cannot be compiled on WINDOWS
std::string addressToDotNotation(IPADDRESS_TYPE IP)
{
    sockaddr_in sa;
    sa.sin_addr.s_addr = IP;
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sa.sin_addr), buffer, sizeof(buffer));
    std::string hostName(buffer);
    return hostName;
}

/*std::string addressToHostName(IPADDRESS_TYPE IP)
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
}*/

bool isTheSameSubnet(IPADDRESS_TYPE a1, IPADDRESS_TYPE a2, IPADDRESS_TYPE subnetMask)
{
    return ((a1 & subnetMask) == (a2 & subnetMask));
}

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

bool getInterfaceAddressAndMask(const std::string &ifaceName, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask)
{
    ifaceIP = ifaceMask = 0;
    // check all interfaces to find better pair Address/Mask to be in the same net as target IP
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
}

// application must provide at least 6 bytes for the return value
bool getInterfaceMacAddress(const std::string &ifaceName, void *pAddress)
{
    char buf[2048];
    struct ifconf ifc;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    // Query available interfaces.
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        perror("ioctl(SIOCGIFCONF)");
        close(sock);
        return false;
    }

    // Iterate through the list of interfaces.
    bool bSuccess = false;
    int nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
    for (int i = 0; i < nInterfaces; i++)
    {
        struct ifreq *pInterface = &ifc.ifc_req[i];
        //printf("Iface %d = %s\n", i, pInterface->ifr_name);

        // device name
        if (ifaceName.compare(pInterface->ifr_name))
            continue;
        if (ioctl(sock, SIOCGIFHWADDR, pInterface) >= 0)
        {
            memcpy(pAddress,  pInterface->ifr_hwaddr.sa_data, 6);
            bSuccess = true;
            break;
        }
        else
        {
            perror("ioctl(SIOCGIFHWADDR)");
        }
    }
    close(sock);
    return bSuccess;
}

bool getInterfaceGateway(const std::string &ifaceName, IPADDRESS_TYPE &GW)
{
    GW = 0;
    return true;
}

