#ifndef __SCHEDULER_H_INCLUDED__
#define __SCHEDULER_H_INCLUDED__

class CArmHubDoc;

class CScheduler
{
public:
	CScheduler();
	virtual ~CScheduler();
	inline void SetDocument(CArmHubDoc *pDoc) { m_pDoc = pDoc; }
	inline CArmHubDoc *GetDocument() { return m_pDoc; }
	void Run(void *a_pParams);
	void KillYourself();
	BOOL IsAlive();
protected:
	static unsigned (__stdcall SchedulerThreadProcedure(void *a_pParams));
	void EnterCritical();
	void LeaveCritical();
	CRITICAL_SECTION m_Critical;
	HANDLE m_hThreadProcEnded;
	BOOL m_bAlive;
	CArmHubDoc *m_pDoc;
};

#endif //__SCHEDULER_H_INCLUDED__