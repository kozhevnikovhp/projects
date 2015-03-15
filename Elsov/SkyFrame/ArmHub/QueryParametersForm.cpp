// QueryParametersForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "QueryParametersForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CQueryParametersForm

IMPLEMENT_DYNCREATE(CQueryParametersForm, CAbstractForm)

CQueryParametersForm::CQueryParametersForm()
	: CAbstractForm(CQueryParametersForm::IDD)
{
	//{{AFX_DATA_INIT(CQueryParametersForm)
	m_bQueryEnabled = FALSE;
	m_QueryPeriod = 0;
	m_NextQueryTime = _T("");
	m_LastReplyTime = _T("");
	m_TimeElapsedSinceLastReply = _T("");
	m_TimeTillNextQuery = _T("");
	m_RequeryPeriod = 0;
	//}}AFX_DATA_INIT
}

CQueryParametersForm::~CQueryParametersForm()
{
}

void CQueryParametersForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryParametersForm)
	DDX_Control(pDX, IDC_REQUERY_PERIOD_SPIN, m_RequeryPeriodSpin);
	DDX_Control(pDX, IDC_QUERY_PERIOD_SPIN, m_QueryPeriodSpin);
	DDX_Check(pDX, IDC_QUERY_ENABLED_CHECK, m_bQueryEnabled);
	DDX_Text(pDX, IDC_QUERY_PERIOD_EDIT, m_QueryPeriod);
	DDX_Text(pDX, IDC_NEXT_QUERY_EDIT, m_NextQueryTime);
	DDX_Text(pDX, IDC_LAST_REPLY_EDIT, m_LastReplyTime);
	DDX_Text(pDX, IDC_TIME_ELAPSED_SINCE_LAST_REPLY_EDIT, m_TimeElapsedSinceLastReply);
	DDX_Text(pDX, IDC_TIME_TILL_NEXT_QUERY_EDIT, m_TimeTillNextQuery);
	DDX_Text(pDX, IDC_REQUERY_PERIOD_EDIT, m_RequeryPeriod);
	//}}AFX_DATA_MAP
}

//virtual
void CQueryParametersForm::OnInitialUpdate()
{
	CAbstractForm::OnInitialUpdate();
	m_QueryPeriodSpin.SetRange(1, 180);
	m_RequeryPeriodSpin.SetRange(1, 180);
}

//virtual
BOOL CQueryParametersForm::SetupControls()
{
	m_bQueryEnabled = m_pStation->IsQueryEnabled();
	m_QueryPeriod = m_pStation->GetQueryPeriod().GetTotalMinutes();
	m_RequeryPeriod = m_pStation->GetRequeryPeriod().GetTotalMinutes();
	UpdateData(FALSE);
	UpdateTimes();
	return TRUE;
}

//virtual
BOOL CQueryParametersForm::IsAlarm(CStation *pStation)
{
	return !pStation->IsTmiConnected();
}

void CQueryParametersForm::UpdateTimes()
{
	UpdateData(TRUE);
	m_LastReplyTime = m_pStation->FormatLastStatReplyTime();
	m_NextQueryTime = m_pStation->FormatNextStatQueryTime();
	m_TimeElapsedSinceLastReply = m_pStation->FormatTimeElapsedSinceLastStatReply();
	m_TimeTillNextQuery = m_pStation->FormatTimeTillNextStatQuery();
	UpdateData(FALSE);
}

//virtual
BOOL CQueryParametersForm::DoTimer()
{
	UpdateTimes();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CQueryParametersForm, CAbstractForm)
	//{{AFX_MSG_MAP(CQueryParametersForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_QUERY_ENABLED_CHECK, OnQueryEnabledCheck)
	ON_BN_CLICKED(IDC_RIGHT_NOW_BUTTON, OnRightNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParametersForm diagnostics

#ifdef _DEBUG
void CQueryParametersForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CQueryParametersForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQueryParametersForm message handlers


void CQueryParametersForm::OnApplyButton() 
{
	UpdateData();
	m_pStation->SetTmiQueryPeriod(m_QueryPeriod);
	m_pStation->SetTmiRequeryPeriod(m_RequeryPeriod);
	m_pStation->EnableStatQuery(m_bQueryEnabled);
}

void CQueryParametersForm::OnQueryEnabledCheck() 
{
	UpdateData();
	m_pStation->EnableStatQuery(m_bQueryEnabled);
}

void CQueryParametersForm::OnRightNowButton() 
{
	m_pStation->QueryTmi();	
}
