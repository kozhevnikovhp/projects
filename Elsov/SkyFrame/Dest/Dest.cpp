// Dest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock.h"

static char buffer[0xFFFF];

int main(int argc, char* argv[])
{
	// Socket data
	WSADATA WSAdata;
	struct sockaddr_in sock_param, sock_from_param;
	SOCKET sock;

	if (argc!=2)
	{
		printf("Command line format: dest PortNo\n");
		return 0;
	}

	// Initialize socket structure
	ZeroMemory(&sock_param, sizeof(sock_param));
	sock_param.sin_family=AF_INET;
	sock_param.sin_addr.s_addr=htonl(INADDR_ANY);
	// Port No
	int PortNo;
	if (sscanf(argv[1], "%d", &PortNo)!=1)
	{
		printf("Error in arg[1]\n");
		return 0;
	}
	sock_param.sin_port=htons(PortNo);

	// Winsock initialization
	int nRC=WSAStartup(0x101, &WSAdata);
	if (nRC)
	{
		printf("WinSock initialization failed\n");
		return 0;
	}

	// Socket creation
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (nRC)
	{
		printf("Socket not created\n");
		WSACleanup();
		return 0;
	}

	// Binding
	if (bind(sock, (LPSOCKADDR)&sock_param, sizeof(sock_param))==SOCKET_ERROR)
	{
		printf("Socket not bound\n");
		WSACleanup();
		return 0;
	}

	printf("Reading started....\n");
	int fromlen=sizeof(sock_from_param);
	int packets=0, BadCRC=0;
	BOOL bPause=FALSE;
	int bytes;
	while (1)
	{
		if (_kbhit())
		{
			char ch=_getch();
			if (ch=='p' || ch=='P')
				bPause=!bPause;
			if (ch=='s' || ch=='S')
			{
				packets=0;
				BadCRC=0;
			}
			if (ch==27) break;
		}
		if (bPause) continue;
		ZeroMemory(buffer, sizeof(buffer));
		bytes=recvfrom(sock, buffer, sizeof(buffer), 0, (LPSOCKADDR)&sock_from_param, &fromlen);
		// CRC
		char crc=0;
		for (int i=0; i<(bytes-1); i++)
			crc+=buffer[i];
		if (crc!=buffer[bytes-1])
			BadCRC++;;
		printf("Packet %06d (%d bytes) from %d.%d.%d.%d: %s BadCRC=%03d\r", packets, bytes,
					sock_from_param.sin_addr.S_un.S_un_b.s_b1,
					sock_from_param.sin_addr.S_un.S_un_b.s_b2,
					sock_from_param.sin_addr.S_un.S_un_b.s_b3,
					sock_from_param.sin_addr.S_un.S_un_b.s_b4, buffer, BadCRC);
		packets++;
	}


	printf("\n\nThe end\n\n");
	// Close and cleanup
	closesocket(sock);
	WSACleanup();

	return 0;
}

