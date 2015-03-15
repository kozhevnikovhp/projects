// PhysicalPortsForm.cpp : implementation file
//

#include "stdafx.h"
#include "PhysicalPortsForm.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CPhysicalPortsForm

IMPLEMENT_DYNCREATE(CPhysicalPortsForm, CAbstractForm)

CPhysicalPortsForm::CPhysicalPortsForm()
	: CAbstractForm(CPhysicalPortsForm::IDD)
{
	//{{AFX_DATA_INIT(CPhysicalPortsForm)
	m_PortConfigString = _T("");
	m_StatusString = _T("");
	//}}AFX_DATA_INIT
	m_pNsgFrad=NULL;
}

CPhysicalPortsForm::~CPhysicalPortsForm()
{
}

void CPhysicalPortsForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhysicalPortsForm)
	DDX_Control(pDX, IDC_PORT_NUMBER_COMBO, m_PortNumberCombo);
	DDX_Text(pDX, IDC_PORT_CONFIG_EDIT, m_PortConfigString);
	DDX_Text(pDX, IDC_STATE_EDIT, m_StatusString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhysicalPortsForm, CFormView)
	//{{AFX_MSG_MAP(CPhysicalPortsForm)
	ON_CBN_SELCHANGE(IDC_PORT_NUMBER_COMBO, OnSelChangePortNumberCombo)
	ON_BN_CLICKED(IDC_SET_PORT_CONFIG_BUTTON, OnSetPortConfigButton)
	ON_BN_CLICKED(IDC_WARM_BUTTON, OnWarmButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhysicalPortsForm diagnostics

#ifdef _DEBUG
void CPhysicalPortsForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CPhysicalPortsForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CPhysicalPortsForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;

	char pszStr[32];
	m_PortNumberCombo.ResetContent();
	for (int i=0; i<m_pNsgFrad->GetPhysicalPortNum(); i++)
	{
		sprintf(pszStr, "%d", i);
		int index=m_PortNumberCombo.AddString(pszStr);
		m_PortNumberCombo.SetItemData(index, i);
	}

	m_PortNumberCombo.SetCurSel(0);
	OnSelChangePortNumberCombo();

	return TRUE;
}

BOOL CPhysicalPortsForm::DoTimer()
{
	RefreshStatus();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPhysicalPortsForm message handlers

void CPhysicalPortsForm::OnSelChangePortNumberCombo() 
{
	int index=m_PortNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int PortNo=m_PortNumberCombo.GetItemData(index);
	CWaitCursor cursor;
	char *pszPortConfigString;
	m_pNsgFrad->GetPhysicalPortConfigString(PortNo, pszPortConfigString);
	m_PortConfigString=pszPortConfigString;

	UpdateData(FALSE);
	RefreshStatus();
}

void CPhysicalPortsForm::OnSetPortConfigButton() 
{
	int index=m_PortNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int PortNo=m_PortNumberCombo.GetItemData(index);
	CWaitCursor cursor;

	UpdateData();
	char *pszPortConfigString=(char *)LPCTSTR(m_PortConfigString);
	m_pNsgFrad->SetPhysicalPortConfigString(PortNo, pszPortConfigString);
	m_PortConfigString=pszPortConfigString;
	UpdateData(FALSE);

}

void CPhysicalPortsForm::OnWarmButton() 
{
	int index=m_PortNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int PortNo=m_PortNumberCombo.GetItemData(index);
	CWaitCursor cursor;
	m_pNsgFrad->PhysicalPortWarmStart(PortNo);
	RefreshStatus();
}

void CPhysicalPortsForm::RefreshStatus() 
{
	UpdateData();
	int index=m_PortNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int PortNo=m_PortNumberCombo.GetItemData(index);

	CWaitCursor cursor;
	char *pszStatus;
	m_pNsgFrad->GetPhysicalPortStatusString(PortNo, pszStatus);
	m_StatusString=pszStatus;
	UpdateData(FALSE);
}

