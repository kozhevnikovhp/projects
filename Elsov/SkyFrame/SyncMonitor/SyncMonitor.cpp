// SyncMonitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../HardwareTest/Datum.h"
#include "../HardwareTest/Radyne.h"
#include <math.h>

int main(int argc, char* argv[])
{
	unsigned int nBusAddress = 0;
	CModem *pModem = NULL;
	int nPort = 1;

	int desired_delay = 0;

	char szFileName[128];

	for (int i = 1; i<argc; i++)
	{
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case 'A':
		case 'a': // buss Address
			sscanf(argv[i]+2, "%d", &nBusAddress);
			if (pModem)
				pModem->SetMCBusAddress(nBusAddress);
			else
			{
				printf("Please check parameters!\n");
				exit(0);
			}
			break;
		case 'D':
		case 'd': // Datum modem
			pModem = new CDatumPsm4900_LBand();
			sscanf(argv[i]+2, "%d", &nPort);
			pModem->Connect(nPort);
			break;
		case 'R':
		case 'r': // Radyne modem
			pModem = new CRadyneDMD2401();
			sscanf(argv[i]+2, "%d", &nPort);
			pModem->Connect(nPort);
			break;
		case 'T':
		case 't': // time delay
			sscanf(argv[i]+2, "%d", &desired_delay);
			break;
		default:
			printf("Error: - Unrecognized command-line option '%c'\n", *(pszArg+1));
			break;
		};
	}

	char szStr[1024];
	char *pszCarrierLock, *pszDemLock;
	char *pszOK = "OK";
	char *pszFailed = "FAILED";
	int bFirst = 1;
	CDemIfStatus status, old_status;
	SYSTEMTIME c_time, last_time;

	while (1)
	{
		GetLocalTime(&c_time);
		DWORD StartTime = GetTickCount();
		DWORD EndTime = StartTime+desired_delay;
		pModem->GetRIfStatus(status, 1);

		int bSomethingChanged = 0;
		if (bFirst)
			bSomethingChanged = 1;

		if (!bSomethingChanged)
			bSomethingChanged |= (old_status.m_bCarrier != status.m_bCarrier);
		if (!bSomethingChanged)
			bSomethingChanged |= (old_status.m_bDemLock != status.m_bDemLock);
		if (!bSomethingChanged)
			bSomethingChanged |= (fabs(old_status.m_SN - status.m_SN) >= 0.1);
		if (!bSomethingChanged)
			bSomethingChanged |= (old_status.m_Offset != status.m_Offset);
		if (!bSomethingChanged)
			bSomethingChanged |= (fabs(old_status.m_InputLevel - status.m_InputLevel) >= 0.1);

		if (!bSomethingChanged)
			bSomethingChanged |= (last_time.wDay != c_time.wDay);
		if (!bSomethingChanged)
			bSomethingChanged |= (last_time.wHour != c_time.wHour);
		if (!bSomethingChanged)
			bSomethingChanged |= (last_time.wMinute != c_time.wMinute);
		if (!bSomethingChanged)
			bSomethingChanged |= (last_time.wSecond != c_time.wSecond);
		if (!bSomethingChanged)
			bSomethingChanged |= (last_time.wMonth != c_time.wMonth);
		if (!bSomethingChanged)
			bSomethingChanged |= (last_time.wYear != c_time.wYear);

		if (!bSomethingChanged)
			continue;

		bFirst = 0;
		
		if (status.m_bCarrier)
			pszCarrierLock = pszOK;
		else
			pszCarrierLock = pszFailed;
		
		if (status.m_bDemLock)
			pszDemLock = pszOK;
		else
			pszDemLock = pszFailed;

		sprintf(szStr, "%02d.%02d.%d %02d:%02d:%02d:%03d\t%5.1f\t%d\t%5.1f\t%s\t%s\n", c_time.wDay, c_time.wMonth, c_time.wYear,
			c_time.wHour, c_time.wMinute, c_time.wSecond, c_time.wMilliseconds,
			status.m_SN, status.m_Offset, status.m_InputLevel, pszCarrierLock, pszDemLock);
		printf(szStr);
		sprintf(szFileName, "%02d.%02d.%d.txt", c_time.wDay, c_time.wMonth, c_time.wYear);
		FILE *fd = fopen(szFileName, "a+");
		if (fd)
		{
			fprintf(fd, "%s", szStr);
			fclose(fd);
		}
		
		memcpy(&old_status, &status, sizeof(status));
		memcpy(&last_time, &c_time, sizeof(c_time));

		while (GetTickCount() < EndTime) Sleep(0);
	}

	return 0;
}

