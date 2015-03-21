#ifndef __AMFXSessionCxxWrapper_h__
#define __AMFXSessionCxxWrapper_h__

#include <windows.h>
#include "amfxapi.h"

#define AMFXAPI_EXTERN(x) static x##_t x

class AMFXSessionCxxWrapper
{
public:
	AMFXSessionCxxWrapper();
	virtual ~AMFXSessionCxxWrapper();
	int create(const char *appName, bool enableChat = true);
	int connect(const AMFXSvrConnParams *params);
	void getKnownTickers();

	int subscribeAllTickers();
	int unsubscribeAllTickers();
	int subscribeTickers(const char *wanted[], int n);
	int unsubscribeTickers(const char *unwanted[], int n);

	int doStreamOperation(
					AMFX_DealSide side, const char *market, const char *amount,
					const char *rate, const char *limit, const char *ref,
					AMFXRProcessingStatus *err);

	int requestMarketQuote(const char *market, const char *amount,
						   enum AMFXRProcessingStatus *preCheckErr);

	int doMarketOperation(int requestId, enum AMFX_DealSide side,
						  const char *limit, const char *ref);

	int tradeAck(const char *ticket);
	int cancelRequest(int requestId);
	int sendChatMessage(const char *msg, size_t msgSize);

	int enableTradeHistory(bool isHistoryWanted);
	int enableAccountingInfo(bool isWanted);
	int enableOrdersList(bool isOrdersListWanted);

	int createSimpleOrder(const OrderCreateInfo *info,
						  AMFXRProcessingStatus *err);
	int createOCOOrders( const OrderCreateInfo *o1,
						 const OrderCreateInfo *o2,
						 AMFXRProcessingStatus *err);
	int createIfDoneOrders( const OrderCreateInfo *primary,
							const OrderCreateInfo *contingent,
							AMFXRProcessingStatus *err);
	int createIfDoneOCOOrders(const OrderCreateInfo *primary,
							  const OrderCreateInfo *oco1,
							  const OrderCreateInfo *oco2,
							  AMFXRProcessingStatus *err);

	int amendOrder(const OrderAmendInfo *changes,
				   AMFXRProcessingStatus *err);
	int amendIfDoneOrders(const OrderAmendInfo *primaryChanges,
						  const OrderAmendInfo *contingentChanges,
						  AMFXRProcessingStatus *err);
	int amendOCOOrders(const OrderAmendInfo *oco1Changes,
					   const OrderAmendInfo *oco2Changes,
					   AMFXRProcessingStatus *err);

	int amendIfDoneOCOOrders(const OrderAmendInfo *primaryChanges, 
							 const OrderAmendInfo *oco1changes,
							 const OrderAmendInfo *oco2changes,
							 AMFXRProcessingStatus *err);

	int cancelOrder(int orderId);


	virtual void onStatusMessage(const char *msg) =0;
	virtual int onConnectionEvent(AMFXConnectionStage stage, int stageFlag) = 0;
	virtual void onTickersListChange(size_t numOfTickers,
									 const TickerInfo* pTickers) =0;
	virtual void onTradeableTickersListChange(size_t numOfTickers,
											  const TradeableTickerInfo* ifs)=0;
	virtual void onQuotesChange(int nChanges, const QuoteChangeEvent *quotes)=0;
	virtual void onRequestProcessing(int requestId, AMFXRProcessingStatus st)=0;
	virtual void onTrade(int isOwned, const struct TradeInfo* ti) =0;
	virtual void onMarketDealQuote(int requestId,
								   const char *bid, const char *offer,
								   int timeout, double timestamp)=0;
	virtual void onChatMessage(int priority, const char *sender,
							   const char *msg, size_t msgSize,
							   double timestamp)=0;
	virtual void onChatMessageDelivery(int msgId, int status,
									   double timestamp)=0;

	virtual void onTradeHistoryChange(int nTrades,const TradeInfo* ti) = 0;
	virtual void onTradeHistoryRemove(int nTrades,
									  const char *removedTickets[]) = 0;

	virtual void onOrdersListChange(int nOrders, const OrderInfo* infos)=0;
	virtual void onOrdersListRemove(int nOrders, const int *removedOrders) =0;

	virtual void onAccountingBaseInfo(int nInfos,
									  const AMFXAccountBaseInfo *infos)=0;

	virtual void onOpenPositionsChange(const char *accountId,
									   int n, const OpenPosition*poss) =0;

	virtual void onNettingChange(const char *accountId,
								 int n, const CcyAmount*ccys) =0;

	virtual void onBalanceChange(const char *accountId,
								 int n, const CcyAmount*ccys) =0;

	static const char *processingStatusStr(AMFXRProcessingStatus status);

protected:
	AMFXSession session;
	static void __onStatusMessage  (void *self, const char *msg);
	static int  __onConnectionEvent(void *self,
									AMFXConnectionStage st, int flag);
	static void __onTickersListChange(void *self, size_t n,
									  const TickerInfo* infos);
	static void __onTradeableTickersListChange(void *self,
											   size_t n,
											   const TradeableTickerInfo* ifs);
	static void __onQuotesChange(void *self,
								 int n, const QuoteChangeEvent *qces);
	static void __onRequestProcessing(void *self,
									  int requestId, AMFXRProcessingStatus st);
	static void __onTrade(void *self, int isOwned, const TradeInfo* ti);
	static void __onMarketDealQuote(void *self, int requestId,
									const char *bid, const char *offer,
									int timeout, double timestamp);
	static void __onChatMessage(void *self, int priority,
								const char *sender,
								const char *msg, size_t msgSize,
								double timestamp);
	static void __onChatMessageDelivery(void *self,
										int msgId, int status, double ts);
	static void __onTradeHistoryChange(void *self,
									   int nTrades, const struct TradeInfo* ti);
	static void __onTradeHistoryRemove(void *self,
									   int n, const char *removedTickets[]);

	static void __onOrdersListChange(void *self,
									 int n, const struct OrderInfo* infos);
	static void __onOrdersListRemove(void *self,
									 int nOrders, const int *removedOrders);

	static void __onAccountingBaseInfo(void *self,
									   int n, const AMFXAccountBaseInfo*abi);

	static void __onOpenPositionsChange(void *self, const char *accountId,
										int n, const OpenPosition*poss);

	static void __onNettingChange(void *self, const char *accountId,
									int n, const CcyAmount*ccys);

	static void __onBalanceChange(void *self, const char *accountId,
									int n, const CcyAmount*ccys);
	

	AMFXAPI_EXTERN(amfxInitialize);
	AMFXAPI_EXTERN(amfxDestroySession);
	AMFXAPI_EXTERN(amfxConnect);
	AMFXAPI_EXTERN(amfxGetKnownTickers);

	AMFXAPI_EXTERN(amfxSubscribeAllTickers);
	AMFXAPI_EXTERN(amfxUnsubscribeAllTickers);
	AMFXAPI_EXTERN(amfxSubscribeTickers);
	AMFXAPI_EXTERN(amfxUnsubscribeTickers);

	AMFXAPI_EXTERN(amfxDoStreamOperation);
	AMFXAPI_EXTERN(amfxRequestMarketQuote);
	AMFXAPI_EXTERN(amfxDoMarketOperation);
	AMFXAPI_EXTERN(amfxCancelRequest);
	AMFXAPI_EXTERN(amfxTradeAck);
	AMFXAPI_EXTERN(amfxSendChatMessage);
	AMFXAPI_EXTERN(amfxEnableTradeHistory);

	AMFXAPI_EXTERN(amfxEnableAccountingInfo);

	AMFXAPI_EXTERN(amfxEnableOrdersList);
	AMFXAPI_EXTERN(amfxCreateSimpleOrder);
	AMFXAPI_EXTERN(amfxCreateOCOOrders);
	AMFXAPI_EXTERN(amfxCreateIfDoneOrders);
	AMFXAPI_EXTERN(amfxCreateIfDoneOCOOrders);

	AMFXAPI_EXTERN(amfxAmendOrder);
	AMFXAPI_EXTERN(amfxAmendIfDoneOrders);
	AMFXAPI_EXTERN(amfxAmendOCOOrders);
	AMFXAPI_EXTERN(amfxAmendIfDoneOCOOrders);

	AMFXAPI_EXTERN(amfxCancelOrder);

	AMFXAPI_EXTERN(amfxProcessingStatusStr);

	#undef AMFXAPI_EXTERN

	static HINSTANCE _hinstAPI;

public:
	static int initAPI(const char *dll="amfxapi");
	static void cleanupAPI();
};

#endif // not defined __AMFXSessionCxxWrapper_h__
