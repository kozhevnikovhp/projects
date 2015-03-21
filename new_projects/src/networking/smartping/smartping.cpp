// SmartPing.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "network/icmp.h"
#include "network/misc.h"
#include "portable/portable.h"

using namespace common;
using namespace network;
using namespace portable;

int main(int argc, char* argv[])
{
	printf("This is SmartPing. Possible options are -l -n -w -i plus several specific ones (-x, for example).\n");
	printf("Please use these options exactly as if you use system ping utility.\n\n");
	printf("For more info please refer to system ping manual(help).\n");
	printf("Example: SmartPing.exe 192.168.31.76 -n 20 -l 1000 -w 3000 -i 64 -x 30\n\n");
	printf("Author: Evgeny Kozhevnikov\n\n");

    cIcmpSocket sock;

	char szTimeoutFileName[64];
	if (argc < 2)
	{
		printf("Not enough arguments\n");
		return 0;
	}

    IPADDRESS_TYPE ip = StringToAddress(argv[1]);
    if (!ip)
    {
        printf("Cannot resolve name %s to IP-address\n", argv[1]);
		return 0;
    }

	strcpy(szTimeoutFileName, argv[1]);
	strcat(szTimeoutFileName, "_timeouts.txt");

	int length = ICMP_DEFAULT_PACKET_SIZE;
	int dwTimeout = 1000;
	int dwSleepCycle = 1;
	int dwPeriod = 0;
	int dwStillWorkingReportEvery = 10000;
	int packets_to_send = -1;
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
            unsigned short usTTL;
			sscanf(argv[i+1], "%hu", &usTTL);
            cTTL = (unsigned char)usTTL;
			break;
		default:
			printf("WARNING: Unknown option '%s' - ignored.\n", pszArg);
			break;
		};
	}
	int sent = 0;
	int rcvd = 0;
	int rcvd_bytes = 0;
	int timed_out = 0;
	int bad = 0;

	FILE *fd = fopen(szTimeoutFileName, "a+");
	time_t currTime; time(&currTime);
    tm *pTM = localtime(&currTime);
	fprintf(fd, "\n%02d.%02d.%02d  %02d:%02d:%02d - program started\n",
		pTM->tm_mday, pTM->tm_mon, pTM->tm_year,
		pTM->tm_hour, pTM->tm_min, pTM->tm_sec); 
	fclose(fd);

	if (!sock.SetPingTimeout(dwTimeout))
	{
		printf("ERROR: Cannot set timeout.\n");
		//return sock.getLastErrorCode();
	}
	if (cTTL > 0)
	{
		if (!sock.SetSendTTL(cTTL))
		{
			printf("ERROR: Cannot set TTL.\n");
			//return sock.getLastErrorCode();
		}
    }

	while (1)
	{
        portableSleep(1000);
		if (packets_to_send >0 && sent >= packets_to_send)
			break;
		time(&currTime);
		int LengthToSend;
		if (dwPeriod == 0)
			LengthToSend = length;
		else
		{
			int seconds = currTime % dwPeriod;
			if (sincos == 's' || sincos == 'S')
				LengthToSend = (int)(length/2*(1.+sin(seconds*6.28/dwPeriod)));
			else
				LengthToSend = (int)(length/2*(1.-sin(seconds*6.28/dwPeriod)));
		}
		if (LengthToSend == 0)
			LengthToSend = 1;

		unsigned int NextPingTimeMsec = portableGetCurrentTimeMsec() + dwSleepCycle;

		if (!sock.SendEchoRequest(ip, LengthToSend))
		{
			switch (sock.getLastErrorCode())
			{
			case ETIMEDOUT:
				portableSleep(100);
				continue;
			case EHOSTUNREACH:
				printf("Host (%s) is unreachable\n", argv[1]);
				portableSleep(1000);
				continue;
			default:
				printf("Error %ud in echo request sending (%s)\n", sock.getLastErrorCode(), argv[1]);
				portableSleep(1000);
				continue;
			}
		}
		sent++;
		
		if (!sock.ReceiveEchoReply())
		{
			printf("Error in echo reply reading!!!\n");
			continue;
		}

		if (sock.IsTimedOut())
		{
			timed_out++;
            time(&currTime);
            pTM = localtime(&currTime);
			FILE *fd = fopen(szTimeoutFileName, "a+");
			fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\treceived %d replies of %d,\t%d requests timed out\n",
                    pTM->tm_mday, pTM->tm_mon, pTM->tm_year,
                    pTM->tm_hour, pTM->tm_min, pTM->tm_sec, 
                    rcvd, sent, timed_out); 
			fclose(fd);
			rcvd_bytes = 0;
		}
		else if (sock.IsBad())
			bad++;
		else if (sock.IsDestUnreachable())
		{
			sent--;
			printf("Host %s is unreachable (reply from %s)\n", argv[1], sock.getReplierIpAddressString().c_str());
		}
		else
		{
			rcvd++;
			rcvd_bytes = sock.GetReceivedDataLength();
		}

		printf("%d bytes from %s (%d of %d) %d timed out delay=%d msec, TTL=%d\n", rcvd_bytes, argv[1], rcvd, sent, timed_out, sock.GetDelayTime(), sock.GetReceivedTTL());

		if (sent % dwStillWorkingReportEvery == 0)
		{
            time(&currTime);
            pTM = localtime(&currTime);
			FILE *fd = fopen(szTimeoutFileName, "a+");
			fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\tstill working\t %d packets sent,\t %d packets received, \t%d packets lost\n", 
                    pTM->tm_mday, pTM->tm_mon, pTM->tm_year,
                    pTM->tm_hour, pTM->tm_min, pTM->tm_sec, 
                    sent, rcvd, timed_out);
			fclose(fd);
		}

        unsigned int currTimeMsec = portableGetCurrentTimeMsec();
		while (currTimeMsec < NextPingTimeMsec)
		{
			if (dwSleepCycle > 1)
			{
				static char szGear[4] = {'-','\\','|','/'};
				static int iGear = 0;
				printf(" %c%c%c %d seconds till next ping\t\t\r", szGear[iGear], szGear[iGear], szGear[iGear], (NextPingTimeMsec-currTimeMsec)/1000);
				iGear = (++iGear) % sizeof(szGear);
			}
			portableSleep(250);
            currTimeMsec = portableGetCurrentTimeMsec(); 
		}
		printf("\t\t\t\t\t\t\r");
	}

	return 0;
}
