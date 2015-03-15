// NewControlTargetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "NewControlTargetDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewControlTargetDialog dialog


CNewControlTargetDialog::CNewControlTargetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewControlTargetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewControlTargetDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewControlTargetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewControlTargetDialog)
	DDX_Control(pDX, IDC_TARGET_LISTBOX, m_TargetsListbox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewControlTargetDialog, CDialog)
	//{{AFX_MSG_MAP(CNewControlTargetDialog)
	ON_LBN_SELCHANGE(IDC_TARGET_LISTBOX, OnSelChangeTargetListbox)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_TARGET_LISTBOX, OnDblclkTargetListbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewControlTargetDialog message handlers

BOOL CNewControlTargetDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 1. Init control texts
	CString text;

	text.LoadString(IDS_NEW_TARGET_DIALOG_CAPTION);
	SetWindowText(text);

	text.LoadString(IDS_TARGETS_FRAME_CAPTION);
	GetDescendantWindow(IDC_TARGETS_FRAME_STATIC)->SetWindowText(text);


	// 2. Init listbox with possible control target descriptions
	typedef char *(*PFN_GET_TARGET_DESCRIPTION) ();

	CFileFind finder;
	BOOL bWorking = finder.FindFile("*.dll");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString FileName = finder.GetFileName();
		HMODULE hLib = LoadLibrary(FileName);
		if (hLib)
		{
			PFN_GET_TARGET_DESCRIPTION pfnGetTargetDesc = (PFN_GET_TARGET_DESCRIPTION)GetProcAddress(hLib, "GetControlTargetDescription");
			if (pfnGetTargetDesc)
			{
				char *pszTargetDesc = pfnGetTargetDesc();
				int index = m_TargetsListbox.AddString(pszTargetDesc);
				char *pszFileName = strdup(FileName);
				m_TargetsListbox.SetItemDataPtr(index, pszFileName);
			}
			FreeLibrary(hLib);
		}		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CNewControlTargetDialog::GetLibraryFileName()
{
	if (DoModal() != IDOK)
		return "";

	return m_FileName;
}

void CNewControlTargetDialog::OnSelChangeTargetListbox() 
{
	int index = m_TargetsListbox.GetCurSel();
	if (index == LB_ERR)
		return;
	char *pszFileName = (char *)m_TargetsListbox.GetItemDataPtr(index);
	m_FileName = pszFileName;
}

void CNewControlTargetDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	for (int index = 0; index < m_TargetsListbox.GetCount(); index++)
	{
		char *pszFileName = (char *)m_TargetsListbox.GetItemDataPtr(index);
		free(pszFileName);
	}
}

void CNewControlTargetDialog::OnDblclkTargetListbox() 
{
	OnOK();
}
