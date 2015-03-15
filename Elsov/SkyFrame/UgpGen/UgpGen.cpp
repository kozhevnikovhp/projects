// UgpGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "Udp.h"

int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		printf("Not enough arguments\n");
		return 0;
	}

	int i1, i2, i3, i4;
	BYTE b1=0, b2=0, b3=0, b4=0;
	unsigned short wPort = 8888;

	if (sscanf(argv[1], "%d.%d.%d.%d", &i1, &i2, &i3, &i4) != 4)
	{
		printf("Invalid address\n");
		return 0;
	}

	b1=LOBYTE(LOWORD(i1));
	b2=LOBYTE(LOWORD(i2));
	b3=LOBYTE(LOWORD(i3));
	b4=LOBYTE(LOWORD(i4));

	int length = 100;
	DWORD dwTimeout = 1000;
	DWORD dwRate = dwTimeout;
	DWORD dwWaitEvery = 0;
	unsigned char cTTL = 0;
	char pszAddr[128];
	char pszBuffer[10000];

	sprintf(pszAddr, "%d.%d.%d.%d", b1, b2, b3, b4);

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
			sscanf(pszArg+2, "%d", &wPort);
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

	CUdpSocket sock;

	if (cTTL > 0)
	{
		if (!sock.SetSendTTL(cTTL))
		{
			printf("ERROR: Cannot set TTL.\n");
			return sock.GetLastErrorCode();
		}
	}

	unsigned int BytesSent = 0;

	while (1)
	{
		Sleep(0);
		DWORD last_ping_time = GetTickCount();
		if (sock.WriteTo(pszBuffer, length, BytesSent, MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4)), wPort))
		{
			sent++;
			printf(" %d sent to %s:%d (length = %d)\n", sent, pszAddr, wPort, length);
		}
		else
		{
			if (sock.GetLastErrorCode() == WSAETIMEDOUT)
			{
				Sleep(100);
				continue;
			}
		}
		
		DWORD time_consumed = GetTickCount() - last_ping_time;
		if (time_consumed < dwRate)
			Sleep(dwRate-time_consumed);
		if (dwWaitEvery)
		{
			if (!(sent % dwWaitEvery))
			{
				printf("\n");
				Sleep(1000);
			}
		}
	}

	return 0;
}
