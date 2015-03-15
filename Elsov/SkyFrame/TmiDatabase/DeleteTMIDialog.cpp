// DeleteTMIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tmidatabase.h"
#include "DeleteTMIDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteTMIDialog dialog


CDeleteTMIDialog::CDeleteTMIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteTMIDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteTMIDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDeleteTMIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteTMIDialog)
	DDX_Control(pDX, IDC_BEGINDATE_CTRL, m_BeginDate);
	DDX_Control(pDX, IDC_ENDDATE_CTRL, m_EndDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteTMIDialog, CDialog)
	//{{AFX_MSG_MAP(CDeleteTMIDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteTMIDialog message handlers

void CDeleteTMIDialog::OnOK() 
{
	m_BeginTime = CTime(m_BeginDate.GetYear(), m_BeginDate.GetMonth(), m_BeginDate.GetDay(), 0, 0, 0);
	m_EndTime = CTime(m_EndDate.GetYear(), m_EndDate.GetMonth(), m_EndDate.GetDay(), 23, 59, 59);
	
	CDialog::OnOK();
}

BOOL CDeleteTMIDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_BeginDate.Today();
	m_EndDate.Today();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
