#ifndef __SNIFFER_SOCKETS_H_INCLUDED__
#define __SNIFFER_SOCKETS_H_INCLUDED__

#include "IpSockets.h"
#include "IcmpSockets.h"
#include "ThreadObject.h"

class CSnifferSocket : public CIpRawSocket
{
//Attributes
public:
	CSnifferSocket();
	virtual ~CSnifferSocket();
// Public methods
// Public overridables
// Public members
	LOGICAL Bind(unsigned long InterfaceIpAddress = INADDR_ANY);
	LOGICAL EnablePromiscMode();
	LOGICAL DisablePromiscMode();
	LOGICAL WaitForPacket();

// Protected methods
protected:
	LOGICAL Open();

// Protected overridables
protected:
	virtual LOGICAL OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength) { return FALSE; } // returns TRUE if packet processing ended and no need to process lower-protocol data (UDP, TCP, ICMP etc.)
	virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
	virtual void OnIgmpPacket(SIpHeader *pIpHeader, SIgmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
	virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
	virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}
	virtual void OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength) {}

// Protected members
protected:
	unsigned char m_szBufferForPackets[0xFFFF];
};

class CSnifferThread : public CThreadObject, public CSnifferSocket
{
//Attributes
public:
	CSnifferThread();
	virtual ~CSnifferThread();

// Public methods
public:

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

	// CThreadObject interface
	virtual LOGICAL ThreadBody();
	virtual void OnThreadStopping();

	// CSnifferSocket interface

// Protected members
protected:
};

#endif // __SNIFFER_SOCKETS_H_INCLUDED__