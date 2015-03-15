// TmiQueryParamsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TmiDatabase.h"
#include "TmiQueryParamsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParamsDialog dialog

//static
CTime CTmiQueryParamsDialog::m_BeginTime = 0;
//static
CTime CTmiQueryParamsDialog::m_EndTime = 0;
//static
BOOL CTmiQueryParamsDialog::m_bCalendarsInitialized = FALSE;


CTmiQueryParamsDialog::CTmiQueryParamsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTmiQueryParamsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTmiQueryParamsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTmiQueryParamsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiQueryParamsDialog)
	DDX_Control(pDX, IDC_BEGINDATE_CTRL, m_BeginDate);
	DDX_Control(pDX, IDC_ENDDATE_CTRL, m_EndDate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmiQueryParamsDialog, CDialog)
	//{{AFX_MSG_MAP(CTmiQueryParamsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParamsDialog message handlers

BOOL CTmiQueryParamsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckRadioButton(IDC_AZS_RADIO, IDC_UZS_RADIO, IDC_UZS_RADIO);
	if (!m_bCalendarsInitialized)
	{
		m_BeginDate.Today();
		m_EndDate.Today();
	}
	else
	{
		m_BeginDate.SetYear(m_BeginTime.GetYear());
		m_BeginDate.SetMonth(m_BeginTime.GetMonth());
		m_BeginDate.SetDay(m_BeginTime.GetDay());
		m_EndDate.SetYear(m_EndTime.GetYear());
		m_EndDate.SetMonth(m_EndTime.GetMonth());
		m_EndDate.SetDay(m_EndTime.GetDay());
	}
	m_bCalendarsInitialized = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTmiQueryParamsDialog::OnOK() 
{
	m_BeginTime = CTime(m_BeginDate.GetYear(), m_BeginDate.GetMonth(), m_BeginDate.GetDay(), 0, 0, 0);
	m_EndTime = CTime(m_EndDate.GetYear(), m_EndDate.GetMonth(), m_EndDate.GetDay(), 23, 59, 59);
	m_bAZS = (GetCheckedRadioButton(IDC_AZS_RADIO, IDC_UZS_RADIO) == IDC_AZS_RADIO);
	CDialog::OnOK();
}
