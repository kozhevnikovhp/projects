// ChannelTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <fstream.h>
#include <math.h>
#include "IcmpSockets.h"

const unsigned long MAX_LOST_IN_ROW = 5;

typedef struct
{
	char pszCityName[128];
	char pszIpAddress[128];
	char pszFileName[128];
	DWORD dwIpAddress;
	unsigned long nSentPackets;
	unsigned long nReceivedPackets;
	double nReceivedBits;
	unsigned long nLostPackets;
	unsigned long nLostInRow;
	unsigned long nBad;
	double AccumulatedDelay;
	double CalcBER()
	{
		if (nReceivedBits < 1)
			return 0;
		return nLostPackets/nReceivedBits;
	}
	double CalcAveDelay()
	{
		if (nReceivedPackets)
			return AccumulatedDelay/nReceivedPackets;
		else
			return -1;
	}
} SCityStat;

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		printf("Usage: ChannelTest.exe <file_name> [options]\n");
		printf("\twhere <file_name> is the path to file of special format,");
		printf("\t[options] are standard ping options");
		return 1;
	}
	printf("Author: Evgeny Kozhevnikov\n\n");

	SCityStat Cities[256];
	memset(Cities, 0, sizeof(Cities));
	char szFileName[128];

	SYSTEMTIME time;
	GetLocalTime(&time);
	const char *pszLogFileNameTemplate = "LogFile_%Y_%B.txt";
	CTime CurrentTime = CTime::GetCurrentTime();
	CString LogFileName = CurrentTime.Format(pszLogFileNameTemplate);
	FILE *fd_log = fopen(LogFileName, "a+");
	printf("%02d:%02d:%02d\tProgram started as ",
				time.wHour,	time.wMinute, time.wSecond); 
	if (fd_log)
	{
		fprintf(fd_log, "\n%02d.%02d.%d\t%02d:%02d:%02d\tProgram started as ",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond);
	}
	
	for (int i = 1; i<argc; i++)
	{
		printf("%s ", argv[i]);
		if (fd_log)
			fprintf(fd_log, "%s ", argv[i]);
	}
	printf("\n");
	if (fd_log)
		fprintf(fd_log, "\n");
	fclose(fd_log);
	if (argc < 2)
	{
		printf("usage: ChannelTest.exe <file_name>\n");
		return 0;
	}

	// Open file with commands
	fstream stream(argv[1], ios::in);
	char pszStr[2048];
	if (!stream.is_open())
	{
		printf("Cannot open file %s\n", argv[1]);
		return 0;
	}
	
	SCityStat *pCity = Cities;
	int nCities = 0;
	FILE *fd = fopen("saved.dat", "r");
	if (fd)
	{
		fread(&nCities, sizeof(nCities), 1, fd);
		fread(&Cities, sizeof(Cities[0]), nCities, fd);
		fclose(fd);
	}
	nCities = 0;
	while (!stream.eof())
	{
		stream.getline(pszStr, sizeof(pszStr));
		if (strlen(pszStr) == 0)
			continue; // ignore empty strings
		if (*pszStr == '!' || 
			*pszStr == '@' ||
			*pszStr == '#' ||
			*pszStr == '$' ||
			*pszStr == '%' ||
			*pszStr == '^' ||
			*pszStr == '&' ||
			*pszStr == '*')
		{
			printf ("Line %s ignored due to symbol '%c' at first position\n", pszStr, *pszStr);
			continue;
		}

		if (sscanf(pszStr, "%s %s", pCity->pszCityName, pCity->pszIpAddress) != 2)
		{
			printf("Cannot parse input string '%s'\n", pszStr);
			continue;
		}
		pCity->dwIpAddress = inet_addr(pCity->pszIpAddress);
		sprintf(pCity->pszFileName, "%s.txt", pCity->pszCityName);
		GetLocalTime(&time);

		fd = fopen(pCity->pszFileName, "a+");
		if (fd)
		{
			fprintf(fd, "\n%02d.%02d.%02d  %02d:%02d:%02d - program started\n",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond); 
			fclose(fd);
		}
		
		nCities++;
		pCity++;		
	}
	stream.close();
	if (nCities == 0)
	{
		printf("No cities to be PINGed from file %s - please check the file\n", argv[1]);
		return 0;
	}


	int length = ICMP_DEFAULT_PACKET_SIZE;
	DWORD dwTimeout = 1000;
	DWORD dwSleepCycle = 1;
	DWORD dwPeriod = 0;
	DWORD dwStillWorkingReportEvery = 10000;
	unsigned char cTTL = 0;
	char sincos = 's';
	for (i = 2; i<argc; i++)
	{
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case 'l':
			sscanf(argv[i+1], "%d", &length);
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
	unsigned long rcvd = 0;
	unsigned long rcvd_bytes = 0;

	CIcmpSocket sock;

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

	for (int nLoop = 0; ; nLoop++) // infinite loop
	{
		if (nLoop % 100 == 0)
		{
			fd = fopen("saved.dat", "w");
			if (fd)
			{
				fwrite(&nCities, sizeof(nCities), 1, fd);
				fwrite(&Cities, sizeof(Cities[0]), nCities, fd);
				fclose(fd);
			}
		}

		CTime FirstPingTime = CTime::GetCurrentTime();
		CTime NextPingTime = FirstPingTime + CTimeSpan(0,0,0,dwSleepCycle);
		for (int nCurrentCity = 0; nCurrentCity < nCities; nCurrentCity++)
		{
			pCity = Cities + nCurrentCity;
			GetLocalTime(&time);
			if (nLoop % 500 == 0)
			{
				sprintf(szFileName, "%s_history.txt", pCity->pszCityName);
				fd = fopen(szFileName, "a+");
				if (fd)
				{
					fprintf(fd, "%02d.%02d.%02d %02d:%02d:%02d\t%ld\t%ld\t%ld\t%9.2e\n",
						time.wDay, time.wMonth, time.wYear,
						time.wHour,	time.wMinute, time.wSecond, pCity->nSentPackets, pCity->nReceivedPackets, pCity->nLostPackets, pCity->CalcBER());
					fclose(fd);
				}
			}
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

			if (!sock.SendEchoRequest(pCity->dwIpAddress, LengthToSend))
			{
				switch (sock.GetLastErrorCode())
				{
				case WSAETIMEDOUT:
					Sleep(100);
					continue;
				case WSAEHOSTUNREACH:
					printf("Host %s (%s) is unreachable\n", pCity->pszCityName, pCity->pszIpAddress);
					Sleep(1000);
					continue;
				default:
					printf("Error in echo request sending %s (%s)\n", pCity->pszCityName, pCity->pszIpAddress);
					Sleep(1000);
					continue;
				}
			}
			pCity->nSentPackets++;
			
			if (!sock.ReceiveEchoReply())
			{
				printf("Error in echo reply reading!!!\n");
				continue;
			}

			if (sock.IsTimedOut())
			{
				pCity->nLostPackets++;
				GetLocalTime(&time);
				fd = fopen(pCity->pszFileName, "a+");
				pCity->nLostInRow++;
				if (pCity->nLostInRow <= MAX_LOST_IN_ROW)
				{
					if (fd)
					{
						fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\treceived %d replies of %d,\t%d lost\n",
							time.wDay, time.wMonth, time.wYear,
							time.wHour,	time.wMinute, time.wSecond, pCity->nReceivedPackets, pCity->nSentPackets, pCity->nLostPackets);
					}
				}
				if (pCity->nLostInRow == MAX_LOST_IN_ROW)
				{
					printf("%s: %d packets were lost in row - modem is switched OFF or unreachable\n", pCity->pszCityName, MAX_LOST_IN_ROW);
					if (fd)
					{
						fprintf(fd, "%d packets were lost in row - modem is switched OFF or unreachable\n", MAX_LOST_IN_ROW);
					}
					pCity->nSentPackets -= pCity->nLostInRow;
					pCity->nLostPackets -= pCity->nLostInRow;
				}
				if (fd)
					fclose(fd);
				if (pCity->nLostInRow > MAX_LOST_IN_ROW)
				{
					pCity->nSentPackets--;
					pCity->nLostPackets--;
				}
				rcvd_bytes = 0;
			}
			else if (sock.IsBad())
				pCity->nBad++;
			else if (sock.IsDestUnreachable())
			{
				pCity->nSentPackets--;
				printf("Host %s (%s) is unreachable (reply from %s)\n", pCity->pszCityName, pCity->pszIpAddress, sock.GetReplierIpAddress());
			}
			else
			{
				pCity->nReceivedPackets++;
				if (pCity->nLostInRow >= MAX_LOST_IN_ROW)
				{
					GetLocalTime(&time);
					fd = fopen(pCity->pszFileName, "a+");
					printf("%s: Reply received - modem is turned ON back\n", pCity->pszCityName); 
					if (fd)
					{
						fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\tReply received - modem is turned ON back\n", 
							time.wDay, time.wMonth, time.wYear,
							time.wHour,	time.wMinute, time.wSecond);
						fclose(fd);
					}
				}
				pCity->nLostInRow = 0;
				rcvd_bytes = sock.GetReceivedDataLength();
				pCity->nReceivedBits += rcvd_bytes * 8;
			}

			if (pCity->nLostInRow <= MAX_LOST_IN_ROW)
				printf("%s: (%d bytes)\n   %7d sent, %7d rcvd, %3d lost, delay = %4d msec, BER = %7.2e\n", pCity->pszCityName, rcvd_bytes, pCity->nSentPackets, pCity->nReceivedPackets, pCity->nLostPackets, sock.GetDelayTime(), pCity->CalcBER());
			else
				printf("%s is switched OFF or unreachable (%d sent, %d rcvd, %d lost), BER = %8.2e\n", pCity->pszCityName, pCity->nSentPackets, pCity->nReceivedPackets, pCity->nLostPackets, pCity->CalcBER());

			if ((pCity->nSentPackets > 0) && (pCity->nSentPackets % dwStillWorkingReportEvery == 0))
			{
				GetLocalTime(&time);
				fd = fopen(pCity->pszFileName, "a+");
				if (fd)
				{
					fprintf(fd, "%02d.%02d.%02d\t%02d:%02d:%02d\tstill working\t %d packets sent,\t %d packets received, \t%d packets lost\n", 
						time.wDay, time.wMonth, time.wYear,
						time.wHour,	time.wMinute, time.wSecond, pCity->nSentPackets, pCity->nReceivedPackets, pCity->nLostPackets);
					fclose(fd);
				}
			}
			
		}
		while (CTime::GetCurrentTime() < NextPingTime)
		{
			if (dwSleepCycle > 1)
			{
				static char szGear[4] = {'-','\\','|','/'};
				static int iGear = 0;
				CTime CurrentTime = CTime::GetCurrentTime();
				CTimeSpan RemainingTime = NextPingTime-CurrentTime;
				printf(" %c%c%c %d seconds till next cycle\t\t\r", szGear[iGear], szGear[iGear], szGear[iGear], RemainingTime.GetTotalSeconds());
				iGear = (++iGear) % sizeof(szGear);
				Sleep(200);
			}
			else
				Sleep(333);	
		}
		printf("\t\t\t\t\t\t\n\n");
	}

	return 0;
}
