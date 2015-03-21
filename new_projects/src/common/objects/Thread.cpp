#include "stdafx.h"
#include "Thread.h"

// Method runs the new thread with specified entry point and arguments
unsigned int RunThread
(
	unsigned ( __stdcall *a_lpfnStartAddress)(void *), // entry point for thead routine
	void *a_pArgList,		// argument list
	const int a_Priority,	// thread priority
	unsigned int *a_pThreadID
)
{
	unsigned ThreadID=0;
	unsigned int ReturnValue=_beginthreadex
	(
		0,				// security attribute pointer
		0,					// stack size (0 - default value)
		a_lpfnStartAddress,	// thread entry point
		a_pArgList,			// argument list
		0,					// 0 - immediately run, may be also CREATE_SUSPENDED 
		a_pThreadID			// ThreadID
	);
	return ReturnValue;
}

// Method kills the thread
unsigned int KillThread
(
	unsigned int ThreadID
)
{
	unsigned int ReturnValue=TerminateThread
	(
		(HANDLE)ThreadID,	// ThreadID
		0					// Threadreturn code
	);
	return ReturnValue;
}

