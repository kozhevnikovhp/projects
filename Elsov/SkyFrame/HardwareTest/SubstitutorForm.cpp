// Substitutor.cpp : implementation file
//

#include "stdafx.h"
#include "SubstitutorForm.h"
#include "DeviceSubstitutor.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubstitutorForm

IMPLEMENT_DYNCREATE(CSubstitutorForm, CAbstractForm)

CSubstitutorForm::CSubstitutorForm()
	: CAbstractForm(CSubstitutorForm::IDD)
{
	//{{AFX_DATA_INIT(CSubstitutorForm)
	m_bEnabled = FALSE;
	//}}AFX_DATA_INIT
	m_pSubstitutor = NULL;
}

CSubstitutorForm::~CSubstitutorForm()
{
}

void CSubstitutorForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubstitutorForm)
	DDX_Check(pDX, IDC_ENABLE_CHECK, m_bEnabled);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSubstitutorForm, CAbstractForm)
	//{{AFX_MSG_MAP(CSubstitutorForm)
	ON_BN_CLICKED(IDC_ENABLE_CHECK, OnEnableCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSubstitutorForm diagnostics

#ifdef _DEBUG
void CSubstitutorForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CSubstitutorForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CSubstitutorForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pSubstitutor = (CDeviceSubstitutor *)pDevice;

	m_bEnabled = m_pSubstitutor->IsStarted();

	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSubstitutorForm message handlers

void CSubstitutorForm::OnEnableCheck() 
{
	UpdateData();
	if (m_bEnabled)
		m_pSubstitutor->Start();
	else
		m_pSubstitutor->Stop();
}
