#include "NpbSession.h"
#include "CurrencyPair.h"
#include "Misc.h"
#include "Calculations.h"
#include <time.h>
#include <math.h>
#include <algorithm>

char *PSZ_PARAMS_FILE_NAME = "params.par";
char *PSZ_GROUPS_FILE_NAME = "groups.par";
char *PSZ_PROFIT_FILE_NAME = "profit.txt";
char *PSZ_SUCCESS_STORY_FILE_NAME = "SuccessStory.txt";

const double eps = 1.e-06;
const char *PSZ_BOOL_PARAM_TEMPLATE = "%s = %d\n";
const char *PSZ_REAL_PARAM_TEMPLATE = "%s = %lf\n";
const char *PSZ_INT_PARAM_TEMPLATE = "%s = %d\n";
const char *PSZ_ALL_PARAM_DESCRIPTION_TEMPLATE = "GROUP%d_PARAM%d = %lf %lf %lf %lf %d\n";
const char *PSZ_TRADING_ENABLED = "TRADING_ENABLED";
const char *PSZ_TRADING_AMOUNT = "TRADING_AMOUNT";
const char *PSZ_BROKER_INTEREST = "BROKER_INTEREST";
const char *PSZ_IMPROVEMENT_ENABLED = "IMPROVEMENT_ENABLED";

const char *PSZ_DAY_OF_WEEK[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char *PSZ_PARAM_DESC[N_PARAMS_IN_GROUP] =
{
	"FALL_TIME_MAX",
	"FALL_TIME_MIN",
	"FALL_DELTA_MIN",
	"FALL_DELTA_MAX",
	"FALL_STOPLOSS",
	"FALL_ABOVE_MIN",
	"FALL_BOUNCE_TAKE_PROFIT",
	"FALL_TAKE_PROFIT",
	"FALL_SPRING",
	"FALL_DEAD_ZONE",
	"FALL_MAX_DURATION",

	"RAISE_TIME_MAX",
	"RAISE_TIME_MIN",
	"RAISE_DELTA_MIN",
	"RAISE_DELTA_MAX",
	"RAISE_STOPLOSS",
	"RAISE_BELOW_MAX",
	"RAISE_BOUNCE_TAKE_PROFIT",
	"RAISE_TAKE_PROFIT",
	"RAISE_SPRING",
	"RAISE_DEAD_ZONE",
	"RAISE_MAX_DURATION"
};

const double MAX_OPEN_DURATION = 2*24*3600; // 2 days

///////////////////////////////////////////////////////////////////////
// cGroup class

cGroup::cGroup()
{
}

void cGroup::Init()
{
	memset(m_Params, 0, sizeof(m_Params));
	for (int i = 0; i < N_PARAMS_IN_GROUP; i++)
	{
		m_Params[i].m_bValid = LOGICAL_FALSE;
	}
}

bool cGroup::operator != (const cGroup &group)
{
	for (int i = 0; i < N_PARAMS_IN_GROUP; i++)
	{
		if (fabs(m_Params[i].m_fValue - group.m_Params[i].m_fValue) >= eps)
			return true;
	}
	return false;
}

void cGroup::SetParam(const int iParam, const double fMin, const double fMax, const double fValue, const double fStep, int iShift)
{
	if (iParam < 0 || iParam >= N_PARAMS_IN_GROUP)
	{
		printf("Invalid parameter Param%d\n", iParam);
		return;
	}
	m_Params[iParam].m_fMinValue = fMin;
	m_Params[iParam].m_fMaxValue = fMax;
	m_Params[iParam].m_fValue = fValue;
	m_Params[iParam].m_fStep = fStep;
	m_Params[iParam].m_iShift = iShift;
	m_Params[iParam].m_bValid = LOGICAL_TRUE;
}

void cGroup::GetParam(const int iParam, double &fMin, double &fMax, double &fValue, double &fStep)
{
	if (iParam < 0 || iParam >= N_PARAMS_IN_GROUP)
	{
		printf("Invalid parameter Param%d\n", iParam);
		return;
	}
	fMin = m_Params[iParam].m_fMinValue;
	fMax = m_Params[iParam].m_fMaxValue;
	fValue = m_Params[iParam].m_fValue;
	fStep = m_Params[iParam].m_fStep;
}

void cGroup::Validate()
{
	for (int i = 0; i < N_PARAMS_IN_GROUP; i++)
	{
		m_Params[i].Validate();
	}

	const double MIN_TIME_DIFF = 10;
	const double MIN_DIFF_RATIO = 1.1;
	if (FALL_TIME_MIN <= 1)
		FALL_TIME_MIN = 1;
	if (FALL_TIME_MAX < (FALL_TIME_MIN+MIN_TIME_DIFF))
		FALL_TIME_MAX = FALL_TIME_MIN+MIN_TIME_DIFF;

	if (FALL_DELTA_MAX < MIN_DIFF_RATIO*FALL_DELTA_MIN)
		FALL_DELTA_MAX = MIN_DIFF_RATIO*FALL_DELTA_MIN;

	if (RAISE_TIME_MIN <= 1)
		RAISE_TIME_MIN = 1;
	if (RAISE_TIME_MAX < (RAISE_TIME_MIN+MIN_TIME_DIFF))
		RAISE_TIME_MAX = RAISE_TIME_MIN+MIN_TIME_DIFF;

	if (RAISE_DELTA_MAX < MIN_DIFF_RATIO*RAISE_DELTA_MIN)
		RAISE_DELTA_MAX = MIN_DIFF_RATIO*RAISE_DELTA_MIN;
}

void cGroup::Validate(const cGroup &group)
{
	/*if (FALL_TIME_MIN < group.FALL_TIME_MAX)
		FALL_TIME_MIN = group.FALL_TIME_MAX;		
	if (RAISE_TIME_MIN < group.RAISE_TIME_MAX)
		RAISE_TIME_MIN = group.RAISE_TIME_MAX;*/
}

bool cGroup::IsFastFall(const SQuote *pThisQuote, const SQuote *pThatQuote) const
{
	double dTime = pThisQuote->m_Time - pThatQuote->m_Time;
	double dPrice = pThisQuote->m_fBid - pThatQuote->m_fBid;
	if (dPrice < 0 && FALL_TIME_MIN < dTime && dTime < FALL_TIME_MAX)
	{
		if ((FALL_DELTA_MIN-eps) <= fabs(dPrice) && fabs(dPrice) <= (FALL_DELTA_MAX+eps))
			return true;
	}
	return false;
}

bool cGroup::IsFastRaise(const SQuote *pThisQuote, const SQuote *pThatQuote) const
{
	double dTime = pThisQuote->m_Time - pThatQuote->m_Time;
	double dPrice = pThisQuote->m_fBid - pThatQuote->m_fBid;
	if (dPrice > 0 && RAISE_TIME_MIN < dTime && dTime < RAISE_TIME_MAX)
	{
		if ((RAISE_DELTA_MIN-eps) <= fabs(dPrice) && fabs(dPrice) <= (RAISE_DELTA_MAX+eps))
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////
// cParams class

cParams::cParams()
{
}

bool cParams::operator != (const cParams &params)
{
	for (int i = 0; i < N_GROUPS; i++)
	{
		if (m_Groups[i] != params.m_Groups[i])
			return true;
	}
	return false;
}

void cParams::Init()
{
	for (int i = 0; i < N_GROUPS; i++)
	{
		m_Groups[i].Init();
	}
}

void cParams::Validate()
{
	m_Groups[0].Validate();
	for (int i = 1; i < N_GROUPS; i++)
	{
		m_Groups[i].Validate(m_Groups[i-1]);
		m_Groups[i].Validate();
	}
}

///////////////////////////////////////////////////////////////////////
// cHistory class

cHistory::cHistory()
{

}

void cHistory::NewQuote(const double timestamp, const double fBid)
{
	if (fBid < 0.001)
		return; // do nothing, error like 0.0000 happened
	
	SQuote quote(timestamp, fBid);
	m_Quotes.push_back(quote);
}

void cHistory::Sort()
{
	printf("  Sorting...");
	std::sort(m_Quotes.begin(), m_Quotes.end());
	printf("done\n\n");
}

///////////////////////////////////////////////////////////////////////
// cDecisionMaker class

cDecisionMaker::cDecisionMaker()
{
	Init();
}

//virtual
cDecisionMaker::~cDecisionMaker()
{
}

void cDecisionMaker::Init()
{
	m_State = STANDBY;
	m_iPlayingGroup = -1;
	m_LastDealTime = 0;
	m_LastOperationDone = DO_NOTHING;
}

int cDecisionMaker::CalcSwapDeltaDays(double last_time, double this_time)
{
	tm *pTM = getGMT(last_time);
	int nLastDealDay = pTM->tm_year*365 + pTM->tm_yday;
	pTM = getGMT(this_time);
	int nToday = pTM->tm_year*365 + pTM->tm_yday;
	return (nToday - nLastDealDay);
}

void cDecisionMaker::PrintStatus()
{
	if (m_State == SOLD)
	{
		printf("\tPlaying group = %d\n", m_iPlayingGroup);
		printf("\tSold @ %f\n", m_fSoldAt);
		printf("\tMinBidAfterSell = %f\n", m_fMinBidAfterSell);
		printf("\tLast operation %d done at %f\n", m_LastOperationDone, m_LastDealTime);
	}
	else if (m_State == BOUGHT)
	{
		printf("\tPlaying group = %d\n", m_iPlayingGroup);
		printf("\tBought @ %f\n", m_fBoughtAt);
		printf("\tMaxBidAfterBuy = %f\n", m_fMaxBidAfterBuy);
		printf("\tLast operation %d done at %f\n", m_LastOperationDone, m_LastDealTime);
	}
}

double cDecisionMaker::CalcProfit(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cLog &log)
{
	Init();
	log.resize(0);
	double StartTime = 0, EndTime = 0;
	bool bTimeSet = false;
	stat.Init();

	for (cHistoryItems::const_iterator pQuote = items.begin(); pQuote != items.end(); pQuote++)
	{
		// Update times
		if (!bTimeSet)
		{
			StartTime = pQuote->m_Time;
			bTimeSet = true;
		}
		if (pQuote->m_Time > EndTime)
			EndTime = pQuote->m_Time;

		// Make decision
		DECISION_ENUM Decision = WhatToDo(params, fBrokerInterest, items, stat, pQuote);
		switch (Decision)
		{
		case DO_BUY:
			if (BOUGHT == m_State)
			{
				printf("Invalid 'WhatToDo' return!\n");
			}
			else
			{
				m_fLastBuy = pQuote->m_fBid + fBrokerInterest;
				if (SOLD == m_State)
				{ // closing position, update sell statistics
					stat.m_StatItems[m_iPlayingGroup].fProfitSell -= m_fLastBuy;
					++stat.m_StatItems[m_iPlayingGroup].nDealsSell;
					// Take swap into account
					stat.m_StatItems[m_iPlayingGroup].fProfitSell -= CalcSwapDeltaDays(m_LastDealTime, pQuote->m_Time)*fBrokerInterest;
				}
				else
				{ // opening position buying, update buy statistics
					stat.m_StatItems[m_iPlayingGroup].fProfitBuy -= m_fLastBuy;
					++stat.m_StatItems[m_iPlayingGroup].nDealsBuy;
				}
				NewLogItem(log, pQuote->m_Time, pQuote->m_fBid, m_fLastBuy, m_iPlayingGroup, "buy ");
				BuyDone(pQuote->m_Time, pQuote->m_fBid, fBrokerInterest);
			}
			break;
		case DO_SELL:
			if (m_State == SOLD)
			{
				printf("Invalid 'WhatToDo' return!\n");
			}
			else
			{
				m_fLastSell = pQuote->m_fBid;
				if (BOUGHT == m_State)
				{ // closing position, update buy statistics
					stat.m_StatItems[m_iPlayingGroup].fProfitBuy += m_fLastSell;
					++stat.m_StatItems[m_iPlayingGroup].nDealsBuy;
					// Take swap inte account
					stat.m_StatItems[m_iPlayingGroup].fProfitBuy -= CalcSwapDeltaDays(m_LastDealTime, pQuote->m_Time)*fBrokerInterest;
				}
				else
				{ // opening position selling, update sell statistics
					stat.m_StatItems[m_iPlayingGroup].fProfitSell += m_fLastSell;
					++stat.m_StatItems[m_iPlayingGroup].nDealsSell;
				}
				NewLogItem(log, pQuote->m_Time, pQuote->m_fBid, m_fLastSell, m_iPlayingGroup, "sell");
				SellDone(pQuote->m_Time, pQuote->m_fBid);
			}
			break;
		case ERROR_HISTORY_GAP:
		{
			if (m_State != STANDBY)
			{
				CancelLastDeal(stat, log);
			}
		}
		case DO_NOTHING:
		default:
			break;
		}
		
		UpdateExtremeBids(pQuote->m_Time, pQuote->m_fBid);		
	}

	if (m_State != STANDBY)
	{
		CancelLastDeal(stat, log);
	}
	
	double DeltaTime = EndTime - StartTime;
	if (DeltaTime)
	{
		float DeltaTimeDays = DeltaTime/(3600.*24.);
		for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
		{
			SStatItem &StatItem = stat.m_StatItems[iGroup];
			StatItem.fProfitPerDaySell = StatItem.fProfitSell/DeltaTimeDays;
			StatItem.nDealsPerDaySell = StatItem.nDealsSell/DeltaTimeDays;
			StatItem.fProfitPerDayBuy = StatItem.fProfitBuy/DeltaTimeDays;
			StatItem.nDealsPerDayBuy = StatItem.nDealsBuy/DeltaTimeDays;
			
			stat.m_StatItems[N_GROUPS].Add(StatItem);
		}
	}

	return (stat.m_StatItems[N_GROUPS].fProfitPerDaySell + stat.m_StatItems[N_GROUPS].fProfitPerDayBuy);
}

DECISION_ENUM cDecisionMaker::WhatToDo(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote)
{
	DECISION_ENUM decision = DO_NOTHING;
	switch (m_State)
	{
	case STANDBY:
		decision = WhatToDoStandby(params, fBrokerInterest, items, stat, pQuote);
		break;
	case SOLD:
		decision = WhatToDoSold(params, fBrokerInterest, items, stat, pQuote);
		break;
	case BOUGHT:
		decision = WhatToDoBought(params, fBrokerInterest, items, stat, pQuote);
		break;
	default:
		printf("Unknown state %d in cDecisionMaker::WhatToDo\n", m_State);
		break;
	}
	
	return decision;
}

DECISION_ENUM cDecisionMaker::WhatToDoStandby(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote)
{
	if (IsEvening(pQuote->m_Time))
		return DO_NOTHING;

	if (IsSunday(pQuote->m_Time))
		return DO_NOTHING;

	if (IsSaturday(pQuote->m_Time))
		return DO_NOTHING;

	double earliest_time = pQuote->m_Time;
	double MaxDepth = 0;
	for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
	{
		const cGroup &group = params.m_Groups[iGroup];
		if (MaxDepth < group.FALL_TIME_MAX)
			MaxDepth = group.FALL_TIME_MAX;
		if (MaxDepth < group.RAISE_TIME_MAX)
			MaxDepth = group.RAISE_TIME_MAX;
	}

	int nBackWardRecords = 0;
	for (cHistoryItems::const_iterator pPrevQuote = pQuote; pPrevQuote != items.begin(); pPrevQuote--)
	{
		// do not operate at 10+ mins breaks and cancel  last deal befor break
		if (earliest_time - pPrevQuote->m_Time > 10*60)
			return ERROR_HISTORY_GAP;
	
		++nBackWardRecords;
		if (nBackWardRecords < 5)
			continue; // filter occasional oclillations - do nothing if there is a sharp step there
		
		if (pPrevQuote->m_Time == pQuote->m_Time)
			continue;
			
		double dPrice = pQuote->m_fBid - pPrevQuote->m_fBid;
		if (dPrice < 0)
		{ // fall, but how long and how fast?
			for (iGroup = 0; iGroup < N_GROUPS; iGroup++)
			{
				const cGroup &group = params.m_Groups[iGroup];
				if (m_LastOperationDone == DO_SELL || (pQuote->m_Time - m_LastDealTime) >= group.FALL_TIME_MAX)
				{
					if (group.IsFastFall(pQuote, pPrevQuote))
					{
						m_iPlayingGroup = iGroup;
						return DO_SELL;
					}
				}
			}
		}
		else if (dPrice > 0)// && 
		{ // raise, but how long and how fast?
			for (iGroup = 0; iGroup < N_GROUPS; iGroup++)
			{
				const cGroup &group = params.m_Groups[iGroup];
				if (m_LastOperationDone == DO_BUY || (pQuote->m_Time - m_LastDealTime) >= group.RAISE_TIME_MAX)
				{
					if (group.IsFastRaise(pQuote, pPrevQuote))
					{
						m_iPlayingGroup = iGroup;
						return DO_BUY;
					}
				}
			}
		}

		// look in past not too deep
		if ((pQuote->m_Time - pPrevQuote->m_Time) > MaxDepth)
			return DO_NOTHING;

		earliest_time = pPrevQuote->m_Time;
	}

	return DO_NOTHING;
}

DECISION_ENUM cDecisionMaker::WhatToDoSold(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote)
{

	if (pQuote->m_Time == m_LastDealTime)
		return DO_NOTHING; // additional defence against extremely fast oscillations
	
	if ((pQuote->m_Time - m_LastDealTime) > MAX_OPEN_DURATION + 60)
		return ERROR_HISTORY_GAP; // history gap

	if ((pQuote->m_Time - m_LastDealTime) > MAX_OPEN_DURATION)
		return DO_BUY; // close too long position

	const cGroup &PlayingGroup = params.m_Groups[m_iPlayingGroup];

	if (pQuote->m_Time > m_LastDealTime + PlayingGroup.FALL_MAX_DURATION*60) // in minutes)
		return DO_BUY; // close position if it is open too long time

	if (IsFridayEvening(pQuote->m_Time))
		return DO_BUY;

	if ((pQuote->m_Time - m_LastDealTime) < PlayingGroup.FALL_DEAD_ZONE)
		return DO_NOTHING; // dead zone

	if (pQuote->m_fBid > (m_fSoldAt + PlayingGroup.FALL_STOPLOSS + eps))
	{
		//printf("\t\tBid %f >= %f -> BUY (stop-loss)\n", pQuote->m_fBid, limit);
		return DO_BUY; // stop loss :-(
	}

	if (pQuote->m_fBid > (m_fMinBidAfterSell + PlayingGroup.FALL_BOUNCE_TAKE_PROFIT + eps))
	{
		if (pQuote->m_fBid < (m_fSoldAt - PlayingGroup.FALL_TAKE_PROFIT - eps))
			return DO_BUY; // take profit
	}

	double fSpring = PlayingGroup.FALL_SPRING*(pQuote->m_Time - m_LastDealTime)/60;
	if (pQuote->m_fBid > (m_fMinBidAfterSell + PlayingGroup.FALL_ABOVE_MIN - fSpring + eps))
	{
		//printf("\t\tBid %f >= %f -> BUY (from MIN)\n", pQuote->m_fBid, limit);
		return DO_BUY;
	}

	double MaxDepth = 0;
	for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
	{
		const cGroup &group = params.m_Groups[iGroup];
		if (MaxDepth < group.RAISE_TIME_MAX)
			MaxDepth = group.RAISE_TIME_MAX;
	}

	int nBackWardRecords = 0;
	for (cHistoryItems::const_iterator pPrevQuote = pQuote; pPrevQuote != items.begin(); pPrevQuote--)
	{
		if (pPrevQuote->m_Time <= m_LastDealTime)
			break; // not earlier then when we open position

		++nBackWardRecords;
		if (nBackWardRecords < 5)
			continue; // filter occasional oclillations - do nothing if there is a sharp step there
		
		if (pPrevQuote->m_Time == pQuote->m_Time)
			continue;
			
		double dPrice = pQuote->m_fBid - pPrevQuote->m_fBid;
		if (dPrice > 0)// && 
		{ // raise, but how long and how fast?
			for (iGroup = 0; iGroup < N_GROUPS; iGroup++)
			{
				const cGroup &group = params.m_Groups[iGroup];
				if (m_LastOperationDone == DO_BUY || (pQuote->m_Time - m_LastDealTime) >= group.RAISE_TIME_MAX)
				{
					if (group.IsFastRaise(pQuote, pPrevQuote))
					{
						return DO_BUY;
					}
				}
			}
		}

		// look in past not too deep
		if ((pQuote->m_Time - pPrevQuote->m_Time) > MaxDepth)
			break;
	}

	return DO_NOTHING;
}

DECISION_ENUM cDecisionMaker::WhatToDoBought(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote)
{
	if (pQuote->m_Time == m_LastDealTime)
		return DO_NOTHING; // additional defence against extremely fast oscillations

	if ((pQuote->m_Time - m_LastDealTime) > MAX_OPEN_DURATION + 60)
		return ERROR_HISTORY_GAP; // history gap

	if ((pQuote->m_Time - m_LastDealTime) > MAX_OPEN_DURATION)
		return DO_SELL; // close too long position

	const cGroup &PlayingGroup = params.m_Groups[m_iPlayingGroup];

	if (pQuote->m_Time > m_LastDealTime + PlayingGroup.RAISE_MAX_DURATION*60) // in minutes
		return DO_SELL; // close position if it is open too long time

	if (IsFridayEvening(pQuote->m_Time))
		return DO_SELL;

	if ((pQuote->m_Time - m_LastDealTime) < PlayingGroup.RAISE_DEAD_ZONE)
		return DO_NOTHING; // dead zone


	if (pQuote->m_fBid < (m_fBoughtAt - PlayingGroup.RAISE_STOPLOSS - eps))
	{
		//printf("\t\tBid %f >= %f -> SELL (stop-loss)\n", pQuote->m_fBid, limit);
		return DO_SELL; // stop loss :-(
	}

	if (pQuote->m_fBid < (m_fMaxBidAfterBuy - PlayingGroup.RAISE_BOUNCE_TAKE_PROFIT - eps))
	{
		if (pQuote->m_fBid > (m_fBoughtAt + PlayingGroup.RAISE_TAKE_PROFIT + eps))
			return DO_SELL; // take profit
	}

	double fSpring = PlayingGroup.RAISE_SPRING*(pQuote->m_Time - m_LastDealTime)/60;
	if (pQuote->m_fBid < (m_fMaxBidAfterBuy - PlayingGroup.RAISE_BELOW_MAX + fSpring - eps))
	{
		//printf("\t\tBid %f >= %f -> SELL (from MAX)\n", pQuote->m_fBid, limit);
		return DO_SELL; // take profit
	}

	double MaxDepth = 0;
	for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
	{
		const cGroup &group = params.m_Groups[iGroup];
		if (MaxDepth < group.FALL_TIME_MAX)
			MaxDepth = group.FALL_TIME_MAX;
	}

	int nBackWardRecords = 0;
	for (cHistoryItems::const_iterator pPrevQuote = pQuote; pPrevQuote != items.begin(); pPrevQuote--)
	{
		if (pPrevQuote->m_Time <= m_LastDealTime)
			break; // not earlier then when we open position

		++nBackWardRecords;
		if (nBackWardRecords < 5)
			continue; // filter occasional oclillations - do nothing if there is a sharp step there
		
		if (pPrevQuote->m_Time == pQuote->m_Time)
			continue;
			
		double dPrice = pQuote->m_fBid - pPrevQuote->m_fBid;
		if (dPrice < 0)
		{ // fall, but how long and how fast?
			for (iGroup = 0; iGroup < N_GROUPS; iGroup++)
			{
				const cGroup &group = params.m_Groups[iGroup];
				if (m_LastOperationDone == DO_SELL || (pQuote->m_Time - m_LastDealTime) >= group.FALL_TIME_MAX)
				{
					if (group.IsFastFall(pQuote, pPrevQuote))
					{
						return DO_SELL;
					}
				}
			}
		}
		// look in past not too deep
		if ((pQuote->m_Time - pPrevQuote->m_Time) > MaxDepth)
			break;
	}
	return DO_NOTHING;
}

void cDecisionMaker::BuyDone(const double t, const double fBid, double fBrokerInterest)
{
	m_LastDealTime = t;
	m_LastOperationDone = DO_BUY;
	m_fMaxBidAfterBuy = fBid;
	m_fBoughtAt = fBid + fBrokerInterest;
	if (SOLD == m_State)
		m_State = STANDBY;
	else if (STANDBY == m_State)
		m_State = BOUGHT;
	else
		printf("Invalid STATE %d at cDecisionMaker::BuyDone!\n", m_State);
//	printf("%d BUY  at %f\n", t, fBid);
}

void cDecisionMaker::SellDone(const double t, const double fBid)
{
	m_LastDealTime = t;
	m_LastOperationDone = DO_SELL;
	m_fMinBidAfterSell = fBid;
	m_fSoldAt = fBid;
	if (BOUGHT == m_State)
		m_State = STANDBY;
	else if (STANDBY == m_State)
		m_State = SOLD;
	else
		printf("Invalid STATE %d at cDecisionMaker::SellDone!\n", m_State);
//	printf("%d SELL at %f\n", t, fBid);
}

void cDecisionMaker::CancelLastDeal(cStatistics &stat, cLog &log)
{
	if (BOUGHT == m_State)
	{ // BUY w/o SELL, cancel last BUY
		stat.m_StatItems[m_iPlayingGroup].fProfitBuy += m_fLastBuy;
		stat.m_StatItems[m_iPlayingGroup].nDealsBuy -= 1;
		log.pop_back();
		m_State = STANDBY;
	}
	else if (SOLD == m_State)
	{ // SELL w/o BUY, cancel last SELL
		stat.m_StatItems[m_iPlayingGroup].fProfitSell -= m_fLastSell;
		stat.m_StatItems[m_iPlayingGroup].nDealsSell -= 1;
		log.pop_back();
		m_State = STANDBY;
	}
}

void cDecisionMaker::UpdateExtremeBids(const double &t, const double fBid)
{
	if (BOUGHT == m_State)
	{
		if (fBid > m_fMaxBidAfterBuy)
		{
			m_fMaxBidAfterBuy = fBid;
		}
	}
	if (SOLD == m_State)
	{
		if (fBid < m_fMinBidAfterSell)
		{
			m_fMinBidAfterSell = fBid;
		}
	}
}

void cDecisionMaker::NewLogItem(cLog &log, double t, double fBid, double fRealPrice, int iPlayingGroup, char *pszOp)
{
	SLogItem item(t, fBid, fRealPrice, iPlayingGroup, pszOp);
	log.push_back(item);
}

///////////////////////////////////////////////////////////////////////
// cCurrencyPair class

cCurrencyPair::cCurrencyPair(const char *pszTicker)
{
	m_Ticker = pszTicker;
	m_szDirName[0] = 0;
	m_LastTime = 0;
	m_fLastBid = 0;
	m_bTradingEnabled = LOGICAL_FALSE;
	m_bImprovementEnabled = LOGICAL_FALSE;
	m_TradingAmount = 0;
	m_bParamsValid = m_bGroupsValid = LOGICAL_FALSE;
	m_MainParams.Init();
	m_ProbeParams.Init();
	m_BaseProfitTime = 0;
	m_fBrokerInterest = 1;// big enough value;
	m_nTrades = 0;
}

cCurrencyPair::~cCurrencyPair()
{
}

void cCurrencyPair::Init()
{
	if (strlen(m_szDirName))
		return; // done already

	::GetCurrentDirectory(MAX_FILE_NAME_LEN-32, m_szDirName);
	strcat(m_szDirName, "\\");
	strcat(m_szDirName, m_Ticker.GetText());
	printf("\nDirectory %s  ", m_szDirName);
	if (::CreateDirectory(m_szDirName, NULL))
		printf(" was created successfully.\n");
	else
	{
		DWORD EC = GetLastError();
		if (EC == ERROR_ALREADY_EXISTS)
			printf("already exists\n");
		else
			printf("creation failed, error code = %d.\n", EC);
	}
}

void cCurrencyPair::Start()
{
	ReadParams();
	ReadGroups();
	ReadHistory();
}

LOGICAL cCurrencyPair::isMy(const char *pszTicker)
{
	return m_Ticker.IsEqualNoCase(pszTicker);
}

void cCurrencyPair::OnQuoteChanged(const QuoteChangeEvent &qce, tm *pTM)
{
	double fBid = atof(qce.bid);
	if (fBid < m_fBrokerInterest)
		return; // do nothing, something like 0.0000
	
	if ((qce.timestamp - m_LastTime) >= 10 || fabs(fBid - m_fLastBid) >= eps)
	{
		sprintf(m_szFileName, "%s\\%02d-%02d-%04d(%s).%s", m_szDirName, pTM->tm_mday, pTM->tm_mon+1, pTM->tm_year+1900, PSZ_DAY_OF_WEEK[pTM->tm_wday], PSZ_HISTORY_FILE_EXTENSION);
		FILE *fd = fopen(m_szFileName, "a");
		if (fd)
		{
			fprintf(fd, "%lf\t%s\n", qce.timestamp, qce.bid);
			fclose(fd);
		}
		else
		{
			printf("ERROR: Cannot write to file %s, error code = %d\n", m_szFileName, GetLastError());
		}
		if (m_bImprovementEnabled || m_bTradingEnabled)
			m_History.NewQuote(qce.timestamp, fBid);
		m_DecisionMaker.UpdateExtremeBids(qce.timestamp, fBid);		
		printf("%s:\t%s\t(%s)\n", getTicker(), qce.bid, getStatus());
		m_DecisionMaker.PrintStatus();
	}
	m_LastTime = qce.timestamp;
	m_fLastBid = fBid;
}

void cCurrencyPair::OnTradeDone(int isOwned, const struct TradeInfo *pInfo)
{
	char szFileName[MAX_FILE_NAME_LEN];
	strcpy(szFileName, m_szDirName);
	strcat(szFileName, "\\");
	strcat(szFileName, "AllTrades.txt");
	FILE *pFile = fopen(szFileName, "a");
	if (pFile)
	{
		time_t t = (time_t)pInfo->timestamp;
		char szTimeString[128];
		TimeString(&t, szTimeString, sizeof(szTimeString));
		fprintf(pFile, "%f\t%s\t%s\t%s\tfor\t%s\t%s\t%s\n", pInfo->timestamp, szTimeString, pInfo->ccy1amount, pInfo->ccy1, pInfo->ccy2amount, pInfo->ccy2, pInfo->ticket);
		++m_nTrades;
		if ((m_nTrades % 2) == 0)
			fprintf(pFile, "\n");
		fclose(pFile);
	}
}

SDecision *cCurrencyPair::WhatToDoNow(const QuoteChangeEvent &qce)
{
//	static int state = 0;
//	static time_t t1 = 0;
//	static time_t t2 = 0;
	if (isTradingEnabled())
	{
		m_Decision.decision = m_DecisionMaker.WhatToDo(m_MainParams, m_fBrokerInterest, m_History.getQuotes(), m_MainStatistics, m_History.getLastQuote());
/*		time(&t2);
		if (!strcmp(qce.ticker, "EURJPY"))
		{
			if (state == 0)
			{
				m_Decision.decision = DO_SELL;
				state = -1;
				time(&t1);
			}
			else if (state == -1 && (t2-t1) > 20)
			{
				m_Decision.decision = DO_BUY;
				state = +1;
			}
			else
			{
				m_Decision.decision = DO_NOTHING;
			}
		}*/
		if (m_DecisionMaker.getCurrentState() == SOLD && m_Decision.decision == DO_SELL)
		{
			printf("Invalid 'WhatToDo' return!\n");
		}
		if (m_DecisionMaker.getCurrentState() == BOUGHT && m_Decision.decision == DO_BUY)
		{
			printf("Invalid 'WhatToDo' return!\n");
		}
		sprintf(m_Decision.szAmount, "%d", m_TradingAmount);
		m_Decision.fBid = atof(qce.bid);
		m_Decision.fOffer = atof(qce.offer);
		double fRealBrokerInterest = m_Decision.fOffer - m_Decision.fBid;
		double limit = 0;
		if (m_Decision.decision == DO_BUY)
			limit = m_Decision.fOffer + 5*fRealBrokerInterest;
		else if (m_Decision.decision == DO_SELL)
			limit = m_Decision.fBid - 5*fRealBrokerInterest;
		sprintf(m_Decision.szLimit, "%.4f", limit);
		m_Decision.timestamp = qce.timestamp;
	}
	else
	{
		m_Decision.decision = DO_NOTHING;
	}
	return &m_Decision;
}

void cCurrencyPair::CommitBuy(SDecision *pDecision)
{
	m_DecisionMaker.BuyDone(pDecision->timestamp, pDecision->fBid, m_fBrokerInterest);
}

void cCurrencyPair::CommitSell(SDecision *pDecision)
{
	m_DecisionMaker.SellDone(pDecision->timestamp, pDecision->fBid);
}

void cCurrencyPair::ReadHistory()
{
	if (!m_bImprovementEnabled)
		return; // do not read history if it is useless
	int nTotalRecords = 0;
	char szTemplate[MAX_FILE_NAME_LEN];
	char szFileName[MAX_FILE_NAME_LEN];
	strcpy(szTemplate, m_szDirName);
	strcat(szTemplate, "\\*.");
	strcat(szTemplate, PSZ_HISTORY_FILE_EXTENSION);
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(szTemplate, &fd);
	BOOL bFound = (hFile != INVALID_HANDLE_VALUE);
	while (bFound)
	{
		if (strcmp(fd.cFileName, ".") &&
			strcmp(fd.cFileName, ".."))
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			strcpy(szFileName, m_szDirName);
			strcat(szFileName, "\\");
			strcat(szFileName, fd.cFileName);
			printf("%s\r", fd.cFileName);
			FILE *pFile = fopen(szFileName, "r");
			if (pFile)
			{
				double last_time = 0;
				int nRecords = 0;
				while (1)
				{
					if (nRecords % 100 == 0)
						printf("%s, line %d\r", fd.cFileName, nRecords);
					double that_time;
					double that_bid;
					int nScanfed = fscanf(pFile, "%lf\t%lf\n", &that_time, &that_bid);
					if (nScanfed == 2)
					{
						while (that_time <= (last_time+eps))
							that_time += 0.1;
						m_History.NewQuote(that_time, that_bid);
						++nRecords;
						++nTotalRecords;
						last_time = that_time;
					}
					else if (nScanfed == EOF)
						break;
				}
				fclose(pFile);
				printf("\t%s, %d records\n", szFileName, nRecords);
			}
		}
		bFound = FindNextFile(hFile, &fd);
	}
	printf("  Totally %d records\n", nTotalRecords);
	FindClose(hFile);
	m_History.Sort();
}

void cCurrencyPair::ReadParams()
{
	m_bParamsValid = LOGICAL_TRUE;
	strcpy(m_szFileName, m_szDirName);
	strcat(m_szFileName, "\\");
	strcat(m_szFileName, PSZ_PARAMS_FILE_NAME);
	FILE *pFile = fopen(m_szFileName, "r");
	if (pFile)
	{
		char szParamName[128];
		LOGICAL bEOF = LOGICAL_FALSE;
		int nScanfed = fscanf(pFile, PSZ_BOOL_PARAM_TEMPLATE, szParamName, &m_bTradingEnabled);
		if (nScanfed != 2 || stricmp(szParamName, PSZ_TRADING_ENABLED))
			ReportCannotReadParam(PSZ_TRADING_ENABLED);
		bEOF = (nScanfed == EOF);

		double f;
		nScanfed = fscanf(pFile, PSZ_REAL_PARAM_TEMPLATE, szParamName, &f);
		m_TradingAmount = (int)f;
		if (nScanfed != 2 || stricmp(szParamName, PSZ_TRADING_AMOUNT))
			ReportCannotReadParam(PSZ_TRADING_AMOUNT);
		bEOF = (nScanfed == EOF);

		nScanfed = fscanf(pFile, PSZ_REAL_PARAM_TEMPLATE, szParamName, &m_fBrokerInterest);
		if (nScanfed != 2 || stricmp(szParamName, PSZ_BROKER_INTEREST))
			ReportCannotReadParam(PSZ_BROKER_INTEREST);
		bEOF = (nScanfed == EOF);

		nScanfed = fscanf(pFile, PSZ_BOOL_PARAM_TEMPLATE, szParamName, &m_bImprovementEnabled);
		if (nScanfed != 2 || stricmp(szParamName, PSZ_IMPROVEMENT_ENABLED))
			ReportCannotReadParam(PSZ_IMPROVEMENT_ENABLED);
		bEOF = (nScanfed == EOF);

		fclose(pFile);
	}
	else
	{
		m_bParamsValid = LOGICAL_FALSE;
		printf("Cannot open file %s, error code = %d\n", m_szFileName, GetLastError());
	}
	WriteParams();
}

void cCurrencyPair::ReadGroups()
{
	m_bGroupsValid = LOGICAL_TRUE;
	strcpy(m_szFileName, m_szDirName);
	strcat(m_szFileName, "\\");
	strcat(m_szFileName, PSZ_GROUPS_FILE_NAME);
	FILE *pFile = fopen(m_szFileName, "r");
	if (pFile)
	{
		LOGICAL bEOF = LOGICAL_FALSE;
		int j, k, nScanfed;
		double fMin, fMax, fValue, fStep;
		int iShift = 0;
		for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
		{
			if (iGroup > 0)
			{
				// Make this group equal to previous one
				m_MainParams.m_Groups[iGroup] = m_MainParams.m_Groups[iGroup-1];
				// By default expand time ranges
				//m_MainParams.m_Groups[iGroup].FALL_TIME_MIN *= 0.8;
				//m_MainParams.m_Groups[iGroup].RAISE_TIME_MIN *= 0.8;
				m_MainParams.m_Groups[iGroup].FALL_TIME_MAX *= 1.2;
				m_MainParams.m_Groups[iGroup].RAISE_TIME_MAX *= 1.2;
				// and make criteria weaker
				m_MainParams.m_Groups[iGroup].FALL_DELTA_MIN *= 0.8;
				m_MainParams.m_Groups[iGroup].RAISE_DELTA_MIN *= 0.8;
				m_MainParams.m_Groups[iGroup].FALL_DELTA_MAX *= 1.2;
				m_MainParams.m_Groups[iGroup].RAISE_DELTA_MAX *= 1.2;
			}
			int nNewParams = 0;
			for (int i = 0; i < N_PARAMS_IN_GROUP; i++)
			{
#if 0
				if (i == 10 || i == 21)
				{
					++nNewParams;
					/*if (strstr(m_Ticker.GetText(), "JPY"))
						m_MainParams.m_Groups[iGroup].SetParam(i, 0.00, 1.00, 0, 0.01, 0);
					else*/
						m_MainParams.m_Groups[iGroup].SetParam(i, 5, 1200, 300, 5, -1); //n, min, max, value, step, shift
					continue;
				}
#endif
				iShift = 0;
				nScanfed = fscanf(pFile, PSZ_ALL_PARAM_DESCRIPTION_TEMPLATE, &k, &j, &fValue, &fMin, &fMax, &fStep, &iShift);
				//if (iGroup == 0)
				{
					if (i == 10  || i == 21)
					{
						fMax = 150;  // 5 secs max for min time threshild (first group)
					}
				}
				SnapToGrid(fMin, fStep);
				SnapToGrid(fMax, fStep);
				SnapToGrid(fValue, fStep);
				bEOF = (nScanfed == EOF);
				if (nScanfed == 7 && (j+nNewParams) == i  && k == iGroup)
				{
					m_MainParams.m_Groups[iGroup].SetParam(i, fMin, fMax, fValue, fStep, iShift);
				}
				else
				{
					ReportCannotReadParam(iGroup, i);
				}
			}
		}
		fclose(pFile);
	}
	else
	{
		m_bGroupsValid = LOGICAL_FALSE;
		printf("Cannot open file %s, error code = %d\n", m_szFileName, GetLastError());
	}
	m_MainParams.Validate();
	WriteGroups();
}

void cCurrencyPair::CalcBaseProfit()
{
	if (areParamsValid())
	{
		m_fBaseProfit = m_ProbeMaker.CalcProfit(m_MainParams, m_fBrokerInterest, m_History.getQuotes(), m_MainStatistics, m_MainLog);
		time(&m_BaseProfitTime);
	}
}

LOGICAL cCurrencyPair::Improve()
{
	int ParamsVector[N_PARAMS_IN_GROUP];
	if (!isImprovementEnabled())
		return LOGICAL_FALSE;
	
	printf("   %s\r", getTicker());
	time_t current_time; time(&current_time);
	if ((current_time - m_BaseProfitTime) > 24*3600) // 1 day and older
	{
		CalcBaseProfit();
		printf("%s\t\t%f\t%f\n", getTicker(), m_fBaseProfit, m_MainStatistics.m_StatItems[N_GROUPS].nDealsPerDaySell + m_MainStatistics.m_StatItems[N_GROUPS].nDealsPerDayBuy);
		WriteProfit(true);
		WriteSuccessStory(m_MainStatistics);
		return LOGICAL_TRUE;
	}

	m_ProbeParams = m_MainParams;
	m_ProbeParams.Validate();
	srand(GetTickCount());
	int nChangedParams = 1;
	int bManyParams = rand() % 2;
	if (bManyParams)
		nChangedParams = (rand() % 3) + 1;
	int iGroup = rand() % N_GROUPS;
	int iFallOrRaise = rand() % 2;
	int nParamsInVector = 0;
	LOGICAL bSystematicallyShifted = LOGICAL_FALSE;
	
	for (int i = 0; i < nChangedParams; i++)
	{
		int iParamInSubgroup = rand() % (N_PARAMS_IN_GROUP/2); // param to be optimised this time
		int iParamInGroup = iParamInSubgroup;

		if (iFallOrRaise)
			iParamInGroup += N_PARAMS_IN_GROUP/2;
		ParamsVector[nParamsInVector] = iParamInGroup;
		++nParamsInVector;

		if (m_ProbeParams.m_Groups[iGroup].m_Params[iParamInGroup].m_iShift)
			bSystematicallyShifted = LOGICAL_TRUE;

		int iDirection = rand() % 2;
		int nRadius = (rand() % 5 + 1) * 5; // 5, 10, 15, 20, 25 - sometimes big steps, sometimes small ones
		int iChangeAreaSize = (rand() % nRadius) + 1;
		int n = (rand() % iChangeAreaSize) + 1;

		SParam &p = m_ProbeParams.m_Groups[iGroup].m_Params[iParamInGroup];
		if (iDirection)
			p.Increase(n);
		else
			p.Decrease(n);
	}
		
	m_ProbeParams.Validate();
	if (m_MainParams != m_ProbeParams)
	{
		double fProfit = m_ProbeMaker.CalcProfit(m_ProbeParams, m_fBrokerInterest, m_History.getQuotes(), m_ProbeStatistics, m_MainLog);

		// If at least 1 parameter with systematic shift was changed, accept that changes even if profit is unchanged
		if (bSystematicallyShifted)
			fProfit += 1.0E-08;

		if (fProfit > m_fBaseProfit)
		{
			double deltaProfit = fProfit - m_fBaseProfit;
			m_fBaseProfit = fProfit;
			m_MainParams = m_ProbeParams;
			m_MainStatistics = m_ProbeStatistics;
			WriteGroups();
			WriteProfit(false);
			WriteSuccessStory(m_MainStatistics);
			printf("%s\t\t%f (%+f)\t%f\tGroup %d (", m_Ticker.GetText(), m_fBaseProfit, deltaProfit, (m_MainStatistics.m_StatItems[N_GROUPS].nDealsPerDaySell+m_MainStatistics.m_StatItems[N_GROUPS].nDealsPerDayBuy), iGroup);
			for (int j = 0; j < nParamsInVector; j++)
			{
				printf("%d,", ParamsVector[j]);
			}
			printf(")\n");
		}
	}
	
	return LOGICAL_TRUE;
}

const char *cCurrencyPair::getStatus()
{
	if (!areParamsValid())
	{
		return "invalid";
	}
	else if (!isTradingEnabled())
	{
		return "disabled";
	}
	else
	{
		switch (m_DecisionMaker.getCurrentState())
		{
		case STANDBY: return "STANDBY";
		case SOLD: return "SOLD";
		case BOUGHT: return "BOUGHT";
		default: return "UNKNOWN";
		}
	}
}

void cCurrencyPair::WriteParams()
{
	//if (!areParamsValid())
	//	return; // do not write invalid params
	strcpy(m_szFileName, m_szDirName);
	strcat(m_szFileName, "\\");
	strcat(m_szFileName, PSZ_PARAMS_FILE_NAME);

	FILE *pFile = fopen(m_szFileName, "w");
	if (pFile)
	{
		fprintf(pFile, PSZ_BOOL_PARAM_TEMPLATE, PSZ_TRADING_ENABLED, m_bTradingEnabled);
		fprintf(pFile, PSZ_INT_PARAM_TEMPLATE, PSZ_TRADING_AMOUNT, m_TradingAmount);
		fprintf(pFile, PSZ_REAL_PARAM_TEMPLATE, PSZ_BROKER_INTEREST, m_fBrokerInterest);
		fprintf(pFile, PSZ_BOOL_PARAM_TEMPLATE, PSZ_IMPROVEMENT_ENABLED, m_bImprovementEnabled);

		fclose(pFile);
	}
	else
	{
		printf("Cannot open file %s for writting, error code = %d\n", m_szFileName, GetLastError());
	}
}

void cCurrencyPair::WriteGroups()
{
	//if (!areParamsValid())
	//	return; // do not write invalid params
	strcpy(m_szFileName, m_szDirName);
	strcat(m_szFileName, "\\");
	strcat(m_szFileName, PSZ_GROUPS_FILE_NAME);

	FILE *pFile = fopen(m_szFileName, "w");
	if (pFile)
	{
		for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
		{
			for (int i = 0; i < N_PARAMS_IN_GROUP; i++)
			{
				SParam &p = m_MainParams.m_Groups[iGroup].m_Params[i];
				fprintf(pFile, PSZ_ALL_PARAM_DESCRIPTION_TEMPLATE, iGroup, i, p.m_fValue, p.m_fMinValue, p.m_fMaxValue, p.m_fStep, p.m_iShift);
			}
		}
		fclose(pFile);
	}
	else
	{
		printf("Cannot open file %s for writting, error code = %d\n", m_szFileName, GetLastError());
	}
}

FILE *cCurrencyPair::openProfitFile()
{
	char szProfitFileName[MAX_FILE_NAME_LEN];
	strcpy(szProfitFileName, m_szDirName);
	strcat(szProfitFileName, "\\");
	strcat(szProfitFileName, PSZ_PROFIT_FILE_NAME);
	FILE *pFile = fopen(szProfitFileName, "a");
	return pFile;
}

void cCurrencyPair::WriteProfit(bool bCRLF)
{
	FILE *pFile = openProfitFile();
	if (pFile)
	{
		if (bCRLF)
			fprintf(pFile, "\n");
		time_t CurrentTime;	time(&CurrentTime);
		char szTimeString[128];
		TimeString(&CurrentTime, szTimeString, sizeof(szTimeString));
		fprintf(pFile, "%s\t%f\t%f", szTimeString, m_fBaseProfit, (m_MainStatistics.m_StatItems[N_GROUPS].nDealsPerDaySell+m_MainStatistics.m_StatItems[N_GROUPS].nDealsPerDayBuy));
		for (int iGroup = 0; iGroup < N_GROUPS; iGroup++)
		{
			fprintf(pFile, "\tGroup%d:", iGroup);
			for (int i = 0; i < N_PARAMS_IN_GROUP; i++)
			{
				if (i == N_PARAMS_IN_GROUP/2)
					fprintf(pFile, "\t|"); // separator
				fprintf(pFile, "\t%.4f", m_MainParams.m_Groups[iGroup].m_Params[i].m_fValue);
			}
		}
		fprintf(pFile, "\n");
		fclose(pFile);
	}	
}

void cCurrencyPair::WriteSuccessStory(const cStatistics &stat)
{
	int iParam;
	char szFileName[MAX_FILE_NAME_LEN];
	strcpy(szFileName, m_szDirName);
	strcat(szFileName, "\\");
	strcat(szFileName, PSZ_SUCCESS_STORY_FILE_NAME);
	FILE *pFile = fopen(szFileName, "w");
	if (pFile)
	{
		fprintf(pFile, "Success Story for %s (%.4f)\n\n", m_Ticker.GetText(), m_fBrokerInterest);
		fprintf(pFile, "Group statistics :\n");
		for (int iGroup = 0; iGroup <= N_GROUPS; iGroup++)
		{
			const cGroup &group = m_MainParams.m_Groups[iGroup];
			if (iGroup == N_GROUPS)
			{ // total
				fprintf(pFile, "=========================================\n");
				fprintf(pFile, "Totally:\n");
			}
			else
			{
				fprintf(pFile, "-----------------------------------------\n");
				fprintf(pFile, "Group %d:\n", iGroup);
			}
			fprintf(pFile, "\tSell:\n");
			if (iGroup != N_GROUPS)
			{
				for (iParam = 0; iParam < N_PARAMS_IN_GROUP/2; iParam++)
				{
					const SParam &p = group.m_Params[iParam];
					fprintf(pFile, "\t\t(%3d)\t%s = %.4f (%.4f...%.4f), step %.4f, shift %+d\n", iParam, PSZ_PARAM_DESC[iParam], p.m_fValue, p.m_fMinValue, p.m_fMaxValue, p.m_fStep, p.m_iShift);
				}
			}
			fprintf(pFile, "\t\tProfit = %f\n", stat.m_StatItems[iGroup].fProfitSell);
			fprintf(pFile, "\t\tProfitPerDay = %f\n", stat.m_StatItems[iGroup].fProfitPerDaySell);
			fprintf(pFile, "\t\tnDeals = %d\n", stat.m_StatItems[iGroup].nDealsSell);
			fprintf(pFile, "\t\tnDealsPerDay = %f\n", stat.m_StatItems[iGroup].nDealsPerDaySell);
			if (stat.m_StatItems[iGroup].nDealsSell > 0)
			{
				fprintf(pFile, "\t\tProfitPerDeal = %f\n", stat.m_StatItems[iGroup].fProfitSell/stat.m_StatItems[iGroup].nDealsSell);
			}
			fprintf(pFile, "\tBuy:\n");
			if (iGroup != N_GROUPS)
			{
				for (iParam = N_PARAMS_IN_GROUP/2; iParam < N_PARAMS_IN_GROUP; iParam++)
				{
					const SParam &p = group.m_Params[iParam];
					fprintf(pFile, "\t\t(%3d)\t%s = %.4f (%.4f...%.4f), step %.4f, shift %+d\n", iParam, PSZ_PARAM_DESC[iParam], p.m_fValue, p.m_fMinValue, p.m_fMaxValue, p.m_fStep, p.m_iShift);
				}
			}
			fprintf(pFile, "\t\tProfit = %f\n", stat.m_StatItems[iGroup].fProfitBuy);
			fprintf(pFile, "\t\tProfitPerDay = %f\n", stat.m_StatItems[iGroup].fProfitPerDayBuy);
			fprintf(pFile, "\t\tnDeals = %d\n", stat.m_StatItems[iGroup].nDealsBuy);
			fprintf(pFile, "\t\tnDealsPerDay = %f\n", stat.m_StatItems[iGroup].nDealsPerDayBuy);
			if (stat.m_StatItems[iGroup].nDealsBuy > 0)
			{
				fprintf(pFile, "\t\tProfitPerDeal = %f\n", stat.m_StatItems[iGroup].fProfitBuy/stat.m_StatItems[iGroup].nDealsBuy);
			}
			fprintf(pFile, "-----------------------------------------\n");
		}

		fprintf(pFile, "\n  Time\t\tbid\tGroup\tOPERATION/PRICE\t\tprofit\tdate/time\n");
		int n = 0;
		double fPrevPrice;
		for (cLog::const_iterator pItem = m_MainLog.begin(); pItem != m_MainLog.end(); pItem++)
		{
			fprintf(pFile, "%.0lf\t%.4f\t%d\t%s\tfor %.4f", pItem->m_Time, pItem->m_fBid, pItem->m_iPlayGroup, pItem->m_pszOp, pItem->m_fRealPrice);
			if (n % 2)
			{ // print profit for that pair
				if (!stricmp(pItem->m_pszOp, "sell"))
				{ // sell after buy
					fprintf(pFile, "\t%+.4f", pItem->m_fRealPrice - fPrevPrice);
				}
				else
				{ // buy after sell
					fprintf(pFile, "\t%+.4f", fPrevPrice - pItem->m_fRealPrice);
				}
			}
			else
			{
				fprintf(pFile, "\t\t"); // blank
			}
			time_t t = (time_t)pItem->m_Time;
			fprintf(pFile, "\t%s", asctime(gmtime(&t)));
			if (n % 2)
				fprintf(pFile, "\n");
			++n;
			fPrevPrice = pItem->m_fRealPrice;
		}
		fclose(pFile);
	}
}

void cCurrencyPair::ReportCannotReadParam(const char *pszParamName)
{
	m_bParamsValid = LOGICAL_FALSE;
	printf("Cannot read parameter %s\n", pszParamName);
}

void cCurrencyPair::ReportCannotReadParam(const int iGroup, const int iParam)
{
	m_bParamsValid = LOGICAL_FALSE;
	printf("Cannot read parameter Param_%d_%d\n", iGroup, iParam);
}

///////////////////////////////////////////////////////////////////
// cCurrencyPairs

cCurrencyPair &cCurrencyPairs::Find(const char *pszTicker)
{
	for (cVectorOfPairs::iterator it = m_PairVector.begin(); it != m_PairVector.end(); it++)
	{
		cCurrencyPair &pair = *it;
		if (pair.isMy(pszTicker))
			return pair;
	}
	cCurrencyPair pair(pszTicker);
	pair.Init();
	m_PairVector.push_back(pair);
	it = m_PairVector.end()-1;
	return *it;
}

cCurrencyPair & cCurrencyPairs::operator[] (int iIndex)
{
	return m_PairVector[iIndex];
}

