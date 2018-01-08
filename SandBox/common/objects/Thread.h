#ifndef __THREAD_H_INCLUDED__
#define __THREAD_H_INCLUDED__

const int PRIORITY_LOW		=1;
const int PRIORITY_NORMAL	=2;
const int PRIORITY_HIGH		=3;

#include <process.h>
#include "Portable.h"

unsigned int RunThread(unsigned ( __stdcall *a_lpfnStartAddress)(void *), void *a_pArgList, const int a_Priority, unsigned int *a_pThreadID);
unsigned int KillThread(unsigned int ThreadID);

#endif //__THREAD_H_INCLUDED__
