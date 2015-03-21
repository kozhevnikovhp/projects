#include "NpbSession.h"
#include "Misc.h"

char *PSZ_HISTORY_FILE_EXTENSION = "his";

const char *PSZ_TICKERS[12] = { 
	"AUDUSD",
	"EURCAD",
	"EURCHF",
	"EURGBP",
	"EURJPY",
	"EURUSD",
	"GBPJPY",
	"GBPUSD",
	"NZDUSD",
	"USDCHF",
	"USDJPY",
	"USDCAD"
};


cNpbSession::cNpbSession()
{
	///time(&m_LastQuoteTime);
	m_LastQuoteTime = 0;
	m_bWantsReconnect = true;
}

//virtual
cNpbSession::~cNpbSession()
{
}

bool cNpbSession::needReconnect()
{
	time_t CurrentTime;
	time(&CurrentTime);
	tm *pTM = gmtime(&CurrentTime);
	return needReconnect(CurrentTime, pTM);
}

bool cNpbSession::needReconnect(const time_t &CurrentTime, const tm *pTM)
{
	return (isDataTooOld(CurrentTime) && isWorkingTime(pTM));
}

bool cNpbSession::isDataTooOld(const time_t &CurrentTime)
{
	return (CurrentTime - m_LastQuoteTime > 60);
}

bool cNpbSession::isWorkingTime(const tm *pTM)
{
	if (IsFridayNight(pTM))
		return false; // Friday > 22-00 GMT is holiday already
	if (IsSaturday(pTM))
		return false; // Whole Saturday is holiday
	if (IsSundayNight(pTM))
		return false; // Sunday, before 23-00 GMT is still holiday

	return true;
}

