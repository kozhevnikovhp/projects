// Datum2SdmsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "SerialPort.h"

class CDatumPort : public CSerialPort
{
public:
	virtual char *GetControlString() { return "9600,n,8,1"; }
	virtual unsigned int GetReadTimeout() { return 300; }		// read timeout
	virtual unsigned int GetWriteTimeout() { return 300; }	// write timeout
};

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Usage: %s.exe <serial_port_number> <timeout>\n", argv[0]);
		return 0;
	}

	int nSerialPort = atoi(argv[1]);
	int Timeout = atoi(argv[2]);

	CDatumPort port;
	if (!port.Open(nSerialPort))
	{
		printf("Cannot open COM%d\n", nSerialPort);
		return 0;
	}

	unsigned char bytes[] = { 0xA5, 0x42, 0x01, 0x00, 0x96, 0x82 };
	unsigned char reply[1024];
	int len = sizeof(bytes)/sizeof(bytes[0]);
	int n = 0;
	while (1)
	{
		unsigned int nWrittenBytes = 0, nReadBytes = 0;
		if (port.Write(bytes, len, nWrittenBytes))
		{
			port.Read(reply, sizeof(reply), nReadBytes);
			if (nReadBytes)
			{
				unsigned int IntValue = *((unsigned int *)(reply+15));
				printf("%d:\tValue = %d\n", ++n, IntValue);
			}
		}
		else
		{
			printf("Cannot write bytes to COM%d\n", nSerialPort);
		}
		Sleep(Timeout);
	}



	return 0;
}
