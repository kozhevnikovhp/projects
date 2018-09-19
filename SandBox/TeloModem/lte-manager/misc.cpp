/*
 *  misc.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <algorithm>
#include <regex>

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
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = false;
    struct ifreq iface;
    strcpy(iface.ifr_name, ifaceName.c_str());
    int ec = ioctl(sock, SIOCGIFHWADDR, &iface);
    if (ec >= 0)
    {
        memcpy(pAddress,  iface.ifr_hwaddr.sa_data, 6);
        bSuccess = true;
    }
    else
    {
        std::string strError = "ioctl(SIOCGIFHWADDR) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
    }

    close(sock);
    return bSuccess;
}

bool getInterfaceGateway(const std::string &ifaceName, IPADDRESS_TYPE &GW)
{
    GW = 0;
    return true;
}


// std::string helpers
void tolower(std::string &s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](int c) -> int { return std::tolower(c); });
}

void toupper(std::string &s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](int c) -> int { return std::toupper(c); });
}

// trim from start (in-place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
}

// trim from end (in-place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base(), s.end());
}

// trim from both ends (in-place)
void trimBlanks(std::string &s)
{
    ltrim(s);
    rtrim(s);
}
