// StationPropertiesForm.cpp : implementation file
//

#include "stdafx.h"
#include "StationPropertiesForm.h"
#include "CuvsDoc.h"
#include "StationsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationPropertiesForm

IMPLEMENT_DYNCREATE(CStationPropertiesForm, CAbstractForm)

CStationPropertiesForm::CStationPropertiesForm()
	: CAbstractForm(CStationPropertiesForm::IDD)
{
	//{{AFX_DATA_INIT(CStationPropertiesForm)
	m_StationName = _T("");
	//}}AFX_DATA_INIT
}

CStationPropertiesForm::~CStationPropertiesForm()
{
}

void CStationPropertiesForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationPropertiesForm)
	DDX_Text(pDX, IDC_NAME_EDIT, m_StationName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStationPropertiesForm, CAbstractForm)
	//{{AFX_MSG_MAP(CStationPropertiesForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationPropertiesForm diagnostics

#ifdef _DEBUG
void CStationPropertiesForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CStationPropertiesForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CStationPropertiesForm::SetupControls()
{
	m_pDoc->GetStationName(m_pAttached->GetStationID(), m_StationName);
	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CStationPropertiesForm message handlers


void CStationPropertiesForm::OnApplyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pDoc->SetStationName(m_pAttached->GetStationID(), m_StationName);
	m_pStationsView->SetSelectedItemLabel(m_StationName);
}
