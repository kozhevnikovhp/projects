// LogFileSplitter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LogFileSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

//using namespace std;

void LookAtDir(char *pszDirName)
{
	char pszTemplate[1024];
	strcpy(pszTemplate, pszDirName);
	strcat(pszTemplate, "\\*.*");
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(pszTemplate, &fd);
	BOOL bFound = (hFile != INVALID_HANDLE_VALUE);
	char pszBuffer[1024];
	char pszLine[1024];
	fstream file;
	
	while (bFound)
	{
		if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
		{
			strcpy(pszBuffer, pszDirName);
			strcat(pszBuffer, "\\");
			strcat(pszBuffer, fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				LookAtDir(pszBuffer);
			}
			else
			{
				if (!strcmp(fd.cFileName, "LogFile.txt"))
				{
					printf("processing file %s\\%s\n", pszDirName, fd.cFileName);
					file.open(pszBuffer, ios::in);
					if (file.is_open())
					{
						CString FileName = "";
						while (!file.eof())
						{
							int nDay = -1, nMonth = -1, nYear = -1;
							int nHour = -1, nMin = -1, nSec = -1;
							CTime CurrentTime;
							if (file.getline(pszLine, sizeof(pszLine), '\n'))
							{
								if (sscanf(pszLine, "%d.%d.%d\t%d:%d:%d", &nDay, &nMonth, &nYear, &nHour, &nMin, &nSec) == 6)
								{ // it is date/time
									FileName = pszDirName;
									FileName += "\\";
									CurrentTime = CTime(nYear, nMonth, nDay, 1,1,1);
									FileName += CurrentTime.Format("LogFile_%Y_%B.txt");
									FILE *fd = fopen(FileName, "a+");
									if (fd)
										fprintf(fd, "%s\n", pszLine);
									fclose(fd);
								}
								else
								{ // it is not date/time - just copy
									if (!FileName.IsEmpty())
									{
										FILE *fd = fopen(FileName, "a+");
										if (fd)
											fprintf(fd, "%s\n", pszLine);
										fclose(fd);										
									}
								}
							}
						}
					}
					file.close();
				}
			}
		}
		bFound = FindNextFile(hFile, &fd);
	}
	FindClose(hFile);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		char pszBuffer[1024];
		GetCurrentDirectory(sizeof(pszBuffer), pszBuffer);
		LookAtDir(pszBuffer);
	}

	return nRetCode;
}


