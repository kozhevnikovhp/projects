/* BackgroundThread.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	10 Aug 2006 - RIP full table requests

*/

#ifndef __BACKGROUND_THREAD_H_INCLUDED__
#define __BACKGROUND_THREAD_H_INCLUDED__

#include "ThreadObject.h"
#include "IcmpSockets.h"

//Forward declarations
class CAdapter;

class CBackgroundThread : public CThreadObject
{
//Attributes
public:
	CBackgroundThread(CAdapter *pAdapter);
	virtual ~CBackgroundThread();

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
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();

// Protected members
protected:
	CAdapter *m_pAdapter;
	SOctetConverter m_CurrentSubnet, m_CurrentKeepAliveHost, m_CurrentTrafficStoringHost;
};


class CTracertThread : public CThreadObject
{
//Attributes
public:
	CTracertThread(CAdapter *pAdapter);
	virtual ~CTracertThread();

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
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();

// Protected members
protected:
	CAdapter *m_pAdapter;

	union
	{
		struct { unsigned char b1,b2,b3,b4; } bytes;
		unsigned long addr;
	} m_CurrentHost;
};


#endif // __BACKGROUND_THREAD_H_INCLUDED__