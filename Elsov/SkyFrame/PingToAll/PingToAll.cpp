// PingToAll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include "stdio.h"
#include "conio.h"

char *pszFmt[] = {
	"Ping 157.109.1.1 -w 5000", "Novomoskovsk",
	"Ping 157.109.2.1 -w 5000", "Arsenevo",
	"Ping 157.109.3.1 -w 5000", "Belev",
	"Ping 157.109.4.1 -w 5000", "Bogoroditsk",
	"Ping 157.109.5.1 -w 5000", "Volovo", 
	"Ping 157.109.6.1 -w 5000", "Dubna", 
	"Ping 157.109.7.1 -w 5000", "Efremov", 
	"Ping 157.109.8.1 -w 5000", "Kireevsk",
	"Ping 157.109.9.1 -w 5000", "Kurkino", 
	"Ping 157.109.10.1 -w 5000", "Odoev",
	"Ping 157.109.11.1 -w 5000", "Suvorov",
	"Ping 157.109.12.1 -w 5000", "Chern",
	"Ping 157.109.13.1 -w 5000", "Yasnogorsk",
	"Ping 157.109.14.1 -w 5000", "Himki"
};


int main(int argc, char* argv[])
{
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb=sizeof(STARTUPINFO);
	si.lpReserved=NULL;
	si.lpReserved2=NULL;
	si.cbReserved2=0;
	si.lpDesktop=NULL;
	si.dwX=si.dwY=si.dwXSize=si.dwYSize=0;
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_SHOWNORMAL;
	int nCycles = 1;
	if (argc >1)
		nCycles = atoi(argv[1]);
	for (int i = 0; i<nCycles; i++)
	{
		for (int j = 0; j<sizeof(pszFmt)/sizeof(pszFmt[0]); j+=2)
		{
			printf("%s %s\n", pszFmt[j], pszFmt[j+1]);
			si.lpTitle = pszFmt[j+1];
			BOOL bCreated = CreateProcess(NULL, pszFmt[j],
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
				NULL,
				NULL,
				&si,
				&pi
				);
			if (!bCreated)
			{
				DWORD dwErrorCode = GetLastError();
				printf("\t-cannot execute due to error code = %d\n", dwErrorCode);
			}
		}
		printf("\n");
	}
	getch();
	return 0;
}
