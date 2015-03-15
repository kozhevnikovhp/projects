#ifndef __TCP_SOCKETS_H_INCLUDED__
#define __TCP_SOCKETS_H_INCLUDED__

#include "IpSockets.h"
#include "Terminal.h"

// CTcpSocket - stream sockets classes will be derived from it
class CTcpSocket : public CTerminal, public CUserSocket
{
//Attributes
public:
	CTcpSocket();
	virtual ~CTcpSocket();
// Public methods
	LOGICAL Connect(unsigned long IpAddress);
	LOGICAL Connect(unsigned long IpAddress, unsigned short PortNo);
	inline BOOL IsConnected() { return m_bConnected; }
// Public overridables
	// CTerminal* interface
	virtual LOGICAL Connect(const char *pszIpAddress, unsigned short PortNo);
	virtual LOGICAL Connect(const char *pszIpAddress);
	virtual LOGICAL Open();
	virtual void Close();
	virtual char *GetConnectionStatus();
	virtual void Disconnect();
	virtual void ClearAllBuffers();
	virtual LOGICAL Read(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes);
	virtual LOGICAL Write(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes);
	virtual LOGICAL PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes);
	virtual LOGICAL SetReadTimeout(DWORD a_dwTimeout) { return CIpSocket::SetReadTimeout(a_dwTimeout); }
	virtual LOGICAL SetWriteTimeout(DWORD a_dwTimeout) { return CIpSocket::SetWriteTimeout(a_dwTimeout); }

	// Common socket methods
	LOGICAL Listen(unsigned short uPort = 0, unsigned long InterfaceIpAddress = INADDR_ANY);
	CTcpSocket *Accept();
	virtual CTcpSocket *CreateAcceptSocket() { return new CTcpSocket(); } // make new socket of the same type
	LOGICAL GetSockName(unsigned long &IpAddress, unsigned short &PortNo);
	LOGICAL GetSockName(char *pszIpAddress, unsigned short &PortNo);
// Public members

protected:
// Protected methods
	void Shutdown();
// Protected overridables
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}
// Protected members
	LOGICAL m_bConnected;
};

#endif //__TCP_SOCKETS_H_INCLUDED__
