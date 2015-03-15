// DeviceForm.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceForm.h"
#include "Attached.h"
#include "Device.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceForm

IMPLEMENT_DYNCREATE(CDeviceForm, CAbstractForm)

CDeviceForm::CDeviceForm()
	: CAbstractForm(CDeviceForm::IDD)
{
	//{{AFX_DATA_INIT(CDeviceForm)
	m_bNVRAMEnabled = FALSE;
	m_StatusString = _T("");
	m_VersionString = _T("");
	m_bSaveInNvRamOnTheFlyEnabled = FALSE;
	m_UnitName = _T("");
	//}}AFX_DATA_INIT
	m_pDevice=NULL;
}

CDeviceForm::~CDeviceForm()
{
}

void CDeviceForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceForm)
	DDX_Control(pDX, IDC_SET_UNIT_NAME_BUTTON, m_SetUnitNameButton);
	DDX_Control(pDX, IDC_UNIT_NAME_EDIT, m_UnitNameCtrl);
	DDX_Control(pDX, IDC_SAVE_ON_THE_FLY_CHECK, m_SaveInNvRamOnTheFlyCheck);
	DDX_Control(pDX, IDC_FACTORY_PRESET_BUTTON, m_FactoryPresetButton);
	DDX_Control(pDX, IDC_SAVE_SETTINGS_BUTTON, m_SaveButton);
	DDX_Control(pDX, IDC_WARM_RESET_BUTTON, m_WarmResetButton);
	DDX_Control(pDX, IDC_TOTAL_RESET_BUTTON, m_TotalResetButton);
	DDX_Control(pDX, IDC_NVRAM_ENABLED_CHECK, m_NVRAMCheck);
	DDX_Check(pDX, IDC_NVRAM_ENABLED_CHECK, m_bNVRAMEnabled);
	DDX_Text(pDX, IDC_STATE_EDIT, m_StatusString);
	DDX_Text(pDX, IDC_VERSION_EDIT, m_VersionString);
	DDX_Check(pDX, IDC_SAVE_ON_THE_FLY_CHECK, m_bSaveInNvRamOnTheFlyEnabled);
	DDX_Text(pDX, IDC_UNIT_NAME_EDIT, m_UnitName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeviceForm, CAbstractForm)
	//{{AFX_MSG_MAP(CDeviceForm)
	ON_BN_CLICKED(IDC_WARM_RESET_BUTTON, OnWarmResetButton)
	ON_BN_CLICKED(IDC_TOTAL_RESET_BUTTON, OnTotalResetButton)
	ON_BN_CLICKED(IDC_NVRAM_ENABLED_CHECK, OnNVRAMEnabledCheck)
	ON_BN_CLICKED(IDC_SAVE_ON_THE_FLY_CHECK, OnSaveOnTheFlyCheck)
	ON_BN_CLICKED(IDC_SAVE_SETTINGS_BUTTON, OnSaveSettingsButton)
	ON_BN_CLICKED(IDC_FACTORY_PRESET_BUTTON, OnFactoryPresetButton)
	ON_BN_CLICKED(IDC_SET_UNIT_NAME_BUTTON, OnSetUnitNameButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceForm diagnostics

#ifdef _DEBUG
void CDeviceForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CDeviceForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

 BOOL CDeviceForm::SetupControls()
{
	m_pDevice=m_pAttached->m_pDevice;

	m_NVRAMCheck.EnableWindow(m_pDevice->CanControlNvRam());
	m_pDevice->IsNvRamEnabled(m_bNVRAMEnabled);

	m_SaveInNvRamOnTheFlyCheck.EnableWindow(m_pDevice->CanControlParamsSavingInNvRamRightAfterSetting());
	m_pDevice->IsNvRamSavingRightAfterSettingEnabled(m_bSaveInNvRamOnTheFlyEnabled);

	m_SaveButton.EnableWindow(m_pDevice->CanWriteConfiguration());
	m_FactoryPresetButton.EnableWindow(m_pDevice->CanFactoryPreset());

	m_WarmResetButton.EnableWindow(m_pDevice->CanWarmReset());
	m_TotalResetButton.EnableWindow(m_pDevice->CanTotalReset());

	m_SetUnitNameButton.EnableWindow(m_pDevice->CanSetUnitName());
	m_UnitNameCtrl.EnableWindow(m_pDevice->CanSetUnitName());

	char *pszUnitName;
	m_pDevice->GetUnitName(pszUnitName);
	m_UnitName = pszUnitName;

	UpdateVersionString();
	RefreshStatus();
	
	return TRUE;
}

BOOL CDeviceForm::DoTimer()
{
	RefreshStatus();
	return TRUE;
}

void CDeviceForm::UpdateVersionString()
{
	char *pszVersion;
	m_pDevice->GetDeviceVersionString(pszVersion);
	m_VersionString = pszVersion;
	UpdateData(FALSE);
}

void CDeviceForm::RefreshStatus()
{
	CWaitCursor cursor;
	UpdateData();
	char *pszStatus;
	m_pDevice->GetDeviceStatusString(pszStatus);
	m_StatusString = pszStatus;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDeviceForm message handlers

void CDeviceForm::OnWarmResetButton() 
{
	CWaitCursor cursor;
	m_pDevice->WarmReset();
	UpdateVersionString();
}

void CDeviceForm::OnTotalResetButton() 
{
	CWaitCursor cursor;
	m_pDevice->TotalReset();
	UpdateVersionString();
}

void CDeviceForm::OnNVRAMEnabledCheck() 
{
	UpdateData();
	m_pDevice->EnableNvRam(m_bNVRAMEnabled);
	UpdateData(FALSE);
}

void CDeviceForm::OnSaveOnTheFlyCheck() 
{
	UpdateData();
	m_pDevice->EnableNvRamSavingRightAfterSetting(m_bSaveInNvRamOnTheFlyEnabled);
	UpdateData(FALSE);
}

void CDeviceForm::OnSaveSettingsButton() 
{
	CWaitCursor cursor;
	m_pDevice->WriteConfiguration();
}

void CDeviceForm::OnFactoryPresetButton() 
{
	m_pDevice->FactoryPreset();	
}


void CDeviceForm::OnSetUnitNameButton() 
{
	UpdateData();
	char *pszUnitName = (char *)LPCTSTR(m_UnitName);
	m_pDevice->SetUnitName(pszUnitName);
}
