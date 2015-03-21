#ifndef __CURRENCY_PAIR_H_INCLUDED__
#define __CURRENCY_PAIR_H_INCLUDED__

#include "Portable.h"
#include <stack>
#include <time.h>

struct QuoteChangeEvent;

const int MAX_FILE_NAME_LEN = 1024;
extern char *PSZ_HISTORY_FILE_EXTENSION;
extern char *PSZ_PARAMS_FILE_NAME;
extern char *PSZ_GROUPS_FILE_NAME;
extern char *PSZ_PROFIT_FILE_NAME;

const int N_GROUPS = 3;
const int N_PARAMS_IN_GROUP = 22;

typedef struct SStatItem
{
	double fProfitBuy, fProfitSell;
	double fProfitPerDayBuy, fProfitPerDaySell;
	int nDealsBuy, nDealsSell;
	double nDealsPerDayBuy, nDealsPerDaySell;

	void Add(const SStatItem &item)
	{
		fProfitBuy += item.fProfitBuy;
		fProfitSell += item.fProfitSell;
		fProfitPerDayBuy += item.fProfitPerDayBuy;
		fProfitPerDaySell += item.fProfitPerDaySell;
		nDealsBuy += item.nDealsBuy;
		nDealsSell += item.nDealsSell;
		nDealsPerDayBuy += item.nDealsPerDayBuy;
		nDealsPerDaySell += item.nDealsPerDaySell;
	}
} SStatItem;

class cStatistics
{
public:
	cStatistics()
	{
	}
	void Init()
	{
		memset(m_StatItems, 0, sizeof(m_StatItems));
	}
	void operator = (const cStatistics &stat)
	{
		memcpy(m_StatItems, stat.m_StatItems, sizeof(m_StatItems));
	}

	SStatItem m_StatItems[N_GROUPS+1]; // last element - for total counters!

protected:
};

typedef struct
{
	double m_fMinValue;
	double m_fMaxValue;
	double m_fValue;
	double m_fStep;
	int m_iShift;
	LOGICAL m_bValid;

	void Increase(int n)
	{
		m_fValue += n*m_fStep;
		m_fValue += m_iShift*m_fStep; //systematic trend
		Validate();
	}
	void Decrease(int n)
	{
		m_fValue -= n*m_fStep;
		m_fValue += m_iShift*m_fStep; //systematic trend
		Validate();
	}
	void Validate()
	{
		if (m_fValue < m_fMinValue)
			m_fValue = m_fMinValue;
		if (m_fValue > m_fMaxValue)
			m_fValue = m_fMaxValue;
	}
} SParam;

#define FALL_TIME_MAX m_Params[0].m_fValue
#define FALL_TIME_MIN m_Params[1].m_fValue
#define FALL_DELTA_MIN m_Params[2].m_fValue
#define FALL_DELTA_MAX m_Params[3].m_fValue
#define FALL_STOPLOSS m_Params[4].m_fValue
#define FALL_ABOVE_MIN m_Params[5].m_fValue
#define FALL_BOUNCE_TAKE_PROFIT m_Params[6].m_fValue
#define FALL_TAKE_PROFIT m_Params[7].m_fValue
#define FALL_SPRING m_Params[8].m_fValue
#define FALL_DEAD_ZONE m_Params[9].m_fValue
#define FALL_MAX_DURATION m_Params[10].m_fValue

#define RAISE_TIME_MAX m_Params[11].m_fValue
#define RAISE_TIME_MIN m_Params[12].m_fValue
#define RAISE_DELTA_MIN m_Params[13].m_fValue
#define RAISE_DELTA_MAX m_Params[14].m_fValue
#define RAISE_STOPLOSS m_Params[15].m_fValue
#define RAISE_BELOW_MAX m_Params[16].m_fValue
#define RAISE_BOUNCE_TAKE_PROFIT m_Params[17].m_fValue
#define RAISE_TAKE_PROFIT m_Params[18].m_fValue
#define RAISE_SPRING m_Params[19].m_fValue
#define RAISE_DEAD_ZONE m_Params[20].m_fValue
#define RAISE_MAX_DURATION m_Params[21].m_fValue


typedef struct SQuote
{
	SQuote(const double timestamp, const double fBid)
	{
		m_Time = timestamp;
		m_fBid = fBid;
	}
	double m_Time;
	double m_fBid;
	bool operator < (const SQuote &item)
	{
		return (m_Time < item.m_Time);
	}
} SQuote;
typedef std::vector<SQuote> cHistoryItems;

class cGroup
{
public:
	cGroup();
	void Init();
	void SetParam(const int iParam, const double fMin, const double fMax, const double fValue, const double fStep, int iShift);
	void GetParam(const int iParam, double &fMin, double &fMax, double &fValue, double &fStep);
	void operator = (const cGroup &group)
	{
		memcpy(m_Params, group.m_Params, sizeof(m_Params));
	}
	bool operator != (const cGroup &group);
	void Validate();
	void Validate(const cGroup &group);
	bool IsFastFall(const SQuote *pThisQuote, const SQuote *pThatQuote) const;
	bool IsFastRaise(const SQuote *pThisQuote, const SQuote *pThatQuote) const;
	
	SParam m_Params[N_PARAMS_IN_GROUP];
};

class cParams
{
public:
	cParams();
	void Init();
	void Validate();
	bool operator != (const cParams &params);
	
	cGroup m_Groups[N_GROUPS];
};


class cHistory
{
public:
	cHistory();
	void NewQuote(const double timestamp, const double fBid);
	cHistoryItems &getQuotes() { return m_Quotes; }
	cHistoryItems::const_iterator getLastQuote() { return m_Quotes.end() - 1; }
	void Sort();
protected:
	cHistoryItems m_Quotes;
};

typedef enum
{
	DO_NOTHING,
	DO_SELL,
	DO_BUY,
	ERROR_HISTORY_GAP
} DECISION_ENUM;

typedef struct SLogItem
{
	double m_Time;
	double m_fBid, m_fRealPrice;
	int m_iPlayGroup;
	char *m_pszOp;

	SLogItem() {}
	SLogItem(double t, double fBid, double fRealPrice, int iPlayingGroup, char *pszOp)
	{
		m_Time = t;
		m_fBid = fBid;
		m_fRealPrice = fRealPrice;
		m_iPlayGroup = iPlayingGroup;
		m_pszOp = pszOp;
	}
} SLogItem;

typedef std::vector<SLogItem> cLog;

typedef enum
{
	STANDBY,
	SOLD,
	BOUGHT
} STATE_ENUM;

class cDecisionMaker
{
public:
	cDecisionMaker();
	virtual ~cDecisionMaker();

public:
	void Init();
	double CalcProfit(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cLog &log);
	DECISION_ENUM WhatToDo(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote);
	void NewLogItem(cLog &log, double t, double fBid, double fRealPrice, int iPlayingGroup, char *pszOp);
	void BuyDone(const double t, const double fBid, double fBrokerInterest);
	void SellDone(const double t, const double fBid);
	STATE_ENUM getCurrentState() { return m_State; }
	int CalcSwapDeltaDays(double last_time, double this_time);
	void UpdateExtremeBids(const double &t, const double fBid);
	void PrintStatus();

protected:
	DECISION_ENUM WhatToDoStandby(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote);
	DECISION_ENUM WhatToDoSold(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote);
	DECISION_ENUM WhatToDoBought(const cParams &params, const double fBrokerInterest, const cHistoryItems &items, cStatistics &stat, cHistoryItems::const_iterator pQuote);
	void CancelLastDeal(cStatistics &stat, cLog &log);

protected:
	STATE_ENUM m_State;
	int m_iPlayingGroup;
	double m_fLastSell, m_fLastBuy;
	double m_fMinBidAfterSell, m_fMaxBidAfterBuy;
	double m_fSoldAt, m_fBoughtAt;
	DECISION_ENUM m_LastOperationDone;
	double m_LastDealTime;
};

typedef struct
{
	DECISION_ENUM decision;
	char szAmount[32];
	char szLimit[32];
	double fBid, fOffer;
	double timestamp;
} SDecision;

class cCurrencyPair
{
public:
	cCurrencyPair(const char *pszTicker);
	~cCurrencyPair();
	void Init();
	void Start();
	LOGICAL isMy(const char *pszTicker);
	LOGICAL areParamsValid() { return m_bParamsValid && m_bGroupsValid; }
	LOGICAL isImprovementEnabled() { return areParamsValid() && m_bImprovementEnabled; }
	LOGICAL isTradingEnabled() { return areParamsValid() && m_bTradingEnabled; }
	void OnQuoteChanged(const QuoteChangeEvent &qce, tm *pTM);
	void OnTradeDone(int isOwned, const struct TradeInfo *pInfo);
	SDecision *WhatToDoNow(const QuoteChangeEvent &qce);
	void CommitBuy(SDecision *pDecision);
	void CommitSell(SDecision *pDecision);
	void ReadHistory();
	void ReadParams();
	void ReadGroups();
	void CalcBaseProfit();
	void WriteParams();
	void WriteGroups();
	void WriteProfit(bool bCRLF);
	void WriteSuccessStory(const cStatistics &stat);
	LOGICAL Improve();
	const char *getTicker() { return m_Ticker.GetText(); }
	const char *getStatus();

protected:
	void ReportCannotReadParam(const char *pszParamName);
	void ReportCannotReadParam(const int iGroup, const int iParam);
	FILE *openProfitFile();

protected:
	CTextString m_Ticker;
	char m_szDirName[MAX_FILE_NAME_LEN];
	char m_szFileName[MAX_FILE_NAME_LEN];
	double m_LastTime;
	double m_fLastBid;
	cHistory m_History;
	cParams m_MainParams, m_ProbeParams;
	LOGICAL m_bParamsValid, m_bGroupsValid;
	LOGICAL m_bTradingEnabled;
	LOGICAL m_bImprovementEnabled;
	int m_TradingAmount;
	double m_fBrokerInterest;

	cDecisionMaker m_DecisionMaker;
	cDecisionMaker m_ProbeMaker;
	SDecision m_Decision;

	cStatistics m_MainStatistics, m_ProbeStatistics;
	cLog m_MainLog;
	
	double m_fBaseProfit;
	time_t m_BaseProfitTime;
	int m_nTrades;
};
typedef std::vector<cCurrencyPair> cVectorOfPairs;

class cCurrencyPairs
{
public:
	cCurrencyPair &Find(const char *pszTicker);
	cCurrencyPair & operator[] (int iIndex);
	int size() { return m_PairVector.size(); }

protected:
	 cVectorOfPairs m_PairVector;
};


#endif