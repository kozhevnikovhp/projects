// IPInterfacesForm.cpp : implementation file
//

#include "stdafx.h"
#include "IPInterfacesForm.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPInterfacesForm

IMPLEMENT_DYNCREATE(CIPInterfacesForm, CAbstractForm)

CIPInterfacesForm::CIPInterfacesForm()
	: CAbstractForm(CIPInterfacesForm::IDD)
{
	//{{AFX_DATA_INIT(CIPInterfacesForm)
	m_ConfigString = _T("");
	m_NUM = 0;
	m_bCanRIP = FALSE;
	m_StatusString = _T("");
	//}}AFX_DATA_INIT
	m_pNsgFrad=NULL;
}

CIPInterfacesForm::~CIPInterfacesForm()
{
}

void CIPInterfacesForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPInterfacesForm)
	DDX_Control(pDX, IDC_CAN_RIP_CHECK, m_CanRipCheck);
	DDX_Control(pDX, IDC_NUM_EDIT, m_NumEdit);
	DDX_Control(pDX, IDC_NUMBER_COMBO, m_NumberCombo);
	DDX_Text(pDX, IDC_CONFIG_EDIT, m_ConfigString);
	DDX_Text(pDX, IDC_NUM_EDIT, m_NUM);
	DDX_Check(pDX, IDC_CAN_RIP_CHECK, m_bCanRIP);
	DDX_Text(pDX, IDC_STATE_EDIT, m_StatusString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPInterfacesForm, CAbstractForm)
	//{{AFX_MSG_MAP(CIPInterfacesForm)
	ON_BN_CLICKED(IDC_SET_CONFIG_BUTTON, OnSetConfigButton)
	ON_CBN_SELCHANGE(IDC_NUMBER_COMBO, OnSelChangeNumberCombo)
	ON_BN_CLICKED(IDC_WARM_BUTTON, OnWarmButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPInterfacesForm diagnostics

#ifdef _DEBUG
void CIPInterfacesForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CIPInterfacesForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CIPInterfacesForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;

	char pszStr[32];
	m_NumberCombo.ResetContent();
	for (int i=0; i<m_pNsgFrad->GetMaxIPInterfaces(); i++)
	{
		sprintf(pszStr, "%d", i);
		int index=m_NumberCombo.AddString(pszStr);
		m_NumberCombo.SetItemData(index, i);
	}

	m_NumberCombo.SetCurSel(0);
	OnSelChangeNumberCombo();
	
	return TRUE;
}

//virtual
BOOL CIPInterfacesForm::DoTimer()
{
	RefreshStatus();
	return TRUE;
}

void CIPInterfacesForm::RefreshStatus()
{
	UpdateData();
	int index=m_NumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int No=m_NumberCombo.GetItemData(index);
	CWaitCursor cursor;
	char *pszStatus;
	m_pNsgFrad->GetIPInterfaceStatusString(No, pszStatus);
	m_StatusString=pszStatus;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CIPInterfacesForm message handlers

void CIPInterfacesForm::OnSetConfigButton() 
{
	int index=m_NumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int No=m_NumberCombo.GetItemData(index);
	CWaitCursor cursor;

	UpdateData();
	char *pszConfigString=(char *)LPCTSTR(m_ConfigString);
	m_pNsgFrad->SetIPInterfaceConfigString(No, pszConfigString, m_NUM, m_bCanRIP);
	m_ConfigString=pszConfigString;
	UpdateData(FALSE);
	RefreshStatus();
}

void CIPInterfacesForm::OnSelChangeNumberCombo() 
{
	int index=m_NumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int No=m_NumberCombo.GetItemData(index);
	CWaitCursor cursor;

	char *pszConfigString;
	m_pNsgFrad->GetIPInterfaceConfigString(No, pszConfigString, m_NUM, m_bCanRIP);
	m_ConfigString=pszConfigString;
	UpdateData(FALSE);

	m_CanRipCheck.EnableWindow(No==0);
	m_NumEdit.EnableWindow(No==0);

	RefreshStatus();
}

void CIPInterfacesForm::OnWarmButton() 
{
	int index=m_NumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int No=m_NumberCombo.GetItemData(index);
	CWaitCursor cursor;
	m_pNsgFrad->IPInterfaceWarmStart(No);
}
