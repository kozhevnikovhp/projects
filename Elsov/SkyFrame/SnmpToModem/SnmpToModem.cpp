// SnmpToModem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "SnifferSockets.h"

class cSpyThread : public CSnifferThread
{
public:
	cSpyThread()
	{
	}

	virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
	{
		unsigned short SrcPortNo = ntohs(pUdpHeader->SrcPortNo);
		unsigned short DstPortNo = ntohs(pUdpHeader->DstPortNo);
		if (DstPortNo == 161)
		{
			printf("SNMP REQUEST:\t%5d/%5d len = %5d (from %s to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
				AddressToString(szFrom, pIpHeader->sourceIP),
				AddressToString(szTo, pIpHeader->destIP));
		}
		else if (SrcPortNo == 161)
		{
			printf("  SNMP REPLY:\t%5d/%5d len = %5d (from %s to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
				AddressToString(szFrom, pIpHeader->sourceIP),
				AddressToString(szTo, pIpHeader->destIP));
		}
	}

protected:
	char szFrom[64], szTo[64];
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <map_file>\n", argv[0]);
		return -1;
	}

	FILE *pFile = fopen(argv[1], "r");
	if (pFile)
	{
		char szIpAddress[2048], szPortName[2048];
		while (2 == fscanf(pFile, "%s %s\n", szIpAddress, szPortName))
		{
			printf("%s mapped to serial port %s\n", szIpAddress, szPortName);
		}
	}
	else
	{
		printf("Cannot open mapping file %s\n", argv[1]);
		return -1;
	}

	InitSockets();

	char name[100];
	struct hostent FAR *he;
	if (gethostname(name,sizeof(name)))
	{
		printf("gethostname failed, quiting...");
		return 1;
	}
	else
	{
		name[99]=0;
		if (he=gethostbyname(name))
		{
			char **pc;
			printf("Host name = %s\n", he->h_name);
			printf(" Addresses:\n");
			for (pc=he->h_addr_list; *pc; pc++)
			{
				struct in_addr *pInAddr = (struct in_addr *)*pc;
				printf("%s\n", inet_ntoa(*pInAddr));
				cSpyThread *pThread = new cSpyThread;
				pThread->Bind(pInAddr->S_un.S_addr);
				pThread->EnablePromiscMode();
				pThread->Run();
			}
		}
	}

	getchar();
	return 0;
}

