// CuvsListener.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "Udp.h"


int main(int argc, char* argv[])
{
	unsigned short wPort = 8888;
	char pszAddr[128];
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
		};
	}
	printf("Listening port %d...\n", wPort);
	int recv = 0;

	CUdpSocket sock;
	sock.Bind(wPort);
	unsigned long IpAddress = 0;
	unsigned int BytesRead = 0;

	while (1)
	{
		sock.ReadFrom(pszBuffer, sizeof(pszBuffer), BytesRead, IpAddress, wPort);
		recv++;
		sprintf(pszAddr, "%d.%d.%d.%d", LOBYTE(LOWORD(IpAddress)), HIBYTE(LOWORD(IpAddress)), LOBYTE(HIWORD(IpAddress)), HIBYTE(HIWORD(IpAddress)));
		//printf(" %d received from %s:%d (length = %d)\n", recv, pszAddr, wPort, BytesRead);
		printf("%s\n", pszBuffer);
		FILE *fd = fopen("log_file.txt", "a+");
		if (fd)
		{
			fprintf(fd, "%s\n", pszBuffer);
			fclose(fd);
		}
	}

	return 0;
}
