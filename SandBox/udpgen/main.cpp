#include <stdio.h>
#include "network/udp.h"
#include "network/misc.h"
#include "portable/portable.h"

using namespace common;
using namespace network;
using namespace portable;

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Not enough arguments\n");
        return 0;
    }

    IPPORT wPort = 8888;

    IPADDRESS_TYPE ip = StringToAddress(argv[1]);
    if (!ip)
    {
        printf("Cannot resolve name %s to IP-address\n", argv[1]);
        return 0;
    }

    size_t length = 100;
    unsigned int dwRate = 1000;
    unsigned int dwWaitEvery = 0;
    unsigned char cTTL = 0;
    char pszBuffer[10000];

    for (int i = 2; i<argc; i++)
    {
        char *pszArg = argv[i];
        if (*pszArg != '-')
            continue;
        switch (*(pszArg+1))
        {
        case 'l':
            sscanf(pszArg+2, "%d", &length);
            break;
        case 'r':
            sscanf(pszArg+2, "%d", &dwRate);
            break;
        case 'p':
            sscanf(pszArg+2, "%ud", &wPort);
            break;
        case 'i':
            sscanf(pszArg+2, "%hu", &cTTL);
            break;
        case 'w':
            sscanf(pszArg+2, "%hu", &dwWaitEvery);
            break;
        default:
            printf("WARNING: Unknown option '%s' - ignored.\n", pszArg);
            break;
        };
    }
    int sent = 0;

    UdpSocket sock;

    if (cTTL > 0)
    {
        if (!sock.SetSendTTL(cTTL))
        {
            printf("ERROR: Cannot set TTL.\n");
            return sock.getLastErrorCode();
        }
    }

    int BytesSent = 0;

    while (1)
    {
        portableSleep(0);
        unsigned int last_ping_time = portableGetCurrentTimeMsec();
        if (sock.WriteTo(pszBuffer, length, BytesSent, ip, wPort))
        {
            sent++;
            printf(" %d sent to %s:%d (length = %d)\n", sent, argv[1], wPort, length);
        }
        else
        {
            if (sock.getLastErrorCode() == ETIMEDOUT)
            {
                portableSleep(100);
                continue;
            }
        }

        unsigned int time_consumed = portableGetCurrentTimeMsec() - last_ping_time;
        if (time_consumed < dwRate)
            portableSleep(dwRate-time_consumed);
        if (dwWaitEvery)
        {
            if (!(sent % dwWaitEvery))
            {
                printf("\n");
                portableSleep(1000);
            }
        }
    }

    return 0;
}
