#ifndef PORTABLE_H
#define PORTABLE_H

namespace common {

namespace portable {

#ifdef OS_WINDOWS
#define strcasecmp stricmp
#define strncasecmp strnicmp
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

} // namespace portable

} // namespace common

#endif // PORTABLE_H
