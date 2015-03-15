// StationsFrame.cpp : implementation of the CStationsFrame class
//

#include "stdafx.h"

#include "StationsFrame.h"
#include "CuvsDoc.h"
#include "StationsView.h"

// Forms
#include "AllNetForm.h"
#include "StationPropertiesForm.h"
#include "StationDevicesForm.h"
#include "ChannelGroupsForm.h"
#include "TmiQueryParametersForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStationsFrame

IMPLEMENT_DYNCREATE(CStationsFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CStationsFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CStationsFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CStationsFrame construction/destruction

CStationsFrame::CStationsFrame()
{
	// TODO: add member initialization code here
	
}

CStationsFrame::~CStationsFrame()
{
}

int CStationsFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	return 0;
}

BOOL CStationsFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CStationsFrame diagnostics

#ifdef _DEBUG
void CStationsFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CStationsFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStationsFrame message handlers


BOOL CStationsFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
		// create a splitter with 1 row, 2 columns
	if (!m_SplitterWnd.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_SplitterWnd.CreateView(0, 0,
		RUNTIME_CLASS(CStationsView), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterWnd.CreateView(0, 1,
		RUNTIME_CLASS(CAbstractForm), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	SetActiveView((CView *)m_SplitterWnd.GetPane(0,0));
	m_pStationsView = (CStationsView *)GetActiveView();
	m_pDoc = (CCuvsDoc *)GetActiveDocument();
	CAttachedBlock::m_pDoc = m_pDoc;
	m_pDoc->m_pStationsView = m_pStationsView;
	m_pStationsView->m_pDoc = m_pDoc;

	m_pDoc->m_pForms[ABSTRACT_INDEX] = (CAbstractForm *)m_SplitterWnd.GetPane(0,1);
	m_pDoc->AddNewForm(ALL_NET_INDEX, new CAllNetForm(), &m_SplitterWnd);
	m_pDoc->AddNewForm(STATION_PROPERTIES_INDEX, new CStationPropertiesForm(), &m_SplitterWnd);
	m_pDoc->AddNewForm(STATION_DEVICES_INDEX, new CStationDevicesForm(), &m_SplitterWnd);
	m_pDoc->AddNewForm(CHANNEL_GROUPS_INDEX, new CChannelGroupsForm(), &m_SplitterWnd);
	m_pDoc->AddNewForm(TMI_PARAMETERS_INDEX, new CTmiQueryParametersForm(), &m_SplitterWnd);
	return TRUE;
}

CAbstractForm *CStationsFrame::GetAbstractForm()
{
	return m_pDoc->m_pForms[ABSTRACT_INDEX];
}
