// PvcForm.cpp : implementation file
//

#include "stdafx.h"
#include "PvcForm.h"
#include "PvcPropertiesDialog.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char CR=0x0D;
static char LF=0x0A;

/////////////////////////////////////////////////////////////////////////////
// CPvcForm

IMPLEMENT_DYNCREATE(CPvcForm, CAbstractForm)

CPvcForm::CPvcForm()
	: CAbstractForm(CPvcForm::IDD)
{
	//{{AFX_DATA_INIT(CPvcForm)
	m_PvcString = _T("");
	//}}AFX_DATA_INIT
}

CPvcForm::~CPvcForm()
{
}

void CPvcForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPvcForm)
	DDX_Text(pDX, IDC_PVC_EDIT, m_PvcString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPvcForm, CAbstractForm)
	//{{AFX_MSG_MAP(CPvcForm)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_MODIFY_BUTTON, OnModifyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPvcForm diagnostics

#ifdef _DEBUG
void CPvcForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CPvcForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CPvcForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;

	RefreshTable();
	return TRUE;
}

void CPvcForm::RefreshTable()
{
	CWaitCursor cursor;

	char *pszPVCs;
	m_pNsgFrad->GetPVCs(-1, pszPVCs);
	m_PvcString=pszPVCs;
	UpdateData(FALSE);

}

/////////////////////////////////////////////////////////////////////////////
// CPvcForm message handlers


void CPvcForm::OnAddButton() 
{
	CPvcPropertiesDialog dialog(this);
	if (dialog.DoModal()!=IDOK) return;
	m_pNsgFrad->AddPVC( (char *)LPCTSTR(dialog.m_Port1), (char *)LPCTSTR(dialog.m_Channel1),
						(char *)LPCTSTR(dialog.m_Port2), (char *)LPCTSTR(dialog.m_Channel2));

}

void CPvcForm::OnDeleteButton() 
{
	static char pszPort[32], pszChannel[32];

/*	int index=m_PvcCtrl.GetSelectionMark();
	if (index==-1) return;

	m_PvcCtrl.GetItemText(index, 0, pszPort, sizeof(pszPort));
	m_PvcCtrl.GetItemText(index, 1, pszChannel, sizeof(pszChannel));
	CString Warning="Do yo really wish to delete PVC for port:";
	Warning+=pszPort;
	Warning+=" and channel:";
	Warning+=pszChannel;
	if (MessageBox(Warning+" ?", "Attention!!!", MB_ICONQUESTION | MB_YESNO)!=IDYES) return;*/
	m_pNsgFrad->DeletePVC(pszPort, pszChannel);
	RefreshTable();
}

void CPvcForm::OnModifyButton() 
{
	static char pszPort[32], pszChannel[32];

	CPvcPropertiesDialog dialog(this);

/*	dialog.m_Port1=pszPort;
	dialog.m_Channel1=pszChannel;
	dialog.m_Port2=m_PvcCtrl.GetItemText(index, 2);
	dialog.m_Channel2=m_PvcCtrl.GetItemText(index, 3);

*/	if (dialog.DoModal()!=IDOK) return;
	m_pNsgFrad->DeletePVC(pszPort, pszChannel);
	m_pNsgFrad->AddPVC( (char *)LPCTSTR(dialog.m_Port1), (char *)LPCTSTR(dialog.m_Channel1),
						(char *)LPCTSTR(dialog.m_Port2), (char *)LPCTSTR(dialog.m_Channel2));

}
