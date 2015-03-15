#include "stdafx.h"
#include "Thread.h"
#include "Scheduler.h"
#include "Station.h"
#include "ArmHubDoc.h"

CScheduler::CScheduler()
{
	m_pDoc = NULL;
	m_hThreadProcEnded = CreateEvent(NULL, TRUE, TRUE, NULL);
	InitializeCriticalSection(&m_Critical);
	m_bAlive = TRUE;
}

//virtual
CScheduler::~CScheduler()
{
	KillYourself();
	DeleteCriticalSection(&m_Critical);
	CloseHandle(m_hThreadProcEnded);
}

void CScheduler::KillYourself()
{
	EnterCritical();
	m_bAlive = FALSE;
	LeaveCritical();
	WaitForSingleObject(m_hThreadProcEnded, INFINITE);
}

void CScheduler::Run(void *a_pParams)
{
	unsigned int ThreadID;
	RunThread(SchedulerThreadProcedure, a_pParams, PRIORITY_NORMAL, &ThreadID);
}

void CScheduler::EnterCritical()
{
	EnterCriticalSection(&m_Critical);
}

void CScheduler::LeaveCritical()
{
	LeaveCriticalSection(&m_Critical);
}

BOOL CScheduler::IsAlive()
{
	EnterCritical();
	BOOL bAlive = m_bAlive;
	LeaveCritical();
	return bAlive;
}

//static
unsigned ( __stdcall CScheduler::SchedulerThreadProcedure) 
(
	void *a_pParams
)
{
	TRACE("CScheduler::SchedulerThreadProcedure started\n");

	CScheduler *pThis = (CScheduler *)a_pParams;
	CArmHubDoc *pDoc = pThis->GetDocument();
	ResetEvent(pThis->m_hThreadProcEnded);

	while (pThis->IsAlive())
	{
		pDoc->LockStations();
		for (CStation *pStation = pDoc->GetFirstStation(); pStation; pStation = pDoc->GetNextStation())
		{
			pStation->QueryTmiSchedule();
			pStation->SendPingSchedule();
		}
		pDoc->UnlockStations();
		Sleep(500);
	}

	SetEvent(pThis->m_hThreadProcEnded);
	TRACE("CScheduler::SchedulerThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}
