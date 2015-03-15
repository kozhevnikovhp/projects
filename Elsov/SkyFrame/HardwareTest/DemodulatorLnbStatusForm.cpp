// DemodulatorLnbStatusForm.cpp : implementation file
//

#include "stdafx.h"
#include "DemodulatorLnbStatusForm.h"
#include "Attached.h"
#include "Modem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorLnbStatusForm

IMPLEMENT_DYNCREATE(CDemodulatorLnbStatusForm, CAbstractForm)

CDemodulatorLnbStatusForm::CDemodulatorLnbStatusForm()
	: CAbstractForm(CDemodulatorLnbStatusForm::IDD)
{
	//{{AFX_DATA_INIT(CDemodulatorLnbStatusForm)
	m_Current = 0.0;
	m_CurrentMax = 0.0;
	m_CurrentMin = 0.0;
	m_HeaderString = _T("");
	//}}AFX_DATA_INIT
	m_pModem = NULL;
}

CDemodulatorLnbStatusForm::~CDemodulatorLnbStatusForm()
{
}

void CDemodulatorLnbStatusForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodulatorLnbStatusForm)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Text(pDX, IDC_CURRENT_EDIT, m_Current);
	DDX_Text(pDX, IDC_MAX_CURRENT_EDIT, m_CurrentMax);
	DDX_Text(pDX, IDC_MIN_CURRENT_EDIT, m_CurrentMin);
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemodulatorLnbStatusForm, CAbstractForm)
	//{{AFX_MSG_MAP(CDemodulatorLnbStatusForm)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorLnbStatusForm diagnostics

#ifdef _DEBUG
void CDemodulatorLnbStatusForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CDemodulatorLnbStatusForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CDemodulatorLnbStatusForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pModem = (CModem *)pDevice;
	m_HeaderString.Format("LNB status is displayed here");

	m_PowerSupplyCombo.initR(m_pModem, m_DeviceNumber);

	UpdateStatus();

	UpdateData(FALSE);
	return TRUE;
}

BOOL CDemodulatorLnbStatusForm::DoTimer()
{
	UpdateStatus();
	return TRUE;
}

void CDemodulatorLnbStatusForm::UpdateStatus()
{
	CLnbStatus status;
	m_pModem->GetLnbStatus(status, m_DeviceNumber);
	m_Current = status.m_Current;
	m_CurrentMin = status.m_CurrentMin;
	m_CurrentMax = status.m_CurrentMax;
	UpdateData(FALSE);

	int PowerSupplyMode = 0;
	m_pModem->GetRPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorLnbStatusForm message handlers


void CDemodulatorLnbStatusForm::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetRPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}
