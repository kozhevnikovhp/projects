// MeshFrame.cpp : implementation of the CMeshFrame class
//

#include "stdafx.h"
#include "Mesh.h"

#include "MeshFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshFrame

IMPLEMENT_DYNCREATE(CMeshFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMeshFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMeshFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_OPEN,
		ID_SEPARATOR,
	ID_VIEW_MESH,
		ID_SEPARATOR,
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_APP_ABOUT
};
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMeshFrame construction/destruction

CMeshFrame::CMeshFrame()
{
	// TODO: add member initialization code here
	
}

CMeshFrame::~CMeshFrame()
{
}

int CMeshFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons, sizeof(buttons)/sizeof(UINT)))
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

	return 0;
}

BOOL CMeshFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMeshFrame diagnostics

#ifdef _DEBUG
void CMeshFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMeshFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMeshFrame message handlers

