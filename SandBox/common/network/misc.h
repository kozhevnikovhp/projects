#ifndef __MISC_H_INCLUDED__
#define __MISC_H_INCLUDED__

#include <string>
#include "ip.h"

namespace common {

namespace network {

////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE getIP(const sockaddr *pSockAddr);
IPADDRESS_TYPE getIP(const in_addr *pAddr);

void setIP(sockaddr *pSockAddr, IPADDRESS_TYPE IP);
void setIP(in_addr *pAddr, IPADDRESS_TYPE IP);

IPADDRESS_TYPE StringToAddress(const std::string &str);
IPADDRESS_TYPE StringToAddress(const char *pszStr);

std::string addressToDotNotation(IPADDRESS_TYPE Address);
std::string addressToHostName(IPADDRESS_TYPE Address);
std::string getFullName(IPADDRESS_TYPE ip);

int Count1s(IPADDRESS_TYPE Address);
IPADDRESS_TYPE GetSubnetMaskByLength(int nMaskLength);
int CalcMaxPossibleHosts(IPADDRESS_TYPE uSubnetMask); // including 0th and last broadcast addresses, just 2**MaskLength
int CalcMaxPossibleHosts(int nMaskLength); // including 0th and last broadcast addresses, just 2**MaskLength

} // namespace network

} // namespace common

#endif // __MISC_H_INCLUDED__
