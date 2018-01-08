#ifndef __SERIAL_PORT_H_INCLUDED__
#define __SERIAL_PORT_H_INCLUDED__

#define WINDOWS 1

#if defined(WINDOWS) // Windows-related header files
#include <windows.h>
#endif // End of Windows-related header files

#if defined (UNIX)
#include <termios.h>
#endif // End of UNIX-related header files

#include "Terminal.h"
#include "Port.h"

class CSerialPort : public CTerminal, public CPort
{
#if defined(WINDOWS) // Windows-related configure parameters
	DCB m_DCB;
	COMMTIMEOUTS m_Timeouts;
	COMSTAT m_StatInfo;
#endif // End of Windows-related configure parameters

#if defined(UNIX) // UNIX-related configure parameters
	struct termios m_InitialTermios;
#endif // End of UNIX-related configure parameters
	unsigned int m_WriteTimeout;

protected:
	LOGICAL Configure();
public:
	CSerialPort();
	virtual ~CSerialPort();
	virtual void Close();
	LOGICAL Open(const char *pszName);
	LOGICAL Open(int PortNumber);
	void ClearReadBuffer();
	void ClearSendBuffer();
	void ClearAllBuffers();
	virtual LOGICAL Connect(const char *pszConnectionName);
	virtual void Disconnect();
	virtual char *GetConnectionStatus();
	virtual LOGICAL Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes);
	virtual LOGICAL Read(void *pszBuffer, unsigned int BytesToRead, unsigned int &nReadBytes);
	virtual LOGICAL PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes);
	virtual LOGICAL SetReadTimeout(DWORD a_dwTimeout) { return LOGICAL_TRUE; }
	virtual LOGICAL SetWriteTimeout(DWORD a_dwTimeout) { return LOGICAL_TRUE; }
	DWORD GetInBufferBytes();
	LOGICAL IsRLSD();

	virtual char *GetControlString() = 0;			// like "96,n,8,1,p" - see BuildCommDCB method description
	virtual unsigned int GetReadTimeout() = 0;		// read timeout
	virtual unsigned int GetWriteTimeout() = 0;		// write timeout
};

// Misc
LOGICAL Comm_GuiUpdateConfigureString(char *pszPortName, char *pszPortConfigureString);
void Comm_DcbToConfigString(DCB *pDcb, char *pszConfigString);

#endif //__SERIAL_PORT_H_INCLUDED__