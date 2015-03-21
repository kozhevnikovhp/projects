
#ifdef WIN32
#ifdef __AMFXAPI_CHECKMEM
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	#define AMFXAPI_STATIC
#endif

#include <windows.h>
#else //POSIX
#include <stdlib.h>
#endif

#include "amfxapicxx.h"



AMFXSessionCxxWrapper::AMFXSessionCxxWrapper() :
	session(NULL)
{
}

AMFXSessionCxxWrapper::~AMFXSessionCxxWrapper()
{
	if( session ){
		amfxDestroySession(session);
		session = NULL;
	}
}

int 
AMFXSessionCxxWrapper::create(const char *appName, bool enableChat)
{
	AMFXClientInterface i;
	i.apiVersion = AMFX_API_VERSION;
	i.interfaceStructSize = sizeof(AMFXClientInterface);

	i.appName = appName;
	i.onStatusMessage = __onStatusMessage;
	i.onConnectionEvent = __onConnectionEvent;
	i.onTickersListChange = __onTickersListChange;
	i.onTradeableTickersListChange = __onTradeableTickersListChange;
	i.onQuotesChange = __onQuotesChange;
	i.onRequestProcessing = __onRequestProcessing;
	i.onTrade = __onTrade;
	i.onMarketDealQuote = __onMarketDealQuote;
	i.onChatMessageDelivery = __onChatMessageDelivery;
	i.onTradeHistoryChange = __onTradeHistoryChange;
	i.onTradeHistoryRemove = __onTradeHistoryRemove;
	i.onOrdersListChange = __onOrdersListChange;
	i.onOrdersListRemove = __onOrdersListRemove;
	i.onAccountingBaseInfo = __onAccountingBaseInfo;
	i.onOpenPositionsChange = __onOpenPositionsChange;
	i.onNettingChange = __onNettingChange;
	i.onBalanceChange = __onBalanceChange;

	if( enableChat ){
		i.onChatMessage = __onChatMessage;
	}else{
		i.onChatMessage = NULL;
	}

	int err;
	session = amfxInitialize(&i, this, &err);
	return err;
}

int 
AMFXSessionCxxWrapper::subscribeAllTickers()
{
	return amfxSubscribeAllTickers(session);
}

int 
AMFXSessionCxxWrapper::unsubscribeAllTickers()
{
	return amfxUnsubscribeAllTickers(session);
}

int 
AMFXSessionCxxWrapper::subscribeTickers(const char *wanted[], int n)
{
	return amfxSubscribeTickers(session, wanted, n);
}

int 
AMFXSessionCxxWrapper::unsubscribeTickers(const char *unwanted[], int n)
{
	return amfxUnsubscribeTickers(session, unwanted, n);
}

int 
AMFXSessionCxxWrapper::connect(const AMFXSvrConnParams *params)
{
	return amfxConnect(session, params);
}

void
AMFXSessionCxxWrapper::getKnownTickers()
{
	amfxGetKnownTickers(session);
}

int
AMFXSessionCxxWrapper::doStreamOperation(
	AMFX_DealSide side, const char *market, const char *amount,
    const char *rate, const char *limit, const char *ref,
    AMFXRProcessingStatus *err)
{
	return amfxDoStreamOperation(session, side, market, amount, rate, limit, ref, err);
}

int
AMFXSessionCxxWrapper::requestMarketQuote(
	const char *market, const char *amount,
    AMFXRProcessingStatus *err)
{
	return amfxRequestMarketQuote(session, market, amount, err);
}


int
AMFXSessionCxxWrapper::doMarketOperation(int requestId, AMFX_DealSide side,
										 const char *limit, const char *ref)
{
	return amfxDoMarketOperation(session, requestId, side, limit, ref);
}


int
AMFXSessionCxxWrapper::cancelRequest(int requestId)
{
	return amfxCancelRequest(session, requestId);
}

int
AMFXSessionCxxWrapper::tradeAck(const char *ticket)
{
	return amfxTradeAck(session, ticket);
}

int
AMFXSessionCxxWrapper::sendChatMessage(const char *m, size_t sz)
{
	return amfxSendChatMessage(session, m, sz);
}
int
AMFXSessionCxxWrapper::enableTradeHistory(bool flag)
{
	return amfxEnableTradeHistory(session, flag?1:0);
}

int
AMFXSessionCxxWrapper::enableAccountingInfo(bool flag)
{
	return amfxEnableAccountingInfo(session, flag?1:0);
}

int
AMFXSessionCxxWrapper::enableOrdersList(bool wanted)
{
	return amfxEnableOrdersList(session, wanted?1:0);
}


int
AMFXSessionCxxWrapper::createSimpleOrder(const OrderCreateInfo *info,
										 AMFXRProcessingStatus *err)
{
	return amfxCreateSimpleOrder(session, info, err);
}


int
AMFXSessionCxxWrapper::createOCOOrders( const OrderCreateInfo *o1,
										const OrderCreateInfo *o2,
										AMFXRProcessingStatus *err)
{
	return amfxCreateOCOOrders(session, o1, o2, err);
}

int
AMFXSessionCxxWrapper::createIfDoneOrders(const OrderCreateInfo *p,
										  const OrderCreateInfo *c,
										  AMFXRProcessingStatus *err)
{
	return amfxCreateIfDoneOrders(session, p, c, err);
}

int
AMFXSessionCxxWrapper::createIfDoneOCOOrders(const OrderCreateInfo *p,
							  const OrderCreateInfo *oco1,
							  const OrderCreateInfo *oco2,
							  AMFXRProcessingStatus *err)
{
	return amfxCreateIfDoneOCOOrders(session, p, oco1, oco2, err);
}

int
AMFXSessionCxxWrapper::amendOrder(const OrderAmendInfo *changes,
								  AMFXRProcessingStatus *err)
{
	return amfxAmendOrder(session, changes, err);
}

int
AMFXSessionCxxWrapper::amendIfDoneOrders(const OrderAmendInfo *c1,
										 const OrderAmendInfo *c2,
										 AMFXRProcessingStatus *err)
{
	return amfxAmendIfDoneOrders(session, c1, c2, err);
}

int
AMFXSessionCxxWrapper::amendOCOOrders(const OrderAmendInfo *o1,
									  const OrderAmendInfo *o2,
									  AMFXRProcessingStatus *err)
{
	return amfxAmendOCOOrders(session, o1, o2, err);
}

int
AMFXSessionCxxWrapper::amendIfDoneOCOOrders(const OrderAmendInfo *p,
											const OrderAmendInfo *o1,
											const OrderAmendInfo *o2,
											AMFXRProcessingStatus *err)
{
	return amfxAmendIfDoneOCOOrders(session, p, o1, o2, err);
}

int
AMFXSessionCxxWrapper::cancelOrder(int orderId)
{
	return amfxCancelOrder(session, orderId);
}


void
AMFXSessionCxxWrapper::__onStatusMessage(void *self, const char *msg)
{
	((AMFXSessionCxxWrapper*)self)->onStatusMessage(msg);
}

int
AMFXSessionCxxWrapper::__onConnectionEvent(void *self,
										   AMFXConnectionStage stage, int flag)
{
	return ((AMFXSessionCxxWrapper*)self)->onConnectionEvent(stage, flag);
}

void 
AMFXSessionCxxWrapper::__onTickersListChange(void *self, size_t n,
											 const TickerInfo* infos)
{
	((AMFXSessionCxxWrapper*)self)->onTickersListChange(n, infos);
}

void 
AMFXSessionCxxWrapper::__onTradeableTickersListChange(void *self,
									size_t n, const TradeableTickerInfo* i)
{
	((AMFXSessionCxxWrapper*)self)->onTradeableTickersListChange(n, i);
}


void
AMFXSessionCxxWrapper::__onQuotesChange(void *self,
										int n, const QuoteChangeEvent *quotes)
{
	((AMFXSessionCxxWrapper*)self)->onQuotesChange(n, quotes);
}

void
AMFXSessionCxxWrapper::__onRequestProcessing(void *self,
											 int r, AMFXRProcessingStatus st)
{
	((AMFXSessionCxxWrapper*)self)->onRequestProcessing(r, st);
}


void
AMFXSessionCxxWrapper::__onTrade(void *self, int isOwned, const TradeInfo* ti)
{
	((AMFXSessionCxxWrapper*)self)->onTrade(isOwned, ti);
}

void AMFXSessionCxxWrapper::__onMarketDealQuote(void *self, int rqid,
												const char *b, const char *o,
												int timeout, double timestamp)
{
	((AMFXSessionCxxWrapper*)self)->onMarketDealQuote(rqid,
															 b, o,
															 timeout,timestamp);
}

void
AMFXSessionCxxWrapper::__onChatMessage(void *self, int priority, 
									   const char *sender, const char *msg,
									   size_t msgSize, double timestamp)
{
	((AMFXSessionCxxWrapper*)self)->onChatMessage(priority, sender, msg,
														 msgSize, timestamp);
}

void
AMFXSessionCxxWrapper::__onChatMessageDelivery(void *self,
											   int msgid, int status,
											   double timestamp)
{
	((AMFXSessionCxxWrapper*)self)->onChatMessageDelivery(
		msgid, status, timestamp);
}

void
AMFXSessionCxxWrapper::__onTradeHistoryChange(void *self,
											  int nTrades, const TradeInfo* ti)
{
	((AMFXSessionCxxWrapper*)self)->onTradeHistoryChange(nTrades, ti);
}

void
AMFXSessionCxxWrapper::__onTradeHistoryRemove(void *self,
											  int nTrades, const char*rm[])
{
	((AMFXSessionCxxWrapper*)self)->onTradeHistoryRemove(nTrades, rm);
}

void
AMFXSessionCxxWrapper::__onOrdersListChange(void *self,
											int n, const OrderInfo* oi)
{
	((AMFXSessionCxxWrapper*)self)->onOrdersListChange(n, oi);
}

void
AMFXSessionCxxWrapper::__onOrdersListRemove(void *self, int n, const int *rm)
{
	((AMFXSessionCxxWrapper*)self)->onOrdersListRemove(n, rm);
}

void
AMFXSessionCxxWrapper::__onAccountingBaseInfo(void *self,
										int n, const AMFXAccountBaseInfo *i)
{
	((AMFXSessionCxxWrapper*)self)->onAccountingBaseInfo(n, i);
}

void
AMFXSessionCxxWrapper::__onOpenPositionsChange(void *self, const char *accid,
											   int n, const OpenPosition *ps)
{
	((AMFXSessionCxxWrapper*)self)->onOpenPositionsChange(accid, n, ps);
}

void
AMFXSessionCxxWrapper::__onNettingChange(void *self, const char *accid,
										 int n, const CcyAmount *ccys)
{
	((AMFXSessionCxxWrapper*)self)->onNettingChange(accid, n, ccys);
}

void
AMFXSessionCxxWrapper::__onBalanceChange(void *self, const char *accid,
										 int n, const CcyAmount *ccys)
{
	((AMFXSessionCxxWrapper*)self)->onBalanceChange(accid, n, ccys);
}

const char *
AMFXSessionCxxWrapper::processingStatusStr(AMFXRProcessingStatus status)
{
	return amfxProcessingStatusStr(status);
}

///////////////////////////////////////////////////////
/////////////// not AMFXAPI_STATIC staff


#ifndef AMFXAPI_STATIC
#define INSTANTIATE(x) x##_t AMFXSessionCxxWrapper:: x =NULL
	INSTANTIATE(amfxInitialize);
	INSTANTIATE(amfxDestroySession);
	INSTANTIATE(amfxConnect);
	INSTANTIATE(amfxGetKnownTickers);

	INSTANTIATE(amfxSubscribeAllTickers);
	INSTANTIATE(amfxUnsubscribeAllTickers);
	INSTANTIATE(amfxSubscribeTickers);
	INSTANTIATE(amfxUnsubscribeTickers);

	INSTANTIATE(amfxDoStreamOperation);
	INSTANTIATE(amfxRequestMarketQuote);
	INSTANTIATE(amfxDoMarketOperation);
	INSTANTIATE(amfxTradeAck);
	INSTANTIATE(amfxCancelRequest);
	INSTANTIATE(amfxSendChatMessage);
	INSTANTIATE(amfxEnableTradeHistory);

	INSTANTIATE(amfxEnableAccountingInfo);

	INSTANTIATE(amfxEnableOrdersList);
	INSTANTIATE(amfxCreateSimpleOrder);
	INSTANTIATE(amfxCreateOCOOrders);
	INSTANTIATE(amfxCreateIfDoneOrders);
	INSTANTIATE(amfxCreateIfDoneOCOOrders);

	INSTANTIATE(amfxAmendOrder);
	INSTANTIATE(amfxAmendIfDoneOrders);
	INSTANTIATE(amfxAmendOCOOrders);
	INSTANTIATE(amfxAmendIfDoneOCOOrders);

	INSTANTIATE(amfxCancelOrder);

	INSTANTIATE(amfxProcessingStatusStr);

#undef INSTANTIATE
	HINSTANCE AMFXSessionCxxWrapper::_hinstAPI = NULL;

int 
AMFXSessionCxxWrapper::initAPI(const char *dll)
{
	int rv =0;

	if( _hinstAPI )
		return 0;

	_hinstAPI = LoadLibrary(dll);

	if( !_hinstAPI ) return GetLastError();

#define LOADSYMBOL(x) x=(x##_t) GetProcAddress(_hinstAPI, #x); \
					  if(x == NULL){rv = GetLastError(); goto getout; }


	LOADSYMBOL(amfxInitialize)
	LOADSYMBOL(amfxDestroySession)
	LOADSYMBOL(amfxConnect)
	LOADSYMBOL(amfxGetKnownTickers)

	LOADSYMBOL(amfxSubscribeAllTickers)
	LOADSYMBOL(amfxUnsubscribeAllTickers)
	LOADSYMBOL(amfxSubscribeTickers)
	LOADSYMBOL(amfxUnsubscribeTickers)

	LOADSYMBOL(amfxDoStreamOperation)
	LOADSYMBOL(amfxRequestMarketQuote)
	LOADSYMBOL(amfxDoMarketOperation)
	LOADSYMBOL(amfxCancelRequest)
	LOADSYMBOL(amfxTradeAck)
	LOADSYMBOL(amfxSendChatMessage)
	LOADSYMBOL(amfxEnableTradeHistory)

	LOADSYMBOL(amfxEnableAccountingInfo)

	LOADSYMBOL(amfxEnableOrdersList)
	LOADSYMBOL(amfxCreateSimpleOrder)
	LOADSYMBOL(amfxCreateOCOOrders)
	LOADSYMBOL(amfxCreateIfDoneOrders)
	LOADSYMBOL(amfxCreateIfDoneOCOOrders)

	LOADSYMBOL(amfxAmendOrder)
	LOADSYMBOL(amfxAmendIfDoneOrders);
	LOADSYMBOL(amfxAmendOCOOrders);
	LOADSYMBOL(amfxAmendIfDoneOCOOrders);

	LOADSYMBOL(amfxCancelOrder)

	LOADSYMBOL(amfxProcessingStatusStr)

#undef LOADSYMBOL

getout:
	if( rv ) cleanupAPI();
	return rv;
}

void
AMFXSessionCxxWrapper::cleanupAPI()
{
	if( _hinstAPI ){
		FreeLibrary(_hinstAPI);
	}
	_hinstAPI = NULL;
	amfxInitialize = NULL;
	amfxDestroySession = NULL;
	amfxConnect = NULL;
	amfxGetKnownTickers = NULL;

	amfxSubscribeAllTickers = NULL;
	amfxUnsubscribeAllTickers = NULL;
	amfxSubscribeTickers = NULL;
	amfxUnsubscribeTickers = NULL;

	amfxDoStreamOperation = NULL;
	amfxTradeAck = NULL;
	amfxCancelRequest = NULL;
	amfxSendChatMessage = NULL;

	amfxEnableTradeHistory=NULL;

	amfxEnableOrdersList=NULL;
	amfxCreateSimpleOrder=NULL;
	amfxCreateOCOOrders=NULL;
	amfxCreateIfDoneOrders=NULL;
	amfxCreateIfDoneOCOOrders=NULL;

	amfxAmendOrder=NULL;
	amfxAmendIfDoneOrders = NULL;
	amfxAmendOCOOrders = NULL;
	amfxAmendIfDoneOCOOrders = NULL;

	amfxCancelOrder=NULL;

	amfxProcessingStatusStr = NULL;
}

#else
#ifndef _WIN32
int 
AMFXSessionCxxWrapper::initAPI(const char *dll)
{
	(void)dll;
	amfxAPIInitialize();
	return 0;
}
void
AMFXSessionCxxWrapper::cleanupAPI()
{
	amfxAPICleanup();
}
#endif
#endif

