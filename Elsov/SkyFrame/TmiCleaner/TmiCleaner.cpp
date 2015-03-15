// TmiCleaner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include "TmiCleaner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

int LookAtDir(char *pszDirName)
{
	int nDeletedFile = 0;
	char pszTemplate[1024];
	strcpy(pszTemplate, pszDirName);
	strcat(pszTemplate, "\\*.*");
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(pszTemplate, &fd);
	BOOL bFound = (hFile != INVALID_HANDLE_VALUE);
	char pszBuffer[1024];
	printf("Cleaning %s directory...\n", pszDirName);
	CTime CurrentTime = CTime::GetCurrentTime();
	
	while (bFound)
	{
		if (strcmp(fd.cFileName, ".") &&
			strcmp(fd.cFileName, "..") &&
			(NULL == strstr(fd.cFileName, "ula.txt")) &&
			(NULL == strstr(fd.cFileName, "ovo.txt")) &&
			(NULL == strstr(fd.cFileName, "ursk.txt")) &&
			(NULL == strstr(fd.cFileName, "opol.txt")) &&
			(NULL == strstr(fd.cFileName, "zan.txt")) )
		{
			strcpy(pszBuffer, pszDirName);
			strcat(pszBuffer, "\\");
			strcat(pszBuffer, fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				nDeletedFile += LookAtDir(pszBuffer);
			}
			else
			{
				CTime LastWriteTime(fd.ftLastWriteTime);
				CTimeSpan DeltaTime = CurrentTime - LastWriteTime;
				int hours = DeltaTime.GetTotalHours();
				if (hours > 24*70) // 70 days
				{
					DeleteFile(pszBuffer);
					++nDeletedFile;
				}
			}
		}
		bFound = FindNextFile(hFile, &fd);
	}
	FindClose(hFile);
	return nDeletedFile;
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		nRetCode = 1;
	}
	else
	{
		char pszBuffer[1024];
		GetCurrentDirectory(sizeof(pszBuffer), pszBuffer);
		int nDeletedFiles = LookAtDir(pszBuffer);
		printf("%d files deleted                           \n", nDeletedFiles);
		getch();
	}

	return nRetCode;
}


