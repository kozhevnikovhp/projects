/* WorldPlaneView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	16 May 2007 - context menu
*/


#include "stdafx.h"
#include "NetworkAnalyzer.h"
#include "WorldPlaneView.h"
#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView

IMPLEMENT_DYNCREATE(CWorldPlaneView, COpenGLView)

CWorldPlaneView::CWorldPlaneView()
{
	m_pWorldPlaneRenderer = NULL;
}

CWorldPlaneView::~CWorldPlaneView()
{
}


BEGIN_MESSAGE_MAP(CWorldPlaneView, COpenGLView)
	//{{AFX_MSG_MAP(CWorldPlaneView)
	ON_COMMAND(ID_TEST_ANTIALIASING, OnTestAntialiasing)
	ON_UPDATE_COMMAND_UI(ID_TEST_ANTIALIASING, OnUpdateTestAntialiasing)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView drawing

void CWorldPlaneView::OnDraw(CDC *pDC)
{
	CDocument *pDoc = GetDocument();
}

//virtual
void CWorldPlaneView::CreateRenderer()
{
	m_pWorldPlaneRenderer = new CWorldPlaneRenderer(m_hWnd);
}

//virtual
void CWorldPlaneView::DestroyRenderer()
{
	if (m_pWorldPlaneRenderer)
		delete m_pWorldPlaneRenderer;
	m_pWorldPlaneRenderer = NULL;
}

void CWorldPlaneView::NewHostCreated(CHost *pHost)
{
	m_pWorldPlaneRenderer->AddHost(pHost);
}

void CWorldPlaneView::HostChanged(CHost *pHost)
{
	m_pWorldPlaneRenderer->HostChanged(pHost);
}

void CWorldPlaneView::NewSubnetCreated(CSubnet *pSubnet)
{
	m_pWorldPlaneRenderer->Redraw();
}

void CWorldPlaneView::SubnetChanged(CSubnet *pSubnet)
{
}

void CWorldPlaneView::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_pWorldPlaneRenderer->HostAddedToSubnet(pHost, pSubnet);
}

void CWorldPlaneView::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
}
	
void CWorldPlaneView::GuiCreated(IGuiAPI *pGui)
{
	m_pWorldPlaneRenderer->SetGui(pGui);
}

void CWorldPlaneView::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	m_pWorldPlaneRenderer->ObjectSelectionChanged(pObject, pWhereChanged);
}

BOOL CWorldPlaneView::OnIdleProcess()
{
	return m_pWorldPlaneRenderer->OnIdleProcess();
}

void CWorldPlaneView::ZoomIn()
{
	m_pWorldPlaneRenderer->ZoomIn();
}

void CWorldPlaneView::ZoomOut()
{
	m_pWorldPlaneRenderer->ZoomOut();
}

void CWorldPlaneView::InitialView()
{
	m_pWorldPlaneRenderer->SetDefaultView();
}

void CWorldPlaneView::SetSelectMode()
{
	m_pWorldPlaneRenderer->SetSelectMode();	
}

void CWorldPlaneView::OnUpdateSelectMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pWorldPlaneRenderer->IsInSelectMode());
}

void CWorldPlaneView::SetDragScrollMode()
{
	m_pWorldPlaneRenderer->SetDragScrollMode();	
}

void CWorldPlaneView::OnUpdateDragScrollMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pWorldPlaneRenderer->IsInDragScrollMode());
}

void CWorldPlaneView::SetZoomRectangleMode()
{
	m_pWorldPlaneRenderer->SetZoomRectangleMode();	
}

void CWorldPlaneView::OnUpdateZoomRectangleMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pWorldPlaneRenderer->IsInZoomRectangleMode());
}

CNetObject *CWorldPlaneView::GetObjectUnderMouse()
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

	return m_pWorldPlaneRenderer->GetObjectAt(ScreenPoint.x, ScreenPoint.y);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView diagnostics

#ifdef _DEBUG
void CWorldPlaneView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CWorldPlaneView::Dump(CDumpContext &dc) const
{
	COpenGLView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView message handlers

void CWorldPlaneView::OnTestAntialiasing() 
{
	m_pWorldPlaneRenderer->ToggleAntialiasing();
}

void CWorldPlaneView::OnUpdateTestAntialiasing(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pWorldPlaneRenderer->AntialiasingEnabled());
}

void CWorldPlaneView::OnContextMenu(CWnd *pWnd, CPoint point) 
{
	CPoint ClientPoint(point);
	ScreenToClient(&ClientPoint);
	CNetObject *pObject = m_pWorldPlaneRenderer->GetObjectAt(ClientPoint.x, ClientPoint.y);
	if (pObject)
	{
		pObject->GuiContextMenu(point.x, point.y);
	}
	else
	{
	}
	m_pWorldPlaneRenderer->Redraw();
}
