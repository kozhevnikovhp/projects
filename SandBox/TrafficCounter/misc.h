#ifndef __MISC_H_INCLUDED__
#define __MISC_H_INCLUDED__

#include <string>
#include "ip.h"

////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE getIP(const sockaddr *pSockAddr);
IPADDRESS_TYPE getIP(const in_addr *pAddr);

void setIP(sockaddr *pSockAddr, IPADDRESS_TYPE IP);
void setIP(sockaddr_in *pSockAddr, IPADDRESS_TYPE IP);
void setIP(in_addr *pAddr, IPADDRESS_TYPE IP);

IPADDRESS_TYPE dotNotationToAddress(const std::string &str);
IPADDRESS_TYPE dotNotationToAddress(const char *pszStr);

std::string addressToDotNotation(IPADDRESS_TYPE IP);
std::string addressToHostName(IPADDRESS_TYPE IP);
std::string getServiceName(IPADDRESS_TYPE IP, IPPORT portNo, bool bUDP);
std::string getFullName(IPADDRESS_TYPE IP);

bool isTheSameSubnet(IPADDRESS_TYPE a1, IPADDRESS_TYPE a2, IPADDRESS_TYPE subnetMask);

#ifdef SOCKETS_BSD
bool isItInterfaceName(const std::string &ifaceName);
#endif

bool findBestInterface(IPADDRESS_TYPE IP, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask, std::string &ifaceName);
bool getInterfaceAddressAndMask(const std::string &ifaceName, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask);
int Count1s(IPADDRESS_TYPE Address);
IPADDRESS_TYPE getSubnetMaskByLength(int nMaskLength);
int calcMaxPossibleHostsInSubnet(IPADDRESS_TYPE subnetMask); // including 0th and last broadcast addresses, just 2**MaskLength
int calcMaxPossibleHostsFromMaskLen(int nMaskLength); // including 0th and last broadcast addresses, just 2**MaskLength

void printIpHeader(const SIpHeader *pHeader);
void printIcmpHeader(const SIcmpHeader *pHeader);
void printTcpHeader(const STcpHeader *pHeader);
void printUdpHeader(const SUdpHeader *pHeader);
void printPacketPayload(char *data , int size);

#endif // __MISC_H_INCLUDED__
