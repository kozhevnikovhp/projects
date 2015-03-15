// Batcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include "stdio.h"
#include "conio.h"
#include <fstream.h>

void Usage(const char *pszExeFileName)
{
		
	printf("Usage: %s <file_name>\n\
\twhere <file_name> is the file with task descriptions\n\n\
\tThis file should look like:\n\n\
\tWindowTitle = Novomoskovsk\n\
\tWorkingDir = C:\\Novomoskovsk\\ping\n\
\tExe = ping 192.168.23.65 -t -w 3000\n\n\
\tWindowTitle = Arsenevo\n\
\tWorkingDir = C:\\Arsenevo\n\
\tExe = ModemSN.exe -m192.168.23.76\n\n\
Press any key please\n", pszExeFileName);
	getch();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		char *pszExeFileName = strrchr(argv[0], '\\');
		if (!pszExeFileName)
			pszExeFileName = argv[0];
		else
		{
			++pszExeFileName;
			Usage(pszExeFileName);
			return 0;
		}
	}
	char pszLine[1024];
	char pszCommandLine[1024];
	char pszWindowTitle[1024], *pszWorkingDirReal = NULL;
	char pszWorkingDir[1024];
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpReserved2 = NULL;
	si.cbReserved2 = 0;
	si.lpDesktop = NULL;
	si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;
	pszCommandLine[0] = 0;
	pszWindowTitle[0] = 0;
	pszWorkingDir[0] = 0;
	fstream stream(argv[1], ios::in);
	while (!stream.eof())
	{
		stream.getline(pszLine, sizeof(pszLine));
		char *pszValue = NULL;
		for (unsigned int i = 0; i<strlen(pszLine); i++)
		{
			if (pszLine[i] == '=')
			{
				pszValue = pszLine+i;
				break;
			}
			pszLine[i] = tolower(pszLine[i]);
		}
		if (pszValue == NULL)
			continue;
		pszValue++;
		while (*pszValue && !isdigit(*pszValue) && !isalpha(*pszValue))
			pszValue++;
		
		if (strncmp(pszLine, "windowtitle", 11) == 0)
			strcpy(pszWindowTitle, pszValue);
		else if (strncmp(pszLine, "workingdir", 10) == 0)
			strcpy(pszWorkingDir, pszValue);
		else if (strncmp(pszLine, "exe", 3) == 0)
		{
			strcpy(pszCommandLine, pszValue);
			if (strlen(pszWorkingDir) != 0)
				pszWorkingDirReal = pszWorkingDir;
			printf("\nWindowTitle = %s\nCommandLine = %s\nWorkingDir = %s\n", pszWindowTitle, pszCommandLine, pszWorkingDirReal);
			si.lpTitle = pszWindowTitle;
			if (strlen(si.lpTitle) == 0)
				si.lpTitle = pszCommandLine;
			BOOL bCreated = CreateProcess(NULL, pszCommandLine,
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
				NULL,
				pszWorkingDirReal,
				&si,
				&pi
				);
			if (!bCreated)
			{
				DWORD dwErrorCode = GetLastError();
				printf("\t- cannot execute due to error code = %d\n", dwErrorCode);
			}
		}
	}
	
	printf("\nPress any key please\n");
	getch();
	return 0;
}
