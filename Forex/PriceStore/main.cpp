#include "stdio.h"
#include "conio.h"
#include "../NpbSession.h"
#include "../Misc.h"

class cStoreSession: public cNpbSession
{
public:
	cStoreSession() {};
	virtual void onStatusMessage(const char *msg);
	virtual int onConnectionEvent(AMFXConnectionStage stage, int stageFlag);
	virtual void onTickersListChange(size_t numOfTickers,
									 const TickerInfo* pTickers);
	virtual void onTradeableTickersListChange(size_t numOfTickers,
										 const TradeableTickerInfo* pTickers);
	virtual void onQuotesChange(int nChanges, const QuoteChangeEvent *quotes);
	virtual void onRequestProcessing(int requestId, AMFXRProcessingStatus st);
	virtual void onTrade(int isOwned, const struct TradeInfo* ti);
	virtual void onMarketDealQuote(int requestId,
								   const char *bid, const char *offer,
								   int timeout, double timestamp);
	virtual void onChatMessage(int priority, const char *sender,
							   const char *msg, size_t msgSize,
							   double timestamp);
	virtual void onChatMessageDelivery(int msgId, int isDelivered,
									   double timestamp);

	virtual void onTradeHistoryChange(int nTrades, const struct TradeInfo* ti);
	virtual void onTradeHistoryRemove(int nTrades,const char *removedTickets[]);

	virtual void onOrdersListChange(int nOrders, const struct OrderInfo* infos);
	virtual void onOrdersListRemove(int nOrders, const int *removedOrders);

	virtual void onAccountingBaseInfo(int nInfos,
									  const AMFXAccountBaseInfo *infos);

	virtual void onOpenPositionsChange(const char *accountId,
									   int n, const OpenPosition* poss);

	virtual void onNettingChange(const char *accountId,
								 int n, const CcyAmount*ccys);

	virtual void onBalanceChange(const char *accountId,
								 int n, const CcyAmount*ccys);
};

void
fprintfTrade(FILE *s, const struct TradeInfo* ti)
{
	fprintf(s , "Trade #%s account #%s user %s request %d\n",
			ti->ticket, ti->accountId, ti->userId, ti->requestId);
	fprintf(s, "CCY1: %s %s\n", ti->ccy1, ti->ccy1amount);
	fprintf(s, "CCY2: %s %s\n", ti->ccy2, ti->ccy2amount);
	fprintf(s, "rate: %s\n", ti->rate);
	fprintf(s, "VD:   %s\n", ti->valueDate);
	fprintf(s, "deal: %s %s\n", ti->dealCcy, ti->dealAmount);

	const char *ref = "(NULL)";
	if(ti->ref) {
		ref = ti->ref;
	}

	fprintf(s, "ref: %s\n", ref);
}

void
cStoreSession::onStatusMessage(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	fflush(stderr);
}

void
cStoreSession::onTickersListChange(size_t n, const TickerInfo* pTickers)
{
	fprintf(stderr, "onTickersListChange: %d info(s) received\n", n);
}

void
cStoreSession::onTradeableTickersListChange(size_t n,
											  const TradeableTickerInfo* ti)
{
	fprintf(stderr, "onTradeableTickersListChange: %d info(s) received\n", n);
	for( size_t i=0; i<n; i++)
	{
		printf("%s: min %d max %d mul %d\n", ti[i].ticker,
			ti[i].minDealSize, ti[i].maxDealSize, ti[i].multiple);
	}
}


void
cStoreSession::onQuotesChange(int n, const QuoteChangeEvent *qce)
{
	time(&m_LastQuoteTime);
	tm *pTM = getGMT(qce[0].timestamp);
	printf("\n\t%s", asctime(pTM));
	for( int i=0; i<n; i++)
	{
		cCurrencyPair &pair  = m_Pairs.Find(qce[i].ticker);
		pair.OnQuoteChanged(qce[i], pTM);
	}
}

int
cStoreSession::onConnectionEvent(AMFXConnectionStage stage, int flag)
{
	char buf[80];
	sprintf(buf, "onConnectionEvent: stage %d flag %d", stage, flag);
	onStatusMessage(buf);
	if (stage == AMFXCS_LOGIN && flag == 100 )
	{
		//this->enableTradeHistory(true);
		//this->enableOrdersList(true);
		this->enableAccountingInfo(true);
		//this->subscribeAllTickers();
		this->subscribeTickers(PSZ_TICKERS, sizeof(PSZ_TICKERS)/sizeof(PSZ_TICKERS[0]));
	}
	return m_bWantsReconnect;
}

void
cStoreSession::onRequestProcessing(int requestId, AMFXRProcessingStatus st)
{
	printf("RequestId %d status %d\n", requestId, st);
}

void
cStoreSession::onTrade(int isOwned, const struct TradeInfo* ti)
{
	printf("Got %s trade event\n", (isOwned)?"OWNED":"NOT OWNED");
	fprintfTrade(stdout, ti);
	
}

void
cStoreSession::onChatMessage(int p, const char *sender,
							   const char *msg, size_t msgSize,
							   double ts)
{
	return;
	printf("Got chat message priority %d from %s at %f\n", p, sender, ts);
	fwrite(msg, 1, msgSize, stdout);
	fputs("\n=============\n", stdout);
}

void
cStoreSession::onChatMessageDelivery(int msgId, int isDelivered,
									   double timestamp)
{
	printf("Message %d ",msgId);
	if( isDelivered )
		printf("has been delivered at %f\n", timestamp);
	else{
		printf("could not be delivered\n");
	}
}

void
cStoreSession::onMarketDealQuote(int requestId,
								   const char *bid, const char *offer,
								   int timeout, double timestamp)
{
	// look at market deal example
}


void
cStoreSession::onTradeHistoryChange(int nChanges, const struct TradeInfo* ti)
{
	printf("Got %d trade changes:\n", nChanges);
	for( int i=0; i< nChanges; i++){
		fprintfTrade(stdout, ti+i);
	}
}
void
cStoreSession::onTradeHistoryRemove(int nChanges, const char *removed[])
{
	printf("Got %d trade removes:\n", nChanges);
	for( int i=0; i< nChanges; i++){
		printf("Trade #%s\n", removed[i]);
	}
}

void
cStoreSession::onOrdersListChange(int nChanges, const struct OrderInfo* oi)
{
	fprintf(stderr, "Got %d order changes:\n", nChanges);
	for( int i=0; i< nChanges; i++){
		char *status = "unknown";
		switch( oi[i].status ){
		case AMFX_OrderActive:
			status = "active";
			break;
		case AMFX_OrderWait:
			status = "wait";
			break;
		case AMFX_OrderExecuted:
			status = "executed";
			break;
		case AMFX_OrderCancelled:
			status = "cancelled";
			break;
		default:
			fprintf(stderr, "Unknown order status %d!!!", oi[i].status);
		}
		fprintf(stderr, "Order # %d (%s): %s %s %s %s %s @ %s",
				oi[i].orderId, status,
				(oi[i].side==AMFX_OP_BUY)?"BUY":"SELL",
				(oi[i].orderType==AMFX_OrderStop)?"STOP":"LIMIT",
				(oi[i].stopType==AMFX_StopBid)?"BID":"OFFER",
				oi[i].market, oi[i].amount, oi[i].price );
		if( oi[i].activateOnExec.nOrders ){
			fprintf(stderr, " CTG");
			for(int j=0; j<oi[i].activateOnExec.nOrders; j++){
				fprintf(stderr, " #%d", oi[i].activateOnExec.orders[j]);
			}
		}
		if( oi[i].cancelOnExec.nOrders ){
			fprintf(stderr, " OCO");
			for(int j=0; j<oi[i].cancelOnExec.nOrders; j++){
				fprintf(stderr, " #%d", oi[i].cancelOnExec.orders[j]);
			}
		}
		fprintf(stderr, " |%s|\n", oi[i].ref);
	}
}
void
cStoreSession::onOrdersListRemove(int nChanges, const int *removed)
{
	printf("Got %d orders removes:\n", nChanges);
	for( int i=0; i< nChanges; i++){
		printf("Order #%d removed\n", removed[i]);
	}
}

void
cStoreSession::onAccountingBaseInfo(int nInfos,
									  const AMFXAccountBaseInfo *infos)
{
	printf("Got base %d info(s) for account(s):\n", nInfos);
	for( int i=0; i< nInfos; i++){
		printf("BaseAccountingInfo: accid %s ccy %s "
			   "lev %s mrcall %s meth %s\n",
			   infos[i].accountId, infos[i].accountCcy,
			   infos[i].leverage, infos[i].mrCallLevel, infos[i].accMethod);
	}
}

void
cStoreSession::onOpenPositionsChange(const char *accountId,
									  int n, const OpenPosition* ps)
{
	for (int i=0; i<n; i++){
		printf("OpenPos: %s %s %s %s\n",
			   ps[i].ccy1, ps[i].ccy2, ps[i].ccy1amount, ps[i].ccy2amount);
	}
}

void
cStoreSession::onNettingChange(const char *accountId,
								 int n, const CcyAmount*ccys)
{
	for (int i=0; i<n; i++){
		printf("Netting CcyAmount: %s %s\n", ccys[i].ccy, ccys[i].amount);
	}
}

void
cStoreSession::onBalanceChange(const char *accountId,
								 int n, const CcyAmount*ccys)
{
	for (int i=0; i<n; i++){
		printf("Balance CcyAmount: %s %s\n", ccys[i].ccy, ccys[i].amount);
	}
}


void showOrdersOps(cStoreSession *session)
{
	AMFXRProcessingStatus err;
	int rqid;

	int mdfOid;

	OrderCreateInfo simpleOrder;
	simpleOrder.side = AMFX_OP_BUY;
	simpleOrder.market="EURUSD";
	simpleOrder.amount="100000";
	simpleOrder.price="1.8000";
	simpleOrder.orderType = AMFX_OrderLimit;
	// simpleOrder.stopType = AMFX_StopOffer -- doesn't required for limit
	simpleOrder.goodTill="";
	simpleOrder.ref = "my best order";
	
	rqid = session->createSimpleOrder(&simpleOrder, &err);
	printf("createSimpleOrder gives %d\n", rqid);

	return;

	mdfOid = -1;
	getch();
	printf("Enter simple order # to modify it: ");
	fflush(stdout);
	scanf("%d", &mdfOid);
	if( mdfOid > 0 ){
#if 0
		OrderAmendInfo ai;
		ai.orderId = mdfOid;
		ai.price = "1.2100";
		ai.amount = "200000";
		ai.goodTill = "";
		printf("Amend order #%d rqid %d\n",
			   mdfOid, session->amendOrder(&ai,&err));
#else
		OrderAmendInfo p, o1, o2;
		p.orderId = 199;
		o1.orderId = 227;
		o2.orderId = 228;
		p.amount = o1.amount = o2.amount = "200000";
		p.price = "1.2020";
		o1.price = "1.2220";
		o2.price = "1.1940";
		p.goodTill = o1.goodTill = o2.goodTill = "";
		printf("Amend order rqid %d\n",
			   session->amendOCOOrders(&o1,&o2,&err));

#endif
		_getch();
	}

	OrderCreateInfo primary, oco1, oco2;
	primary.side = AMFX_OP_BUY;
	primary.market="EURUSD";
	primary.amount="100000";
	primary.price="1.2000";
	primary.orderType = AMFX_OrderLimit;
	// simpleOrder.stopType = AMFX_StopOffer -- doesn't required for limit
	primary.goodTill="";
	primary.ref = "my primary order";

	oco1.side = AMFX_OP_SELL;
	oco1.market="EURUSD";
	oco1.amount="100000";
	oco1.price="1.2200";
	oco1.orderType = AMFX_OrderLimit;
	// simpleOrder.stopType = AMFX_StopOffer -- doesn't required for limit
	oco1.goodTill="";
	oco1.ref = "my take profit order";

	oco2.side = AMFX_OP_SELL;
	oco2.market="EURUSD";
	oco2.amount="100000";
	oco2.price="1.1920";
	oco2.orderType = AMFX_OrderStop;
	oco2.stopType = AMFX_StopOffer;
	oco2.goodTill="";
	oco2.ref = "my stop loss order";

	rqid = session->createIfDoneOCOOrders(&primary, &oco1, &oco2, &err);
	printf("createIfDoneOCOOrders rqid %d\n", rqid);
}

void cancelOrders(cStoreSession *session)
{
	getch();
	for(;;) {
		int oid;
		printf("\nEnter order # to cancel (q to finish): ");
		fflush(stdout);
		if( !scanf("%d", &oid ) ) break;
		int rqid = session->cancelOrder(oid);
		printf("\nCancelling order #%d rqid %d\n", oid, rqid);
	}
}

void showChatOps(cStoreSession *session)
{
	char *msg = "Hi, dealer!\nCould you please send me some money?\n";
	session->sendChatMessage(msg, strlen(msg));

	getch();
}

int main(int argc, char **argv)
{
	AMFXRProcessingStatus err;
	int rqid;

	(void)err;
	(void)rqid;


	if( AMFXSessionCxxWrapper::initAPI() != 0 ){
		printf("Can't load library\n");
		return 1;
	}

	const char * host = "195.128.74.8";
	int port = 3110;
	const char * account, *user, *passwd;
	
	if ( argc == 4 )
	{
		account = argv[1];
		user = argv[2];
		passwd = argv[3];
	}
	else if ( argc == 5 )
	{
		account = argv[1];
		user = argv[2];
		passwd = argv[3];
		host = argv[4];
	}
	else 
	{
		printf("Usage is:\n%s <account> <user> <password> [host]\n", argv[0]);
		return 1;
	}

	printf("Will connect to %s:%d\n", host, port);

	AMFXSvrConnParams cparams = { host, port, 1, 0, account, user, passwd};

	cStoreSession *pSession = new cStoreSession();
	pSession->create("BasicAPIdemo");

	while (1)
	{
		bool bBreak = false;
		if (pSession->needReconnect())
		{
			printf("Reconnecting...\n");
			pSession->connect(&cparams);
		}
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
		Sleep(1000);
		if (_kbhit())
			break;
	}

// 	showChatOps(session);
// 	showOrdersOps(session);
// 	cancelOrders(session);

// 	_getch();
// 	rqid = session->doStreamOperation(AMFX_OP_BUY, "EURGBP", "100000",
// 									  "0.6700", "0.7", "Buy Trade", &err);
// 	_getch();
// 	rqid = session->doStreamOperation(AMFX_OP_SELL, "EURGBP", "100000",
// 									  "0.6700", "0.5", "Sell Trade", &err);
//  	_getch();

// 	showOrdersOps(session);
// 	_getch();


//	char *msg = "Hi, dealer!\nCould you please send me some money?\n";
//	session->sendChatMessage(msg, strlen(msg));
//	_getch();


	delete pSession;
	printf("**** Session destroyed, exiting...\n");
	Sleep(2000);

	return 0;
}