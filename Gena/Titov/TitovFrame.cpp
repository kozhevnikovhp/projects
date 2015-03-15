// TitovFrame.cpp : implementation of the CTitovFrame class
//

#include "stdafx.h"
#include "Titov.h"

#include "TitovFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitovFrame

IMPLEMENT_DYNCREATE(CTitovFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CTitovFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CTitovFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME, OnUpdateTime)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_N_LEFT, OnUpdateNLeft)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_N_RIGHT, OnUpdateNRight)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_TIME,
	ID_INDICATOR_N_LEFT,
	ID_INDICATOR_N_RIGHT
};

/////////////////////////////////////////////////////////////////////////////
// CTitovFrame construction/destruction

CTitovFrame::CTitovFrame()
{
	// TODO: add member initialization code here
	
}

CTitovFrame::~CTitovFrame()
{
}

int CTitovFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_TimerID = SetTimer(1, 500, NULL);

	return 0;
}

BOOL CTitovFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTitovFrame diagnostics

#ifdef _DEBUG
void CTitovFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CTitovFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTitovFrame message handlers

void CTitovFrame::OnUpdateTime(CCmdUI* pCmdUI)
{
	CTitovApp *pApp = (CTitovApp *)AfxGetApp();
	CString str = pApp->GetTimeString();
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(str);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, size.cx);
	pCmdUI->SetText(str);
	pCmdUI->Enable(TRUE);
}

void CTitovFrame::OnUpdateNLeft(CCmdUI* pCmdUI)
{
	CTitovApp *pApp = (CTitovApp *)AfxGetApp();
	CString str = pApp->GetNLeftString();
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(str);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle, size.cx);
	pCmdUI->SetText(str);
	pCmdUI->Enable(TRUE);
}

void CTitovFrame::OnUpdateNRight(CCmdUI* pCmdUI)
{
	CTitovApp *pApp = (CTitovApp *)AfxGetApp();
	CString str = pApp->GetNRightString();
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(str);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(3, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(3, nID, nStyle, size.cx);
	pCmdUI->SetText(str);
	pCmdUI->Enable(TRUE);
}


void CTitovFrame::OnDestroy() 
{
	KillTimer(m_TimerID);
	CFrameWnd::OnDestroy();
}
