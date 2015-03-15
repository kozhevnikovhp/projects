// ControlSystemFrame.cpp : implementation of the CControlSystemFrame class
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "AbstractForm.h"
#include "ControlSystemFrame.h"
#include "ControlSystemView.h"
#include "ControlSystemDoc.h"

// Forms
#include "TargetForm.h"
#include "ObjectForm.h"
#include "EventLogForm.h"
#include "MonitorAndControlForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlSystemFrame

IMPLEMENT_DYNCREATE(CControlSystemFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CControlSystemFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CControlSystemFrame)
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
// CControlSystemFrame construction/destruction

CControlSystemFrame::CControlSystemFrame()
{
	// TODO: add member initialization code here
	
}

CControlSystemFrame::~CControlSystemFrame()
{
}

int CControlSystemFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

BOOL CControlSystemFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CControlSystemFrame diagnostics

#ifdef _DEBUG
void CControlSystemFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CControlSystemFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlSystemFrame message handlers


BOOL CControlSystemFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_SplitterVert.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_SplitterVert.CreateView(0, 0,
		RUNTIME_CLASS(CControlSystemView), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - which is a nested splitter with 2 rows
	if (!m_SplitterHor.CreateStatic(
		&m_SplitterVert,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_SplitterVert.IdFromRowCol(0, 1)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}	 

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterHor.CreateView(0, 0,
		RUNTIME_CLASS(CAbstractForm), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create second pane (0, 0)\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterHor.CreateView(1, 0,
		RUNTIME_CLASS(CAbstractForm), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane (1, 0)\n");
		return FALSE;
	}

	SetActiveView((CView *)m_SplitterVert.GetPane(0,0));
	CControlSystemDoc *pDoc = (CControlSystemDoc *)GetActiveDocument();
	pDoc->m_pTreeView = (CControlSystemView *)m_SplitterVert.GetPane(0,0);
	pDoc->m_pForms[0] = (CAbstractForm *)m_SplitterHor.GetPane(0,0);
	
	pDoc->AddNewForm(TARGET_FORM_INDEX, new CTargetForm(), &m_SplitterHor);
	pDoc->AddNewForm(OBJECT_FORM_INDEX, new CObjectForm(), &m_SplitterHor);
	pDoc->AddNewForm(EVENT_LOG_FORM_INDEX, new CEventLogForm(), &m_SplitterHor);
	pDoc->AddNewForm(MONITOR_AND_CONTROL_FORM_INDEX, new CMonitorAndControlForm(), &m_SplitterHor);

	return TRUE;
}
