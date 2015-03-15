// HostTrafficDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NetworkAnalyzer.h"
#include "HostTrafficDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHostTrafficDialog dialog


CHostTrafficDialog::CHostTrafficDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CHostTrafficDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHostTrafficDialog)
	//}}AFX_DATA_INIT
}


void CHostTrafficDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostTrafficDialog)
	DDX_Control(pDX, IDC_DIAGRAM_STATIC, m_Diagram);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHostTrafficDialog, CDialog)
	//{{AFX_MSG_MAP(CHostTrafficDialog)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostTrafficDialog message handlers

int CHostTrafficDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}

BOOL CHostTrafficDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Diagram.Setup();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

