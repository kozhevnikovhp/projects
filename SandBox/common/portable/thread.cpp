#include "thread.h"
#if OS == WINDOWS
#endif
#if OS == UNIX
#include <pthread.h>
#include <sys/types.h>
#endif

namespace common {

namespace portable {

int getCurrentThreadID()
{
#ifdef OS_WINDOWS
	return GetCurrentThreadId();
#endif
#ifdef OS_UNIX
	return pthread_self();
#endif
}

} // namespace portable

} // namespace common
