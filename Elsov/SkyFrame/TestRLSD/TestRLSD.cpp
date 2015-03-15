// TestRLSD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>


int main(int argc, char* argv[])
{
	HANDLE hFile=CreateFile("COM1:", GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Port not opened!");
		return 0;
	}
	DWORD status=0, old_status=0;
	while(1)
	{
		GetCommModemStatus(hFile, &status);
		if (status!=old_status)
		{
			printf("Status=%d\n", status);
		}
		old_status=status;
	}
	return 0;
}
