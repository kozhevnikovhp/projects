/* Terminal.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail ek538@mail.ru

Revision history:
	31 Jan 2003 - initial creation

*/

#ifndef __TERMINAL_H_INCLUDED__
#define __TERMINAL_H_INCLUDED__

#include "Portable.h"

class CTerminal
{
// Construction/destructiom
public:
	CTerminal();
	virtual ~CTerminal();
// Virtual interfaces
public:
	virtual LOGICAL Connect(const char *pszConnectionName) = 0;
	virtual LOGICAL Connect() { return Connect(GetConnectionName()); }
	virtual void Disconnect() = 0;
	virtual void ClearAllBuffers() {}
	virtual char *GetConnectionName() { return m_pszConnectionName; }
	virtual void SetConnectionName(const char *pszConnectionName) { strcpy(m_pszConnectionName, pszConnectionName); }
	virtual char *GetConnectionStatus() = 0;
	virtual LOGICAL Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes) = 0;
	virtual LOGICAL Write(void *pszBuffer, unsigned int BytesToWrite);
	virtual LOGICAL WriteString(void *pszBuffer) { return Write(pszBuffer, strlen((char *)pszBuffer)); }
	virtual LOGICAL Read(void *pszBuffer, unsigned int BytesToRead, unsigned int &nReadBytes) = 0;
	virtual LOGICAL Read(void *pszBuffer, unsigned int BytesToRead);
	virtual LOGICAL PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes) = 0;
	virtual LOGICAL SetReadTimeout(DWORD a_dwTimeout) = 0;
	virtual LOGICAL SetWriteTimeout(DWORD a_dwTimeout) = 0;

// Protected members
protected:
	char m_pszConnectionName[128];
	char m_pszConnectionStatus[128];
};

#endif //__TERMINAL_H_INCLUDED__