#ifndef __UDP_THREAD_SOCKET_H_INCLUDED__
#define __UDP_THREAD_SOCKET_H_INCLUDED__

#include "Udp.h"
#include "ThreadObject.h"

class CUdpThreadSocket : public CUdpSocket, public CThreadObject
{
//Attributes
public:
	CUdpThreadSocket();
	virtual ~CUdpThreadSocket();
// Public methods

// Public overridables

// Public members

protected:
// Protected methods
// Protected overridables

// Protected members
	WSAOVERLAPPED m_Overlapped;		// structure for overlapped WSARecv, WSARecvFrom etc.
};

#endif // __UDP_THREAD_SOCKET_H_INCLUDED__