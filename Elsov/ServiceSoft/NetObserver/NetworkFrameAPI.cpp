/* NetworkFrameAPI.cpp : implementation of the CNetworkAnalyzerFrame class

  Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	07 Sep 2006 - initial creation
	20 Mar 2008 - moving host between groups event handling
*/

#include "stdafx.h"

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
// CNetworkFrameAPI


IMPLEMENT_DYNCREATE(CNetworkFrameAPI, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CNetworkFrameAPI, CMDIChildWnd)
	//{{AFX_MSG_MAP(CNetworkFrameAPI)
	ON_WM_CREATE()
    ON_MESSAGE(WM_HOST_CREATED, OnHostCreated)
    ON_MESSAGE(WM_HOST_CHANGED, OnHostChanged)
    ON_MESSAGE(WM_SUBNET_CREATED, OnSubnetCreated)
    ON_MESSAGE(WM_SUBNET_CHANGED, OnSubnetChanged)
    ON_MESSAGE(WM_HOST_ADDED_TO_SUBNET, OnHostAddedToSubnet)
    ON_MESSAGE(WM_HOST_REMOVED_FROM_SUBNET, OnHostRemovedFromSubnet)
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

void CNetworkFrameAPI::SetDocument(CNetworkDocAPI *pDoc)
{
	m_pDoc = pDoc; 
	m_pDoc->SetFrame(this);
}

int CNetworkFrameAPI::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

LRESULT CNetworkFrameAPI::OnHostCreated(WPARAM wParam, LPARAM lParam)
{
	CHost *pHost = (CHost *)lParam;
	NewHostCreated(pHost);
	return 0;
}

LRESULT CNetworkFrameAPI::OnHostChanged(WPARAM wParam, LPARAM lParam)
{
	m_pDoc->UpdateWindowCaption();
	CHost *pHost = (CHost *)lParam;
	HostChanged(pHost);
	return 0;
}

LRESULT CNetworkFrameAPI::OnSubnetCreated(WPARAM wParam, LPARAM lParam)
{
	CSubnet *pSubnet = (CSubnet *)lParam;
	NewSubnetCreated(pSubnet);
	return 0;
}

LRESULT CNetworkFrameAPI::OnSubnetChanged(WPARAM wParam, LPARAM lParam)
{
	CSubnet *pSubnet = (CSubnet *)lParam;
	SubnetChanged(pSubnet);
	return 0;
}

LRESULT CNetworkFrameAPI::OnHostAddedToSubnet(WPARAM wParam, LPARAM lParam)
{
	CHost *pHost = (CHost *)wParam;
	CSubnet *pSubnet = (CSubnet *)lParam;
	HostAddedToSubnet(pHost, pSubnet);
	return 0;
}

LRESULT CNetworkFrameAPI::OnHostRemovedFromSubnet(WPARAM wParam, LPARAM lParam)
{
	CHost *pHost = (CHost *)wParam;
	CSubnet *pSubnet = (CSubnet *)lParam;
	HostRemovedFromSubnet(pHost, pSubnet);
	return 0;
}

BOOL CNetworkFrameAPI::PreCreateWindow(CREATESTRUCT &cs)
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
	CMDIChildWnd::AssertValid();
}

void CNetworkFrameAPI::Dump(CDumpContext &dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetworkFrameAPI message handlers


