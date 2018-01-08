#include <stdio.h>
#include "Sockets.h"

int InitSockets()
{
#define VERSION_MINOR 1
#define VERSION_MAJOR 1

	printf("Winsock initialization ");
	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD(VERSION_MINOR, VERSION_MAJOR);
 
	int err = WSAStartup(wVersionRequested, &WsaData);
	if (err != 0)
	{
		printf("failed!\n");
		return 0;
	}
 
	if (LOBYTE(WsaData.wVersion) != VERSION_MAJOR ||
        HIBYTE(WsaData.wVersion) != VERSION_MINOR )
	{ // We could not find a usable WinSock DLL
		WSACleanup();
		return 0;
	}
	printf("OK\n");
	return 1; // success

} //  InitSockets()


int FinitSockets()
{
	WSACleanup();
	return 1; // success
} //  FinitSockets()


SOCKET CreateUdpSocket
(
	unsigned short PortNo,		// TCP/IP port number
	struct sockaddr_in *pSocketParams,
	BOOL bNeedBind
)
{
	SOCKET sock;
	pSocketParams->sin_family=AF_INET;
	pSocketParams->sin_port=htons(PortNo);
	pSocketParams->sin_addr.s_addr=htonl(INADDR_ANY);

	// Socket creation
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (sock==INVALID_SOCKET)
	{
		printf("Socket not created\n");
//		TRACE("Socket not created\n");
		return sock;
	}

	// Binding
	if (bNeedBind)
	{
		int optVal=1;
		if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR,
					   (char *) &optVal, sizeof(optVal)) == SOCKET_ERROR)
		{
			printf("setsockopt failed\n");
//			TRACE("setsockopt failed\n");
		}

		if (bind(sock, (LPSOCKADDR)pSocketParams, sizeof(sockaddr_in))==SOCKET_ERROR)
		{
			int a=WSAGetLastError();
			printf("Socket not bound\n");
			return sock;
		}
	}
	
	return sock;
}

void CloseSocket
(
	SOCKET sock
)
{
	closesocket(sock);
}

// misc
