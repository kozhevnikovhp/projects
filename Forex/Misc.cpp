#include "Misc.h"
#include "Portable.h"


void TimeString(time_t *pTime, char *pszString, size_t max_len)
{
	CPortableLocale locale;
	tm *pTM = gmtime(pTime);
	strftime(pszString, max_len, locale.GetShortDateAndTimeFormatStr(), pTM);
}

bool IsFridayNight(const tm *pTM)
{
	return (pTM->tm_wday == 5 && pTM->tm_hour >= 22);
}

bool IsFridayNight(const double t)
{
	return IsFridayNight(getGMT(t));
}

bool IsFridayEvening(const tm *pTM)
{
	return (pTM->tm_wday == 5 && IsEvening(pTM));
}

bool IsFridayEvening(const double t)
{
	return IsFridayEvening(getGMT(t));
}

bool IsSaturday(const tm *pTM)
{
	return (pTM->tm_wday == 6);
}

bool IsSaturday(const double t)
{
	return IsSaturday(getGMT(t));
}

bool IsSundayNight(const tm *pTM)
{
	return (pTM->tm_wday == 0 && pTM->tm_hour < 23);
}

bool IsSundayNight(const double t)
{
	return IsSundayNight(getGMT(t));
}

bool IsSunday(const tm *pTM)
{
	return (pTM->tm_wday == 0);
}

bool IsSunday(const double t)
{
	return IsSunday(getGMT(t));
}

bool IsEvening(const tm *pTM)
{
	return (pTM->tm_hour >= 21);
}

bool IsEvening(const double t)
{
	return IsEvening(getGMT(t));
}

tm *getGMT(double t)
{
	time_t ttt = (time_t)t;
	return gmtime(&ttt);
}
