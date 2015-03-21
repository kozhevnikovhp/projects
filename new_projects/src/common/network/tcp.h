#ifndef __TCP_SOCKETS_H_INCLUDED__
#define __TCP_SOCKETS_H_INCLUDED__

#include "ip.h"

namespace common {

namespace network {

// cTcpSocket - stream sockets classes will be derived from it

class cTcpSocket : public cUserSocket
{
    typedef cUserSocket inherited;
    
// Constructors/destructors
public:
	cTcpSocket();
	virtual ~cTcpSocket();

// Public methods
	bool connect(IPADDRESS_TYPE IpAddress);
	bool connect(const char *pszIpAddress);
	bool connect(IPADDRESS_TYPE IpAddress, IPPORT portNo);
	bool connect(const char *pszIpAddress, IPPORT portNo);
	bool isConnected() { return m_bConnected; }
	char *getConnectionName() { return m_pszConnectionName; }
	bool open();
	void disconnect();

// Public overridables

	virtual void onBeforeClosing();
	virtual char *getConnectionStatus();
	virtual cTcpSocket *createAcceptSocket() { return new cTcpSocket(); } // make new socket of the same type

	// Common socket methods
	bool listen(unsigned short uPort = 0, IPADDRESS_TYPE InterfaceIpAddress = INADDR_ANY);
	cTcpSocket *accept();
	bool getSockName(IPADDRESS_TYPE &IpAddress, unsigned short &PortNo);
	bool getSockName(char *pszIpAddress, unsigned short &PortNo);
	bool getPeerName(IPADDRESS_TYPE &IpAddress, unsigned short &PortNo);
	bool getPeerName(char *pszIpAddress, unsigned short &PortNo);
	bool Read(void *pBufferToFill, size_t nBufferSize, int &nReadBytes);
	bool Write(const void *pBufferToSend, size_t nBytesToSend, int &nSentBytes);
	bool PeekInputSymbols(void *pBufferToFill, size_t nBufferSize, int &nReadBytes);
	void clearAllBuffers();
	bool enableKeepAlive(bool bEnable);
	bool isKeepAliveEnabled(bool &bEnabled);
	bool setKeepAliveTime(int t);
	bool getKeepAliveTime(int &t);

// Public members

protected:
// Protected methods

// Protected overridables
	virtual void onConnected() {}
	virtual void onDisconnected() {}

// Protected members
	bool m_bConnected;
	char m_pszConnectionName[512];
	char m_pszConnectionStatus[512];
};

} // namespace network

} // namespace common

#endif //__TCP_SOCKETS_H_INCLUDED__
