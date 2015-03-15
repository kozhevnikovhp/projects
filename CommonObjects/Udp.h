#ifndef __UDP_H_INCLUDED__
#define __UDP_H_INCLUDED__

#include "IpSockets.h"

///////////////////////////////////////////////////////////////////////
// CUdpSocket - user datagram sockets classes will be derived from it

class CUdpSocket : public CUserSocket
{
// Constructors/destructors
public:
	CUdpSocket();
	CUdpSocket(unsigned short PortNo);
	virtual ~CUdpSocket();
// Public methods
	BOOL SendEchoRequest(unsigned long IpAddress, unsigned short nPacketSize = 30);
	BOOL SendEchoRequest(const char *pszIpAddress, unsigned short nPacketSize = 30);
	BOOL SendBroadcastEchoRequest(unsigned short nPacketSize = 30);

// Public overridables
	virtual unsigned long GetMaxDatagramSize() { return 2048; }
// Public members

protected:
// Protected methods
	BOOL CreateAndInit();
// Protected overridables
// Protected members
	char *m_pDatagramBuffer;
};

#endif // __UDP_H_INCLUDED__