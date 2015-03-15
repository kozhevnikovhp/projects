// FtpTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "conio.h"
#include "Ftp.h"

int main(int argc, char* argv[])
{
	CFtpServer server;
	if (!server.Start())
	{
		printf("Cannot start FTP-server.\n");
		return 0;
	}

	while (1)
	{
		getch();
		printf("Press Ctrl+Break to stop server and close program\n");
	}
	server.Stop();
	return 0;
}
