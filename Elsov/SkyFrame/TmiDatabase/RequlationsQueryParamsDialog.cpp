// RequlationsQueryParamsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tmidatabase.h"
#include "RequlationsQueryParamsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegulationsQueryParamsDialog dialog


CRegulationsQueryParamsDialog::CRegulationsQueryParamsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRegulationsQueryParamsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegulationsQueryParamsDialog)
	//}}AFX_DATA_INIT
}


void CRegulationsQueryParamsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegulationsQueryParamsDialog)
	DDX_Control(pDX, IDC_STATIONS_LIST, m_StationsListBox);
	DDX_Control(pDX, IDC_BEGINDATE_CTRL, m_BeginDate);
	DDX_Control(pDX, IDC_ENDDATE_CTRL, m_EndDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegulationsQueryParamsDialog, CDialog)
	//{{AFX_MSG_MAP(CRegulationsQueryParamsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegulationsQueryParamsDialog message handlers

void CRegulationsQueryParamsDialog::OnOK() 
{
	m_BeginTime = CTime(m_BeginDate.GetYear(), m_BeginDate.GetMonth(), m_BeginDate.GetDay(), 0, 0, 0);
	m_EndTime = CTime(m_EndDate.GetYear(), m_EndDate.GetMonth(), m_EndDate.GetDay(), 23, 59, 59);
	
	CDialog::OnOK();
}
