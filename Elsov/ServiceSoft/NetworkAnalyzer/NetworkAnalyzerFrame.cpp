/* NetworkAnalyzer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	15 May 2007 - object name in status bar

*/


#include "stdafx.h"
#include "NetworkAnalyzer.h"

#include "Network.h"
#include "NetworkAnalyzerFrame.h"
#include "NetworkAnalyzerView.h"
#include "WorldPlaneView.h"
#include "NetworkAnalyzerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerFrame

IMPLEMENT_DYNCREATE(CNetworkAnalyzerFrame, CNetworkFrameAPI)

BEGIN_MESSAGE_MAP(CNetworkAnalyzerFrame, CNetworkFrameAPI)
	//{{AFX_MSG_MAP(CNetworkAnalyzerFrame)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_SELECT, OnViewSelect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECT, OnUpdateViewSelect)
	ON_COMMAND(ID_VIEW_INITIALVIEW, OnViewInitialview)
	ON_COMMAND(ID_VIEW_DRAG_SCROLL, OnViewDragScroll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAG_SCROLL, OnUpdateViewDragScroll)
	ON_COMMAND(ID_VIEW_ZOOM_RECTANGLE, OnViewZoomRectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_RECTANGLE, OnUpdateViewZoomRectangle)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECT, OnUpdateObject)
END_MESSAGE_MAP()

static UINT PlaneViewButtons[] =
{
	ID_VIEW_ZOOM_IN,
	ID_VIEW_ZOOM_OUT,
	ID_VIEW_INITIALVIEW,
	ID_VIEW_SELECT,
	ID_VIEW_ZOOM_RECTANGLE,
	ID_VIEW_DRAG_SCROLL
};

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerFrame construction/destruction

CNetworkAnalyzerFrame::CNetworkAnalyzerFrame()
{
	// TODO: add member initialization code here
	
}

CNetworkAnalyzerFrame::~CNetworkAnalyzerFrame()
{
}

int CNetworkAnalyzerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CNetworkFrameAPI::OnCreate(lpCreateStruct) == -1)
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

	if (!m_WorldPlaneViewToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_WorldPlaneViewToolBar.LoadToolBar(IDR_WORLD_PLANE_VIEW_TOOLBAR) ||
		!m_WorldPlaneViewToolBar.SetButtons(PlaneViewButtons, sizeof(PlaneViewButtons)/sizeof(UINT)) )
	{
		TRACE0("Failed to create LayerVisibilityBar\n");
		return -1;      // fail to create
	}
	m_WorldPlaneViewToolBar.SetWindowText("World PlaneView Toolbar");


	m_WorldPlaneViewToolBar.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_WorldPlaneViewToolBar, AFX_IDW_DOCKBAR_RIGHT);
	return 0;
}

BOOL CNetworkAnalyzerFrame::PreCreateWindow(CREATESTRUCT &cs)
{
	if( !CNetworkFrameAPI::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerFrame diagnostics

#ifdef _DEBUG
void CNetworkAnalyzerFrame::AssertValid() const
{
	CNetworkFrameAPI::AssertValid();
}

void CNetworkAnalyzerFrame::Dump(CDumpContext &dc) const
{
	CNetworkFrameAPI::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerFrame message handlers


BOOL CNetworkAnalyzerFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_SplitterWnd.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_SplitterWnd.CreateView(0, 0,
		RUNTIME_CLASS(CNetworkAnalyzerView), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterWnd.CreateView(0, 1,
		RUNTIME_CLASS(CWorldPlaneView), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	m_pTreeView = (CNetworkAnalyzerView *)m_SplitterWnd.GetPane(0,0);
	m_pWorldPlaneView = (CWorldPlaneView *)m_SplitterWnd.GetPane(0,1);
	SetActiveView(m_pTreeView);

	return CNetworkFrameAPI::OnCreateClient(lpcs, pContext);
}

//virtual
void CNetworkAnalyzerFrame::NewHostCreated(CHost *pHost)
{
	m_pTreeView->NewHostCreated(pHost);
	m_pWorldPlaneView->NewHostCreated(pHost);
}

//virtual
void CNetworkAnalyzerFrame::HostChanged(CHost *pHost)
{
	m_pTreeView->HostChanged(pHost);
	m_pWorldPlaneView->HostChanged(pHost);
}

//virtual
void CNetworkAnalyzerFrame::NewSubnetCreated(CSubnet *pSubnet)
{
	m_pTreeView->NewSubnetCreated(pSubnet);
	m_pWorldPlaneView->NewSubnetCreated(pSubnet);
}

//virtual
void CNetworkAnalyzerFrame::SubnetChanged(CSubnet *pSubnet)
{
	m_pTreeView->SubnetChanged(pSubnet);
}

//virtual
void CNetworkAnalyzerFrame::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pTreeView->HostAddedToSubnet(pHost, pSubnet);
	m_pWorldPlaneView->HostAddedToSubnet(pHost, pSubnet);
}

//virtual
void CNetworkAnalyzerFrame::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pTreeView->HostRemovedFromSubnet(pHost, pSubnet);
}

//virtual
void CNetworkAnalyzerFrame::GuiCreated(IGuiAPI *pGui)
{
	m_pTreeView->GuiCreated(pGui);
	m_pWorldPlaneView->GuiCreated(pGui);
}

//virtual
void CNetworkAnalyzerFrame::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	m_pTreeView->ObjectSelectionChanged(pObject, pWhereChanged);
	m_pWorldPlaneView->ObjectSelectionChanged(pObject, pWhereChanged);
}

//virtual
BOOL CNetworkAnalyzerFrame::OnIdleProcess()
{
	BOOL bResult = 0;
	bResult |= m_pTreeView->OnIdleProcess();
	bResult |= m_pWorldPlaneView->OnIdleProcess();
	return bResult;
}

void CNetworkAnalyzerFrame::OnContextMenu(CWnd *pWnd, CPoint point) 
{
}

void CNetworkAnalyzerFrame::OnViewZoomIn() 
{
	m_pWorldPlaneView->ZoomIn();
}

void CNetworkAnalyzerFrame::OnViewZoomOut() 
{
	m_pWorldPlaneView->ZoomOut();
}

void CNetworkAnalyzerFrame::OnViewSelect() 
{
	m_pWorldPlaneView->SetSelectMode();
}

void CNetworkAnalyzerFrame::OnUpdateViewSelect(CCmdUI *pCmdUI) 
{
	m_pWorldPlaneView->OnUpdateSelectMode(pCmdUI);
}

void CNetworkAnalyzerFrame::OnViewInitialview() 
{
	m_pWorldPlaneView->InitialView();
}

void CNetworkAnalyzerFrame::OnViewDragScroll() 
{
	m_pWorldPlaneView->SetDragScrollMode();
}

void CNetworkAnalyzerFrame::OnUpdateViewDragScroll(CCmdUI *pCmdUI) 
{
	m_pWorldPlaneView->OnUpdateDragScrollMode(pCmdUI);
}

void CNetworkAnalyzerFrame::OnViewZoomRectangle() 
{
	m_pWorldPlaneView->SetZoomRectangleMode();
}

void CNetworkAnalyzerFrame::OnUpdateViewZoomRectangle(CCmdUI *pCmdUI) 
{
	m_pWorldPlaneView->OnUpdateZoomRectangleMode(pCmdUI);
}

void CNetworkAnalyzerFrame::OnUpdateObject(CCmdUI *pCmdUI)
{
	static char pszFullName[1024];
	pszFullName[0] = 0;
	CNetObject *pObject = m_pWorldPlaneView->GetObjectUnderMouse();
	if (pObject)
		pObject->GetFullName(pszFullName);
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(pszFullName);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, size.cx);
	pCmdUI->SetText(pszFullName);
	pCmdUI->Enable(TRUE);
}

