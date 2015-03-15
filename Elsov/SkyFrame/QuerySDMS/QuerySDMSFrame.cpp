// QuerySDMSFrame.cpp : implementation of the CQuerySDMSFrame class
//

#include "stdafx.h"
#include "QuerySDMS.h"
#include "QuerySDMSView.h"

#include "QuerySDMSFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSFrame

IMPLEMENT_DYNCREATE(CQuerySDMSFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CQuerySDMSFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CQuerySDMSFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_PARAMS, OnViewParams)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PARAMS, OnUpdateViewParams)
	ON_COMMAND(ID_VIEW_MODIFTABLE, OnViewModIfTable)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODIFTABLE, OnUpdateViewModIfTable)
	ON_COMMAND(ID_VIEW_DEMODIFTABLE, OnViewDemodIfTable)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEMODIFTABLE, OnUpdateViewDemodIfTable)
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_DEMODSTATUS, OnViewDemodStatus)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEMODSTATUS, OnUpdateViewDemodStatus)
	ON_COMMAND(ID_VIEW_MODDATATABLE, OnViewModDataTable)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODDATATABLE, OnUpdateViewModDataTable)
	ON_COMMAND(ID_VIEW_DEMODDATATABLE, OnViewDemodDataTable)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEMODDATATABLE, OnUpdateViewDemodDataTable)
	ON_COMMAND(ID_VIEW_MOD_BUC_FORM, OnViewModBucForm)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOD_BUC_FORM, OnUpdateViewModBucForm)
	ON_COMMAND(ID_VIEW_DEMOD_LNB_FORM, OnViewDemodLnbForm)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEMOD_LNB_FORM, OnUpdateViewDemodLnbForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT buttons[] =
{
	ID_VIEW_PARAMS,
		ID_SEPARATOR,
	ID_VIEW_MODIFTABLE,
	ID_VIEW_MODDATATABLE,
	ID_VIEW_MOD_BUC_FORM,
		ID_SEPARATOR,
	ID_VIEW_DEMODIFTABLE,
	ID_VIEW_DEMODDATATABLE,
	ID_VIEW_DEMODSTATUS,
		ID_SEPARATOR,
	ID_APP_ABOUT
};

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_TIME
};

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSFrame construction/destruction

CQuerySDMSFrame::CQuerySDMSFrame()
{
	m_LastTimerTime = 0;
}

CQuerySDMSFrame::~CQuerySDMSFrame()
{
}

int CQuerySDMSFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	SetTimer(1, 500, NULL);
	m_pApp = (CQuerySDMSApp *)AfxGetApp();
	m_pApp->SetFrame(this);


	return 0;
}

BOOL CQuerySDMSFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSFrame diagnostics

#ifdef _DEBUG
void CQuerySDMSFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CQuerySDMSFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSFrame message handlers


BOOL CQuerySDMSFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	m_pParamsForm = (CControlForm *)CreateView(pContext);
	m_pCurrentlyVisibleForm = m_pParamsForm;
	m_pCurrentlyVisibleForm->Entering();

	m_pModIfTableForm = new CModIfTableForm;
	AddNewForm(m_pModIfTableForm, pContext->m_pCurrentDoc);
	
	m_pModDataTableForm = new CModDataTableForm;
	AddNewForm(m_pModDataTableForm, pContext->m_pCurrentDoc);
	
	m_pModBucTableForm = new CModBucTableForm;
	AddNewForm(m_pModBucTableForm, pContext->m_pCurrentDoc);
	
	m_pDemodIfTableForm = new CDemodIfTableForm;
	AddNewForm(m_pDemodIfTableForm, pContext->m_pCurrentDoc);

	m_pDemodDataTableForm = new CDemodDataTableForm;
	AddNewForm(m_pDemodDataTableForm, pContext->m_pCurrentDoc);
	
	m_pDemodLnbTableForm = new CDemodLnbTableForm;
	AddNewForm(m_pDemodLnbTableForm, pContext->m_pCurrentDoc);
	
	m_pDemodStatusForm = new CDemodStatusForm;
	AddNewForm(m_pDemodStatusForm, pContext->m_pCurrentDoc);

	m_pApp = (CQuerySDMSApp *)AfxGetApp();
	CControlForm::SetIpAddress(m_pApp->GetIpAddress());
	
	return TRUE;
}

void CQuerySDMSFrame::AddNewForm(CView *pFormToAdd, CDocument *pDoc)
{
	static PaneIndex = 0;
	pFormToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, this,
		AFX_IDW_PANE_FIRST+PaneIndex+1, NULL);
	++PaneIndex;
	pDoc->AddView(pFormToAdd);
}

void CQuerySDMSFrame::SwitchToForm(CControlForm *pForm)
{
	UINT OldIDD = m_pCurrentlyVisibleForm->GetDlgCtrlID();
	UINT NewIDD = pForm->GetDlgCtrlID();
	if (OldIDD == NewIDD)
		return;
	m_pCurrentlyVisibleForm->ShowWindow(SW_HIDE);
	pForm->ShowWindow(SW_SHOW);
	m_pCurrentlyVisibleForm->SetDlgCtrlID(NewIDD);
	pForm->SetDlgCtrlID(OldIDD);
	RecalcLayout();
	m_pCurrentlyVisibleForm = pForm;
	m_pCurrentlyVisibleForm->Entering();
	m_pCurrentlyVisibleForm->UpdateControls("");
	Timer(TRUE);
}

BOOL CQuerySDMSFrame::IsFormVisible(CControlForm *pForm)
{
	return (m_pCurrentlyVisibleForm == pForm);
}

void CQuerySDMSFrame::OnViewParams() 
{
	SwitchToForm(m_pParamsForm);
}

void CQuerySDMSFrame::OnUpdateViewParams(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pParamsForm));
}

void CQuerySDMSFrame::OnViewModIfTable() 
{
	SwitchToForm(m_pModIfTableForm);
}

void CQuerySDMSFrame::OnUpdateViewModIfTable(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pModIfTableForm));
}


void CQuerySDMSFrame::OnViewModDataTable() 
{
	SwitchToForm(m_pModDataTableForm);
}

void CQuerySDMSFrame::OnUpdateViewModDataTable(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pModDataTableForm));
}

void CQuerySDMSFrame::OnViewDemodIfTable() 
{
	SwitchToForm(m_pDemodIfTableForm);
}

void CQuerySDMSFrame::OnUpdateViewDemodIfTable(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pDemodIfTableForm));
}

void CQuerySDMSFrame::OnViewDemodDataTable() 
{
	SwitchToForm(m_pDemodDataTableForm);
}

void CQuerySDMSFrame::OnUpdateViewDemodDataTable(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pDemodDataTableForm));
}

void CQuerySDMSFrame::OnViewDemodStatus() 
{
	SwitchToForm(m_pDemodStatusForm);
}

void CQuerySDMSFrame::OnUpdateViewDemodStatus(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pDemodStatusForm));
}

void CQuerySDMSFrame::OnViewModBucForm() 
{
	SwitchToForm(m_pModBucTableForm);
}

void CQuerySDMSFrame::OnUpdateViewModBucForm(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pModBucTableForm));
}

void CQuerySDMSFrame::OnViewDemodLnbForm() 
{
	SwitchToForm(m_pDemodLnbTableForm);
}

void CQuerySDMSFrame::OnUpdateViewDemodLnbForm(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(IsFormVisible(m_pDemodLnbTableForm));
}

BOOL CQuerySDMSFrame::DeltaTimeElapsed()
{
	DWORD time = GetTickCount();
	DWORD DeltaTime = time - m_LastTimerTime;
	return (DeltaTime >= m_pParamsForm->GetDeltaTime()*1000);
}

void CQuerySDMSFrame::Timer(BOOL bAnyway)
{
	if (bAnyway || DeltaTimeElapsed())
	{
		m_LastTimerTime = GetTickCount();
		m_pCurrentlyVisibleForm->DoTimer();
	}
}

void CQuerySDMSFrame::OnTimer(UINT nIDEvent) 
{
	Timer(FALSE);
	if (!IsFormVisible(m_pParamsForm))
	{
		CString str;
		int nRemaining = m_pParamsForm->GetDeltaTime() - (GetTickCount()-m_LastTimerTime)/1000;
		str.Format("ƒо следующего опроса осталось %d секунд", nRemaining);
		m_wndStatusBar.SetWindowText(str);
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void CQuerySDMSFrame::OnIdle()
{
	m_pCurrentlyVisibleForm->OnIdle();
}


