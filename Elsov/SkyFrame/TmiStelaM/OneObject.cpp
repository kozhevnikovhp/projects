// OneObject.cpp : Defines procedure for one object like modem, tranceiver, power meter, StelaM etc.
// Method OneObject() can be called from application or as thread procedure.
//

#include "stdafx.h"
#include "../HardwareTest/Datum.h"
#include "../HardwareTest/Radyne.h"
#include "../HardwareTest/WaveSat.h"
#include "../HardwareTest/Vertex.h"
#include "../HardwareTest/Advantech.h"
#include "../HardwareTest/AgilentPowerMeter.h"
#include "../HardwareTest/StelaM.h"
#include "OneObject.h"
#include "IcmpSockets.h"
#include <conio.h>
#include <math.h>

const int UPTIME_PERIOD = 15;
const int LNB_STATUS_PERIOD = 14;
const int BUC_STATUS_PERIOD = 14;
const int SERIAL_PERIOD = 7;
const int ETHERNET_PERIOD = 7;
const int MODULATOR_PERIOD = 9;
const int IP_STATISTICS_PERIOD = 8;
const int ICMP_STATISTICS_PERIOD = 11;
const int UDP_STATISTICS_PERIOD = 13;
const int TCP_STATISTICS_PERIOD = 17;

const int EVENT_PING_TIMEOUT = 1;
const int EVENT_CONNECTION_ESTABLISHED = 2;
const int EVENT_CONNECTION_BROKEN = 3;


SRegionSync SyncArray[MAX_REGION_SIZE];
CRITICAL_SECTION g_CriticalSection;
BOOL g_bUrgentGetSN;
CString g_UzsLogFilePath;

// CUVSes (reports a-la Arjanukhin's soft - OK, ALARM, WARNING, UNKNOWN)
char g_szCuvsAddress[MAX_CUVS_COUNT][32];
short g_CuvsPort[MAX_CUVS_COUNT];
int g_nCuvsCount = 0;

// SNMP TRAPs
char g_szTrapAddress[MAX_CUVS_COUNT][32];
short g_TrapPort[MAX_CUVS_COUNT];
int g_nTrapCount = 0;

char g_szRegionPrefix[MAX_CUVS_COUNT][64];

SRegionSync *FindSyncData(char *pszIpAddress)
{
	DWORD IpAddress = inet_addr(pszIpAddress);
	if (IpAddress == 0)
		return NULL;
	EnterCriticalSection(&g_CriticalSection);
	SRegionSync *pData = SyncArray, *pReturnData = NULL;
	for (int i = 0; i < sizeof(SyncArray)/sizeof(SyncArray[0]); i++, pData++)
	{
		if (pData->address == IpAddress)
		{
			pReturnData = pData;
			break;
		}
	}
	LeaveCriticalSection(&g_CriticalSection);

	return pReturnData;
}

void CorrectUzsLevel
(
	SRegionSync *pSyncData,
	CSdmsAPI *pAzs,
	CSdmsAPI *pUzs,
	CString ModulatorFileName
)
{
	char pszReportLine[512];
	if (pSyncData->DesiredLevel < -35. || pSyncData->DesiredLevel > 5.)
		return; // bad desired level
	if (fabs(pSyncData->DesiredLevel - pSyncData->CurrentLevel) < 0.1)
		return;
	printf("Uzs level correction: from %5.1f dbm to %5.1f dBm (%+5.1f dBm)\n", pSyncData->CurrentLevel, pSyncData->DesiredLevel, pSyncData->DesiredLevel-pSyncData->CurrentLevel);
 /* Uzs level correction needed */
	SYSTEMTIME time;
	GetLocalTime(&time);
//	printf("\n\n UZS level correction by %f dBm\n\n", pSyncData->DesiredLevel - pSyncData->CurrentLevel);
	CModIfParams params;
	pAzs->GetTIfParams(params, 1);
	FILE *fd_modulator = fopen(ModulatorFileName, "a+");
	if (fd_modulator)
	{
		fprintf(fd_modulator, "%02d.%02d.%d\t%02d:%02d:%02d",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond);
	}
	else
		printf("Cannot open file %s for writting\n", ModulatorFileName);
	double AzsLevel = params.m_OutputLevel;
	BOOL bAzsModulatorOn = params.m_bOutputEnabled;
	if (fd_modulator)
	{
		fprintf(fd_modulator, "\t%d\t%6.1f\t", bAzsModulatorOn, AzsLevel);
	}
	
	pUzs->GetTIfParams(params, 1);
	double UzsLevel = params.m_OutputLevel;
	BOOL bUzsModulatorOn = params.m_bOutputEnabled;
	if (fd_modulator)
	{
		fprintf(fd_modulator, "\t%d\t%6.1f\n", bUzsModulatorOn, UzsLevel);
	}

	if (UzsLevel <= 5. && UzsLevel >= -35.)
	{
		EnterCriticalSection(&g_CriticalSection);
		sprintf(pszReportLine, "Modulator level was changed from %5.1f dBm to %5.1f dBm (%+5.1f dBm)\n", UzsLevel, pSyncData->DesiredLevel, (pSyncData->DesiredLevel-UzsLevel));
		UzsLevel = pSyncData->DesiredLevel;
		pUzs->SetOutputLevel(UzsLevel, 1);
		pSyncData->CurrentLevel = UzsLevel;
		if (fabs(pSyncData->CurrentLevel - pSyncData->DesiredLevel) > 0.01)
			strcat(pszReportLine, "ERROR: Cannot set modulator level!\n");
		if (fd_modulator)
		{
			fprintf(fd_modulator, "%02d.%02d.%d\t%02d:%02d:%02d",
					time.wDay, time.wMonth, time.wYear,
					time.wHour,	time.wMinute, time.wSecond);
			fprintf(fd_modulator, "\t%d\t%6.1f\t", bAzsModulatorOn, AzsLevel);
			fprintf(fd_modulator, "\t%d\t%6.1f\n", bUzsModulatorOn, UzsLevel);
		}
		for (int j = 0; j < sizeof(SyncArray)/sizeof(SyncArray[0]); j++)
		{
			SyncArray[j].SN = -1;
			SyncArray[j].bUrgentGetSN = TRUE;
		}
		g_bUrgentGetSN = TRUE;
		LeaveCriticalSection(&g_CriticalSection);
		printf(pszReportLine);
		FILE *fd_log = fopen(g_UzsLogFilePath, "a+");
		if (fd_log)
			fprintf(fd_log, pszReportLine);
		fclose(fd_log);
	} // valid output level value received from AZS
	
	fclose(fd_modulator);
}

void ReportToCuvs(const char *pszIpAddress, const char *pszCityName, double SN, double P, double T, BOOL bAzs)
{
	static CUdpSocket socket;
	static char szBufferForReport[512];
	if (g_CuvsPort <= 0)
		return; // not initialized
	char status[32], szStationType[32];
	if (SN >= 5 && SN <= 15 && P <= 29)
		strcpy(status, "OK");
	else if (SN < 0)
		strcpy(status, "UNKNOWN");
	else
		strcpy(status, "ALARM");

	if (bAzs)
		strcpy(szStationType, "AZS");
	else
		strcpy(szStationType, "UZS");
	CTime Time = CTime::GetCurrentTime();
	CString strTime = Time.Format("%d.%m.%Y %H:%M:%S");

	EnterCriticalSection(&g_CriticalSection);
	/*if (bSocketInitialized)*/
	{
		unsigned int nSentBytes;
		for (int iCuvs = 0; iCuvs < g_nCuvsCount; iCuvs++)
		{
			sprintf(szBufferForReport, "%s\t%s\t%s %s %s\tStatus=%s", strTime, pszIpAddress, szStationType, g_szRegionPrefix[iCuvs], pszCityName, status);
			socket.WriteTo(szBufferForReport, strlen(szBufferForReport)+1, nSentBytes, g_szCuvsAddress[iCuvs], g_CuvsPort[iCuvs]);
		}
	}
	
	LeaveCriticalSection(&g_CriticalSection);
}

void ParseParamsForSdms(CSdmsAPI *pSDMS, char *pszArg)
{
	char pszIpAddress[128], pszConnectionName[128];
	if (!pszArg)
		return;

	int j = 0;
	while (!isdigit(*(pszArg+2+j)))
	{
		switch (*(pszArg+2+j))
		{
		case 'B':
		case 'b': // BUC status
			pSDMS->NeedBucStatus();
			break;
		case 'D':
		case 'd': // enable M&C dump
			pSDMS->EnableLogFile(TRUE);
			break;
		case 'E':
		case 'e': // ethernet interface
			pSDMS->NeedEthernetTmi();
			break;
		case 'I':
		case 'i': // IP-statistics
			pSDMS->NeedIpStatistics();
			break;
		case 'L':
		case 'l': // LNB status
			pSDMS->NeedLnbStatus();
			break;
		case 'M':
		case 'm': // Monoblock TMI (POWER, Temperature)
			pSDMS->NeedMonoblockTMI();
			break;
		case 'P':
		case 'p': // ping enabled
			pSDMS->EnablePing();
			break;
		case 'S':
		case 's': // serial interface
			pSDMS->NeedSerialTmi();
			break;
		case 'U':
		case 'u': // uptime
			pSDMS->NeedUpTimeTmi();
			break;
		}
		++j;
	}
	sscanf(pszArg+2+j, "%s", pszIpAddress);
	if (strlen(pszIpAddress) <= 2)
	{ // SerialPort
		pSDMS->SetControlType(MC_SERIAL_PORT);
		sprintf(pszConnectionName, "Com%s", pszIpAddress);
	}
	else
	{ // Telnet connection
		pSDMS->SetControlType(MC_TELNET);
		strcpy(pszConnectionName, pszIpAddress);
	}
	pSDMS->SetConnectionName(pszConnectionName);
}

void AddInfoToSyncData(CDevice *pDevice, const char *pszCityName, BOOL bAZS)
{
	EnterCriticalSection(&g_CriticalSection);
	// look for free sync data
	for (int iData = 0; iData < sizeof(SyncArray)/sizeof(SyncArray[0]); iData++)
	{
		if (SyncArray[iData].address == 0)
		{
			SyncArray[iData].SN = -1;
			SyncArray[iData].address = inet_addr(pDevice->GetConnectionName());
			strcpy(SyncArray[iData].pszCityName, pszCityName);
			strcpy(SyncArray[iData].pszConnectionName, pDevice->GetConnectionName());
			SyncArray[iData].bAutoUZSLevel = FALSE;
			SyncArray[iData].CurrentLevel = SyncArray[iData].DesiredLevel = -1000;
			SyncArray[iData].Power = -1000;
			SyncArray[iData].bUrgentGetSN = FALSE;
			SyncArray[iData].bAZS = bAZS;
			break;
		}
	}
	if (iData >= sizeof(SyncArray)/sizeof(SyncArray[0]))
		printf("ERROR: Cannot find empty place for sync data. Probably too large region (>%d)\n", MAX_REGION_SIZE);
	LeaveCriticalSection(&g_CriticalSection);
}

void EstablishConnection(CDevice *pDevice, CString &LogFilePath, char *pszReportLine)
{
	char pszReportAdd[1024];
	SYSTEMTIME time;
	GetLocalTime(&time);
	BOOL bConnected = pDevice->Connect(pDevice->GetConnectionName());
	FILE *fd_log = fopen(LogFilePath, "a+");
	if (fd_log)
	{
		fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s - connection <%s> ",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond,
				pDevice->GetName(), pDevice->GetConnectionName());
		sprintf(pszReportAdd, "%02d:%02d:%02d\t%s - connection <%s> ",
				time.wHour,	time.wMinute, time.wSecond,
				pDevice->GetName(), pDevice->GetConnectionName());
		strcat(pszReportLine, pszReportAdd);

		if (bConnected)
		{
			strcpy(pszReportAdd, "SUCCESSFUL\n");
			strcat(pszReportLine, pszReportAdd);
			fprintf(fd_log, pszReportAdd);
			/*unsigned int UpTime = 0;
			pDevice->GetUpTime(UpTime);
			fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\tStation %s (<%s>) up time %d hours %d minutes \n",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond,
				pDevice->GetName(), pDevice->GetConnectionName(),
				UpTime/60, UpTime - UpTime/60*60); 
			sprintf(pszReportAdd, "%02d:%02d:%02d\tStation %s (<%s>) up time %d hours %d minutes \n",
				time.wHour,	time.wMinute, time.wSecond,
				pDevice->GetName(), pDevice->GetConnectionName(),
				UpTime/60, UpTime - UpTime/60*60); 
			strcat(pszReportLine, pszReportAdd);*/
		}
		else
		{
			strcpy(pszReportAdd, "FAILED !!!\n");
			strcat(pszReportLine, pszReportAdd);
			fprintf(fd_log, pszReportAdd);
		}
	}
	fclose(fd_log);
}

int OneObject(int argc, char* argv[], const char *pszCityName)
{
	unsigned int BusAddress = 0;
	const char *pszLogFileNameTemplate = "LogFile_%Y_%B.txt";
	const int AlarmMinSN = 3;
	CString LogFilePath;
	CModem *pModem;
	CDevice *pDevices[100], *pDevice;
	memset(pDevices, 0, sizeof(pDevices));
	
	CSdmsAPI *pStelas[2], *pStelaJustCreated = NULL;
	memset(pStelas, 0, sizeof(pStelas));
	int m_nStelas=0;

	int m_nCycles = 0;

	char szWorkingDir[1024];
	GetCurrentDirectory(sizeof(szWorkingDir), szWorkingDir);
	CString NewDirName = "";
	CTime CurrentTime = CTime::GetCurrentTime();
	if (strlen(pszCityName) != 0)
	{
		NewDirName.Format("%s\\%s", szWorkingDir, pszCityName);
		CreateDirectory(NewDirName, NULL);
		LogFilePath = NewDirName + "\\" + CurrentTime.Format(pszLogFileNameTemplate);
	}
	else
		LogFilePath = CurrentTime.Format(pszLogFileNameTemplate);
	
	BOOL bAutomaticOffEnabled = FALSE;
	BOOL bAutomaticSN = FALSE;
	int i;
	double MinSN = 7.;
	double MaxSN = 10.;
	double MinLevel = -35;
	double MaxLevel = 5;

	CTime LastCarrierTime[100];
	for (i = 0; i<sizeof(LastCarrierTime)/sizeof(LastCarrierTime[0]); i++)
		LastCarrierTime[i] = CTime::GetCurrentTime();

	BOOL bBerTest[100];
	memset(bBerTest, 0, sizeof(bBerTest));
	char c;

	int nDevice = 0;
	int nPort = 1;

	int desired_delay = 5000;
	int delay;

	CString FileName;
	SYSTEMTIME time;
	GetLocalTime(&time);
	BOOL bConnected = FALSE;
	FILE *fd_log;
	char pszReportLine[1024], pszReportAdd[1024];
	pszReportLine[0] = 0;

	if (strlen(pszCityName) != 0)
	{
		fd_log = fopen(LogFilePath, "a+");
		sprintf(pszReportAdd, "\n%02d:%02d:%02d\tCity %s started as ",
					time.wHour,	time.wMinute, time.wSecond, pszCityName);
		strcat(pszReportLine, pszReportAdd);
		if (fd_log)
		{
			fprintf(fd_log, "\n%02d.%02d.%d\t%02d:%02d:%02d\tCity %s started as ",
					time.wDay, time.wMonth, time.wYear,
					time.wHour,	time.wMinute, time.wSecond, pszCityName);
		}
		
		for (int i = 1; i<argc; i++)
		{
			sprintf(pszReportAdd, "%s ", argv[i]);
			strcat(pszReportLine, pszReportAdd);
			if (fd_log)
				fprintf(fd_log, pszReportAdd);
		}
		strcat(pszReportLine, "\n");
		if (fd_log)
			fprintf(fd_log, "\n");
		fclose(fd_log);
	}
	
	for (i = 1; i<argc; i++)
	{
		c = 0;
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case '7': // Datum modem 70 MHz
			pDevice = new CDatumPsm4900_70MHz();
			pDevices[nDevice] = pDevice;
			sscanf(argv[i]+2, "%d", &nPort);
			pDevice->Connect(nPort);
			if (strlen(argv[i])>2)
			{
				sscanf(argv[i]+3, "%s", &c);
				if (c == 'B' || c == 'b')
				{
					bBerTest[nDevice] = TRUE;
				}
			}
			nDevice++;
			break;
		case 'A':
		case 'a': // Advantech tranceiver
			pDevice = new CAdvantechWty1();
			pDevices[nDevice++] = pDevice;
			sscanf(argv[i]+2, "%d", &nPort);
			pDevice->Connect(nPort);
			break;
		case 'C':
		case 'c': // Check controlability
		{
			unsigned int CheckPeriod = 5;
			sscanf(argv[i]+2, "%d", &CheckPeriod);
			if (pStelaJustCreated)
				pStelaJustCreated->EnableControlabilityCheck(TRUE, CheckPeriod);
			break;
		}
		case 'D':
		case 'd': // Datum modem
			pDevice = new CDatumPsm4900_LBand();
			pDevices[nDevice] = pDevice;
			sscanf(argv[i]+2, "%d", &nPort);
			pDevice->Connect(nPort);
			if (strlen(argv[i])>2)
			{
				sscanf(argv[i]+3, "%s", &c);
				if (c == 'B' || c == 'b')
				{
					bBerTest[nDevice] = TRUE;
				}
			}
			nDevice++;
			break;
		case 'E':
		case 'e': // Enable automatic turning OFF
			bAutomaticOffEnabled = TRUE;
			break;
		case 'H':
		case 'h': // Hewlett-Packard power meter
			pDevice = new CAgilentE4418B();
			pDevices[nDevice++] = pDevice;
			sscanf(argv[i]+2, "%d", &nPort);
			pDevice->Connect(nPort);
			break;
		case 'N':
		case 'n': // Enable automatic S/N
			bAutomaticSN = TRUE;
			sscanf(argv[i]+2, "%lf,%lf,%lf,%lf", &MinSN, &MaxSN, &MinLevel, &MaxLevel);
			break;
		case 'T':
		case 't': // time delay
			sscanf(argv[i]+2, "%d", &desired_delay);
			desired_delay *= 1000; // msec
			break;
		case '4':
		case '5': // New SDMS (so-called SDMS-VSAT)
			if (*(pszArg+1) == '4')
				pStelaJustCreated = new CSdmsVsat4900L();
			else
				pStelaJustCreated = new CSdmsVsatM500L();
			pDevice = (CDevice *)pStelaJustCreated;
			pDevices[nDevice++] = pDevice;
			ParseParamsForSdms(pStelaJustCreated, argv[i]);
			AddInfoToSyncData(pDevice, pszCityName, (m_nStelas == 0));
			EstablishConnection(pDevice, LogFilePath, pszReportLine);
			if (m_nStelas <= 2)
			{
				pStelas[m_nStelas] = (CSdmsAPI *)pDevice;
			}
			m_nStelas++;
			break;
		case 'W':
		case 'w': // Wavesat tranceiver
			pDevice = new CWaveSatWtr70();
			pDevices[nDevice++] = pDevice;
			sscanf(argv[i]+2, "%d", &nPort);
			pDevice->Connect(nPort);
			break;			
		case 'X':
		case 'x': // Vertex tranceiver
			pDevice = new CVertex50W();
			pDevices[nDevice++] = pDevice;
			sscanf(argv[i]+2, "%d", &nPort);
			pDevice->Connect(nPort);
			break;			
		default:
			printf("Error: - Unrecognized command-line option '%c'\n", *(pszArg+1));
			break;
		};
	}

	delay = desired_delay;
	if (MinSN < AlarmMinSN || MaxSN < AlarmMinSN || MinSN > 20 || MaxSN > 20 || MinSN > MaxSN)
	{
		printf("\nERROR! Please check desired S/N limits\nPress any key\n");
		getch();
		return 0;
	}

	MC_ErrorCode EC = MC_OK;
	srand(GetTickCount());
	
	while (1)
	{
		FILE *fd_uptime = NULL;
		FILE *fd_buc = NULL;
		FILE *fd_lnb = NULL;
		FILE *fd_serial = NULL;
		FILE *fd_serial_errors = NULL;
		FILE *fd_ethernet = NULL;
		FILE *fd_ethernet_errors = NULL;
		FILE *fd_modulator = NULL;
		FILE *fd_ip_statistics = NULL;
		FILE *fd_icmp_statistics = NULL;
		FILE *fd_udp_statistics = NULL;
		FILE *fd_tcp_statistics = NULL;
		int nCurrentStela = 0;
		double SN = 0.;
		double Level = 0;
		double T = 0;
		double Power = 0;
		int Offset = 0;
		BOOL bCarrier = FALSE;
		GetLocalTime(&time);
		CurrentTime = CTime::GetCurrentTime();
		if (strlen(pszCityName) == 0)
			FileName = CurrentTime.Format("%d_%B%Y,%A.");
		else
			FileName.Format("%s\\%s\\%s", szWorkingDir, pszCityName, CurrentTime.Format("%d_%B%Y,%A."));
		DWORD StartTime = GetTickCount();

		if (strlen(pszCityName) != 0)
		{
			LogFilePath = NewDirName + "\\" + CurrentTime.Format(pszLogFileNameTemplate);
		}
		else
			LogFilePath = CurrentTime.Format(pszLogFileNameTemplate);
		
		FILE *fd = fopen(FileName+"txt", "a+");
		if (fd)
		{
			fprintf(fd, "%02d.%02d.%d\t%02d:%02d:%02d",
					time.wDay, time.wMonth, time.wYear,
					time.wHour,	time.wMinute, time.wSecond);
		}
		sprintf(pszReportAdd, "%02d:%02d:%02d %s:",
				time.wHour,	time.wMinute, time.wSecond, pszCityName);
		strcat(pszReportLine, pszReportAdd);

		nDevice = 0;
		while (pDevice = pDevices[nDevice])
		{
			T = -273;
			Power = -1;
			SN = -1;
			Level = -200.;
			Offset = 0;
			GetLocalTime(&time);

			if (fd)
				fprintf(fd, "\t");
			sprintf(pszReportAdd, "\n");
			strcat(pszReportLine, pszReportAdd);

			if (pDevice->IsStation())
			{
				nCurrentStela++;
				CSdmsAPI *pSDMS = (CSdmsAPI *)pDevice;
				pSDMS->SetControlParams(MinSN, MaxSN);
				MC_ErrorCode EC;

				// Uptime
				if ((m_nCycles % UPTIME_PERIOD) == 0 && pSDMS->IsUpTimeTmiNeeded())
				{
					if (!fd_uptime)
					{
						fd_uptime = fopen(FileName+"upt", "a+");
						if (fd_uptime)
						{
							fprintf(fd_uptime, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// IP-statistics
				if ((m_nCycles % IP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					if (!fd_ip_statistics)
					{
						fd_ip_statistics = fopen(FileName+"ip", "a+");
						if (fd_ip_statistics)
						{
							fprintf(fd_ip_statistics, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// ICMP-statistics
				if ((m_nCycles % ICMP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					if (!fd_icmp_statistics)
					{
						fd_icmp_statistics = fopen(FileName+"icmp", "a+");
						if (fd_icmp_statistics)
						{
							fprintf(fd_icmp_statistics, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// UDP-statistics
				if ((m_nCycles % UDP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					if (!fd_udp_statistics)
					{
						fd_udp_statistics = fopen(FileName+"udp", "a+");
						if (fd_udp_statistics)
						{
							fprintf(fd_udp_statistics, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// TCP-statistics
				if ((m_nCycles % TCP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					if (!fd_tcp_statistics)
					{
						fd_tcp_statistics = fopen(FileName+"tcp", "a+");
						if (fd_tcp_statistics)
						{
							fprintf(fd_tcp_statistics, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// Serial Interface
				if ((m_nCycles % SERIAL_PERIOD) == 0 && pSDMS->IsSerialTmiNeeded())
				{
					if (!fd_serial)
					{
						fd_serial = fopen(FileName+"ser", "a+");
						if (fd_serial)
						{
							fprintf(fd_serial, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
					if (!fd_serial_errors)
					{
						fd_serial_errors = fopen(FileName+"see", "a+");
						if (fd_serial_errors)
						{
							fprintf(fd_serial_errors, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// Ethernet Interface
				if ((m_nCycles % ETHERNET_PERIOD) == 0 && pSDMS->IsEthernetTmiNeeded())
				{
					if (!fd_ethernet)
					{
						fd_ethernet = fopen(FileName+"eth", "a+");
						if (fd_ethernet)
						{
							fprintf(fd_ethernet, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
					if (!fd_ethernet_errors)
					{
						fd_ethernet_errors = fopen(FileName+"ete", "a+");
						if (fd_ethernet_errors)
						{
							fprintf(fd_ethernet_errors, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// Modulator
				if ((m_nCycles % MODULATOR_PERIOD) == 0)
				{
					if (!fd_modulator)
					{
						fd_modulator = fopen(FileName+"mod", "a+");
						if (fd_modulator)
						{
							fprintf(fd_modulator, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// BUC status
				if ((m_nCycles % BUC_STATUS_PERIOD) == 0 && pSDMS->IsBucStatusNeeded())
				{
					if (!fd_buc)
					{
						fd_buc = fopen(FileName+"buc", "a+");
						if (fd_buc)
						{
							fprintf(fd_buc, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}
				// LNB status
				if ((m_nCycles % LNB_STATUS_PERIOD) == 0 && pSDMS->IsLnbStatusNeeded())
				{
					if (!fd_lnb)
					{
						fd_lnb = fopen(FileName+"lnb", "a+");
						if (fd_lnb)
						{
							fprintf(fd_lnb, "%02d.%02d.%d\t%02d:%02d:%02d",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond);
						}
					}
				}

				unsigned int UpTime = 0;
				EC = pSDMS->GoTop();
				EC = pSDMS->GetUpTime(UpTime);
				if (EC == MC_DEVICE_NOT_RESPONDING || EC == MC_GENERIC_ERROR)
				{
					fd_log = fopen(LogFilePath, "a+");
					if (!pSDMS->IsConnectionFailureReported())
					{
						if (fd_log)
						{
							fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s - connection <%s> FAILED !!!\n",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond, pSDMS->GetName(), pSDMS->GetConnectionName()); 
						}
					}
					bConnected = pSDMS->Connect();
					if (bConnected)
					{
						EC = pSDMS->GetUpTime(UpTime);
						if (fd_log)
						{
							fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s - reconnection <%s> SUCCESSFUL\n",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond, pSDMS->GetName(), pSDMS->GetConnectionName()); 
							pSDMS->ConnectionFailureUnReported();
							fprintf(fd_log, "%s (<%s>) up time %d hours %d minutes\n%02d.%02d.%d\t%02d:%02d:%02d\t",
									pSDMS->GetName(), pSDMS->GetConnectionName(),
									UpTime/60, UpTime - UpTime/60*60,
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond); 
						}
						sprintf(pszReportAdd, "%s (<%s>) up time %d hours %d minutes\n%02d:%02d:%02d\t",
								pSDMS->GetName(), pSDMS->GetConnectionName(),
								UpTime/60, UpTime - UpTime/60*60,
								time.wHour,	time.wMinute, time.wSecond); 
						strcat(pszReportLine, pszReportAdd);
					}
					else
					{
						if (!pSDMS->IsConnectionFailureReported())
						{
							if (fd_log)
							{
								fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s - reconnection <%s> FAILED !!!\n",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond, pSDMS->GetName(), pSDMS->GetConnectionName()); 
								pSDMS->ConnectionFailureReported();
							}
						}
					}
					fclose(fd_log);
				} // end of "connection failed"
				
				if ((m_nCycles % UPTIME_PERIOD) == 0)
				{
/*					fd_log = fopen(LogFilePath, "a+");
					if (fd_log)
					{
						fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s (<%s>) up time %d hours %d minutes\n",
								time.wDay, time.wMonth, time.wYear,
							time.wHour,	time.wMinute, time.wSecond,
							pStation->GetName(), pStation->GetConnectionName(),
							UpTime/60, UpTime - UpTime/60*60
							); 
					}
					fclose(fd_log);*/
					sprintf(pszReportAdd, "\n%02d:%02d:%02d\t%s (<%s>) up time %d hours %d minutes\n",
						time.wHour,	time.wMinute, time.wSecond,
						pSDMS->GetName(), pSDMS->GetConnectionName(),
						UpTime/60, UpTime - UpTime/60*60
						); 
					strcat(pszReportLine, pszReportAdd);
				}
				// Demodulator
				CDemIfStatus status;
				EC = pSDMS->GetRIfStatus(status, 1);
				// S/N
				SN = status.m_SN;
				sprintf(pszReportAdd, "\t%4.1f", SN);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);
				// Input Level
				Level = status.m_InputLevel;
				sprintf(pszReportAdd, "\t%4.1f", Level);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);
				// Offset
				Offset = status.m_Offset;
				sprintf(pszReportAdd, "\t%+d", Offset);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);

				// Tranceiver
				if (pSDMS->IsMonoblockTMINeeded())
				{
					EC = pSDMS->GetTranceiverStatus();
					// Temperature
					pSDMS->GetTranceiverTemperature(T);
					pSDMS->GetTranceiverPower(Power);
					sprintf(pszReportAdd, "\t%4.0f\t%6.2f", T, Power);
					strcat(pszReportLine, pszReportAdd);
					if (fd)
						fprintf(fd, pszReportAdd);
					if (EC != MC_OK)
					{
					/*	sprintf(pszReportAdd, "\tCannot get T and P from ");
						if (nCurrentStela == 1)
							strcat(pszReportAdd, "AZS");
						else
							strcat(pszReportAdd, "UZS");
						strcat(pszReportAdd, "!");
						strcat(pszReportLine, pszReportAdd);*/
					}
				}
				// Save info in sync array
				SRegionSync *pData = FindSyncData(pSDMS->GetConnectionName());
				if (pData)
				{
					EnterCriticalSection(&g_CriticalSection);
					pData->SN = SN;
					pData->Power = Power;
					LeaveCriticalSection(&g_CriticalSection);
				}
				ReportToCuvs(pSDMS->GetConnectionName(), pszCityName, SN, Power, T, (nCurrentStela == 1));
			
				if ((m_nCycles % MODULATOR_PERIOD) == 0)
				{
					CModIfParams TParams;
					pSDMS->GetTIfParams(TParams, 1);
					Level = TParams.m_OutputLevel;

					if (fd_modulator)
					{
						fprintf(fd_modulator, "\t%d\t%6.1f\t", TParams.m_bOutputEnabled, Level);
					}
					if (pData)
					{
						EnterCriticalSection(&g_CriticalSection);
						pData->CurrentLevel = Level;
						LeaveCriticalSection(&g_CriticalSection);
					}
				}
				// Automatic SN
				if (bAutomaticSN && m_nStelas>=2 && nCurrentStela == 2)
				{
					SN = -1;
					CSdmsAPI *pAzs = pStelas[0];
					CSdmsAPI *pUzs = pSDMS;
					MC_ErrorCode UzsEC = pUzs->GetRIfStatus(status, 1);
					if (UzsEC == MC_OK)
					{ // UZS is controllable
						SN = status.m_SN;
					}
					else
					{
						sprintf(pszReportAdd, "\tCannot get info from UZS to keep S/N automatically!");
						strcat(pszReportLine, pszReportAdd);
					}
					if (SN > 1 && SN <= 20)
					{ // valid SN
						pUzs->NewMeasure(SN);
						double LevelPlus; // Level should be changed by this value
						if (pUzs->NeedControl(LevelPlus))
						{
							fd_log = fopen(LogFilePath, "a+");
							CModIfParams AzsTIfParams;
							MC_ErrorCode AzsEC = pAzs->GetTIfParams(AzsTIfParams, 1);
							if (AzsEC == MC_OK)
							{ // AZS is controllable
								if (!fd_modulator)
									fd_modulator = fopen(FileName+"mod", "a+");
								else
									fprintf(fd_modulator, "\n");
								if (fd_modulator)
								{
									fprintf(fd_modulator, "%02d.%02d.%d\t%02d:%02d:%02d",
											time.wDay, time.wMonth, time.wYear,
											time.wHour,	time.wMinute, time.wSecond);
								}
								double OldLevel = AzsTIfParams.m_OutputLevel;
								BOOL bModulatorOn = AzsTIfParams.m_bOutputEnabled;
								if (fd_modulator)
								{
									fprintf(fd_modulator, "\t%d\t%6.1f\t", bModulatorOn, OldLevel);
								}
								
								CModIfParams UzsTIfParams;
								pUzs->GetTIfParams(UzsTIfParams, 1);
								double UzsLevel = UzsTIfParams.m_OutputLevel;
								BOOL bUzsModulatorOn = UzsTIfParams.m_bOutputEnabled;
								if (fd_modulator)
								{
									fprintf(fd_modulator, "\t%d\t%6.1f\n", bUzsModulatorOn, UzsLevel);
								}

								if (OldLevel == MinLevel && LevelPlus<0)
								{
									sprintf(pszReportAdd, "\t%+5.1f dBm (MIN modulator level limit)", LevelPlus);
									strcat(pszReportLine, pszReportAdd);
									if (fd)
										fprintf(fd, pszReportAdd);
								}
								else if(OldLevel == MaxLevel && LevelPlus>0)
								{
									sprintf(pszReportAdd, "\t%+5.1f dBm (MAX modulator level limit)", LevelPlus);
									strcat(pszReportLine, pszReportAdd);
									if (fd)
										fprintf(fd, pszReportAdd);
								}
								else
								{
									if (fd_log)
									{
										fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\tSignal from AZS %s (%s) detected as %5.1f dB (should be between %5.1f and %5.1f dB)\n",
												time.wDay, time.wMonth, time.wYear,
												time.wHour,	time.wMinute, time.wSecond, pSDMS->GetName(), pszCityName, SN, MinSN, MaxSN);
									}
									if (OldLevel <= 5. && OldLevel >= -35.)
									{
										sprintf(pszReportAdd, "\t%+5.1f dBm", LevelPlus);
										strcat(pszReportLine, pszReportAdd);
										if (fd)
											fprintf(fd, pszReportAdd);
										Level = OldLevel + LevelPlus;
										if (Level < MinLevel)
											Level = MinLevel;
										if (Level > MaxLevel)
											Level = MaxLevel;
										pAzs->SetOutputLevel(Level, 1);
										if (fd_modulator)
										{
											fprintf(fd_modulator, "%02d.%02d.%d\t%02d:%02d:%02d",
													time.wDay, time.wMonth, time.wYear,
													time.wHour,	time.wMinute, time.wSecond);
											fprintf(fd_modulator, "\t%d\t%6.1f\t", bModulatorOn, Level);
											fprintf(fd_modulator, "\t%d\t%6.1f", bUzsModulatorOn, UzsLevel);
										}
										pUzs->ResetAutomatics();
										if (fd_log)
										{
											fprintf(fd_log, "\t\t\tModulator output level changed by %5.1f dB (from %5.1f to %5.1f)\n",
												LevelPlus, OldLevel, OldLevel+LevelPlus);
										} // fd_log
										if (fabs(OldLevel+LevelPlus-Level) > 0.1)
										{
											sprintf(pszReportAdd, "  ERROR: Cannot set modulator level!");
											strcat(pszReportLine, pszReportAdd);
											if (fd_log)
											{
												fprintf(fd_log, "\t\t\t%s", pszReportAdd);
												fprintf(fd_log, "\n");
											}
										}

									} // valid output level value received from AZS
								} //else
							} // AZS is controllable
							fclose(fd_log);
						} // Need Control
						if (SN <= (MinSN-1.) || SN >= (MaxSN+1.))
							delay = 0;
						else if (SN < MinSN || SN > MaxSN)
							delay = desired_delay/4;
						else if (SN <= (MinSN+0.1) || SN >= (MaxSN-0.1))
							delay = desired_delay/2;
						else
							delay = desired_delay;
					} // S/N > 0
				} // Automatic S/N

				// Check controlability
				if (pSDMS->IsControlabilityCheckEnabled() && pSDMS->Pass())
				{
					int DeltaFreq = 1;
					if (Offset < 0)
					{
						DeltaFreq = -1;
					}
					unsigned int InitialFreq = 0;
					EC = pSDMS->GetRFrequency(InitialFreq, 1);
					if (EC == MC_OK)
					{
						unsigned int TestFreq = InitialFreq + DeltaFreq;
						EC = pSDMS->SetRFrequency(TestFreq, 1);
						if (TestFreq == (InitialFreq + DeltaFreq))
						{ // OK - return initial value
							pSDMS->SetRFrequency(InitialFreq, 1);
							pSDMS->LostControlabilityReported(FALSE);
						}
						else
						{ // problem!
							sprintf(pszReportAdd, "\t Cannot control demodulator!!!\t");
							strcat(pszReportLine, pszReportAdd);
							if (fd_log && !pSDMS->IsLostControlabilityReported())
							{
								fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\tCannot control demodulator of %s\n",
										time.wDay, time.wMonth, time.wYear,
										time.wHour,	time.wMinute, time.wSecond, pSDMS->GetConnectionName());
								pSDMS->LostControlabilityReported(TRUE);
							}
						}
					}
				}

				// Uptime
				if ((m_nCycles % UPTIME_PERIOD) == 0 && pSDMS->IsUpTimeTmiNeeded())
				{
					if (fd_uptime)
					{
						fprintf(fd_uptime, "\t%.2f", UpTime/60.);
					}
				}
				if ((m_nCycles % IP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					SIpStatistics statistics;
					EC = pSDMS->GetIpStatistics(statistics);
					if (fd_ip_statistics)
					{
						fprintf(fd_ip_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.TotalPacketsReceived,
							statistics.BadCRC,
							statistics.PacketsTooShort,
							statistics.NotEnoughData,
							statistics.IpHeaderTooShort);
						fprintf(fd_ip_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.IpPacketSmallerThanIpHeader,
							statistics.FragmentsReceived,
							statistics.FragmentsDropped,
							statistics.FragmentsTimeouted,
							statistics.PacketsForwarded);
						fprintf(fd_ip_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.PacketsNotForwarded,
							statistics.RedirectsSent,
							statistics.UnknownProtocol,
							statistics.DeliveredToKernel,
							statistics.LocallyGeneratedOutput);
						fprintf(fd_ip_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.LostPackets,
							statistics.Reassembled,
							statistics.Fragmented,
							statistics.OutputFragments,
							statistics.CannotFragment);
						fprintf(fd_ip_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.BadIpOptions,
							statistics.NoRouteDrops,
							statistics.BadIpVersion,
							statistics.RawIpPacketsGenerated,
							statistics.TooLongPackets);
						fprintf(fd_ip_statistics, "\t%I64d\t%I64d\t%I64d\t",
							statistics.AccessDeniedByFW,
							statistics.SwitchedToAnotherInterface,
							statistics.DroppedWhileSwitching);
					}
				}
				if ((m_nCycles % ICMP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					SIcmpStatistics statistics;
					EC = pSDMS->GetIcmpStatistics(statistics);
					if (fd_icmp_statistics)
					{
						fprintf(fd_icmp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.CallsToIcmpError,
							statistics.NoErrorBecauseOldIpTooShort,
							statistics.NoErrorBecauseOldPacketWasIcmp,
							statistics.IcmpCodeOutOfRangeReceived,
							statistics.TooShortPackets);
						fprintf(fd_icmp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.BadChecksum,
							statistics.CalculatedBoundsMismatch,
							statistics.NumberOfResponses,
							statistics.BroadcastMulticastEchoRequestsDropped,
							statistics.BroadcastMulticastTimestampRequestsDropped);
						fprintf(fd_icmp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.RcvdEchoReply,		statistics.SentEchoReply,
							statistics.RcvdUnreach,			statistics.SentUnreach,
							statistics.RcvdSourceQuench,	statistics.SentSourceQuench,
							statistics.RcvdRedirect,		statistics.SentRedirect,
							statistics.RcvdEcho,			statistics.SentEcho);
						fprintf(fd_icmp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.RcvdRouterAdvert,	statistics.SentRouterAdvert,
							statistics.RcvdRouterSolicit,	statistics.SentRouterSolicit,
							statistics.RcvdTtlExceed,		statistics.SentTtlExceed,
							statistics.RcvdParamProbe,		statistics.SentParamProbe,
							statistics.RcvdTimeStamp,		statistics.SentTimeStamp);
						fprintf(fd_icmp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t",
							statistics.RcvdTimeStampReply,	statistics.SentTimeStampReply,
							statistics.RcvdInformation,		statistics.SentInformation,
							statistics.RcvdInformationReply,statistics.SentInformationReply,
							statistics.RcvdMask,			statistics.SentMask,
							statistics.RcvdMaskReply,		statistics.SentMaskReply);
					}
				}
				if ((m_nCycles % UDP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					SUdpStatistics statistics;
					EC = pSDMS->GetUdpStatistics(statistics);
					if (fd_udp_statistics)
					{
						fprintf(fd_udp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.TotalInputPackets,
							statistics.PacketsShorterThanHeader,
							statistics.ChecksumErrors,
							statistics.DataLengthLargerThanPacket,
							statistics.NoSocketOnPort);
						fprintf(fd_udp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t",
							statistics.OfAboveArrivedAsBroadcast,
							statistics.NotDeliveredInputSocketFull,
							statistics.InputPacketsMissingPcbCache,
							statistics.InputPacketsNotForHashedPcb,
							statistics.TotalOutputPackets);
					}
				}
				if ((m_nCycles % TCP_STATISTICS_PERIOD) == 0 && pSDMS->IsIpStatisticsNeeded())
				{
					STcpStatistics statistics;
					EC = pSDMS->GetTcpStatistics(statistics);
					if (fd_tcp_statistics)
					{
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.ConnectionsInitiated,
							statistics.ConnectionsAccepted,
							statistics.ConnectionsEstablished,
							statistics.ConnectionsDropped,
							statistics.EmbryonicConnectionsDropped);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.ConnectionsClosed,
							statistics.SegsWhereWeTriedToGetRTT,
							statistics.TimesWeSucceeded,
							statistics.DelayedAcksSent,
							statistics.ConnectionsDroppedInRxmtTimeout);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.RetransmitTimeouts,
							statistics.PersistTimeouts,
							statistics.KeepaliveTimeouts,
							statistics.KeepaliveProbesSent,
							statistics.ConnectionsDroppedInKeepalive);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.TotalPacketsSent,
							statistics.DataPacketsSent,
							statistics.DataBytesSent,
							statistics.DataPacketsRetransmitted,
							statistics.DataBytesRetransmitted);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.AckOnlyPacketsSent,
							statistics.WindowProbesSent,
							statistics.PacketsSentWithUrgOnly,
							statistics.WindowUpdateOnlyPacketsSent,
							statistics.ControlPacketsSent);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.TotalPacketsReceived,
							statistics.PacketsReceivedInSequence,
							statistics.BytesReceivedInSequence,
							statistics.PacketsReceivedWithChecksumError,
							statistics.PacketsReceivedWithBadOffset);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.PacketsReceivedTooShort,
							statistics.DuplicateOnlyPacketsReceived,
							statistics.DuplicateOnlyBytesReceived,
							statistics.PacketsWithSomeDuplicateData,
							statistics.DupBytesInPartDupPackets);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.OutOfOrderPacketsReceived,
							statistics.OutOfOrderBytesReceived,
							statistics.PacketsWithDataAfterWindow,
							statistics.BytesRcvdAfterWindow,
							statistics.PacketsRcvdAfterClose);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.RcvdWindowProbePackets,
							statistics.RcvdDuplicateAcks,
							statistics.RcvdAcksForUnsentData,
							statistics.RcvdAckPackets,
							statistics.BytesAckedByRcvdAcks);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.RcvdWindowUpdatePackets,
							statistics.SegmentsDroppedDueToPAWS,
							statistics.TimesHdrPredictOkForAcks,
							statistics.TimesHdrPredictOkForDataPkts,
							statistics.PcbCacheMisses);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							statistics.TimesCachedRTTInRouteUpdated,
							statistics.TimesCachedRTTVarUpdated,
							statistics.TimesCachedSsThreshUpdated,
							statistics.TimesRTTIninitializedFromRoute,
							statistics.TimesRTTVarInitializedFromRT);
						fprintf(fd_tcp_statistics, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t",
							statistics.TimesSsThreshInitializedFromRT,
							statistics.TimeoutInPersistState,
							statistics.BogusSYNPrematureACK,
							statistics.ResendsDueToMTUDiscovery,
							statistics.ListenQueueOverflows);
					}
				}
				if ((m_nCycles % SERIAL_PERIOD) == 0 && pSDMS->IsSerialTmiNeeded())
				{
					SInterfaceStatus status;
					SSerialErrors errors;
					EC = pSDMS->GetSerialInterfaceStatus(status, errors);
					if (fd_serial)
					{
						fprintf(fd_serial, "\t%I64d\t%I64d\t%I64d\t%I64d\t\t%I64d\t%I64d\t%I64d\t%I64d\t",
							status.InputPackets, status.InputBytes, status.InputErrors, status.InputDrops,
							status.OutputPackets, status.OutputBytes, status.OutputErrors, status.OutputDrops);
					}
					if (fd_serial_errors)
					{
						fprintf(fd_serial_errors, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t",
								errors.ReceiveCRC, errors.ReceiveOverrun, errors.ReceiveLostCarrier, errors.ReceiveFrameViolations, errors.ReceiveNonOctet, errors.ReceiveAbortSequence, errors.ReceiveDpll,
								errors.SendUnderrun, errors.SendLostCTS, errors.SendKeepaliveState, errors.SendRxSeq, errors.SendTxSeq, errors.SendLastPoll);
					}
				}
				if ((m_nCycles % ETHERNET_PERIOD) == 0 && pSDMS->IsEthernetTmiNeeded())
				{
					SInterfaceStatus status;
					SEthernetErrors errors;
					EC = pSDMS->GetEthernetInterfaceStatus(status, errors);
					if (fd_ethernet)
					{
						fprintf(fd_ethernet, "\t%I64d\t%I64d\t%I64d\t%I64d\t\t%I64d\t%I64d\t%I64d\t%I64d\t",
							status.InputPackets, status.InputBytes, status.InputErrors, status.InputDrops,
							status.OutputPackets, status.OutputBytes, status.OutputErrors, status.OutputDrops);
					}
					if (fd_ethernet_errors)
					{
						fprintf(fd_ethernet_errors, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t",
								errors.ReceiveShort, errors.ReceiveGiant, errors.ReceiveNonOctet, errors.ReceiveCRC, errors.ReceiveOverrun, errors.ReceiveCollision, errors.ReceiveDiscarded,
								errors.SendUnderrun, errors.SendLateCollisions, errors.SendDeffered, errors.SendLostCarrier, errors.SendHeartBeat, errors.SendRetries, errors.SendRetransmitLimits);
					}
				}
				if ((m_nCycles % BUC_STATUS_PERIOD) == 0 && pSDMS->IsBucStatusNeeded())
				{
					CBucStatus status;
					EC = pSDMS->GetBucStatus(status, 1);
					if (fd_buc)
					{
						fprintf(fd_buc, "\t%7.3f\t%7.3f\t", status.m_Voltage, status.m_Current);
					}
				}
				if ((m_nCycles % LNB_STATUS_PERIOD) == 0 && pSDMS->IsLnbStatusNeeded())
				{
					CLnbStatus status;
					EC = pSDMS->GetLnbStatus(status, 1);
					if (fd_lnb)
					{
						fprintf(fd_lnb, "\t%7.3f\t%7.3f\t", status.m_Voltage, status.m_Current);
					}
				}
			} // pDevice->IsStation()
			else if (pDevice->IsModem())
			{
				pModem = (CModem *)pDevice;
				CurrentTime = CTime::GetCurrentTime();
				// Carrier
				bCarrier = FALSE;
				EC = pModem->IsRCarrier(bCarrier, 1);
				if (bCarrier)
				{
					LastCarrierTime[nDevice] = CurrentTime;
				}
				CTimeSpan NotLockedTime = CurrentTime - LastCarrierTime[nDevice];

				if (bAutomaticOffEnabled && !bCarrier && (NotLockedTime.GetTotalSeconds() > 30))
				{
					// Turn output off
					BOOL bOn = FALSE;
					pModem->IsOutputOn(bOn, 1);
					if (bOn)
					{
						bOn = FALSE;
						pModem->TurnOutputOn(bOn, 1);
						fd_log = fopen(LogFilePath, "a+");
						if (fd_log)
						{
							fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s(%s) - Output off because of carrier loss\n",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond, pModem->GetName(), pModem->GetConnectionName()); 
						}
						fclose(fd_log);
					}
				}
				/*if (bCarrier)
				{
					BOOL bOn = FALSE;
					pModem->IsOutputOn(bOn, 1);
					// Turn output on
					if (!bOn)
					{
						bOn = TRUE;
						pModem->TurnOutputOn(bOn, 1);
						if (fd_log)
						{
							fprintf(fd_log, "%02d.%02d.%d\t%02d:%02d:%02d\t%s(%s) - Output on because of carrier locked\n",
									time.wDay, time.wMonth, time.wYear,
									time.wHour,	time.wMinute, time.wSecond, pModem->GetName(), pModem->GetPortName()); 
						}
					}
				}*/
				// S/N
				CDemIfStatus status;
				pModem->GetRIfStatus(status, 1);
				if (status.m_bCarrier)
				{
					SN = status.m_SN;
				}
				else
				{
					SN = 0;
				}

				sprintf(pszReportAdd, "\t%4.1f", SN);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);

				// Input level
				Level = status.m_InputLevel;
				sprintf(pszReportAdd, "\t%4.1f", Level);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);
				
				// Offset
				Offset = status.m_Offset;
				sprintf(pszReportAdd, "\t%+d", Offset);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);

				if (bBerTest[nDevice])
				{
					unsigned int SyncLoss, ErredSecs, TotalTestDuration;
					double BER, Errors, TotalBits, ErrorFreeSecsPercentage;
					EC = pModem->GetBerTestStatus(BER, SyncLoss, Errors, TotalBits, ErrorFreeSecsPercentage, ErredSecs, TotalTestDuration);
					sprintf(pszReportAdd, "\t%7.2e\t%d\t%6.1e\t%6.2f%%\t%d\t%dsec\t%6.1ebits", BER, SyncLoss, Errors, ErrorFreeSecsPercentage, ErredSecs, TotalTestDuration, TotalBits);
					strcat(pszReportLine, pszReportAdd);
					if (fd)
						fprintf(fd, pszReportAdd);
				} // end if BERT
			}
			
			else if (pDevice->IsTranceiver())
			{
				CTranceiver *pTranceiver = (CTranceiver *)pDevice;
				EC = pTranceiver->GetTemperature(T);
				Power = -1;
				pTranceiver->GetPower(Power);
				sprintf(pszReportAdd, "\t%4.0f\t%6.2f", T, Power);
				strcat(pszReportLine, pszReportAdd);
				if (fd)
					fprintf(fd, pszReportAdd);
			}
			
			else if (pDevice->IsMeasureDevice())
			{
				CMeasureDevice *pMeasureDevice = (CMeasureDevice *)pDevice;
				if (pMeasureDevice->IsPowerMeter())
				{
					CPowerMeter *pPowerMeter = (CPowerMeter *)pDevice;
					Power = -1000;
					EC = pPowerMeter->MeasurePower(Power, 1);
					sprintf(pszReportAdd, "\t%6.2f", Power);
					strcat(pszReportLine, pszReportAdd);
					if (fd)
						fprintf(fd, pszReportAdd);
				}
			}
			
			nDevice++;
		}

		if (fd)
		{
			fprintf(fd, "\n");
			fclose(fd);
		}
		if (fd_uptime)
		{
			fprintf(fd_uptime, "\n");
			fclose(fd_uptime);
		}
		if (fd_buc)
		{
			fprintf(fd_buc, "\n");
			fclose(fd_buc);
		}
		if (fd_lnb)
		{
			fprintf(fd_lnb, "\n");
			fclose(fd_lnb);
		}
		if (fd_ip_statistics)
		{
			fprintf(fd_ip_statistics, "\n");
			fclose(fd_ip_statistics);
		}
		if (fd_icmp_statistics)
		{
			fprintf(fd_icmp_statistics, "\n");
			fclose(fd_icmp_statistics);
		}
		if (fd_udp_statistics)
		{
			fprintf(fd_udp_statistics, "\n");
			fclose(fd_udp_statistics);
		}
		if (fd_tcp_statistics)
		{
			fprintf(fd_tcp_statistics, "\n");
			fclose(fd_tcp_statistics);
		}
		if (fd_serial)
		{
			fprintf(fd_serial, "\n");
			fclose(fd_serial);
		}
		if (fd_serial_errors)
		{
			fprintf(fd_serial_errors, "\n");
			fclose(fd_serial_errors);
		}
		if (fd_ethernet)
		{
			fprintf(fd_ethernet, "\n");
			fclose(fd_ethernet);
		}
		if (fd_ethernet_errors)
		{
			fprintf(fd_ethernet_errors, "\n");
			fclose(fd_ethernet_errors);
		}
		if (fd_modulator)
		{
			fprintf(fd_modulator, "\n");
			fclose(fd_modulator);
		}
		sprintf(pszReportAdd, "\n");
		strcat(pszReportLine, pszReportAdd);
		printf("%s", pszReportLine);
		pszReportLine[0] = 0;
		DWORD DesiredEndTime = StartTime + delay;
		int CurrentStela = 0;
		CIcmpSocket PingSocket;
		PingSocket.SetPingTimeout(3000);
		while (1)
		{
			// UZS automatic levels
			if (m_nStelas >= 2)
			{ // UZS exists
				CSdmsAPI *pAzs = pStelas[0];
				CSdmsAPI *pUzs = pStelas[1];
				SRegionSync *pSyncData = FindSyncData(pUzs->GetConnectionName());
				if (pSyncData)
				{
					EnterCriticalSection(&g_CriticalSection);
					BOOL bAutoUZSLevel = pSyncData->bAutoUZSLevel;
					LeaveCriticalSection(&g_CriticalSection);
					if (bAutoUZSLevel)
						CorrectUzsLevel(pSyncData, pAzs, pUzs, FileName+"mod");
				}
			}
			EnterCriticalSection(&g_CriticalSection);
			BOOL bUrgent = g_bUrgentGetSN;
			LeaveCriticalSection(&g_CriticalSection);
			if (bUrgent)
				break;
			DWORD ThisTime = GetTickCount();
			if (ThisTime >= DesiredEndTime)
				break;
			if ((DesiredEndTime - ThisTime) < 5000)
			{
				Sleep(DesiredEndTime - ThisTime);
				break;
			}

			if (m_nStelas)
			{
				CurrentStela %= m_nStelas;
				Sleep(1000 + (rand() % 1000));
				CSdmsAPI *pSDMS = pStelas[CurrentStela];
				if (pSDMS->IsPingEnabled())
				{
					EC = pSDMS->IsMCConnectionAlive();
					if (EC == MC_OK)
					{
						unsigned short PingLen = 32 + (rand() % 100);
						PingSocket.SendEchoRequest(pSDMS->GetConnectionName(), PingLen);
						PingSocket.ReceiveEchoReply();
						if (PingSocket.IsTimedOut())
						{
							GetLocalTime(&time);
							FILE *fd_ping = fopen(FileName+"evt", "a+");
							fprintf(fd_ping, "%02d.%02d.%02d\t%02d:%02d:%02d\t%d\t%s\t%d\n",
								time.wDay, time.wMonth, time.wYear,
								time.wHour,	time.wMinute, time.wSecond, EVENT_PING_TIMEOUT, pSDMS->GetConnectionName(), PingLen); 
							fclose(fd_ping);
						}
					}
				}
				else
				{
				}
				CurrentStela++;
			}
		}
		m_nCycles++;
	}
	return 0;
}

