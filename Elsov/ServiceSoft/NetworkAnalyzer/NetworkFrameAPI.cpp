// NetworkAnalyzerFrame.cpp : implementation of the CNetworkAnalyzerFrame class
//

#include "stdafx.h"
#include "NetworkAnalyzer.h"

#include "NetworkFrameAPI.h"
#include "NetworkDocAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNetworkSplitter


IMPLEMENT_DYNAMIC(CNetworkSplitter, CSplitterWnd)

CNetworkSplitter::CNetworkSplitter()
{
}

CNetworkSplitter::~CNetworkSplitter()
{
}

BEGIN_MESSAGE_MAP(CNetworkSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CNetworkSplitter)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNetworkSplitter::OnContextMenu(CWnd *pWnd, CPoint point) 
{
	pWnd->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd->m_hWnd, MAKELONG(LOWORD(point.x), LOWORD(point.y)));
}

#ifdef _DEBUG
void CNetworkSplitter::AssertValid() const
{
	CSplitterWnd::AssertValid();
}

void CNetworkSplitter::Dump(CDumpContext &dc) const
{
	CSplitterWnd::Dump(dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerFrameAPI

IMPLEMENT_DYNCREATE(CNetworkFrameAPI, CFrameWnd)

BEGIN_MESSAGE_MAP(CNetworkFrameAPI, CFrameWnd)
	//{{AFX_MSG_MAP(CNetworkFrameAPI)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkFrameAPI construction/destruction

CNetworkFrameAPI::CNetworkFrameAPI()
{
	m_pDoc = NULL;
}

CNetworkFrameAPI::~CNetworkFrameAPI()
{
}

int CNetworkFrameAPI::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

BOOL CNetworkFrameAPI::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkFrameAPI diagnostics

#ifdef _DEBUG
void CNetworkFrameAPI::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CNetworkFrameAPI::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetworkFrameAPI message handlers


BOOL CNetworkFrameAPI::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	m_pDoc = (CNetworkDocAPI *)GetActiveDocument();
	m_pDoc->SetFrame(this);
	return TRUE;
}

