/*
 * amfxapi.h : main header file for the amfx API
*/

#ifndef __amfx_api_h__
#define __amfx_api_h__

#define AMFX_API_VERSION 0x010000

enum AMFXConnectionStage {
	AMFXCS_TRANSPORT,
	AMFXCS_LOGIN,
};

/*
	AMFX_OnConnectionEvent:
	stage == AMFXCS_TRANSPORT
			flag == 0 connection established, return value ignored

			flag == 1 connection lost (disconnected)
					  return non-zero to let API try to reconnect

	stage == AMFXCS_LOGIN
			flag == 0   login failed (API will not retry to login)
			            return value ignored
			flag == 1   login accepted, waiting for permissions
			flag == 100 client-server handshake finished

*/

enum AMFX_SystemStatus {
	AMFX_SYSTEM_READY = 0,
	AMFX_SYSTEM_STOPPED,
};

#define AMFX_MAX_CHAT_MESSAGE_SIZE 4000
#define AMFX_URGENT_MESSAGE_PRIORITY 10

enum AMFX_DealSide {
	AMFX_OP_BUY =0,
	AMFX_OP_SELL=1
};

enum QuoteType {
	QT_TRADABLE = 0,
	QT_MARKETCLOSED,
	QT_INDICATIVE
};

struct TickerInfo {
	const char *ticker;
	int ppu;
	const char *descr;
};

struct QuoteChangeEvent {
	const char *ticker;
	const char *bid, *offer;
	QuoteType  qtype;
	double	   timestamp;
};

struct TradeableTickerInfo {
	const char *ticker;
	unsigned minDealSize;
	unsigned maxDealSize;
	unsigned multiple;
};

enum AMFXOrderType {
	AMFX_OrderStop = 0,
	AMFX_OrderLimit = 1,
};

enum AMFXStopOrderSubtype {
	AMFX_StopBid =0,
	AMFX_StopOffer =1,
};

enum AMFXOrderStatus {
	AMFX_OrderActive = 0,
	AMFX_OrderWait =1,
	AMFX_OrderExecuted =2,
	AMFX_OrderCancelled =3,
};

struct OrderInfo {
	int orderId;
	const char *account;
	AMFXOrderStatus status;
	enum AMFX_DealSide side;
	const char *market;
	const char *amount;
	const char *price;
	enum AMFXOrderType orderType;
	enum AMFXStopOrderSubtype stopType;
	const char *goodTill;
	const char *ref;

	struct {
		int nOrders;
		int *orders;
	} primary, cancelOnExec, activateOnExec;

	struct {
		int nTickets;
		char **tickets;
	} fillTrades;

	const char *cuid, *muid;
	double ctime, mtime, etime;
};

struct OrderCreateInfo {
	enum AMFX_DealSide side;
	const char *market;
	const char *amount;
	const char *price;
	enum AMFXOrderType orderType;
	enum AMFXStopOrderSubtype stopType;
	const char *goodTill;
	const char *ref;
};

struct OrderAmendInfo {
	int orderId;
	const char *price;
	const char *amount;
	const char *goodTill;
};

enum AMFXRProcessingStatus {
	RPQ_NOT_CONNECTED =-1, /* that is */
	RQP_ACCEPTED =0, /* accepted for proccesing */
	RQP_REMAIN_AUTOCANCELLED=1, /* remining quantity can't be filled */
	RQP_TIMEDOUT=2,             /* deal by request timeout */
	RQP_INTERRUPTED=3,           /* deal by request interrupted */

	RQP_UNKNOWN_MARKET = 10,  /* unknown currency pair */
	RQP_WRONG_FORMAT=11,        /* wrong side, price or amount */
	RQP_INSUFFICIENT_MARGIN=12, /* insufficient credit */
	RPQ_DUPLICATED,             /* request with the same id exist */

	RQP_TRADESYSTEM_STOPPED = 20, /* system stopped */
	RQP_CLIENT_TRADES_STOPPED=21,   /* trades disable for the user */
	RQP_CLIENT_HAS_UNCONFIRMED=22,  /* must confirm trades first */

	RPQ_WRONG_USER_OR_ACCOUNT=30,

	RQP_CRYPTOSIGN_WRONG = 100, /* wrong cryptosign (wrong user, method, etc.)*/
	RQP_CRYPTOSIGN_MISMATCH=101,/* cryptosign doesn't match trade params */

	RQP_CANCELLED = 200,          /* request cancelled*/
	RQP_CANNOT_BE_CANCELLED = 201,/* request already filled/cancelled/unknown */

	RPQ_ORDER_REQUEST_DONE = 1000, /* order has been created/amended/cancelled*/
	RPQ_ORDER_CANNOT_BE_CHANGED=1001, /* amend/cancel order request failed */
	RPQ_ORDER_PRICE_MISMATCH=1002, /* order side-type-price */
	RPQ_ORDER_STOPSTYPES_UNSUPPORTED=1003,/*stops sellBid/buyOffer unsupported*/

	RPQ_UNEXPECTED_ERROR=100000, /* Unexpected system error */
};


struct TradeInfo {
	const char *accountId, *userId; /* 8 */
	int requestId; /* 12 */
	const char *ticket; /* 16 */
	const char *ccy1, *ccy2; /* 24 */
	const char *rate; /* 28 */
	const char *ccy1amount, *ccy2amount; /* 36 */
	const char *dealCcy, *dealAmount, *dealCommission; /* 48 */
	const char *valueDate; /* 52 */
	const char *ref; /* 56 */
	short isFullyFilled, isAggressor, isSigned, isSignRequired; /* 64 */
	double timestamp; /* 72 */
};

struct AMFXAccountBaseInfo {
	const char *accountId;
	const char *accountCcy;
	const char *leverage;
	const char *mrCallLevel;
	const char *accMethod;
};

struct OpenPosition {
	const char *ccy1, *ccy2;
	const char *ccy1amount, *ccy2amount;
};

struct CcyAmount {
	const char *ccy, *amount;
};

typedef int  (*AMFX_OnConnectionEvent)(void *customData,
									   enum AMFXConnectionStage stage,int flag);
typedef void (*AMFX_OnStatusMessage)(void *customData, const char *msg);
typedef void (*AMFX_OnSystemStatus)(void *customData,
									enum AMFX_SystemStatus systemStatus);
typedef void (*AMFX_OnQuotesChange)(void *customData,
									int nChanges,
									const struct QuoteChangeEvent *quotes);
typedef void (*AMFX_OnMarketDealQuote)(void *customData,
									   int requestId,
									   const char *bid, const char *offer,
									   int timeout, double timestamp);
typedef void (*AMFX_OnTickersListChange)(void *customData,
										 size_t numOfTickers,
										 const struct TickerInfo* pTickers);
typedef void (*AMFX_OnTradeableTickersListChange)(void *customData,
									size_t numOfTickers,
									const struct TradeableTickerInfo* pTickers);
typedef void (*AMFX_OnRequestProcessing)(void *customData,
										 int requestId,
										 enum AMFXRProcessingStatus status);
typedef void (*AMFX_OnTrade)(void *customData,
							 int isOwned, const struct TradeInfo* ti);
typedef void (*AMFX_OnChatMessage)(void *customData,
								   int priority, const char *sender,
								   const char *msg, size_t msgSize,
								   double timestamp);
typedef void (*AMFX_OnChatMessageDelivery)(void *customData,
										   int msgId, int isDelivered,
										   double timestamp);

typedef void (*AMFX_OnTradeHistoryChange)(void *customData,
										  int nTrades,
										  const struct TradeInfo* tinfos);
typedef void (*AMFX_OnTradeHistoryRemove)(void *customData,
										  int nTrades, const char *removed[]);

typedef void (*AMFX_OnOrdersListChange)(void *customData,
										int n, const struct OrderInfo* infos);

typedef void (*AMFX_OnOrdersListRemove)(void *customData,
										int n, const int *removedOrders);

typedef void (*AMFX_OnAccountingBaseInfo)(void *customData, 
										  int n,
										  const struct AMFXAccountBaseInfo*abi);

typedef void (*AMFX_OnOpenPositionsChange)(void *customData, 
										   const char *accountId,
										   int n,
										   const struct OpenPosition*poss);

typedef void (*AMFX_OnNettingChange)(void *customData, 
									 const char *accountId, 
									 int n, const struct CcyAmount*ccys);

typedef void (*AMFX_OnBalanceChange)(void *customData, 
									 const char *accountId, 
									 int n, const struct CcyAmount*ccys);

typedef void * AMFXSession;

struct AMFXClientInterface {
	/* version info */
	int apiVersion;
	int interfaceStructSize;

	/* misc */

	const char *appName;

	/* callbacks, set to NULL to ignore the call */
	AMFX_OnConnectionEvent            onConnectionEvent;
	AMFX_OnSystemStatus               onSystemStatus; /*NC */
	AMFX_OnStatusMessage              onStatusMessage;
	AMFX_OnTickersListChange          onTickersListChange;
	AMFX_OnQuotesChange               onQuotesChange;
	AMFX_OnTradeableTickersListChange onTradeableTickersListChange;
	AMFX_OnRequestProcessing          onRequestProcessing;
	AMFX_OnTrade                      onTrade;
	AMFX_OnMarketDealQuote			  onMarketDealQuote;
	AMFX_OnChatMessage                onChatMessage;
	AMFX_OnChatMessageDelivery        onChatMessageDelivery;
	AMFX_OnTradeHistoryChange		  onTradeHistoryChange;
	AMFX_OnTradeHistoryRemove		  onTradeHistoryRemove;
	AMFX_OnOrdersListChange           onOrdersListChange;
	AMFX_OnOrdersListRemove           onOrdersListRemove;
	AMFX_OnAccountingBaseInfo         onAccountingBaseInfo;
	AMFX_OnOpenPositionsChange        onOpenPositionsChange;
	AMFX_OnNettingChange              onNettingChange;
	AMFX_OnBalanceChange              onBalanceChange;
};

struct AMFXSvrConnParams {
	const char *server; int port;
	int useSSL, useCrypto;
	const char *accountId, *userId, *passwd;
};

#ifndef INSIDE_AMFXAPI
	#ifndef AMFXAPI_STATIC
		/* prefereable client way:
		   app uses GetProcAddress to obtain entry points
		*/
		#define AMFXAPIFDECL(tp,nm) typedef tp (*nm ## _t)
	#else
		#if defined(WIN32)
			#define AMFXAPI __declspec(dllimport)
		#else
			#define AMFXAPI
		#endif
		#define AMFXAPIFDECL(tp,nm) AMFXAPI tp nm
	#endif
#else
	#if defined(WIN32) && !defined(__AMFXAPI_CHECKMEM)
		#define AMFXAPI __declspec(dllexport)
	#else
		#define AMFXAPI
	#endif
	#define AMFXAPIFDECL(tp,nm) AMFXAPI tp nm
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
AMFXAPIFDECL(int,amfxAPIInitialize)(void);
AMFXAPIFDECL(void,amfxAPICleanup)(void);
#endif


AMFXAPIFDECL(AMFXSession,amfxInitialize)(const AMFXClientInterface *intfc,
									     const void *customData, int *err);

AMFXAPIFDECL(void,amfxDestroySession)(AMFXSession);
AMFXAPIFDECL(int,amfxConnect)(AMFXSession, const AMFXSvrConnParams *p);
AMFXAPIFDECL(void,amfxGetKnownTickers)(AMFXSession); 
AMFXAPIFDECL(void,amfxGetTradeableTickers)(AMFXSession);

AMFXAPIFDECL(int,amfxSubscribeAllTickers)(AMFXSession);
AMFXAPIFDECL(int,amfxUnsubscribeAllTickers)(AMFXSession);
AMFXAPIFDECL(int,amfxSubscribeTickers)(AMFXSession,
									   const char *wanted[], int nTickers);
AMFXAPIFDECL(int,amfxUnsubscribeTickers)(AMFXSession,
										 const char *unwanted[], int nTickers);

AMFXAPIFDECL(const char *,amfxProcessingStatusStr)(
	enum AMFXRProcessingStatus status);

AMFXAPIFDECL(int,amfxDoStreamOperation)(AMFXSession,
			 enum AMFX_DealSide side, const char *market, const char *amount,
			 const char *rate, const char *limit, const char *clientRef,
			 enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxRequestMarketQuote)(AMFXSession,
			 const char *market, const char *amount,
			 enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxDoMarketOperation) (AMFXSession,
										 int requestId,
										 enum AMFX_DealSide side,
										 const char *limit,
										 const char *clientRef);

AMFXAPIFDECL(int,amfxTradeAck)(AMFXSession, const char *ticket);

AMFXAPIFDECL(int,amfxCancelRequest)(AMFXSession, int requestId);

AMFXAPIFDECL(int,amfxSendChatMessage)(AMFXSession,
									  const char *msg, size_t msgSize);

AMFXAPIFDECL(int,amfxEnableTradeHistory)(AMFXSession, int flag);

AMFXAPIFDECL(int,amfxEnableAccountingInfo)(AMFXSession, int flag);

/* orders stuff */
AMFXAPIFDECL(int,amfxEnableOrdersList)(AMFXSession, int flag);

AMFXAPIFDECL(int,amfxCreateSimpleOrder)(AMFXSession,
									const struct OrderCreateInfo *info,
									enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxCreateOCOOrders)(AMFXSession,
									  const struct OrderCreateInfo *o1,
									  const struct OrderCreateInfo *o2,
									  enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxCreateIfDoneOrders)(AMFXSession,
										const struct OrderCreateInfo *primary,
									const struct OrderCreateInfo *contingent,
									enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxCreateIfDoneOCOOrders)(AMFXSession,
									const struct OrderCreateInfo *primary,
									const struct OrderCreateInfo *oco1,
									const struct OrderCreateInfo *oco2,
									enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxAmendOrder)(AMFXSession,
								 const struct OrderAmendInfo *changes,
								 enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxAmendIfDoneOrders)(AMFXSession,
							  const struct OrderAmendInfo *primaryChanges,
							  const struct OrderAmendInfo *contingentChanges,
							  enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxAmendOCOOrders)(AMFXSession,
							   const struct OrderAmendInfo *oco1Changes,
							   const struct OrderAmendInfo *oco2Changes,
							   enum AMFXRProcessingStatus *preCheckErr);

AMFXAPIFDECL(int,amfxAmendIfDoneOCOOrders)(AMFXSession, 
								 const struct OrderAmendInfo *primaryCnages, 
								 const struct OrderAmendInfo *oco1changes,
								 const struct OrderAmendInfo *oco2changes,
								 enum AMFXRProcessingStatus *preCheckErr);
/* TODO:
low-level order-management functions should be here
*/
AMFXAPIFDECL(int,amfxCancelOrder)(AMFXSession, int orderId);

#ifdef __cplusplus
}
#endif

#undef AMFXAPIFDECL
#undef AMFXAPI

#endif
