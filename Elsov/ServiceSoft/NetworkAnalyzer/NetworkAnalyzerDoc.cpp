// NetworkAnalyzerDoc.cpp : implementation of the CNetworkAnalyzerDoc class
//

#include "stdafx.h"
#include "NetworkAnalyzer.h"
#include "NetworkAnalyzerDoc.h"
#include "NetworkAnalyzerView.h"
#include "NetworkAnalyzerFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "IcmpSockets.h"

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerDoc

IMPLEMENT_DYNCREATE(CNetworkAnalyzerDoc, CNetworkDocAPI)

BEGIN_MESSAGE_MAP(CNetworkAnalyzerDoc, CNetworkDocAPI)
	//{{AFX_MSG_MAP(CNetworkAnalyzerDoc)
	ON_COMMAND(ID_TEST_PING, OnTestPing)
	ON_COMMAND(ID_TEST_BROADCASTPING, OnTestBroadcastPing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerDoc construction/destruction

CNetworkAnalyzerDoc::CNetworkAnalyzerDoc()
{
}

CNetworkAnalyzerDoc::~CNetworkAnalyzerDoc()
{
}

BOOL CNetworkAnalyzerDoc::OnNewDocument()
{
	if (!CNetworkDocAPI::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerDoc serialization

void CNetworkAnalyzerDoc::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerDoc diagnostics

#ifdef _DEBUG
void CNetworkAnalyzerDoc::AssertValid() const
{
	CNetworkDocAPI::AssertValid();
}

void CNetworkAnalyzerDoc::Dump(CDumpContext &dc) const
{
	CNetworkDocAPI::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerDoc commands

void CNetworkAnalyzerDoc::OnTestPing() 
{
	CIcmpSocket pinger;
	BOOL bSend = pinger.SendEchoRequest("192.168.77.218");
	BOOL bReceive = pinger.ReceiveEchoReply();

	bReceive = bReceive;
}

void CNetworkAnalyzerDoc::OnTestBroadcastPing() 
{
	CIcmpSocket pinger;
	pinger.EnableBroadcasting();
	BOOL bSend = pinger.SendBroadcastEchoRequest();
	BOOL bReceive = pinger.ReceiveEchoReply();

	bReceive = bReceive;
}

void CNetworkAnalyzerDoc::OnCloseDocument() 
{
	CNetworkDocAPI::OnCloseDocument();
}

