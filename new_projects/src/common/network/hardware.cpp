#include <string.h>
#include <stdio.h>
#include "ip.h"
#include "hardware.h"

#ifdef SOCKETS_WSA
#include <Winsock2.h>
#endif

#ifdef SOCKETS_BSD
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#ifdef OS_SOLARIS
#include <sys/sockio.h>  //SIOCGIFHWADDR
#endif // OS_SOLARIS

namespace common {

namespace network {


bool getMacAddress(char *pszAddress, const char *pszIfaceName)
{
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, pszIfaceName);

#ifdef OS_SOLARIS
    bool bSuccess = (ioctl(sock, SIOCGENADDR, &ifr) >= 0);
#else
    bool bSuccess = (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0);
#endif // OS_SOLARIS
    close(sock);

#ifdef OS_SOLARIS
    sprintf(pszAddress, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
         (unsigned char)ifr.ifr_ifru.ifru_enaddr[0],
         (unsigned char)ifr.ifr_ifru.ifru_enaddr[1],
         (unsigned char)ifr.ifr_ifru.ifru_enaddr[2],
         (unsigned char)ifr.ifr_ifru.ifru_enaddr[3],
         (unsigned char)ifr.ifr_ifru.ifru_enaddr[4],
         (unsigned char)ifr.ifr_ifru.ifru_enaddr[5]);
#else
    sprintf(pszAddress, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
         (unsigned char)ifr.ifr_hwaddr.sa_data[0],
         (unsigned char)ifr.ifr_hwaddr.sa_data[1],
         (unsigned char)ifr.ifr_hwaddr.sa_data[2],
         (unsigned char)ifr.ifr_hwaddr.sa_data[3],
         (unsigned char)ifr.ifr_hwaddr.sa_data[4],
         (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
#endif // OS_SOLARIS
    return bSuccess;    
}


} // namespace network

} // namespace common

