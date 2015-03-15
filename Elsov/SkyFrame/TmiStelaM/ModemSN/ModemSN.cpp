// ModemSN.cpp - defines ModemSN.exe program entry point

#include "stdafx.h"
#include "..\OneObject.h"

int main(int argc, char* argv[])
{
	memset(SyncArray, 0, sizeof(SyncArray));
	InitializeCriticalSection(&g_CriticalSection);
	g_bUrgentGetSN = FALSE;
	g_UzsLogFilePath = "";

	SYSTEMTIME time;
	GetLocalTime(&time);
	const char *pszLogFileNameTemplate = "LogFile_%Y_%B.txt";
	CTime CurrentTime = CTime::GetCurrentTime();
	CString LogFilePath = CurrentTime.Format(pszLogFileNameTemplate);
	FILE *fd_log = fopen(LogFilePath, "a+");
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
	
	return OneObject(argc, argv);
}
