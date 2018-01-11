#include "portable.h"
#include <stdlib.h>
#ifdef OS_WINDOWS
#include <windows.h>
#endif
#ifdef OS_UNIX
#include <unistd.h>
#include <sys/time.h>
#endif


namespace common {

namespace portable {


///////////////////////////////////////////////////////////////////
// Misc

void portableSleep(unsigned int uSleepTime)
{
#ifdef OS_WINDOWS
	Sleep(uSleepTime);
#endif
#ifdef OS_UNIX
	usleep(uSleepTime*1000);
#endif
}

void portableSwitchContext()
{
#ifdef OS_WINDOWS
	Sleep(0);
#endif
#ifdef OS_UNIX
	usleep(0); //??
#endif
}

unsigned int portableGetCurrentTimeMsec()
{
#ifdef OS_WINDOWS
    return GetTickCount();
#endif
#ifdef OS_UNIX
    timeval t;
    gettimeofday(&t, NULL);
    return 1000*t.tv_sec + t.tv_usec/1000.;
#endif
}

unsigned int portableGetCurrentTimeSec()
{
#ifdef OS_WINDOWS
    return GetTickCount()/1000;
#endif
#ifdef OS_UNIX
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec;
#endif
}

} // namespace network

} // namespace portable
