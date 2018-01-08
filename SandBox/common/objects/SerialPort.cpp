#include "stdafx.h"

#include <stdio.h>

#define WINDOWS 1
//#include "Defs.h"

#if defined(WINDOWS) // Windows-related header files
#include <windows.h>
#endif // End of Windows-related header files

#if defined(LINUX) // Linux-related header files
#include <fcntl.h>
#include <unistd.h>
#endif // End of Linux-related header files

#if defined(RTEMS) // RTEMS-related header files
#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#endif // End of RTEMS-related header files

#include "SerialPort.h"

CSerialPort::CSerialPort()
{
	m_WriteTimeout = 3000;
	strcpy(m_pszConnectionName, "????");
}

CSerialPort::~CSerialPort()
{
	Close();
}

LOGICAL CSerialPort::Open(int PortNumber)
{
	char pszName[32];

#if defined(WINDOWS) // Windows-related code
	sprintf(pszName, "Com%d", PortNumber);
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code
	sprintf(pszName, "/dev/ttyS%d", PortNumber-1); // first com port is cua0
#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code
	sprintf(pszName, "/dev/mc%d", PortNumber-1); // first com port is cua0
#endif // End of RTEMS-related code

	return Open(pszName);
}

LOGICAL CSerialPort::Open(const char *pszName)
{
	Close();

#if defined(WINDOWS) // Windows-related code
	char pszRealFileName[32];
	sprintf(pszRealFileName, "\\\\.\\%s", pszName);
	m_PortDesc = CreateFile(pszRealFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif // End of Windows-related code

#if defined(UNIX) // UNIX-related code
	m_PortDesc = open(pszName, O_RDWR, 0);
        ioctl(GetPortDesc(), FIONBIO, 1);
	if (IsOpened())
	    tcgetattr(GetPortDesc(), &m_InitialTermios);
#endif // End of UNIX-related code

	if (IsOpened())
	{
		NewPortOpened(this);
	}
	else
	{
		if (CanBeShared() && (m_pSharedPort = FindSharedPort(pszName)))
		{
			m_pSharedPort->Open();
		}
		else
		{
			ReportLastError(pszName, ErrorToScreen);
			return FALSE;
		}
	}

	SetName(pszName);
	m_LastErrorCode = 0;
	m_nSentBytes = m_nReceivedBytes = 0;
	Configure();

	return TRUE;
}

void CSerialPort::Close()
{
#if defined(UNIX) // UNIX-related code
// 	Restore initial termios
    if (IsOpened())
	tcsetattr(GetPortDesc(), TCSANOW, &m_InitialTermios);
#endif // End of UNIX-related code
    CPort::Close();
}

//virtual
LOGICAL CSerialPort::Connect(const char *pszConnectionName)
{
	strcpy(m_pszConnectionName, pszConnectionName);
	return Open(pszConnectionName);
}

//virtual
void CSerialPort::Disconnect()
{
	Close();
}

char *pszOpened = "opened";
char *pszNotOpened = "not opened";

//virtual
char *CSerialPort::GetConnectionStatus()
{
	char *pszStatus = pszNotOpened;
	if (IsOpened())
		pszStatus = pszOpened;
	sprintf(m_pszConnectionStatus, "Serial port %s is %s", GetConnectionName(), pszStatus);
	return m_pszConnectionStatus;
}

//virtual
LOGICAL CSerialPort::Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes)
{
	BOOL bSuccess;
	if (!IsOpened()) return FALSE;

#if defined(WINDOWS) // Windows-related code
	nWrittenBytes=0;
	bSuccess = WriteFile(GetPortDesc(), pszBuffer, BytesToWrite, (LPDWORD)&nWrittenBytes, NULL) &&
			(BytesToWrite == nWrittenBytes);
	if (!bSuccess)
	{
		DWORD dwErrorCode = GetLastError();
		dwErrorCode = dwErrorCode;
	}
#endif // End of Windows-related code

#if defined(LINUX)
	nWrittenBytes=write(GetPortDesc(), pszBuffer, BytesToWrite);
	bSuccess=(nWrittenBytes==BytesToWrite);
#endif // End of LINUX-related code

#if defined(RTEMS)
	int SentBytes=write(GetPortDesc(), pszBuffer, BytesToWrite);
	bSuccess=(SentBytes==(int)BytesToWrite);
#endif
	
	m_nSentBytes += nWrittenBytes;
	return bSuccess;
}

//virtual
LOGICAL CSerialPort::Read(void *pszBuffer, unsigned int BytesToRead, unsigned int &nReadBytes)
{
	BOOL bSuccess;
	if (!IsOpened()) return FALSE;

#if defined(WINDOWS) // Windows-related code
	nReadBytes = 0;
	bSuccess = ReadFile(GetPortDesc(), pszBuffer, BytesToRead, (unsigned long *)&nReadBytes,NULL) &&
			(BytesToRead == nReadBytes);
#endif // End of Windows-related code

#if defined(LINUX) // LINUX-related code

ssize_t ReadBytes=read(GetPortDesc(), pszBuffer, BytesToRead);
	bSuccess=(ReadBytes==BytesToRead);
#endif // End of UNIX-related code

#if defined(RTEMS) // RTEMS-related code
        int  ReadBytes = 0;
        int  nRead = 0;
        int  nWait = 0;
#if 0
        ReadBytes = read(GetPortDesc(), pszBuffer, BytesToRead);
#else
        while (1) {

  nRead =read(GetPortDesc(), pszBuffer + ReadBytes, BytesToRead - ReadBytes);
          ReadBytes += nRead;
          if (nRead == 0)
	  {
	    //4(1000) 16(250)
            if (nWait < 40)
	    {
              //sleep(1);
	      rtems_task_wake_after(MILLISECONDS_TO_TICKS(100));
              nWait++;
              continue;
            } else {
              break;
            }
          } else {
            nWait=0;
          }

          if (ReadBytes >= BytesToRead) {
            break;
          }
        }
#endif
	bSuccess=(ReadBytes==(int)BytesToRead);
#endif // End of UNIX-related code

	m_nReceivedBytes += nReadBytes;
	return bSuccess;
}

//virtual
LOGICAL CSerialPort::PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	unsigned int InBytes = GetInBufferBytes();
	if (0 == InBytes)
		return FALSE;
	if (InBytes > nBufferSize)
	{
		ClearReadBuffer(); // ???
		return FALSE;		
	}
	Read(pBufferToFill, InBytes, nReadBytes);
	return nReadBytes;
}

DWORD CSerialPort::GetInBufferBytes()
{
	if (!IsOpened()) return 0;

#if defined(WINDOWS) // Windows-related code
	DWORD dwBytes;
	ClearCommError(GetPortDesc(), &dwBytes, &m_StatInfo);
	return m_StatInfo.cbInQue;	
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code
	return 1;
// ???
#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code
	return 1; //???
#endif // End of RTEMS-related code
}

void CSerialPort::ClearReadBuffer()
{
#if defined(WINDOWS) // Windows-related code
	PurgeComm(GetPortDesc(), PURGE_RXCLEAR );
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code

#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code

#endif // End of RTEMS-related code
}

void CSerialPort::ClearSendBuffer()
{
#if defined(WINDOWS) // Windows-related code
	PurgeComm(GetPortDesc(), PURGE_TXCLEAR);
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code

#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code

#endif // End of RTEMS-related code
}

void CSerialPort::ClearAllBuffers()
{
#if defined(WINDOWS) // Windows-related code
	PurgeComm(GetPortDesc(), PURGE_TXCLEAR | PURGE_RXCLEAR);
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code

#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code

#endif // End of RTEMS-related code
}

LOGICAL CSerialPort::Configure()
{
#if defined(WINDOWS) // Windows-related code
	// Timeouts
	m_Timeouts.ReadIntervalTimeout = 0;//MAXDWORD;
	m_Timeouts.ReadTotalTimeoutMultiplier = 0;//MAXDWORD;
	m_Timeouts.ReadTotalTimeoutConstant = GetReadTimeout();
	m_Timeouts.WriteTotalTimeoutMultiplier = 0;
	m_Timeouts.WriteTotalTimeoutConstant = GetWriteTimeout();
	if(!SetCommTimeouts(GetPortDesc(), &m_Timeouts))					
	{
		ReportLastError(GetName(), ErrorToScreen);
		return LOGICAL_FALSE;
	}

	// Data control block
	memset(&m_DCB, 0, sizeof(DCB));
	m_DCB.DCBlength	= sizeof(DCB);
	if(!GetCommState(GetPortDesc(), &m_DCB))
	{
		ReportLastError(GetName(), ErrorToScreen);
		return LOGICAL_FALSE;
	}
	
	if (!BuildCommDCB(GetControlString(), &m_DCB))
	{
		ReportLastError(GetName(), ErrorToScreen);
		return LOGICAL_FALSE;
	}

	if(!SetCommState(GetPortDesc(), &m_DCB))
	{
		ReportLastError(GetName(), ErrorToScreen);
		return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code
	if (IsOpened())
	{
	    struct termios Termios;
	    memcpy(&Termios, &m_InitialTermios, sizeof(Termios));
	    Termios.c_cflag = CS8 | CREAD | HUPCL | CRTSCTS;
	    Termios.c_iflag = IGNBRK | IGNPAR | ISTRIP;
	    Termios.c_oflag = 0;
	    
	    int speed = B9600;
	    cfsetospeed(&Termios, speed);
	    cfsetispeed(&Termios, speed);
	    
	    tcsetattr(GetPortDesc(), TCSANOW, &Termios);
	    return TRUE;
	}
#endif // End of LINUX-related code


#if defined(RTEMS) // RTEMS-related code
	return LOGICAL_TRUE;
#endif // End of RTEMS-related code
}


LOGICAL CSerialPort::IsRLSD()
{
	if (!IsOpened())
		return LOGICAL_FALSE;

#if defined(WINDOWS) // Windows-related code
	DWORD status;
	GetCommModemStatus(GetPortDesc(), &status);
	return ((status & MS_RLSD_ON)!=0);
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code
	return LOGICAL_FALSE;
#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code

return LOGICAL_FALSE;
#endif // End of RTEMS-related code
}

/////////////////////////////////////////////////////////////////
// Misc

LOGICAL Comm_GuiUpdateConfigureString(char *pszPortName, char *pszPortConfigureString)
{
	COMMCONFIG config;
	memset(&config, 0, sizeof(config));
	config.dwSize = sizeof(config);
	config.wVersion = 1;
	config.dcb.DCBlength = sizeof(DCB);
	BOOL bSuccess = BuildCommDCB(pszPortConfigureString, &config.dcb);
	if (!bSuccess)
	{
		DWORD dwErrorCode = GetLastError();
	}

	bSuccess = CommConfigDialog(pszPortName, NULL, &config);
	if (!bSuccess)
	{
		DWORD dwErrorCode = GetLastError();
		if (dwErrorCode == ERROR_CANCELLED)
			return FALSE; // canceled => do nothing
	}
	
	Comm_DcbToConfigString(&config.dcb, pszPortConfigureString);
	return LOGICAL_TRUE;
}

void Comm_DcbToConfigString(DCB *pDcb, char *pszConfigString)
{
	char szDummy[32];
	// Baud rate
	sprintf(pszConfigString, "%d,", pDcb->BaudRate);
	// Parity
	switch (pDcb->Parity)
	{
	case 0: strcat(pszConfigString, "N"); break;
	case 1: strcat(pszConfigString, "O"); break;
	case 2: strcat(pszConfigString, "E"); break;
	case 3: strcat(pszConfigString, "M"); break;
	case 4: strcat(pszConfigString, "S"); break;
	default: strcat(pszConfigString, "N"); break;
	}
	strcat(pszConfigString, ",");
	// Byte size
	strcat(pszConfigString, itoa(pDcb->ByteSize, szDummy, 10));
	strcat(pszConfigString, ",");
	// Stop bits
	switch (pDcb->StopBits)
	{
	case 0: strcat(pszConfigString, "1"); break;
	case 1: strcat(pszConfigString, "1.5"); break;
	case 2: strcat(pszConfigString, "2"); break;
	default: strcat(pszConfigString, "1"); break;
	}
	// Flow control
	if (pDcb->fInX == 1 && pDcb->fOutX == 1)
		strcat(pszConfigString, ",x"); // Xon/Xoff
	else if ((pDcb->fOutxDsrFlow == 1 || pDcb->fOutxCtsFlow == 1) && (pDcb->fRtsControl != RTS_CONTROL_DISABLE) && (pDcb->fDtrControl != DTR_CONTROL_DISABLE))
		strcat(pszConfigString, ",p"); // hardware
}
