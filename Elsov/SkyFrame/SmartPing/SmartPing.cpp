// SmartPing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "IcmpSockets.h"


int main(int argc, char* argv[])
{
	printf("This is SmartPing. Possible options are -l -n -w -i plus several specific ones (-x, for example).\n");
	printf("Please use these options exactly as if you use system ping utility.\n\n");
	printf("For more info please refer to system ping manual(help).\n");
	printf("Example: SmartPing.exe 192.168.31.76 -n 20 -l 1000 -w 3000 -i 64 -x 30\n\n");
	printf("Author: Evgeny Kozhevnikov\n\n");

	CIcmpSocket sock;

	char szTimeoutFileName[64];
	if (argc < 2)
	{
		printf("Not enough arguments\n");
		return 0;
	}

	unsigned int i1, i2, i3, i4;
	BYTE b1=0, b2=0, b3=0, b4=0;
	if (sscanf(argv[1], "%d.%d.%d.%d", &i1, &i2, &i3, &i4) == 4)
	{
		b1=LOBYTE(LOWORD(i1));
		b2=LOBYTE(LOWORD(i2));
		b3=LOBYTE(LOWORD(i3));
		b4=LOBYTE(LOWORD(i4));
	}
	else
	{
		struct  hostent *pHost = gethostbyname(argv[1]);
		if (!pHost)
		{
			printf("Cannot resolve name %s to IP-address\n", argv[1]);
			return 0;
		}
		char *pIpAddress = pHost->h_addr;
		b1 = *(pIpAddress+0);
		b2 = *(pIpAddress+1);
		b3 = *(pIpAddress+2);
		b4 = *(pIpAddress+3);
	}

	strcpy(szTimeoutFileName, argv[1]);
	strcat(szTimeoutFileName, "_timeouts.txt");

	int length = ICMP_DEFAULT_PACKET_SIZE;
	DWORD dwTimeout = 1000;
	DWORD dwSleepCycle = 1;
	DWORD dwPeriod = 0;
	DWORD dwStillWorkingReportEvery = 10000;
	DWORD packets_to_send = -1;
	unsigned char cTTL = 0;
	char sincos = 's';

	for (int i = 2; i<argc; i++)
	{
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case 'l':
			sscanf(argv[i+1], "%d", &length);
			break;
		case 'n':
			sscanf(argv[i+1], "%d", &packets_to_send);
			break;
		case 'w':
			sscanf(argv[i+1], "%d", &dwTimeout);
			break;
		case 'x':
			sscanf(argv[i+1], "%d", &dwSleepCycle);
			break;
		case 'p':
			sscanf(argv[i+1], "%d,%c", &dwPeriod, &sincos);
			break;
		case 'e':
			sscanf(argv[i+1], "%d", &dwStillWorkingReportEvery);
			break;
		case 'i':
			sscanf(argv[i+1], "%hu", &cTTL);
			break;
		default:
			printf("WARNING: Unknown option '%s' - ignored.\n", pszArg);
			break;
		};
	}
	DWORD sent = 0;
	DWORD rcvd = 0;
	DWORD rcvd_bytes = 0;
	DWORD timed_out = 0;
	DWORD bad = 0;

	SYSTEMTIME time;
	GetLocalTime(&time);
	FILE *fd=fopen(szTimeoutFileName, "a+");
	fprintf(fd, "\n%02d.%02d.%02d  %02d:%02d:%02d - program started\n",
		time.wDay, time.wMonth, time.wYear,
		time.wHour,	time.wMinute, time.wSecond); 
	fclose(fd);

	if (!sock.SetPingTimeout(dwTimeout))
	{
		printf("ERROR: Cannot set timeout.\n");
		return sock.GetLastErrorCode();
	}
	if (cTTL > 0)
	{
		if (!sock.SetSendTTL(cTTL))
		{
			printf("ERROR: Cannot set TTL.\n");
			return sock.GetLastErrorCode();
		}
	}

	while (1)
	{
		if (packets_to_send >0 && sent >= packets_to_send)
			break;
		GetLocalTime(&time);
		int LengthToSend;
		if (dwPeriod == 0)
			LengthToSend = length;
		else
		{
			int seconds = time.wHour*3600+time.wMinute*60+time.wSecond;
			seconds = seconds % dwPeriod;
			if (sincos == 's' || sincos == 'S')
				LengthToSend = (int)(length/2*(1.+sin(seconds*6.28/dwPeriod)));
			else
				LengthToSend = (int)(length/2*(1.-sin(seconds*6.28/dwPeriod)));
		}
		if (LengthToSend == 0)
			LengthToSend = 1;

		CTime FirstPingTime = CTime::GetCurrentTime();
		CTime NextPingTime = FirstPingTime + CTimeSpan(0,0,0,dwSleepCycle);

		if (!sock.SendEchoRequest(MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4)), LengthToSend))
		{
			switch (sock.GetLastErrorCode())
			{
			case WSAETIMEDOUT:
				Sleep(100);
				continue;
			case WSAEHOSTUNREACH:
				printf("Host (%s) is unreachable\n", argv[1]);
				Sleep(1000);
				continue;
			default:
				printf("Error %d in echo request sending (%s)\n", sock.GetLastErrorCode(), argv[1]);
				Sleep(1000);
				continue;
			}
		}
		DWORD last_ping_time = GetTickCount();
		sent++;
		
		if (!sock.ReceiveEchoReply())
		{
			printf("Error in echo reply reading!!!\n");
			continue;
		}

		if (sock.IsTimedOut())
		{
			timed_out++;
			GetLocalTime(&time);
			FILE *fd=fopen(szTimeoutFileName, "a+");
			fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\treceived %d replies of %d,\t%d requests timed out\n",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond, rcvd, sent, timed_out); 
			fclose(fd);
			rcvd_bytes = 0;
		}
		else if (sock.IsBad())
			bad++;
		else if (sock.IsDestUnreachable())
		{
			sent--;
			printf("Host %s is unreachable (reply from %s)\n", argv[1], sock.GetReplierIpAddress());
		}
		else
		{
			rcvd++;
			rcvd_bytes = sock.GetReceivedDataLength();
		}

		printf("%d bytes from %s (%d of %d) %d timed out delay=%d msec, TTL=%d\n", rcvd_bytes, argv[1], rcvd, sent, timed_out, sock.GetDelayTime(), sock.GetReceivedTTL());

		if (sent % dwStillWorkingReportEvery == 0)
		{
			GetLocalTime(&time);
			FILE *fd=fopen(szTimeoutFileName, "a+");
			fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\tstill working\t %d packets sent,\t %d packets received, \t%d packets lost\n", 
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond, sent, rcvd, timed_out);
			fclose(fd);
		}
		
		while (CTime::GetCurrentTime() < NextPingTime)
		{
			if (dwSleepCycle > 1)
			{
				static char szGear[4] = {'-','\\','|','/'};
				static int iGear = 0;
				CTime CurrentTime = CTime::GetCurrentTime();
				CTimeSpan RemainingTime = NextPingTime-CurrentTime;
				printf(" %c%c%c %d seconds till next ping\t\t\r", szGear[iGear], szGear[iGear], szGear[iGear], RemainingTime.GetTotalSeconds());
				iGear = (++iGear) % sizeof(szGear);
			}
			Sleep(250);	
		}
		printf("\t\t\t\t\t\t\r");
	}

	return 0;
}
