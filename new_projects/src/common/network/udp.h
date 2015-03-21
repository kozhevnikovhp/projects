#ifndef __UDP_H_INCLUDED__
#define __UDP_H_INCLUDED__

#include "ip.h"

namespace common {

namespace network {

///////////////////////////////////////////////////////////////////////
// cUdpSocket - user datagram sockets classes will be derived from it

class cUdpSocket : public cUserSocket
{
// Constructors/destructors
public:
	cUdpSocket();
	cUdpSocket(IPPORT portNo);
	virtual ~cUdpSocket();
// Public methods
	bool SendEchoRequest(IPADDRESS_TYPE IpAddress, unsigned short nPacketSize = 30);
	bool SendEchoRequest(const char *pszIpAddress, unsigned short nPacketSize = 30);
	bool SendBroadcastEchoRequest(unsigned short nPacketSize = 30);

// Public overridables
	virtual bool bind(IPPORT portNo);
	virtual unsigned long GetMaxDatagramSize() { return 2048; }
// Public members

protected:
// Protected methods
	bool CreateAndInit();
// Protected overridables
// Protected members
	char *m_pDatagramBuffer;
};

} // namespace network

} // namespace common

#endif // __UDP_H_INCLUDED__
