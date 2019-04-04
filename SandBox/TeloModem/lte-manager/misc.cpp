/*
 *  misc.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <algorithm>
#include <regex>
#include <fstream>

#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "misc.h"

////////////////////////////////////////////////////////////////////////
// Misc

double getCurrentTimeSec()
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + t.tv_nsec/1.E+09;
}

double getCurrentTimeMSec()
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec*1.E+03 + t.tv_nsec/1.E+06;
}

double getCurrentTimeUSec()
{
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec*1.E+06 + t.tv_nsec/1.E+03;
}

IPADDRESS_TYPE getIP(const sockaddr_in *pSockAddr)
{
    return pSockAddr->sin_addr.s_addr;
}

void setIP(sockaddr_in *pSockAddr, IPADDRESS_TYPE IP)
{
    pSockAddr->sin_family = AF_INET;
    pSockAddr->sin_addr.s_addr = IP;
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

bool isInterfaceUp(const std::string &ifaceName, bool &bUp)
{
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = true;
    struct ifreq iface;
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);

    if (ioctl(sock, SIOCGIFFLAGS, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCGIFFLAGS) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    else
    {
        bUp = ((IFF_UP & iface.ifr_flags) != 0);
    }

    close(sock);
    return bSuccess;
}

bool putInterfaceDown(const std::string &ifaceName)
{
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = true;
    struct ifreq iface;
    memset(&iface, 0, sizeof(iface));
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);

    if (ioctl(sock, SIOCGIFFLAGS, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCGIFFLAGS) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    if (bSuccess)
    {
        iface.ifr_flags &= ~IFF_UP;
        if (ioctl(sock, SIOCSIFFLAGS, &iface) < 0)
        {
            std::string strError = "ioctl(SIOCSIFFLAGS) for interface ";
            strError += ifaceName;
            perror(strError.c_str());
            bSuccess = false;
        }
    }

    close(sock);
    return bSuccess;
}

bool setInterfaceUp(const std::string &ifaceName)
{
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = true;
    struct ifreq iface;
    memset(&iface, 0, sizeof(iface));
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);

    if (ioctl(sock, SIOCGIFFLAGS, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCGIFFLAGS) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    if (bSuccess)
    {
        iface.ifr_flags |= IFF_UP;
        if (ioctl(sock, SIOCSIFFLAGS, &iface) < 0)
        {
            std::string strError = "ioctl(SIOCSIFFLAGS) for interface ";
            strError += ifaceName;
            perror(strError.c_str());
            bSuccess = false;
        }
    }

    close(sock);
    return bSuccess;
}

bool getInterfaceAddressAndMask(const std::string &ifaceName, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask)
{
    ifaceIP = ifaceMask = 0;
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = true;
    struct ifreq iface;
    memset(&iface, 0, sizeof(iface));
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);

    // IP-address
    if (ioctl(sock, SIOCGIFADDR, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCGIFADDR) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    else
    {
        ifaceIP = getIP((sockaddr_in *)&iface.ifr_addr);
    }

    // SubnetMask
    if (ioctl(sock, SIOCGIFNETMASK, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCGIFNETMASK) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    else
    {
        ifaceMask = getIP((sockaddr_in *)&iface.ifr_netmask);
    }

    close(sock);
    return bSuccess;
}

bool setInterfaceIpAddress(const std::string &ifaceName, IPADDRESS_TYPE ifaceIP)
{
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = true;
    struct ifreq iface;
    memset(&iface, 0, sizeof(iface));
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);

    setIP((sockaddr_in *)&iface.ifr_addr, ifaceIP);

    if (ioctl(sock, SIOCSIFADDR, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCSIFADDR) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    close(sock);
    return bSuccess;
}

bool setInterfaceMask(const std::string &ifaceName, IPADDRESS_TYPE mask)
{
    if (ifaceName.size() >= IFNAMSIZ)
        return false; // wrong (too long) name
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    bool bSuccess = true;
    struct ifreq iface;
    memset(&iface, 0, sizeof(iface));
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);

    setIP((sockaddr_in *)&iface.ifr_netmask, mask);

    if (ioctl(sock, SIOCSIFNETMASK, &iface) < 0)
    {
        std::string strError = "ioctl(SIOCSIFNETMASK) for interface ";
        strError += ifaceName;
        perror(strError.c_str());
        bSuccess = false;
    }
    close(sock);
    return bSuccess;
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
    memset(&iface, 0, sizeof(iface));
    iface.ifr_addr.sa_family = AF_INET;
    strncpy(iface.ifr_name, ifaceName.c_str(), IFNAMSIZ);
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

// to avoid including <linux/route.h>
#ifndef RTF_GATEWAY
#define	RTF_GATEWAY	0x0002		// destination is a gateway
#endif

bool getGateway(IPADDRESS_TYPE &GW)
{
    GW = 0;
    std::ifstream f("/proc/net/route");
    if (!f.is_open())
        return false;

    // Iface	Destination	Gateway 	Flags	RefCnt	Use	Metric	Mask	MTU	Window	IRTT
    bool bSuccess = false;
    std::string line;
    char szName[IFNAMSIZ+1];
    unsigned int uiDest, uiGW, uiFlags;
    while (!f.eof() && !f.bad() && !f.fail())
    {
        std::getline(f, line);
        trimBlanks(line);
        if (line.empty())
            continue;
        int nScanfed = sscanf(line.c_str(), "%s %X %X %X", szName, &uiDest, &uiGW, &uiFlags);
        if (nScanfed == 4)
        {
            if (uiDest != 0)
                continue; // probably gateway, but not default one
            if (uiFlags & RTF_GATEWAY)
            {
                GW = uiGW;
                bSuccess = true;
                break;
            }
        }
    }

    return bSuccess;
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

bool makeDirRecursively(const std::string &path, mode_t mode)
{
    const char cSeparator = '/'; // on Unix/Linux
    std::string fullDirName, dirName;

    std::stringstream ss(path);
    while (!ss.eof() && !ss.bad() && !ss.fail())
    {
        std::getline(ss, dirName, cSeparator);
        if (dirName.empty())
            continue;
        fullDirName += cSeparator;
        fullDirName += dirName;
        int ec = ::mkdir(fullDirName.c_str(), mode);
        if (ec != 0)
        {
            if (errno != EEXIST)
                return false;
        }
    }
    return true;
}

