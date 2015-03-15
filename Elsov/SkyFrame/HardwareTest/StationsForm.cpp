// StationsForm.cpp : implementation file
//

#include "stdafx.h"
#include "StationsForm.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationsForm

IMPLEMENT_DYNCREATE(CStationsForm, CAbstractForm)

CStationsForm::CStationsForm()
	: CAbstractForm(CStationsForm::IDD)
{
	//{{AFX_DATA_INIT(CStationsForm)
	m_StationConfigString = _T("");
	m_StatusString = _T("");
	//}}AFX_DATA_INIT
	m_pNsgFrad=NULL;
}

CStationsForm::~CStationsForm()
{
}

void CStationsForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationsForm)
	DDX_Control(pDX, IDC_STATION_NUMBER_COMBO, m_StationNumberCombo);
	DDX_Text(pDX, IDC_STATION_CONFIG_EDIT, m_StationConfigString);
	DDX_Text(pDX, IDC_STATE_EDIT, m_StatusString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationsForm, CAbstractForm)
	//{{AFX_MSG_MAP(CStationsForm)
	ON_CBN_SELCHANGE(IDC_STATION_NUMBER_COMBO, OnSelChangeStationNumberCombo)
	ON_BN_CLICKED(IDC_SET_STATION_CONFIG_BUTTON, OnSetStationConfigButton)
	ON_BN_CLICKED(IDC_WARM_BUTTON, OnWarmButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationsForm diagnostics

#ifdef _DEBUG
void CStationsForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CStationsForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CStationsForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;

	char pszStr[32];
	m_StationNumberCombo.ResetContent();
	for (int i=0; i<GetStationNum(); i++)
	{
		sprintf(pszStr, "%d", i);
		int index=m_StationNumberCombo.AddString(pszStr);
		m_StationNumberCombo.SetItemData(index, i);
	}

	m_StationNumberCombo.SetCurSel(0);
	OnSelChangeStationNumberCombo();

	GetDescendantWindow(IDC_WARM_BUTTON)->EnableWindow(CanWarmStart());
	return TRUE;
}

//virtual
BOOL CStationsForm::DoTimer()
{
	RefreshStatus();
	return TRUE;
}

void CStationsForm::RefreshStatus()
{
	UpdateData();
	int index=m_StationNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int StationNo=m_StationNumberCombo.GetItemData(index);
	CWaitCursor cursor;

	char *pszStatus=GetStationStatusString(StationNo);
	m_StatusString=pszStatus;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CStationsForm message handlers

void CStationsForm::OnSelChangeStationNumberCombo() 
{
	int index=m_StationNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int StationNo=m_StationNumberCombo.GetItemData(index);
	CWaitCursor cursor;

	m_StationConfigString=GetStationConfigString(StationNo);

	UpdateData(FALSE);
	RefreshStatus();
}

void CStationsForm::OnSetStationConfigButton() 
{
	int index=m_StationNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int StationNo=m_StationNumberCombo.GetItemData(index);
	CWaitCursor cursor;

	UpdateData();
	char *pszStationConfigString=(char *)LPCTSTR(m_StationConfigString);
	SetStationConfigString(StationNo, pszStationConfigString);
	m_StationConfigString=pszStationConfigString;
	UpdateData(FALSE);
	RefreshStatus();
}

void CStationsForm::OnWarmButton() 
{
	int index=m_StationNumberCombo.GetCurSel();
	if (index==CB_ERR) return;
	int StationNo=m_StationNumberCombo.GetItemData(index);
	CWaitCursor cursor;

	WarmStart(StationNo);
	RefreshStatus();
}
