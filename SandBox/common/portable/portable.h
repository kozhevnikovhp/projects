#ifndef PORTABLE_H
#define PORTABLE_H

namespace common {

namespace portable {

#ifdef OS_WINDOWS
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#ifndef strncasecmp
#define strncasecmp strnicmp
#endif
const char SUBDIRECTORIES_SEPARATOR = '\\';
#endif

#ifdef OS_UNIX
#define stricmp strcasecmp
#define strnicmp strncasecmp
const char SUBDIRECTORIES_SEPARATOR = '/';
#endif


///////////////////////////////////////////////////////////////////
// Misc

void portableSleep(unsigned int uSleepTime);
void portableSwitchContext();
unsigned int portableGetCurrentTimeMsec();
unsigned int portableGetCurrentTimeSec();

} // namespace portable

} // namespace common

#endif // PORTABLE_H
