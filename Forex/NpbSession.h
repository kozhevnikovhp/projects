#ifndef __NPB_SESSION_H_INCLUDED__
#define __NPB_SESSION_H_INCLUDED__

#include "amfxapicxx.h"
#include <time.h>
#include "CurrencyPair.h"

extern char *PSZ_HISTORY_FILE_EXTENSION;
extern const char *PSZ_TICKERS[12];

class cNpbSession : public AMFXSessionCxxWrapper
{
public:
	cNpbSession();
	virtual ~cNpbSession();

public:

	bool needReconnect();
	bool needReconnect(const time_t &CurrentTime, const tm *pTM);
	bool isDataTooOld(const time_t &CurrentTime);
	bool isWorkingTime(const tm *pTM);

protected:
	CPortableCriticalSection m_CriticalSection;
	time_t m_LastQuoteTime;
	cCurrencyPairs m_Pairs;
	bool m_bWantsReconnect;
};

#endif __NPB_SESSION_H_INCLUDED__