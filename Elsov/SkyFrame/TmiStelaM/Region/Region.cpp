// Region.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream.h>
#include <conio.h>
#include "..\OneObject.h"
#include "..\..\HardwareTest\StelaM.h"
#include "Thread.h"
#include <winsock.h>

//static
unsigned ( __stdcall OneObjectThreadProcedure) 
(
	void *a_pParams
)
{
	TRACE("OneObjectThreadProcedure started\n");

	char **pArgs = (char **)a_pParams;

	int nArgs = 0;
	while (pArgs[nArgs] != NULL)
		nArgs++;
	OneObject(nArgs, pArgs, pArgs[0]);

	TRACE("OneObjectThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}

double CalcNewLevel
(
	double WorstSN,
	double BestSN,
	double MinDesiredSN,
	double MaxDesiredSN,
	double CurrentLevel,
	double CurrentPower,
	double MinLevel,
	double MaxLevel,
	double MaxPower
)
{
	printf("Calc new level: BestSN = %5.1f\tWorstSN = %5.1f\nMinDesiredSN = %5.1f\tMaxDesiredSN = %5.1f\nCurrentLevel = %5.1f\tCurrentPower = %5.1f\nMinLevel = %5.1f\tMaxLevel = %5.1f\n",
		BestSN, WorstSN, MinDesiredSN, MaxDesiredSN, CurrentLevel, CurrentPower, MinLevel, MaxLevel);
	double DesiredLevelPlus = (MinDesiredSN+MaxDesiredSN)/2. - WorstSN;
	printf("DesiredLevelPlus = %f\n", DesiredLevelPlus);
	if (DesiredLevelPlus > 3.)
		DesiredLevelPlus = 3.;
	if (DesiredLevelPlus < -1.)
		DesiredLevelPlus = -1;

	printf("DesiredLevelPlus = %f\n", DesiredLevelPlus);
	double DesiredLevel = CurrentLevel + DesiredLevelPlus;
	if (DesiredLevel < MinLevel)
		DesiredLevel = MinLevel;
	if (DesiredLevel > MaxLevel)
		DesiredLevel = MaxLevel;
	DesiredLevelPlus = DesiredLevel - CurrentLevel;
	printf("DesiredLevelPlus = %f\n", DesiredLevelPlus);

	return DesiredLevel;
}

//static
unsigned ( __stdcall AutoUzsThreadProcedure) 
(
	void *a_pParams
)
{
	const char *pszLogFileNameTemplate = "LogFile_%Y_%B.txt";
	int i, j;
	char c, pszIpAddress[32];
	TRACE("AutoUZSThreadProcedure started\n");
	double MinSN = 7.;
	double MaxSN = 10.;
	double MinLevel = -35;
	double MaxLevel = 5;
	double MaxPower = 30;
	DWORD desired_delay = 2*60*1000;
	SRegionSync *pSyncData;
	char cDeviceSymbol = ' ';
	char *pszOptions = NULL;

	char **pArgs = (char **)a_pParams;
//	ResetEvent(pThis->m_hThreadProcEnded);

	char szWorkingDir[1024];
	CString LogFilePath;
	char *pszCityName = pArgs[0];
	GetCurrentDirectory(sizeof(szWorkingDir), szWorkingDir);
	CString NewDirName;
	NewDirName.Format("%s\\%s", szWorkingDir, pszCityName);
	CreateDirectory(NewDirName, NULL);
	CTime CurrentTime = CTime::GetCurrentTime();
	LogFilePath = NewDirName + "\\" + CurrentTime.Format(pszLogFileNameTemplate);
	g_UzsLogFilePath = LogFilePath;

	int nArgs = 0;
	while (pArgs[nArgs] != NULL)
		nArgs++;
	char pszReportLine[1024], pszReportAdd[1024];
	pszReportLine[0] = 0;

	FILE *fd_log = fopen(LogFilePath, "a+");
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf(pszReportAdd, "\n%02d:%02d:%02d\tCity %s started as ",
				time.wHour,	time.wMinute, time.wSecond, pszCityName);
	strcat(pszReportLine, pszReportAdd);
	if (fd_log)
	{
		fprintf(fd_log, "\n%02d.%02d.%d\t%02d:%02d:%02d\tCity %s started as ",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond, pszCityName);
	}
	
	for (i = 1; i<nArgs; i++)
	{
		sprintf(pszReportAdd, "%s ", pArgs[i]);
		strcat(pszReportLine, pszReportAdd);
		if (fd_log)
			fprintf(fd_log, pszReportAdd);
	}
	strcat(pszReportLine, "\n");
	printf(pszReportLine);
	if (fd_log)
		fprintf(fd_log, "\n");
	fclose(fd_log);
	pszReportLine[0] = 0;
	
	for (i = 1; i<nArgs; i++)
	{
		c = 0;
		char *pszArg = pArgs[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case 'T':
		case 't': // time delay
			sscanf(pArgs[i]+2, "%d", &desired_delay);
			desired_delay *= 1000; // msec
			break;
		case 'M':
		case 'm': // SDMS
		case 'V':
		case 'v': // SDMS-VSAT
			j = 0;
			while (!isdigit(*(pArgs[i]+2+j)))
				++j;
			sscanf(pArgs[i]+2+j, "%s", pszIpAddress);
			cDeviceSymbol = *(pszArg+1);
			pszOptions = pszArg;
			break;
		case 'N':
		case 'n': // Enable automatic S/N
			sscanf(pArgs[i]+2, "%lf,%lf,%lf,%lf,%lf", &MinSN, &MaxSN, &MinLevel, &MaxLevel, &MaxPower);
			break;
		default:
			printf("Error: - Unrecognized command-line option '%c'\n", *(pszArg+1));
			break;
		};
	}

	CSdmsAPI *pSdms = NULL;
	if (!FindSyncData(pszIpAddress))
	{
		if (cDeviceSymbol == 'v' || cDeviceSymbol == 'V')
			pSdms = new CSdmsVsat4900L;
		if (pSdms)
		{
			ParseParamsForSdms(pSdms, pszOptions);
			AddInfoToSyncData(pSdms, pszCityName, FALSE);
			EstablishConnection(pSdms, LogFilePath, pszReportLine);
			printf(pszReportLine);
		}
	}
	Sleep(5*60*1000);
	
	while (1)
	{
		pSyncData = FindSyncData(pszIpAddress);
		if (pSyncData)
		{
			EnterCriticalSection(&g_CriticalSection);
			pSyncData->bAutoUZSLevel = TRUE;
			LeaveCriticalSection(&g_CriticalSection);
		}
		GetLocalTime(&time);
		pszReportLine[0] = 0;
		CurrentTime = CTime::GetCurrentTime();
		EnterCriticalSection(&g_CriticalSection);
		double WorstSN = 1000.;
		double BestSN = -1.;
		double CurrentUzsPower = -1000;
		char pszBestSNCityName[64], pszWorstSNCityName[64];
		pszBestSNCityName[0] = 0;
		pszWorstSNCityName[0] = 0;

		int AZSsResponding = 0;
		for (i = 0; i<sizeof(SyncArray)/sizeof(SyncArray[0]); i++)
		{
			if (SyncArray[i].address == 0)
				continue;//skip not assigned data
			if (SyncArray[i].bAZS == FALSE && SyncArray[i].Power >= 0)
				CurrentUzsPower = SyncArray[i].Power;
			if (SyncArray[i].bAZS == FALSE)
				continue; // skip UZS SN
			if (SyncArray[i].SN < 1.)
				continue; // skip not responding AZSs
			++AZSsResponding;
			if (SyncArray[i].SN < WorstSN)
			{
				WorstSN = SyncArray[i].SN;
				strcpy(pszWorstSNCityName, SyncArray[i].pszCityName);
			}
			if (SyncArray[i].SN > BestSN)
			{
				BestSN = SyncArray[i].SN;
				strcpy(pszBestSNCityName, SyncArray[i].pszCityName);
			}
		}
		LeaveCriticalSection(&g_CriticalSection);
		if (AZSsResponding > 1)
		{
			CString FileName;
			if (strlen(pszCityName) == 0)
				FileName = CurrentTime.Format("%d_%B%Y,%A.");
			else
				FileName.Format("%s\\%s\\%s", szWorkingDir, pszCityName, CurrentTime.Format("%d_%B%Y,%A."));
			FileName += "rgl";
			FILE *fd = fopen(FileName, "a+");
			if (fd)
			{
				fprintf(fd, "%02d.%02d.%d\t%02d:%02d:%02d\t",
						time.wDay, time.wMonth, time.wYear,
						time.wHour,	time.wMinute, time.wSecond);
				fprintf(fd, "%5.1f\t%s\t%5.1f\t%s", BestSN, pszBestSNCityName, WorstSN, pszWorstSNCityName);
			}
			if (WorstSN < MinSN || WorstSN > MaxSN)
			{
				sprintf(pszReportAdd, "\n%02d.%02d.%d\t%02d:%02d:%02d\t",
					time.wDay, time.wMonth, time.wYear,
					time.wHour,	time.wMinute, time.wSecond);
				strcat(pszReportLine, pszReportAdd);
				sprintf(pszReportAdd, "The following SNs were detected on AZSs (should be between %5.1f and %5.1f dB):\n", MinSN, MaxSN);
				strcat(pszReportLine, pszReportAdd);
				
				EnterCriticalSection(&g_CriticalSection);
				for (i = 0; i<sizeof(SyncArray)/sizeof(SyncArray[0]); i++)
				{
					if (SyncArray[i].bAZS == TRUE && SyncArray[i].SN > -0.5)
					{
						sprintf(pszReportAdd, "\t%5.1f(%s)", SyncArray[i].SN, SyncArray[i].pszCityName);
						strcat(pszReportLine, pszReportAdd);
					}
				}
				strcat(pszReportLine, "\n");
				LeaveCriticalSection(&g_CriticalSection);
				
				SRegionSync *pData = FindSyncData(pszIpAddress);
				if (pData)
				{
					EnterCriticalSection(&g_CriticalSection);
					if (pData->CurrentLevel < -35 || pData->CurrentLevel > 5)
					{
						if (pSdms)
						{
							CModIfParams params; 
							pSdms->GetTIfParams(params, 1);
							pData->CurrentLevel = params.m_OutputLevel;
						}
					}

					if (pData->CurrentLevel >= -35. && pData->CurrentLevel <= 5.)
					{
						printf("BestSN = %5.1f (%s)\tWorstSN = %5.1f (%s)\n", BestSN, pszBestSNCityName, WorstSN, pszWorstSNCityName);
						pData->DesiredLevel = CalcNewLevel(WorstSN, BestSN, MinSN, MaxSN, pData->CurrentLevel, pData->Power, MinLevel, MaxLevel, MaxPower);
						sprintf(pszReportAdd, "\nCurrent Power = %5.1f dBm, PowerLimit = %5.1f dBm\n", CurrentUzsPower, MaxPower);
						strcat(pszReportLine, pszReportAdd);
						sprintf(pszReportAdd, "Current modulator level = %5.1f dBm should be changed to %5.1f dBm (%+5.1f dBm)\nModulator limits: %5.1f...%5.1f\n", pData->CurrentLevel, pData->DesiredLevel, (pData->DesiredLevel-pData->CurrentLevel), MinLevel, MaxLevel);
						strcat(pszReportLine, pszReportAdd);
						if (fd)
						{
							fprintf(fd, "\t%5.1f\t%5.1f\t%+5.1f", pData->CurrentLevel, pData->DesiredLevel, (pData->DesiredLevel-pData->CurrentLevel));
						}
					}
					else
						printf("\nERROR: UZS modulator current level = %5.1f dBm\n", pData->CurrentLevel);
					LeaveCriticalSection(&g_CriticalSection);
				}
				else
				{
					printf("\nERROR: cannot find connection %s (%ld)!\n", pszIpAddress, inet_addr(pszIpAddress));
				}
				printf(pszReportLine);
				LogFilePath = NewDirName + "\\" + CurrentTime.Format(pszLogFileNameTemplate);
				g_UzsLogFilePath = LogFilePath;
				FILE *fd_log = fopen(LogFilePath, "a+");
				if (fd_log)
					fprintf(fd_log, pszReportLine);
				fclose(fd_log);
			}
			else
			{
				EnterCriticalSection(&g_CriticalSection);
				g_bUrgentGetSN = FALSE;
				LeaveCriticalSection(&g_CriticalSection);
			}
			if (fd)
			{
				fprintf(fd, "\n");
				fclose(fd);
			}
		}
		Sleep(desired_delay);
	}

//	SetEvent(pThis->m_hThreadProcEnded);
	TRACE("AutoUZSThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}

int main(int argc, char* argv[])
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	const char *pszLogFileNameTemplate = "LogFile_%Y_%B.txt";
	memset(SyncArray, 0, sizeof(SyncArray));
	InitializeCriticalSection(&g_CriticalSection);
	g_bUrgentGetSN = FALSE;
	g_UzsLogFilePath = "";
	
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
		printf("usage: region.exe file_name\n");
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
	
	while (!stream.eof())
	{
		stream.getline(pszStr, sizeof(pszStr));
		if (strlen(pszStr) == 0)
			continue; // ignore empty strings
		if (strstr(pszStr, "CUVS") == NULL && strchr(pszStr, '-') == NULL)
		{
			printf("invalid string %s\n", pszStr);
			continue;
		}
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
		char *pszNewCmdLine = strdup(pszStr);
		for (i=0; i < (int)strlen(pszNewCmdLine); i++)
		{
			if (pszNewCmdLine[i] == '\t')
				pszNewCmdLine[i] = ' ';
		}
		// split command line by arguments
		// 1. Find city name
		while (*pszNewCmdLine == ' ')
			++pszNewCmdLine;
		// 2. break city name (terminating zero)
		char *pszZero = strchr(pszNewCmdLine, ' ');
		if (!pszZero || strchr(pszZero, '-') == NULL)
		{
			printf("not enough arguments in line %s\n", pszNewCmdLine);
			continue;
		}
		*pszZero = 0; // terminating zero
		char **pszCmdLineArgs = (char **)malloc(100*sizeof(char *));
		memset(pszCmdLineArgs, 0, 100*sizeof(char *));
		pszCmdLineArgs[0] = pszNewCmdLine;
		int nArgs = 1;
		char *pszArg = pszZero+1;
		while ((pszArg = strchr(pszArg, '-')) != NULL)
		{
			pszCmdLineArgs[nArgs] = pszArg;
			++nArgs;
			pszZero = strchr(pszArg, ' ');
			if (pszZero != NULL)
			{
				*pszZero = 0;
				pszArg = pszZero+1;
			}
			else
				break;
		}
		unsigned int ThreadID = 0;
		if (strncmp(pszCmdLineArgs[0], "UZS", 3) == 0)
			RunThread(AutoUzsThreadProcedure, (void *)pszCmdLineArgs, PRIORITY_NORMAL, &ThreadID);
		else if (strncmp(pszCmdLineArgs[0], "CUVS", 4) == 0)
		{
			sscanf(pszCmdLineArgs[1]+2, "%s", g_szCuvsAddress[g_nCuvsCount]);
			g_CuvsPort[g_nCuvsCount] = 8888;
			char *pszComma = strchr(pszCmdLineArgs[1]+2, ':');
			if (pszComma)
			{
				*pszComma = 0;
				sscanf(pszCmdLineArgs[1]+2, "%s", g_szCuvsAddress[g_nCuvsCount]);
				sscanf(pszComma+1, "%d", &g_CuvsPort[g_nCuvsCount]);
			}
			strcpy(g_szRegionPrefix[g_nCuvsCount], "");
			if (nArgs >= 3)
				sscanf(pszCmdLineArgs[2]+2, "%s", g_szRegionPrefix[g_nCuvsCount]);
			else
				printf("WARNING: cannot find region prefix!\n");

			++g_nCuvsCount;
		}
		else if (strncmp(pszCmdLineArgs[0], "TRAP", 4) == 0)
		{
			sscanf(pszCmdLineArgs[1]+2, "%s", g_szTrapAddress[g_nTrapCount]);
			g_TrapPort[g_nTrapCount] = 162;
			char *pszComma = strchr(pszCmdLineArgs[1]+2, ':');
			if (pszComma)
			{
				*pszComma = 0;
				sscanf(pszCmdLineArgs[1]+2, "%s", g_szTrapAddress[g_nTrapCount]);
				sscanf(pszComma+1, "%d", &g_TrapPort[g_nTrapCount]);
			}

			++g_nTrapCount;
		}
		else
			RunThread(OneObjectThreadProcedure, (void *)pszCmdLineArgs, PRIORITY_NORMAL, &ThreadID);
		Sleep(7335);
	}
	stream.close();

	while (1)
	{
		getch();
		printf("Press Ctrl+Break to terminate the program\n");
	}

	return 0;
}
