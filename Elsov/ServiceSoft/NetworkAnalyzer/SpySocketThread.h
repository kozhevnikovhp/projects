/* SpySocketThread.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Mar 2006 - initial creation

*/

#ifndef __SPY_SOCKET_THREAD_H_INCLUDED__
#define __SPY_SOCKET_THREAD_H_INCLUDED__

#include "SnifferSockets.h"

//Forward declarations
class CAdapter;

class CSpySocketThread : public CSnifferThread
{
//Attributes
public:
	CSpySocketThread(CAdapter *pAdapter);
	virtual ~CSpySocketThread();

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
	virtual void OnThreadStarted();
	virtual void OnThreadEnded();

	// CSnifferSocket interface
	virtual LOGICAL OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	virtual void OnIgmpPacket(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	virtual void OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength);

// Protected members
protected:
	CAdapter *m_pAdapter;
};


#endif // __SPY_SOCKET_THREAD_H_INCLUDED__