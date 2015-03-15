#ifndef __UDP_BASED_TERMINAL_H_INCLUDED__
#define __UDP_BASED_TERMINAL_H_INCLUDED__

#include "Terminal.h"
#include "Udp.h"

class CUdpBasedTerminal : public CTerminal, public CUdpSocket
{
//Construction/destruction
public:
	CUdpBasedTerminal();
	virtual ~CUdpBasedTerminal();

// Public methods
// Public overridables

	// CTerminal* interface
	virtual LOGICAL Connect(const char *pszConnectionName);
	virtual void Disconnect() {} // stub (UDP - connectionless protocol)
	virtual char *GetConnectionStatus() { return ""; }
	virtual LOGICAL Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes);
	virtual LOGICAL Read(void *pszBuffer, unsigned int BytesToRead, unsigned int &nReadBytes);
	virtual LOGICAL PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes);
	virtual LOGICAL SetReadTimeout(DWORD a_dwTimeout);
	virtual LOGICAL SetWriteTimeout(DWORD a_dwTimeout);

// Protected members
	unsigned long m_IpAddress;
	unsigned int m_CurrentReadPos;
	unsigned int m_LastDatagramSize;
};

#endif // __UDP_BASED_TERMINAL_H_INCLUDED__