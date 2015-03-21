// UdpListener.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "network/udp.h"
#include "network/misc.h"

using namespace common;
using namespace network;

int main(int argc, char* argv[])
{
	IPPORT wPort = 8888;
	char pszBuffer[100000];

	for (int i = 1; i<argc; i++)
	{
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case 'p':
			sscanf(pszArg+2, "%d", &wPort);
			break;
		default:
			printf("WARNING: Unknown option '%s' - ignored.\n", pszArg);
			break;
		};
	}
	printf("Listening port %d...\n", wPort);
	int recv = 0;

	cUdpSocket sock;
	sock.bind(wPort);
	IPADDRESS_TYPE IpAddress = 0;
	int BytesRead = 0;

	while (1)
	{
		sock.ReadFrom(pszBuffer, sizeof(pszBuffer), BytesRead, IpAddress, wPort);
		recv++;
		printf(" %d received from %s:%d (length = %d)\n", recv, addressToDotNotation(IpAddress).c_str(), wPort, BytesRead);
		
	}

	return 0;
}
