#ifndef __MISC_H_INCLUDED__
#define __MISC_H_INCLUDED__

#include "time.h"

void TimeString(time_t *pTime, char *pszString, size_t max_len);
bool IsFridayNight(const tm *pTM);
bool IsFridayNight(const double t);
bool IsFridayEvening(const tm *pTM);
bool IsFridayEvening(const double t);
bool IsSaturday(const tm *pTM);
bool IsSaturday(const double t);
bool IsSundayNight(const tm *pTM);
bool IsSundayNight(const double t);
bool IsSunday(const tm *pTM);
bool IsSunday(const double t);
bool IsEvening(const tm *pTM);
bool IsEvening(const double t);
tm *getGMT(double t);


#endif //__MISC_H_INCLUDED__