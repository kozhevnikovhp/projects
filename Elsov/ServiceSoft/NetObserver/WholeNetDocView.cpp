/* WholeNetDocView.cpp - implementation file for all MFC doc-view architecture WholeNet functionality.

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	16 May 2007 - context menu
	20 Oct 2007 - OpenGL tree view instead of MFC tree view
	15 Nov 2007 - global IGuiAPI pointer instead of member variables
	15 Jan 2008 - splitting MFC- and NonMFC-visitors to avoid multithread problems with MFC-controls
	19 Jan 2008 - merged view-doc-frame files into this one
*/


#include "stdafx.h"
#include "NetObserver.h"
#include "WholeNetDocView.h"
#include "Network.h"
#include "Groups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CWholeNetDoc

IMPLEMENT_DYNCREATE(CWholeNetDoc, CNetworkDocAPI)

BEGIN_MESSAGE_MAP(CWholeNetDoc, CNetworkDocAPI)
	//{{AFX_MSG_MAP(CWholeNetDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWholeNetDoc construction/destruction

CWholeNetDoc::CWholeNetDoc()
{
	// TODO: add one-time construction code here

}

CWholeNetDoc::~CWholeNetDoc()
{
}

BOOL CWholeNetDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CWholeNetDoc diagnostics

#ifdef _DEBUG
void CWholeNetDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWholeNetDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWholeNetDoc commands

BOOL CWholeNetDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CWholeNetView

IMPLEMENT_DYNCREATE(CWholeNetView, COpenGLView)

CWholeNetView::CWholeNetView()
{
}

CWholeNetView::~CWholeNetView()
{
}

BEGIN_MESSAGE_MAP(CWholeNetView, COpenGLView)
	//{{AFX_MSG_MAP(CWholeNetView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CNetObject *CWholeNetView::GetObjectUnderMouse()
{
	CRect ClientRect(0,0,0,0);
	GetClientRect(&ClientRect);
	CPoint ScreenPoint;
	GetCursorPos(&ScreenPoint);
	ScreenToClient(&ScreenPoint);
	if (ScreenPoint.x < 0 || ScreenPoint.y < 0)
		return NULL;
	if (ScreenPoint.x > ClientRect.Width() || ScreenPoint.y > ClientRect.Height())
		return NULL;

	return GetWorldRenderer()->GetObjectAt(ScreenPoint.x, ScreenPoint.y);
}

void CWholeNetView::RefreshView(LOGICAL bRefreshAnyway)
{
	GetWorldRenderer()->RefreshView(bRefreshAnyway);
}

/////////////////////////////////////////////////////////////////////////////
// CWholeNetTreeView

IMPLEMENT_DYNCREATE(CWholeNetTreeView, CWholeNetView)

CWholeNetTreeView::CWholeNetTreeView()
{
	m_pRenderer = NULL;
}

CWholeNetTreeView::~CWholeNetTreeView()
{
}


BEGIN_MESSAGE_MAP(CWholeNetTreeView, CWholeNetView)
	//{{AFX_MSG_MAP(CWholeNetTreeView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_SORTITEMS_BYIPADDRESS, OnViewSortItemsByIpAddress)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTITEMS_BYIPADDRESS, OnUpdateViewSortItemsByIpAddress)
	ON_COMMAND(ID_VIEW_SORTITEMS_BYNAME, OnViewSortItemsByName)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTITEMS_BYNAME, OnUpdateViewSortItemsByName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWholeNetTreeView drawing

void CWholeNetTreeView::OnDraw(CDC *pDC)
{
	CDocument *pDoc = GetDocument();
}

//virtual
void CWholeNetTreeView::CreateRenderer()
{
	m_pRenderer = new CWholeNetTreeRenderer();
}

//virtual
void CWholeNetTreeView::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}

void CWholeNetTreeView::NewHostCreated(CHost *pHost)
{
	m_pRenderer->HostCreated(pHost);
}

void CWholeNetTreeView::HostChanged(CHost *pHost)
{
	m_pRenderer->HostChanged(pHost);
}

void CWholeNetTreeView::NewSubnetCreated(CSubnet *pSubnet)
{
	m_pRenderer->SubnetCreated(pSubnet);
}

void CWholeNetTreeView::SubnetChanged(CSubnet *pSubnet)
{
	m_pRenderer->SubnetChanged(pSubnet);
}

void CWholeNetTreeView::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pRenderer->HostAddedToSubnet(pHost, pSubnet);
}

void CWholeNetTreeView::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
}
	
void CWholeNetTreeView::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	m_pRenderer->ObjectSelectionChanged(pObject, pWhereChanged);
}

void CWholeNetTreeView::NetObjectIgnored(CNetObject *pObject)
{
	m_pRenderer->NetObjectIgnored(pObject);
}

void CWholeNetTreeView::NetObjectHidden(CNetObject *pObject)
{
	m_pRenderer->NetObjectHidden(pObject);
}

void CWholeNetTreeView::ShowHiddenObjects()
{
	m_pRenderer->ShowHiddenObjects();
}

void CWholeNetTreeView::ShowIgnoredObjects()
{
	m_pRenderer->ShowIgnoredObjects();
}

BOOL CWholeNetTreeView::OnIdleNonMFCThread()
{
	if (!m_pRenderer)
		return FALSE;
	return m_pRenderer->OnIdleNonMFCThread();
}

void CWholeNetTreeView::InitialView()
{
	if (m_pRenderer)
		m_pRenderer->SetDefaultView();
}

void CWholeNetTreeView::SetSelectMode()
{
	m_pRenderer->SetSelectMode();	
}

void CWholeNetTreeView::OnUpdateSelectMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pRenderer->IsInSelectMode());
}

void CWholeNetTreeView::SetDragScrollMode()
{
	m_pRenderer->SetDragScrollMode();	
}

void CWholeNetTreeView::OnUpdateDragScrollMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pRenderer->IsInDragScrollMode());
}

void CWholeNetTreeView::SetZoomRectangleMode()
{
	m_pRenderer->SetZoomRectangleMode();	
}

void CWholeNetTreeView::OnUpdateZoomRectangleMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pRenderer->IsInZoomRectangleMode());
}

/////////////////////////////////////////////////////////////////////////////
// CWholeNetTreeView diagnostics

#ifdef _DEBUG
void CWholeNetTreeView::AssertValid() const
{
	CWholeNetView::AssertValid();
}

void CWholeNetTreeView::Dump(CDumpContext &dc) const
{
	CWholeNetView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CWholeNetTreeView message handlers

void CWholeNetTreeView::OnContextMenu(CWnd *pWnd, CPoint point) 
{
	CPoint ClientPoint(point);
	ScreenToClient(&ClientPoint);
	CNetObject *pObject = m_pRenderer->GetObjectAt(ClientPoint.x, ClientPoint.y);
	if (pObject)
	{
		pObject->GuiContextMenu(point.x, point.y);
	}
	else
	{
		CMenu menu;
		BOOL b1 = menu.CreatePopupMenu();
		BOOL b2 = menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_VIEW_SHOW_IGNORED_OBJECTS, "Show deleted objects");
		b2 = menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_VIEW_SHOW_HIDDEN_OBJECTS, "Show hidden objects");
		b2 = menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_VIEW_SORTITEMS_BYNAME, "by name");
		b2 = menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_VIEW_SORTITEMS_BYIPADDRESS, "by IP-address");
		BOOL b3 = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, GetApplication()->m_pMainWnd, NULL);
		menu.DestroyMenu();
	}
}

void CWholeNetTreeView::OnViewSortItemsByIpAddress() 
{
	m_pRenderer->SortByIpAddress();
}

void CWholeNetTreeView::OnUpdateViewSortItemsByIpAddress(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsSortedByIpAddress());
}

void CWholeNetTreeView::OnViewSortItemsByName() 
{
	m_pRenderer->SortByName();
}

void CWholeNetTreeView::OnUpdateViewSortItemsByName(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsSortedByName());
}

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView

IMPLEMENT_DYNCREATE(CWholeNetPlaneView, CWholeNetView)

CWholeNetPlaneView::CWholeNetPlaneView()
{
	m_pRenderer = NULL;
}

CWholeNetPlaneView::~CWholeNetPlaneView()
{
}


BEGIN_MESSAGE_MAP(CWholeNetPlaneView, CWholeNetView)
	//{{AFX_MSG_MAP(CWorldPlaneView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView drawing

void CWholeNetPlaneView::OnDraw(CDC *pDC)
{
}

//virtual
void CWholeNetPlaneView::CreateRenderer()
{
	m_pRenderer = new CWholeNetPlaneRenderer();
}

//virtual
void CWholeNetPlaneView::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}

void CWholeNetPlaneView::NewHostCreated(CHost *pHost)
{
	m_pRenderer->HostCreated(pHost);
}

void CWholeNetPlaneView::HostChanged(CHost *pHost)
{
	m_pRenderer->HostChanged(pHost);
}

void CWholeNetPlaneView::NewSubnetCreated(CSubnet *pSubnet)
{
	m_pRenderer->MakeDirty();
}

void CWholeNetPlaneView::SubnetChanged(CSubnet *pSubnet)
{
	m_pRenderer->MakeDirty();
}

void CWholeNetPlaneView::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pRenderer->HostAddedToSubnet(pHost, pSubnet);
}

void CWholeNetPlaneView::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
}
	
void CWholeNetPlaneView::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	m_pRenderer->ObjectSelectionChanged(pObject, pWhereChanged);
}

void CWholeNetPlaneView::NetObjectIgnored(CNetObject *pObject)
{
	m_pRenderer->NetObjectIgnored(pObject);
}

void CWholeNetPlaneView::NetObjectHidden(CNetObject *pObject)
{
	m_pRenderer->NetObjectHidden(pObject);
}

void CWholeNetPlaneView::ShowHiddenObjects()
{
	m_pRenderer->ShowHiddenObjects();
}

void CWholeNetPlaneView::ShowIgnoredObjects()
{
	m_pRenderer->ShowIgnoredObjects();
}

BOOL CWholeNetPlaneView::OnIdleNonMFCThread()
{
	if (!m_pRenderer)
		return FALSE;
	return m_pRenderer->OnIdleNonMFCThread();
}

void CWholeNetPlaneView::ZoomIn()
{
	if (m_pRenderer)
		m_pRenderer->ZoomIn();
}

void CWholeNetPlaneView::ZoomOut()
{
	if (m_pRenderer)
		m_pRenderer->ZoomOut();
}

void CWholeNetPlaneView::InitialView()
{
	if (m_pRenderer)
		m_pRenderer->SetDefaultView();
}

void CWholeNetPlaneView::SetSelectMode()
{
	m_pRenderer->SetSelectMode();	
}

void CWholeNetPlaneView::OnUpdateSelectMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pRenderer->IsInSelectMode());
}

void CWholeNetPlaneView::SetDragScrollMode()
{
	m_pRenderer->SetDragScrollMode();	
}

void CWholeNetPlaneView::OnUpdateDragScrollMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pRenderer->IsInDragScrollMode());
}

void CWholeNetPlaneView::SetZoomRectangleMode()
{
	m_pRenderer->SetZoomRectangleMode();	
}

void CWholeNetPlaneView::OnUpdateZoomRectangleMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pRenderer->IsInZoomRectangleMode());
}

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView diagnostics

#ifdef _DEBUG
void CWholeNetPlaneView::AssertValid() const
{
	CWholeNetView::AssertValid();
}

void CWholeNetPlaneView::Dump(CDumpContext &dc) const
{
	CWholeNetView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView message handlers

void CWholeNetPlaneView::OnContextMenu(CWnd *pWnd, CPoint point) 
{
	CPoint ClientPoint(point);
	ScreenToClient(&ClientPoint);
	CNetObject *pObject = m_pRenderer->GetObjectAt(ClientPoint.x, ClientPoint.y);
	if (pObject)
	{
		pObject->GuiContextMenu(point.x, point.y);
	}
	else
	{
		CMenu menu;
		BOOL b1 = menu.CreatePopupMenu();
		BOOL b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_VIEW_SHOW_IGNORED_OBJECTS, "Show deleted objects");
		b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_VIEW_SHOW_HIDDEN_OBJECTS, "Show hidden objects");
		BOOL b3 = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, GetApplication()->m_pMainWnd, NULL);
		menu.DestroyMenu();
	}
	m_pRenderer->MakeDirty();
}


/////////////////////////////////////////////////////////////////////////////
// CWholeNetFrame

IMPLEMENT_DYNCREATE(CWholeNetFrame, CNetworkFrameAPI)

BEGIN_MESSAGE_MAP(CWholeNetFrame, CNetworkFrameAPI)
	//{{AFX_MSG_MAP(CWholeNetFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_DRAG_SCROLL, OnViewDragScroll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAG_SCROLL, OnUpdateViewDragScroll)
	ON_COMMAND(ID_VIEW_INITIALVIEW, OnViewInitialView)
	ON_COMMAND(ID_VIEW_SELECT, OnViewSelect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECT, OnUpdateViewSelect)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOM_RECTANGLE, OnViewZoomRectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_RECTANGLE, OnUpdateViewZoomRectangle)
	ON_COMMAND(ID_TREE_VIEW_SELECT, OnViewTreeViewSelect)
	ON_UPDATE_COMMAND_UI(ID_TREE_VIEW_SELECT, OnUpdateViewTreeViewSelect)
	ON_COMMAND(ID_TREE_VIEW_DRAG_SCROLL, OnViewTreeViewDragScroll)
	ON_UPDATE_COMMAND_UI(ID_TREE_VIEW_DRAG_SCROLL, OnUpdateViewTreeViewDragScroll)
	ON_COMMAND(ID_TREE_VIEW_INITIALVIEW, OnTreeViewInitialView)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECT, OnUpdateObject)
END_MESSAGE_MAP()

static UINT TreeViewButtons[] =
{
	ID_TREE_VIEW_INITIALVIEW,
	ID_TREE_VIEW_SELECT,
	ID_TREE_VIEW_DRAG_SCROLL
};

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
// CWholeNetFrame construction/destruction

CWholeNetFrame::CWholeNetFrame()
{
	// TODO: add member initialization code here
	
}

CWholeNetFrame::~CWholeNetFrame()
{
}

BOOL CWholeNetFrame::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CNetworkFrameAPI::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWholeNetFrame diagnostics

#ifdef _DEBUG
void CWholeNetFrame::AssertValid() const
{
	CNetworkFrameAPI::AssertValid();
}

void CWholeNetFrame::Dump(CDumpContext& dc) const
{
	CNetworkFrameAPI::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWholeNetFrame message handlers

int CWholeNetFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CNetworkFrameAPI::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Tree view toolbar
	EnableDocking(CBRS_ALIGN_ANY);
	if (!m_TreeViewToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_TreeViewToolBar.LoadToolBar(IDR_WORLD_TREE_VIEW_TOOLBAR) ||
		!m_TreeViewToolBar.SetButtons(TreeViewButtons, sizeof(TreeViewButtons)/sizeof(UINT)) )
	{
		TRACE0("Failed to create TreeView bar\n");
		return -1;      // fail to create
	}
	m_TreeViewToolBar.SetWindowText("Tree View Toolbar");
	m_TreeViewToolBar.EnableDocking(CBRS_ALIGN_LEFT);
	DockControlBar(&m_TreeViewToolBar, AFX_IDW_DOCKBAR_LEFT);

	// Plane view toolbar
	if (!m_PlaneViewToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_PlaneViewToolBar.LoadToolBar(IDR_WORLD_PLANE_VIEW_TOOLBAR) ||
		!m_PlaneViewToolBar.SetButtons(PlaneViewButtons, sizeof(PlaneViewButtons)/sizeof(UINT)) )
	{
		TRACE0("Failed to create Plane View bar\n");
		return -1;      // fail to create
	}
	m_PlaneViewToolBar.SetWindowText("PlaneView Toolbar");
	m_PlaneViewToolBar.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_PlaneViewToolBar, AFX_IDW_DOCKBAR_RIGHT);
	
	return 0;
}

BOOL CWholeNetFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_SplitterWnd.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_SplitterWnd.CreateView(0, 0,
		RUNTIME_CLASS(CWholeNetTreeView), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterWnd.CreateView(0, 1,
		RUNTIME_CLASS(CWholeNetPlaneView), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	m_pTreeView = (CWholeNetTreeView *)m_SplitterWnd.GetPane(0,0);
	m_pPlaneView = (CWholeNetPlaneView *)m_SplitterWnd.GetPane(0,1);

	return TRUE;
}

//virtual
void CWholeNetFrame::NewHostCreated(CHost *pHost)
{
	m_pTreeView->NewHostCreated(pHost);
	m_pPlaneView->NewHostCreated(pHost);
}

//virtual
void CWholeNetFrame::HostChanged(CHost *pHost)
{
	m_pTreeView->HostChanged(pHost);
	m_pPlaneView->HostChanged(pHost);
}

//virtual
void CWholeNetFrame::NewSubnetCreated(CSubnet *pSubnet)
{
	m_pTreeView->NewSubnetCreated(pSubnet);
	m_pPlaneView->NewSubnetCreated(pSubnet);
}

//virtual
void CWholeNetFrame::SubnetChanged(CSubnet *pSubnet)
{
	m_pTreeView->SubnetChanged(pSubnet);
	m_pPlaneView->SubnetChanged(pSubnet);
}

//virtual
void CWholeNetFrame::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pTreeView->HostAddedToSubnet(pHost, pSubnet);
	m_pPlaneView->HostAddedToSubnet(pHost, pSubnet);
}

//virtual
void CWholeNetFrame::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pTreeView->HostRemovedFromSubnet(pHost, pSubnet);
	m_pPlaneView->HostRemovedFromSubnet(pHost, pSubnet);
}

//virtual
void CWholeNetFrame::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	m_pTreeView->ObjectSelectionChanged(pObject, pWhereChanged);
	m_pPlaneView->ObjectSelectionChanged(pObject, pWhereChanged);
}

//virtual
void CWholeNetFrame::GroupChanged(CGroup *pGroup)
{
	if (pGroup->GetType() == APPEARANCE_GROUP_INDEX)
	{
		m_pTreeView->AppearanceGroupChanged();
		m_pPlaneView->AppearanceGroupChanged();
	}
}

//virtual
void CWholeNetFrame::NetObjectIgnored(CNetObject *pObject)
{
	m_pTreeView->NetObjectIgnored(pObject);
	m_pPlaneView->NetObjectIgnored(pObject);
}

//virtual
void CWholeNetFrame::NetObjectHidden(CNetObject *pObject)
{
	m_pTreeView->NetObjectHidden(pObject);
	m_pPlaneView->NetObjectHidden(pObject);
}

//virtual
void CWholeNetFrame::ShowHiddenObjects()
{
	m_pTreeView->ShowHiddenObjects();
	m_pPlaneView->ShowHiddenObjects();
}

//virtual
void CWholeNetFrame::ShowIgnoredObjects()
{
	m_pTreeView->ShowIgnoredObjects();
	m_pPlaneView->ShowIgnoredObjects();
}

//virtual
BOOL CWholeNetFrame::OnIdleNonMFCThread()
{
	BOOL bResult = 0;
	bResult |= m_pTreeView->OnIdleNonMFCThread();
	m_pTreeView->RefreshView(LOGICAL_FALSE);
	bResult |= m_pPlaneView->OnIdleNonMFCThread();
	m_pPlaneView->RefreshView(LOGICAL_FALSE);
	return bResult;
}

void CWholeNetFrame::OnViewDragScroll() 
{
	m_pPlaneView->SetDragScrollMode();
}

void CWholeNetFrame::OnUpdateViewDragScroll(CCmdUI *pCmdUI) 
{
	m_pPlaneView->OnUpdateDragScrollMode(pCmdUI);
}

void CWholeNetFrame::OnViewInitialView() 
{
	m_pPlaneView->InitialView();
}

void CWholeNetFrame::OnViewSelect() 
{
	m_pPlaneView->SetSelectMode();
}

void CWholeNetFrame::OnUpdateViewSelect(CCmdUI *pCmdUI) 
{
	m_pPlaneView->OnUpdateSelectMode(pCmdUI);
}

void CWholeNetFrame::OnViewZoomIn() 
{
	m_pPlaneView->ZoomIn();
}

void CWholeNetFrame::OnViewZoomOut() 
{
	m_pPlaneView->ZoomOut();
}

void CWholeNetFrame::OnViewZoomRectangle() 
{
	m_pPlaneView->SetZoomRectangleMode();
}

void CWholeNetFrame::OnUpdateViewZoomRectangle(CCmdUI *pCmdUI) 
{
	m_pPlaneView->OnUpdateZoomRectangleMode(pCmdUI);
}

void CWholeNetFrame::OnViewTreeViewSelect() 
{
	m_pTreeView->SetSelectMode();
}

void CWholeNetFrame::OnUpdateViewTreeViewSelect(CCmdUI *pCmdUI) 
{
	m_pTreeView->OnUpdateSelectMode(pCmdUI);
}

void CWholeNetFrame::OnViewTreeViewDragScroll() 
{
	m_pTreeView->SetDragScrollMode();
}

void CWholeNetFrame::OnUpdateViewTreeViewDragScroll(CCmdUI *pCmdUI) 
{
	m_pTreeView->OnUpdateDragScrollMode(pCmdUI);
}

void CWholeNetFrame::OnTreeViewInitialView() 
{
	m_pTreeView->InitialView();
}

void CWholeNetFrame::OnUpdateObject(CCmdUI *pCmdUI)
{
	static char pszFullName[1024];
	pszFullName[0] = 0;
	CNetObject *pObject = m_pPlaneView->GetObjectUnderMouse();
	if (!pObject)
		pObject = m_pTreeView->GetObjectUnderMouse();
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

