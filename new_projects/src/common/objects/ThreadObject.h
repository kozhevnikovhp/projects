/* ThreadObject.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	26 Feb 2003 - initial creation

*/

#ifndef __THREAD_OBJECT_H_INCLUDED__
#define __THREAD_OBJECT_H_INCLUDED__

#include "Thread.h"

class CThreadObject
{
// Constructors/destructors
public:
	CThreadObject();
	virtual ~CThreadObject();

// Public methods
public:
	LOGICAL Run();
	LOGICAL Stop();
//	inline IsAlive() { return NeedContinueThread(); }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	LOGICAL NeedContinueThread();
	static unsigned ( __stdcall ThreadProcedure) (void *a_pParams);

// Protected overridables
protected:
	virtual void OnThreadStarted() {}
	virtual LOGICAL ThreadBody() { return LOGICAL_TRUE; } // returns TRUE/FALSE if further execution is needed
	virtual void OnThreadStopping() {}
	virtual void OnThreadEnded() {}

// Protected members
protected:
	unsigned int m_ThreadID;
	CPortableCriticalSection m_LoopCriticalSection, m_ThreadCriticalSection;
	LOGICAL m_bNeedToContinue;

// Private methods
private:

// Private overridables
private:

// Private members
private:
};

#endif // __THREAD_OBJECT_H_INCLUDED__