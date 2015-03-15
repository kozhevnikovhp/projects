#ifndef __THREADED_SOCKETS_H_INCLUDED__
#define __THREADED_SOCKETS_H_INCLUDED__

#include "IpSockets.h"
#include "Udp.h"
#include "TcpSockets.h"
#include "IcmpSockets.h"
#include "Thread.h"
#include "Terminal.h"

class CThreaded
{
//Attributes
public:
	CThreaded();
	virtual ~CThreaded();
// Public methods
	inline IsAlive() { return m_bAlive; }
// Public overridables
// Public members

protected:
// Protected methods
	BOOL RunOwnThread(unsigned ( __stdcall *a_lpfnStartAddress)(void *), void *a_pArgList);
	BOOL KillOwnThread();
	BOOL NeedContinueThread();
// Protected overridables
// Protected members
	unsigned int m_ThreadID;
	HANDLE m_hContinueThreadEvent;	// signalled state - the thread is to be continued
	HANDLE m_hAbortThreadEvent;		// signalled state - the thread is to be aborted
	WSAOVERLAPPED m_Overlapped;		// structure for overlapped WSARecv, WSARecvFrom etc.
	BOOL m_bAlive;
private:
// Private methods
// Private overridables
// Private members
};

class CUdpThreadedSocket : public CUdpSocket, public CThreaded
{
//Attributes
public:
	CUdpThreadedSocket();
	virtual ~CUdpThreadedSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
	unsigned long WaitForDatagram(struct sockaddr_in *a_pSrcSocket);
	static unsigned (__stdcall ThreadProcedure(void *a_pParams));
// Protected overridables
	virtual void OnDatagramReceived(unsigned long IpAddress, int DatagramLength) {}

// Protected members
};


class CTcpThreadedSocket : public CTcpSocket, public CThreaded
{
//Attributes
public:
	CTcpThreadedSocket();
	virtual ~CTcpThreadedSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
	BOOL WaitForSymbol(unsigned char &symbol);
	static unsigned (__stdcall ThreadProcedure(void *a_pParams));
// Protected overridables
	virtual void OnSymbolReceived(char symbol) {}
	virtual void OnConnected();
	virtual void OnDisconnected();

// Protected members
};

class CIcmpThreadedSocket : public CIcmpSocket, public CThreaded
{
//Attributes
public:
	CIcmpThreadedSocket();
	virtual ~CIcmpThreadedSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
	static unsigned (__stdcall ThreadProcedure(void *a_pParams));
// Protected overridables

// Protected members
};

#endif //__THREADED_SOCKETS_H_INCLUDED__