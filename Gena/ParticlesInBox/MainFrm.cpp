// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ParticlesInBox.h"

#include "MainFrm.h"
#include "World.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
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
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_TimerID = SetTimer(1, 1000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnUpdateTime(CCmdUI *pCmdUI)
{
	CParticlesInBoxApp *pApp = (CParticlesInBoxApp *)AfxGetApp();
	CWorld *pWorld = pApp->GetWorld();
	if (!pWorld)
		return; // world is not created yet
	CString str;
	str.Format("Time = %.2f, step = %.1e", pWorld->Time, pWorld->DeltaTime);
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

void CMainFrame::OnUpdateNLeft(CCmdUI *pCmdUI)
{
	CParticlesInBoxApp *pApp = (CParticlesInBoxApp *)AfxGetApp();
	CWorld *pWorld = pApp->GetWorld();
	if (!pWorld)
		return; // world is not created yet
	CString str;
	str.Format("Left: %d (%4.1f%%), <V>=%7.3f", pWorld->nLeft, (pWorld->nLeft*100./pWorld->nParticles), pWorld->VaverageL);
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

void CMainFrame::OnUpdateNRight(CCmdUI *pCmdUI)
{
	CParticlesInBoxApp *pApp = (CParticlesInBoxApp *)AfxGetApp();
	CWorld *pWorld = pApp->GetWorld();
	if (!pWorld)
		return; // world is not created yet
	CString str;
	str.Format("Right: %d (%4.1f%%), <V>=%7.3f", pWorld->nRight, (pWorld->nRight*100./pWorld->nParticles), pWorld->VaverageR);
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


void CMainFrame::OnDestroy() 
{
	KillTimer(m_TimerID);
	CFrameWnd::OnDestroy();
	
}
