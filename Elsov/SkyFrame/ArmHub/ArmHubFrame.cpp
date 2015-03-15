// ArmHubFrame.cpp : implementation of the CArmHubFrame class
//

#include "stdafx.h"
#include "ArmHub.h"

#include "ArmHubFrame.h"
#include "ArmHubDoc.h"
#include "StationsView.h"

// Forms
#include "StationForm.h"
#include "IpAddressForm.h"
#include "QueryParametersForm.h"
#include "TmiIpStatisticsForm.h"
#include "TmiUdpStatisticsForm.h"
#include "TmiNetDeviceStatisticsForm.h"
#include "TmiModemStatusForm.h"
#include "TmiBstStatusForm.h"
#include "TelnetForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArmHubFrame

IMPLEMENT_DYNCREATE(CArmHubFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CArmHubFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CArmHubFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CArmHubFrame construction/destruction

CArmHubFrame::CArmHubFrame()
{
	// TODO: add member initialization code here
	
}

CArmHubFrame::~CArmHubFrame()
{
}

int CArmHubFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

BOOL CArmHubFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CArmHubFrame diagnostics

#ifdef _DEBUG
void CArmHubFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CArmHubFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArmHubFrame message handlers


BOOL CArmHubFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
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
	CArmHubDoc *pDoc=(CArmHubDoc *)GetActiveDocument();
	pDoc->m_pTreeView=(CStationsView *)m_SplitterWnd.GetPane(0,0);
	pDoc->m_pForms[0]=(CAbstractForm *)m_SplitterWnd.GetPane(0,1);
	
	pDoc->AddNewForm(STATION_INDEX, new CStationForm(), &m_SplitterWnd);
	pDoc->AddNewForm(IP_ADDRESS_INDEX, new CIpAddressForm(), &m_SplitterWnd);
	pDoc->AddNewForm(QUERY_PARAMETERS_INDEX, new CQueryParametersForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TMI_IP_STATISTICS_INDEX, new CTmiIpStatisticsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TMI_UDP_STATISTICS_INDEX, new CTmiUdpStatisticsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TMI_NET_DEVICE_STATISTICS_INDEX, new CTmiNetDeviceStatisticsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TMI_MODEM_STATUS_INDEX, new CTmiModemStatusForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TMI_BST_STATUS_INDEX, new CTmiBstStatusForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TELNET_INDEX, new CTelnetForm(), &m_SplitterWnd);

	return TRUE;
}
