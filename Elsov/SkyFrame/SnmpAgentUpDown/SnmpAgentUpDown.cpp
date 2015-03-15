// SnmpAgentUpDown.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "Snmp_stuff.h"

int main(int argc, char* argv[])
{
	unsigned char szBuffer[0xFFFF];
	unsigned int BytesRead = 0;
	IPADDRESS_TYPE IpAddress;
	unsigned short wPort = 161;
	CSnmpSocket sock;
	sock.Bind(wPort);
	int nRecv = 0;
	char szAddr[64];
	SSnmpDgm dgm;
	while (1)
	{
		sock.ReadFrom(szBuffer, sizeof(szBuffer), BytesRead, IpAddress, wPort);
		nRecv++;
		sprintf(szAddr, "%d.%d.%d.%d", LOBYTE(LOWORD(IpAddress)), HIBYTE(LOWORD(IpAddress)), LOBYTE(HIWORD(IpAddress)), HIBYTE(HIWORD(IpAddress)));
		printf(" %d received from %s:%d (length = %d)\n", nRecv, szAddr, wPort, BytesRead);
		DecodeSnmpRequest(szBuffer, &dgm);
	}

	return 0;
}

