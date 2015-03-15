/* ThreadObject.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	26 Feb 2003 - initial creation

*/

#include "stdafx.h"
#include "ThreadObject.h"

///////////////////////////////////////////////////////////////////
// Class CThreadObject implementation

CThreadObject::CThreadObject()
{
	m_ThreadID = 0;
	m_bNeedToContinue = LOGICAL_FALSE;
}

//virtual
CThreadObject:: ~CThreadObject()
{
	Stop();
}

LOGICAL CThreadObject::Run()
{
	if (NeedContinueThread())
	{
		//TRACE("Thread is running already\n");
		return LOGICAL_FALSE;
	}
	m_bNeedToContinue = LOGICAL_TRUE;
	return RunThread(ThreadProcedure, this, PRIORITY_NORMAL, &m_ThreadID);
}

LOGICAL CThreadObject::Stop()
{
	m_LoopCriticalSection.Enter();
	OnThreadStopping();
	m_bNeedToContinue = LOGICAL_FALSE;
	m_LoopCriticalSection.Leave();

	m_ThreadCriticalSection.Enter();
	m_ThreadCriticalSection.Leave();
	return LOGICAL_TRUE;
}

LOGICAL CThreadObject::NeedContinueThread()
{
	return m_bNeedToContinue;
}

//static
unsigned ( __stdcall CThreadObject::ThreadProcedure) 
(
	void *a_pParams
)
{
	CThreadObject *pThis = (CThreadObject *)a_pParams;
	LOGICAL bNeedAnotherLoop = LOGICAL_TRUE;
	pThis->m_ThreadCriticalSection.Enter();

	pThis->OnThreadStarted();

	
	while (bNeedAnotherLoop)
	{
		pThis->m_LoopCriticalSection.Enter();
		if (pThis->NeedContinueThread())
			bNeedAnotherLoop = pThis->ThreadBody();
		else
			bNeedAnotherLoop = LOGICAL_FALSE;
		pThis->m_LoopCriticalSection.Leave();
	}

	pThis->OnThreadEnded();
	pThis->m_bNeedToContinue = LOGICAL_FALSE;

	pThis->m_ThreadCriticalSection.Leave();

	_endthreadex(0);
	return 0;
}


