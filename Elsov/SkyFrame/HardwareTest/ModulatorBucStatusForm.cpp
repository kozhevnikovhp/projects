// ModulatorBucStatusForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModulatorBucStatusForm.h"
#include "Attached.h"
#include "Modem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModulatorBucStatusForm

IMPLEMENT_DYNCREATE(CModulatorBucStatusForm, CAbstractForm)

CModulatorBucStatusForm::CModulatorBucStatusForm()
	: CAbstractForm(CModulatorBucStatusForm::IDD)
{
	//{{AFX_DATA_INIT(CModulatorBucStatusForm)
	m_Current = 0.0;
	m_CurrentMax = 0.0;
	m_CurrentMin = 0.0;
	m_Voltage = 0.0;
	m_HeaderString = _T("");
	//}}AFX_DATA_INIT
	m_pModem = NULL;
}

CModulatorBucStatusForm::~CModulatorBucStatusForm()
{
}

void CModulatorBucStatusForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModulatorBucStatusForm)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Text(pDX, IDC_CURRENT_EDIT, m_Current);
	DDX_Text(pDX, IDC_MAX_CURRENT_EDIT, m_CurrentMax);
	DDX_Text(pDX, IDC_MIN_CURRENT_EDIT, m_CurrentMin);
	DDX_Text(pDX, IDC_VOLTAGE_EDIT, m_Voltage);
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModulatorBucStatusForm, CAbstractForm)
	//{{AFX_MSG_MAP(CModulatorBucStatusForm)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModulatorBucStatusForm diagnostics

#ifdef _DEBUG
void CModulatorBucStatusForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CModulatorBucStatusForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CModulatorBucStatusForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pModem = (CModem *)pDevice;
	m_HeaderString.Format("BUC status is displayed here");

	m_PowerSupplyCombo.initT(m_pModem, m_DeviceNumber);

	UpdateStatus();

	UpdateData(FALSE);
	return TRUE;
}

BOOL CModulatorBucStatusForm::DoTimer()
{
	UpdateStatus();
	return TRUE;
}

void CModulatorBucStatusForm::UpdateStatus()
{
	CBucStatus status;
	m_pModem->GetBucStatus(status, m_DeviceNumber);
	m_Current = status.m_Current;
	m_CurrentMin = status.m_CurrentMin;
	m_CurrentMax = status.m_CurrentMax;
	m_Voltage = status.m_Voltage;
	UpdateData(FALSE);

	int PowerSupplyMode = 0;
	m_pModem->GetTPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

/////////////////////////////////////////////////////////////////////////////
// CModulatorBucStatusForm message handlers


void CModulatorBucStatusForm::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetTPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}
