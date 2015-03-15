// StationForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "StationForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"
#include "StationsView.h"

/////////////////////////////////////////////////////////////////////////////
// CStationForm

IMPLEMENT_DYNCREATE(CStationForm, CAbstractForm)

CStationForm::CStationForm()
	: CAbstractForm(CStationForm::IDD)
{
	//{{AFX_DATA_INIT(CStationForm)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}

CStationForm::~CStationForm()
{
}

void CStationForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationForm)
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	//}}AFX_DATA_MAP
}

//virtual
BOOL CStationForm::SetupControls()
{
	m_Name=m_pStation->GetName();
	UpdateData(FALSE);
	return TRUE;
}

//virtual
BOOL CStationForm::IsAlarm(CStation *pStation)
{
	return !pStation->IsSpConnected() || !pStation->IsRChConnected() || !pStation->IsTmiConnected();
}

BEGIN_MESSAGE_MAP(CStationForm, CAbstractForm)
	//{{AFX_MSG_MAP(CStationForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationForm diagnostics

#ifdef _DEBUG
void CStationForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CStationForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStationForm message handlers

void CStationForm::OnApplyButton() 
{
	UpdateData();
	m_pStation->SetName(m_Name);
	m_pStationsView->UpdateSelectedStationName();
}
