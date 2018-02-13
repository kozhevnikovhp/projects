#include "counter.h"

int main(int argc, char* argv[])
{
#ifdef SOCKETS_WSA
#define VERSION_MINOR 1
#define VERSION_MAJOR 1

    WSADATA WsaData;
    WORD wVersionRequested = MAKEWORD(VERSION_MINOR, VERSION_MAJOR);

    int err = WSAStartup(wVersionRequested, &WsaData);
    if (err != 0)
        return 0;

    if (LOBYTE(WsaData.wVersion) != VERSION_MAJOR ||
        HIBYTE(WsaData.wVersion) != VERSION_MINOR )
    { // We could not find a usable WinSock DLL
        WSACleanup();
        return 0;
    }
#endif

    TrafficCounter counter;
    if (!counter.isCreated())
        return 1;

    IPADDRESS_TYPE teloIP = 0;
    bool bListenSuccess = false;

#ifdef SOCKETS_WSA
    teloIP = dotNotationToAddress(argv[1]);
    if (teloIP)
        bListenSuccess = counter.listenTo(teloIP);
#endif
#ifdef SOCKETS_BSD
    std::string ifaceName = "eth0";//"enp0s3";
    if (argc > 1)
    {
        ifaceName = argv[1];
        if (!isItInterfaceName(ifaceName))
        {
            teloIP = dotNotationToAddress(argv[1]);
        }
    }

    if (teloIP)
        bListenSuccess = counter.listenTo(teloIP);
    else
        bListenSuccess = counter.listenTo(ifaceName);
#endif //SOCKETS_BSD

    if (!bListenSuccess)
        return 1;

    counter.reportStatistics(true);
    while (1)
    {
        if (counter.waitForPacket())
        {
        }
        counter.reportStatistics(false);
    }

#ifdef SOCKETS_WSA
    WSACleanup();
#endif

    return 0;
}
