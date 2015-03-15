/* DisplayHostTraffic.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Jun 2006 - initial creation
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables

*/

#include "stdafx.h"
#include "netobserver.h"
#include "DisplayHostTraffic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficDoc

IMPLEMENT_DYNCREATE(CDisplayHostTrafficDoc, CNetworkDocAPI)

CDisplayHostTrafficDoc::CDisplayHostTrafficDoc()
{
}

BOOL CDisplayHostTrafficDoc::OnNewDocument()
{
	if (!CNetworkDocAPI::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDisplayHostTrafficDoc::~CDisplayHostTrafficDoc()
{
}

//virtual
void CDisplayHostTrafficDoc::Initialize(CHost *pHost)
{
	m_pNetObject = pHost;
	CDisplayHostTrafficFrame *pFrame = (CDisplayHostTrafficFrame *)m_pFrame;
	pFrame->Init(pHost);
}

BEGIN_MESSAGE_MAP(CDisplayHostTrafficDoc, CNetworkDocAPI)
	//{{AFX_MSG_MAP(CDisplayHostTrafficDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficDoc diagnostics

#ifdef _DEBUG
void CDisplayHostTrafficDoc::AssertValid() const
{
	CNetworkDocAPI::AssertValid();
}

void CDisplayHostTrafficDoc::Dump(CDumpContext &dc) const
{
	CNetworkDocAPI::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficDoc commands


/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficFrame

static UINT NavigationButtons[] =
{
	ID_VIEW_ZOOM_IN,
	ID_VIEW_ZOOM_OUT,
	ID_VIEW_ZOOM_IN_TRAFFIC,
	ID_VIEW_ZOOM_OUT_TRAFFIC,
	ID_VIEW_INITIALVIEW,
	ID_VIEW_ZOOM_RECTANGLE,
	ID_VIEW_DRAG_SCROLL
};

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_DATETIME,
	ID_INDICATOR_TRAFFIC
};

IMPLEMENT_DYNCREATE(CDisplayHostTrafficFrame, CNetworkFrameAPI)

CDisplayHostTrafficFrame::CDisplayHostTrafficFrame()
{
}

CDisplayHostTrafficFrame::~CDisplayHostTrafficFrame()
{
}

void CDisplayHostTrafficFrame::Init(CHost *pHost)
{
	m_pChart->Init(pHost);
	m_pChart->Refresh();
}

BEGIN_MESSAGE_MAP(CDisplayHostTrafficFrame, CNetworkFrameAPI)
	//{{AFX_MSG_MAP(CDisplayHostTrafficFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOM_RECTANGLE, OnViewZoomRectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_RECTANGLE, OnUpdateViewZoomRectangle)
	ON_COMMAND(ID_VIEW_INITIALVIEW, OnViewInitialView)
	ON_COMMAND(ID_VIEW_DRAG_SCROLL, OnViewDragScroll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAG_SCROLL, OnUpdateViewDragScroll)
	ON_COMMAND(ID_VIEW_ZOOM_IN_TRAFFIC, OnViewZoomInTraffic)
	ON_COMMAND(ID_VIEW_ZOOM_OUT_TRAFFIC, OnViewZoomOutTraffic)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DATETIME, OnUpdateDateTime)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TRAFFIC, OnUpdateTraffic)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficFrame message handlers

int CDisplayHostTrafficFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CNetworkFrameAPI::OnCreate(lpCreateStruct) == -1)
		return -1;
	
/*	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	} */

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	if (!m_NavigationToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_NavigationToolbar.LoadToolBar(IDR_HOST_TRAFFIC_TOOLBAR) ||
		!m_NavigationToolbar.SetButtons(NavigationButtons, sizeof(NavigationButtons)/sizeof(UINT)) )
	{
		TRACE0("Failed to create Navigation Bar\n");
		return -1;      // fail to create
	}
	m_NavigationToolbar.SetWindowText("Navigation Toolbar");


	m_NavigationToolbar.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_NavigationToolbar, AFX_IDW_DOCKBAR_RIGHT);
	
	return 0;
}

BOOL CDisplayHostTrafficFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_SplitterWnd.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_SplitterWnd.CreateView(0, 0,
		RUNTIME_CLASS(CDisplayHostTrafficCtrls), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterWnd.CreateView(0, 1,
		RUNTIME_CLASS(CDisplayHostTrafficView), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	m_pControlView = (CDisplayHostTrafficCtrls *)m_SplitterWnd.GetPane(0,0);
	m_pChartView = (CDisplayHostTrafficView *)m_SplitterWnd.GetPane(0,1);
	m_pChart = m_pChartView->GetChart();
	m_pControlView->SetChart(m_pChart);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficView message handlers

void CDisplayHostTrafficFrame::OnViewZoomIn() 
{
	m_pChartView->ZoomTimeIn();
}

void CDisplayHostTrafficFrame::OnViewZoomOut() 
{
	m_pChartView->ZoomTimeOut();
}


void CDisplayHostTrafficFrame::OnViewZoomInTraffic() 
{
	m_pChartView->ZoomTrafficIn();
}

void CDisplayHostTrafficFrame::OnViewZoomOutTraffic() 
{
	m_pChartView->ZoomTrafficOut();
}

void CDisplayHostTrafficFrame::OnViewZoomRectangle() 
{
	m_pChartView->SetZoomRectangleMode();
}

void CDisplayHostTrafficFrame::OnUpdateViewZoomRectangle(CCmdUI *pCmdUI) 
{
	m_pChartView->OnUpdateZoomRectangleMode(pCmdUI);
}

void CDisplayHostTrafficFrame::OnViewInitialView() 
{
	m_pChartView->InitialView();
}

void CDisplayHostTrafficFrame::OnViewDragScroll() 
{
	m_pChartView->SetDragScrollMode();
}

void CDisplayHostTrafficFrame::OnUpdateViewDragScroll(CCmdUI *pCmdUI) 
{
	m_pChartView->OnUpdateDragScrollMode(pCmdUI);
}

void CDisplayHostTrafficFrame::OnUpdateDateTime(CCmdUI *pCmdUI)
{
	char szStr[32];
	szStr[0] = 0;
	time_t t;
	LOGICAL bSuccess = m_pChartView->GetDateTimeUnderMouse(t);
	if (bSuccess)
	{
		struct tm *pTM = localtime(&t);
		strftime(szStr, sizeof(szStr)/sizeof(szStr[0]), "%d.%m.%Y %H:%M", pTM);
	}
	
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(szStr);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, size.cx);
	pCmdUI->SetText(szStr);
	pCmdUI->Enable(TRUE);
}

void CDisplayHostTrafficFrame::OnUpdateTraffic(CCmdUI *pCmdUI)
{
	char szStr[32];
	szStr[0] = 0;
	double traffic;
	LOGICAL bSuccess = m_pChartView->GetTrafficUnderMouse(traffic);
	if (bSuccess)
		sprintf(szStr, "%.1f", traffic);
	
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(szStr);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle, size.cx);
	pCmdUI->SetText(szStr);
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficCtrls

IMPLEMENT_DYNCREATE(CDisplayHostTrafficCtrls, CFormView)

CDisplayHostTrafficCtrls::CDisplayHostTrafficCtrls()
	: CFormView(CDisplayHostTrafficCtrls::IDD)
{
	//{{AFX_DATA_INIT(CDisplayHostTrafficCtrls)
	m_bTcpOut = FALSE;
	m_bTcpIn = FALSE;
	m_bUdpOut = FALSE;
	m_bUdpIn = FALSE;
	m_bIcmpOut = FALSE;
	m_bIcmpIn = FALSE;
	m_bIgmpOut = FALSE;
	m_bTotalOut = FALSE;
	m_bTotalTo = FALSE;
	m_FromTime = _T("");
	m_ToTime = _T("");
	//}}AFX_DATA_INIT
	m_pChart = NULL;
}

CDisplayHostTrafficCtrls::~CDisplayHostTrafficCtrls()
{
}

void CDisplayHostTrafficCtrls::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayHostTrafficCtrls)
	DDX_Check(pDX, IDC_TCP_FROM_CHECK, m_bTcpOut);
	DDX_Check(pDX, IDC_TCP_TO_CHECK, m_bTcpIn);
	DDX_Check(pDX, IDC_UDP_FROM_CHECK, m_bUdpOut);
	DDX_Check(pDX, IDC_UDP_TO_CHECK, m_bUdpIn);
	DDX_Check(pDX, IDC_ICMP_FROM_CHECK, m_bIcmpOut);
	DDX_Check(pDX, IDC_ICMP_TO_CHECK, m_bIcmpIn);
	DDX_Check(pDX, IDC_IGMP_FROM_CHECK, m_bIgmpOut);
	DDX_Check(pDX, IDC_TOTAL_FROM_CHECK, m_bTotalOut);
	DDX_Check(pDX, IDC_TOTAL_TO_CHECK, m_bTotalTo);
	DDX_Text(pDX, IDC_FROM_TIME_EDIT, m_FromTime);
	DDX_Text(pDX, IDC_TO_TIME_EDIT, m_ToTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayHostTrafficCtrls, CFormView)
	//{{AFX_MSG_MAP(CDisplayHostTrafficCtrls)
	ON_BN_CLICKED(IDC_ICMP_FROM_CHECK, OnIcmpFromCheck)
	ON_BN_CLICKED(IDC_ICMP_TO_CHECK, OnIcmpToCheck)
	ON_BN_CLICKED(IDC_IGMP_FROM_CHECK, OnIgmpFromCheck)
	ON_BN_CLICKED(IDC_TCP_FROM_CHECK, OnTcpFromCheck)
	ON_BN_CLICKED(IDC_TCP_TO_CHECK, OnTcpToCheck)
	ON_BN_CLICKED(IDC_TOTAL_FROM_CHECK, OnTotalFromCheck)
	ON_BN_CLICKED(IDC_TOTAL_TO_CHECK, OnTotalToCheck)
	ON_BN_CLICKED(IDC_UDP_FROM_CHECK, OnUdpFromCheck)
	ON_BN_CLICKED(IDC_UDP_TO_CHECK, OnUdpToCheck)
	ON_BN_CLICKED(IDC_OCTETS_RADIO, OnOctetsRadio)
	ON_BN_CLICKED(IDC_PACKETS_RADIO, OnPacketsRadio)
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, OnRefreshButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDisplayHostTrafficCtrls::UpdateFromToTimes()
{
	struct tm *t;
	time_t ttt= m_pChart->GetLatestTime();
	t = localtime(&ttt);
	m_ToTime = asctime(t);

	ttt = m_pChart->GetEarliestTime();
	t = localtime(&ttt);
	m_FromTime = asctime(t);

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficCtrls diagnostics

#ifdef _DEBUG
void CDisplayHostTrafficCtrls::AssertValid() const
{
	CFormView::AssertValid();
}

void CDisplayHostTrafficCtrls::Dump(CDumpContext &dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficCtrls message handlers

void CDisplayHostTrafficCtrls::OnInitialUpdate() 
{
	m_bTotalOut = m_pChart->NeedTotalSumOut();
	m_bTotalTo = m_pChart->NeedTotalSumIn();;
	m_bIcmpOut = m_pChart->NeedIcmpOut();
	m_bIcmpIn = m_pChart->NeedIcmpIn();
	m_bIgmpOut = m_pChart->NeedIgmpOut();
	m_bUdpOut = m_pChart->NeedUdpOut();
	m_bUdpIn = m_pChart->NeedUdpIn();
	m_bTcpOut = m_pChart->NeedTcpOut();
	m_bTcpIn = m_pChart->NeedTcpIn();
	if (m_pChart->IsInOctetsMode())
		CheckRadioButton(IDC_PACKETS_RADIO, IDC_OCTETS_RADIO, IDC_OCTETS_RADIO);
	else
		CheckRadioButton(IDC_PACKETS_RADIO, IDC_OCTETS_RADIO, IDC_PACKETS_RADIO);
	UpdateFromToTimes();
	
	CFormView::OnInitialUpdate();
}

void CDisplayHostTrafficCtrls::RefreshChart()
{
	CWaitCursor cursor;
	m_pChart->Refresh();
	UpdateFromToTimes();
}

void CDisplayHostTrafficCtrls::OnIcmpFromCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableIcmpOut(m_bIcmpOut);
}

void CDisplayHostTrafficCtrls::OnIcmpToCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableIcmpIn(m_bIcmpIn);
}

void CDisplayHostTrafficCtrls::OnIgmpFromCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableIgmpOut(m_bIgmpOut);
}

void CDisplayHostTrafficCtrls::OnTcpFromCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableTcpOut(m_bTcpOut);
}

void CDisplayHostTrafficCtrls::OnTcpToCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableTcpIn(m_bTcpIn);
}

void CDisplayHostTrafficCtrls::OnTotalFromCheck() 
{
	UpdateData();
	CWaitCursor cursor;
}

void CDisplayHostTrafficCtrls::OnTotalToCheck() 
{
	UpdateData();
	CWaitCursor cursor;
}

void CDisplayHostTrafficCtrls::OnUdpFromCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableUdpOut(m_bUdpOut);
}

void CDisplayHostTrafficCtrls::OnUdpToCheck() 
{
	UpdateData();
	CWaitCursor cursor;
	m_pChart->EnableUdpIn(m_bUdpIn);
}

void CDisplayHostTrafficCtrls::OnOctetsRadio() 
{
	m_pChart->SetOctetsMode(LOGICAL_TRUE);
}

void CDisplayHostTrafficCtrls::OnPacketsRadio() 
{
	m_pChart->SetOctetsMode(LOGICAL_FALSE);
}


void CDisplayHostTrafficCtrls::OnRefreshButton() 
{
	RefreshChart();
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficView

IMPLEMENT_DYNCREATE(CDisplayHostTrafficView, COpenGLView)

CDisplayHostTrafficView::CDisplayHostTrafficView()
{
}

CDisplayHostTrafficView::~CDisplayHostTrafficView()
{
}


BEGIN_MESSAGE_MAP(CDisplayHostTrafficView, COpenGLView)
	//{{AFX_MSG_MAP(CDisplayHostTrafficView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDisplayHostTrafficView::ZoomTimeIn()
{
	m_pHostTrafficRenderer->IncreaseZoomX(2);
}

void CDisplayHostTrafficView::ZoomTimeOut()
{
	m_pHostTrafficRenderer->IncreaseZoomX(0.5);
}

void CDisplayHostTrafficView::ZoomTrafficIn()
{
	m_pHostTrafficRenderer->IncreaseZoomY(2);
}

void CDisplayHostTrafficView::ZoomTrafficOut()
{
	m_pHostTrafficRenderer->IncreaseZoomY(0.5);
}

void CDisplayHostTrafficView::InitialView()
{
	m_pHostTrafficRenderer->SetDefaultView();
}

void CDisplayHostTrafficView::SetDragScrollMode()
{
	m_pHostTrafficRenderer->SetDragScrollMode();
}

void CDisplayHostTrafficView::OnUpdateDragScrollMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pHostTrafficRenderer->IsInDragScrollMode());
}

void CDisplayHostTrafficView::SetZoomRectangleMode()
{
	m_pHostTrafficRenderer->SetZoomRectangleMode();
}

void CDisplayHostTrafficView::OnUpdateZoomRectangleMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pHostTrafficRenderer->IsInZoomRectangleMode());
}

LOGICAL CDisplayHostTrafficView::GetMousePosition(int &x, int &y)
{
	CRect ClientRect(0,0,0,0);
	GetClientRect(&ClientRect);
	CPoint ScreenPoint;
	GetCursorPos(&ScreenPoint);
	ScreenToClient(&ScreenPoint);
	if (ScreenPoint.x < 0 || ScreenPoint.y < 0)
		return LOGICAL_FALSE;
	if (ScreenPoint.x > ClientRect.Width() || ScreenPoint.y > ClientRect.Height())
		return LOGICAL_FALSE;
	x = ScreenPoint.x;
	y = ScreenPoint.y;
	return LOGICAL_TRUE;
}

LOGICAL CDisplayHostTrafficView::GetDateTimeUnderMouse(time_t &t)
{
	int x, y;
	if (!GetMousePosition(x,y))
		return LOGICAL_FALSE;
	t = m_pHostTrafficRenderer->GetDateTimeAt(x);
	return LOGICAL_TRUE;
}

LOGICAL CDisplayHostTrafficView::GetTrafficUnderMouse(double &traffic)
{
	int x, y;
	if (!GetMousePosition(x,y))
		return LOGICAL_FALSE;
	traffic = m_pHostTrafficRenderer->GetTrafficAt(y);
	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficView drawing

void CDisplayHostTrafficView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

//virtual
void CDisplayHostTrafficView::CreateRenderer()
{
	m_pHostTrafficRenderer = new CHostTrafficRenderer();
}

//virtual
void CDisplayHostTrafficView::DestroyRenderer()
{
	if (m_pHostTrafficRenderer)
		delete m_pHostTrafficRenderer;
	m_pHostTrafficRenderer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficView diagnostics

#ifdef _DEBUG
void CDisplayHostTrafficView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CDisplayHostTrafficView::Dump(CDumpContext &dc) const
{
	COpenGLView::Dump(dc);
}
#endif //_DEBUG



