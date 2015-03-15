#ifndef __TCP_THREAD_SOCKET_H_INCLUDED__
#define __TCP_THREAD_SOCKET_H_INCLUDED__

#include "TcpSockets.h"
#include "ThreadObject.h"

class CTcpThreadSocket : public CTcpSocket, public CThreadObject
{
//Attributes
public:
	CTcpThreadSocket();
	virtual ~CTcpThreadSocket();
// Public methods

// Public overridables

// Public members

protected:
// Protected methods
	BOOL WaitForSymbol(char &symbol, DWORD dwTimeToWait =INFINITE);
	BOOL WaitForData(char *pszData, unsigned int nBytesToRead, unsigned int &nReadBytes);
// Protected overridables

// Protected members
};

#endif // __TCP_THREAD_SOCKET_H_INCLUDED__