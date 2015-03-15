// DeleteObjectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "controlsystemwin.h"
#include "DeleteObjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteObjectDialog dialog


CDeleteObjectDialog::CDeleteObjectDialog(CControlObject *pObject, CWnd *pParent /*=NULL*/)
	: CDialog(CDeleteObjectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteObjectDialog)
	//}}AFX_DATA_INIT
	m_pObject = pObject;
	m_DeletionType = DT_UNKNOWN;
}


void CDeleteObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteObjectDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteObjectDialog, CDialog)
	//{{AFX_MSG_MAP(CDeleteObjectDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteObjectDialog message handlers

BOOL CDeleteObjectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 1. Init control texts
	CString text;

	text.LoadString(IDS_DELETE_OBJECT_DIALOG_CAPTION);
	SetWindowText(text);

	text.LoadString(IDC_DELETE_OBJECT_DIALOG_DELETE);
	GetDescendantWindow(IDC_DELETE_RADIO)->SetWindowText(text);
	text.LoadString(IDC_DELETE_OBJECT_DIALOG_DESTROY);
	GetDescendantWindow(IDC_DESTROY_RADIO)->SetWindowText(text);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteObjectDialog::OnOK() 
{
	int ID = GetCheckedRadioButton(IDC_DELETE_RADIO, IDC_DESTROY_RADIO);
	switch (ID)
	{
	case IDC_DELETE_RADIO: m_DeletionType = DT_DELETE; break;
	case IDC_DESTROY_RADIO: m_DeletionType = DT_DESTROY; break;
	}
	CDialog::OnOK();
}
