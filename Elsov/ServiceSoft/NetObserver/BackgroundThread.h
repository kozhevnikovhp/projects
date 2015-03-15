/* BackgroundThread.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	10 Aug 2006 - RIP full table requests
	14 Feb 2008 - splitting one background thread on three ones - keep-alive, subnet and database
	28 Mar 2008 - organize hosts behind routers thread
	09 Sep 2008 - scheduler thread
	10 Sep 2008 - got rid of keep-alive thread, its functionality moved to scheduler
	11 Sep 2008 - got rid of subnet thread, its functionality moved to scheduler
*/

#ifndef __BACKGROUND_THREAD_H_INCLUDED__
#define __BACKGROUND_THREAD_H_INCLUDED__

#include "ThreadObject.h"
#include "IcmpSockets.h"

//Forward declarations
class CAdapter;
class CSchedule;

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
	void SleepSomeTime(unsigned long timeout);

// Protected overridables
protected:

	// CThreadObject interface
	virtual void OnThreadStarted();
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();

// Protected members
protected:
	CAdapter *m_pAdapter;
};

class CScheduleThread : public CBackgroundThread
{
//Attributes
public:
	CScheduleThread(CAdapter *pAdapter, CSchedule *pSchedule);
	virtual ~CScheduleThread();

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
	CSchedule *m_pSchedule;
};


class CDatabaseThread : public CBackgroundThread
{
//Attributes
public:
	CDatabaseThread(CAdapter *pAdapter);
	virtual ~CDatabaseThread();

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
};


class CTracertThread : public CBackgroundThread
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
};

// Thread for organizing hosts connected throug the same router, by the subnets
class COrganizeThread : public CBackgroundThread
{
//Attributes
public:
	COrganizeThread(CAdapter *pAdapter);
	virtual ~COrganizeThread();

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
};


#endif // __BACKGROUND_THREAD_H_INCLUDED__