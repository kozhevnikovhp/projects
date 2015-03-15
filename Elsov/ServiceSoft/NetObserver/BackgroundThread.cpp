/* BackgroundThread.cpp

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


#include "stdafx.h"
#include "BackgroundThread.h"
#include "Network.h"
#include "Adapter.h"
#include "Portable.h"


/////////////////////////////////////////////////////////////////////
// CBackgroundThread class implementation

//Constuctors/destructors
CBackgroundThread::CBackgroundThread(CAdapter *pAdapter)
{
	m_pAdapter = pAdapter;
}

//virtual
CBackgroundThread::~CBackgroundThread()
{
}

//virtual
void CBackgroundThread::OnThreadStarted()
{
}

void CBackgroundThread::SleepSomeTime(unsigned long timeout)
{
	unsigned long end_time = portableGetCurrentTimeMsec() + timeout;
	while (NeedContinueThread() && portableGetCurrentTimeMsec() < end_time)
	{
		portableSleep(100);
	}
}

//virtual
LOGICAL CBackgroundThread::ThreadBody()
{
	//if (NeedContinueThread())
	//	m_pAdapter->SendRipFullTableRequest();
		
	return LOGICAL_TRUE;
}

//virtual
void CBackgroundThread::OnThreadEnded()
{
}

/////////////////////////////////////////////////////////////////////
// CScheduleThread

CScheduleThread::CScheduleThread(CAdapter *pAdapter, CSchedule *pSchedule)
	: CBackgroundThread(pAdapter)
{
	m_pSchedule = pSchedule;
}

//virtual
CScheduleThread::~CScheduleThread()
{
}

//virtual
void CScheduleThread::OnThreadStarted()
{
}

//virtual
LOGICAL CScheduleThread::ThreadBody()
{
	while (m_pSchedule->ExecuteNextItem())
	{
		portableSwitchContext();
	}
	SleepSomeTime(1000);
	return LOGICAL_TRUE;
}

//virtual
void CScheduleThread::OnThreadEnded()
{
}


/////////////////////////////////////////////////////////////////////
// CDatabaseThread class implementation

//Constuctors/destructors
CDatabaseThread::CDatabaseThread(CAdapter *pAdapter)
	: CBackgroundThread(pAdapter)
{
}

//virtual
CDatabaseThread::~CDatabaseThread()
{
}

//virtual
void CDatabaseThread::OnThreadStarted()
{
}

//virtual
LOGICAL CDatabaseThread::ThreadBody()
{
	if (NeedContinueThread())
		m_pAdapter->UpdateTrafficDatabase();

	// Sleep 1 sec
	SleepSomeTime(1000);

	return LOGICAL_TRUE;
}

//virtual
void CDatabaseThread::OnThreadEnded()
{
}


/////////////////////////////////////////////////////////////////////
// CTracertThread class implementation

//Constuctors/destructors
CTracertThread::CTracertThread(CAdapter *pAdapter)
	: CBackgroundThread(pAdapter)
{
}

//virtual
CTracertThread::~CTracertThread()
{
}

//virtual
void CTracertThread::OnThreadStarted()
{
}

//virtual
LOGICAL CTracertThread::ThreadBody()
{
	if (NeedContinueThread())
		m_pAdapter->TracertAllHosts();

	// Sleep 1 sec
	SleepSomeTime(1000);

	return LOGICAL_TRUE;
}

//virtual
void CTracertThread::OnThreadEnded()
{
}


/////////////////////////////////////////////////////////////////////
// COrganizeThread class implementation

//Constuctors/destructors
COrganizeThread::COrganizeThread(CAdapter *pAdapter)
	: CBackgroundThread(pAdapter)
{
}

//virtual
COrganizeThread::~COrganizeThread()
{
}

//virtual
void COrganizeThread::OnThreadStarted()
{
}

//virtual
LOGICAL COrganizeThread::ThreadBody()
{
	if (NeedContinueThread())
		m_pAdapter->OrganizeHostsBehindRouters();

	// Sleep 1 sec
	SleepSomeTime(1000);

	return LOGICAL_TRUE;
}

//virtual
void COrganizeThread::OnThreadEnded()
{
}

