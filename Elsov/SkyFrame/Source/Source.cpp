// Source.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock.h"
#include "mmsystem.h"

static char buffer[0xFFFF];

int main(int argc, char* argv[])
{
	// Socket data
	WSADATA WSAdata;
	struct sockaddr_in sock_to_param;
	SOCKET sock;

	int packet_size, data_rate;

	if (argc!=5)
	{
		printf("Command line format: source address datarate packet_size\n");
		return 0;
	}

	// Initialize socket structure
	ZeroMemory(&sock_to_param, sizeof(sock_to_param));
	sock_to_param.sin_family=AF_INET;

	// ip address
	int ch1, ch2, ch3, ch4;
	if (sscanf(argv[1], "%d.%d.%d.%d", &ch1, &ch2, &ch3, &ch4)!=4)
	{
		printf("Error in arg[1]\n");
		return 0;
	}
	else
	{
		sock_to_param.sin_addr.S_un.S_un_b.s_b1=ch1;
		sock_to_param.sin_addr.S_un.S_un_b.s_b2=ch2;
		sock_to_param.sin_addr.S_un.S_un_b.s_b3=ch3;
		sock_to_param.sin_addr.S_un.S_un_b.s_b4=ch4;
	}

	// Port No
	int PortNo;
	if (sscanf(argv[2], "%d", &PortNo)!=1)
	{
		printf("Error in arg[1]\n");
		return 0;
	}
	sock_to_param.sin_port=htons(PortNo);

	//Data rate
	if (sscanf(argv[3], "%d", &data_rate)!=1)
	{
		printf("Error in arg[3]\n");
		return 0;
	}

	// Packet size
	if (sscanf(argv[4], "%d", &packet_size)!=1)
	{
		printf("Error in arg[2]\n");
		return 0;
	}
	DWORD time_delay=(int)(8.*1000*packet_size/data_rate);

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

	printf("Sending started....\n");
	int packets=0;
	BOOL bPause=FALSE;
	DWORD time=timeGetTime();
	DWORD DeltaTime;
	while (1)
	{
		if (_kbhit())
		{
			char ch=_getch();
			if (ch=='p' || ch=='P')
			{
				bPause=!bPause;
				time=timeGetTime();
			}
			if (ch=='s' || ch=='S')
			{
				packets=0;
				time=timeGetTime();
			}
			if (ch==27) break;
		}
		if (bPause) continue;
		// fill
		for (int i=0; i<packet_size; i++)
			buffer[i]=i;
		sprintf(buffer, "Packet number %06d", packets);
		// CRC
		char crc=0;
		for (i=0; i<(packet_size-1); i++)
			crc+=buffer[i];
		buffer[packet_size-1]=crc;
		// time delay
		while ((DeltaTime=timeGetTime()-time)<time_delay);
		time=timeGetTime();
		// send
		if (sendto(sock, buffer, packet_size, 0, (LPSOCKADDR)&sock_to_param, sizeof(sock_to_param))==packet_size)
		{
			packets++;
			printf("%s Speed %d baud\r", buffer, 8*1000*packet_size/DeltaTime);
		}
	}

	printf("\n\nThe end\n\n");
	// Close and cleanup
	closesocket(sock);
	WSACleanup();

	return 0;
}

