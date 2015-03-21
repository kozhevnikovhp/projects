#include "stdafx.h"
#include "TcpThreadSocket.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Class CTcpThreadedSocket implementation

CTcpThreadSocket::CTcpThreadSocket()
{
//	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
///	m_Overlapped.hEvent = m_hAbortThreadEvent;
}

//virtual
CTcpThreadSocket::~CTcpThreadSocket()
{
	TRACE("CTcpThreadSocket object is destructed\n");
}

BOOL CTcpThreadSocket::WaitForSymbol(char &symbol, DWORD dwTimeToWait /*=INFINITE*/)
{
	unsigned int nBytesReceived = 0;
	return Read(&symbol, 1, nBytesReceived);
}

BOOL CTcpThreadSocket::WaitForData(char *pszData, unsigned int nBytesToRead, unsigned int &nReadBytes)
{
	return Read(pszData, nBytesToRead, nReadBytes);
}

