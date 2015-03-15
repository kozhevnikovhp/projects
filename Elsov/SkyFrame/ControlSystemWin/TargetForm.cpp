// TargetForm.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "ControlSystemView.h"
#include "TargetInstance.h"
#include "TargetForm.h"
#include "Attached.h"
#include "ControlTargetAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTargetForm

IMPLEMENT_DYNCREATE(CTargetForm, CAbstractForm)

CTargetForm::CTargetForm()
	: CAbstractForm(CTargetForm::IDD)
{
	//{{AFX_DATA_INIT(CTargetForm)
	m_NameEdit = _T("");
	m_NameStatic = _T("");
	m_DbConnectionEdit = _T("");
	//}}AFX_DATA_INIT
}

CTargetForm::~CTargetForm()
{
}

void CTargetForm::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTargetForm)
	DDX_Text(pDX, IDC_NAME_EDIT, m_NameEdit);
	DDX_Text(pDX, IDC_NAME_STATIC, m_NameStatic);
	DDX_Text(pDX, IDC_DATABASE_CONNECTION_EDIT, m_DbConnectionEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTargetForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTargetForm)
	ON_BN_CLICKED(IDC_SET_DB_CONNECTION_BUTTON, OnSetDbConnectionButton)
	ON_BN_CLICKED(IDC_SET_NAME_BUTTON, OnSetNameButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTargetForm diagnostics

#ifdef _DEBUG
void CTargetForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTargetForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTargetForm message handlers

void CTargetForm::OnInitialUpdate() 
{
	m_NameStatic.LoadString(IDS_NAME_STATIC);
	CString str;
	// SetName button
	str.LoadString(IDS_SET_NAME);
	GetDescendantWindow(IDC_SET_NAME_BUTTON)->SetWindowText(str);
	// DB connection groupbox
	str.LoadString(IDS_DB_CONNECTION);
	GetDescendantWindow(IDC_DB_CONNECTION_STATIC)->SetWindowText(str);
	// Set DB connection button
	str.LoadString(IDS_SET_DB_CONNECTION);
	GetDescendantWindow(IDC_SET_DB_CONNECTION_BUTTON)->SetWindowText(str);
	
	CFormView::OnInitialUpdate();
}

//virtual
CTargetInstance *CTargetForm::DeleteSelectedObject(CAttachedBlock *pAttached)
{
	if (!CanBeDeleted())
		return NULL;
	return pAttached->GetTargetInstance();
}

//virtual
BOOL CTargetForm::SetupControls()
{
	CTargetInstance *pInstance = m_pAttached->GetTargetInstance();
	m_NameEdit = pInstance->GetTargetName();
	m_DbConnectionEdit = pInstance->GetDbConnectionString();

	UpdateData(FALSE);
	return TRUE;
}

//virtual
BOOL CTargetForm::BeforeHiding()
{
	return TRUE;
}


//virtual
CControlObject *CTargetForm::CreateNewObject(CTargetInstance *pTargetInstance)
{
	CControlObject *pNewObject = pTargetInstance->CreateControlObject(NULL);
	return pNewObject;
}

void CTargetForm::OnSetDbConnectionButton() 
{
	CTargetInstance *pTargetInstance = m_pAttached->GetTargetInstance();
	BOOL bCreated = pTargetInstance->CreateDB();
	if (!bCreated)
		return;
	CString NewConnectionString = pTargetInstance->GetDbConnectionString();
	if (!m_DbConnectionEdit.Compare(NewConnectionString))
		return; // do nothing, connection is the same
	m_DbConnectionEdit = NewConnectionString;
	UpdateData(FALSE);
	// Refresh all children
	GetTreeView()->RefreshSelectedTarget(pTargetInstance);
}

void CTargetForm::OnSetNameButton() 
{
	UpdateData(TRUE);
	CTargetInstance *pTargetInstance = m_pAttached->GetTargetInstance();
	pTargetInstance->SetTargetName(m_NameEdit);
	CString NewItemText;
	pTargetInstance->GetFullName(NewItemText);
	GetTreeView()->UpdateSelectedText(NewItemText);
}
