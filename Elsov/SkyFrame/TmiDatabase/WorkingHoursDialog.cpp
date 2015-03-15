// WorkingHoursDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tmidatabase.h"
#include "WorkingHoursDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkingHoursDialog dialog


CWorkingHoursDialog::CWorkingHoursDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CWorkingHoursDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkingHoursDialog)
	//}}AFX_DATA_INIT
	m_bHours = TRUE;
}


void CWorkingHoursDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkingHoursDialog)
	DDX_Control(pDX, IDC_BEGINDATE_CTRL, m_BeginDate);
	DDX_Control(pDX, IDC_ENDDATE_CTRL, m_EndDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkingHoursDialog, CDialog)
	//{{AFX_MSG_MAP(CWorkingHoursDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkingHoursDialog message handlers

void CWorkingHoursDialog::OnOK() 
{
	m_BeginTime = CTime(m_BeginDate.GetYear(), m_BeginDate.GetMonth(), m_BeginDate.GetDay(), 0, 0, 0);
	m_EndTime = CTime(m_EndDate.GetYear(), m_EndDate.GetMonth(), m_EndDate.GetDay(), 23, 59, 59);
	m_bHours = (GetCheckedRadioButton(IDC_HOURS_RADIO, IDC_PERCENTS_RADIO) == IDC_HOURS_RADIO);
	
	CDialog::OnOK();
}

BOOL CWorkingHoursDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_BeginDate.Today();
	m_EndDate.Today();
	
	CheckRadioButton(IDC_HOURS_RADIO, IDC_PERCENTS_RADIO, IDC_HOURS_RADIO);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
