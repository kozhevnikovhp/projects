// ModulatorCarrierForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModulatorCarrierForm.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierForm

IMPLEMENT_DYNCREATE(CModulatorCarrierForm, CAbstractForm)

CModulatorCarrierForm::CModulatorCarrierForm()
	: CAbstractForm(CModulatorCarrierForm::IDD)
{
	//{{AFX_DATA_INIT(CModulatorCarrierForm)
	m_Frequency = 0;
	m_bOutputOn = FALSE;
	m_bSpectralInvEnabled = FALSE;
	m_bContiniousWaveOn = FALSE;
	m_OutputLevel = 0.0;
	m_Shift = 0;
	m_FineTune = 0;
	//}}AFX_DATA_INIT
	m_pModem=NULL;
}

CModulatorCarrierForm::~CModulatorCarrierForm()
{
}

void CModulatorCarrierForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModulatorCarrierForm)
	DDX_Control(pDX, IDC_BUC_10MHZ_COMBO, m_Buc10MHzCombo);
	DDX_Control(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevelEdit);
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Control(pDX, IDC_SET_FINE_TUNE_BUTTON, m_SetFineTuneButton);
	DDX_Control(pDX, IDC_FINE_TUNE_SPIN, m_FineTuneSpin);
	DDX_Control(pDX, IDC_FINE_TUNE_EDIT, m_FineTuneEdit);
	DDX_Control(pDX, IDC_SHIFT_SPIN, m_ShiftSpin);
	DDX_Control(pDX, IDC_SHIFT_EDIT, m_ShiftEdit);
	DDX_Control(pDX, IDC_SET_SHIFT_BUTTON, m_SetShiftButton);
	DDX_Control(pDX, IDC_SET_FREQUENCY_BUTTON, m_SetFrequencyButton);
	DDX_Control(pDX, IDC_TRANSMITTER_FREQUENCY_EDIT, m_FrequencyCtrl);
	DDX_Control(pDX, IDC_CW_CHECK, m_ContiniousWaveCtrl);
	DDX_Control(pDX, IDC_ONOFF_CHECK, m_OutputOnOffCheck);
	DDX_Control(pDX, IDC_SPECTRAL_INV_CHECK, m_SpectralInvCheck);
	DDX_Control(pDX, IDC_OUTPUT_LEVEL_SPIN, m_OutputLevelSpin);
	DDX_Control(pDX, IDC_TRANSMITTER_FREQUENCY_SPIN, m_FrequencySpin);
	DDX_Text(pDX, IDC_TRANSMITTER_FREQUENCY_EDIT, m_Frequency);
	DDX_Check(pDX, IDC_ONOFF_CHECK, m_bOutputOn);
	DDX_Check(pDX, IDC_SPECTRAL_INV_CHECK, m_bSpectralInvEnabled);
	DDX_Check(pDX, IDC_CW_CHECK, m_bContiniousWaveOn);
	DDX_Text(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevel);
	DDX_Text(pDX, IDC_SHIFT_EDIT, m_Shift);
	DDX_Text(pDX, IDC_FINE_TUNE_EDIT, m_FineTune);
	//}}AFX_DATA_MAP
	if (m_pModem)
		DDV_MinMaxDouble(pDX, m_OutputLevel, m_pModem->GetMinOutputLevel(), m_pModem->GetMaxOutputLevel());
}

BEGIN_MESSAGE_MAP(CModulatorCarrierForm, CAbstractForm)
	//{{AFX_MSG_MAP(CModulatorCarrierForm)
	ON_BN_CLICKED(IDC_SET_FREQUENCY_BUTTON, OnSetFrequencyButton)
	ON_BN_CLICKED(IDC_SET_OUTPUT_LEVEL_BUTTON, OnSetOutputLevelButton)
	ON_BN_CLICKED(IDC_ONOFF_CHECK, OnOutputOnOffCheck)
	ON_BN_CLICKED(IDC_SPECTRAL_INV_CHECK, OnSpectralInvCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_OUTPUT_LEVEL_SPIN, OnDeltaPosOutputLevelSpin)
	ON_BN_CLICKED(IDC_CW_CHECK, OnCwCheck)
	ON_BN_CLICKED(IDC_SET_SHIFT_BUTTON, OnSetShiftButton)
	ON_BN_CLICKED(IDC_SET_FINE_TUNE_BUTTON, OnSetFineTuneButton)
	ON_CBN_SELCHANGE(IDC_MODULATION_TYPE_COMBO, OnSelChangeModulationTypeCombo)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	ON_CBN_SELCHANGE(IDC_BUC_10MHZ_COMBO, OnSelChangeBuc10MHzCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierForm diagnostics

#ifdef _DEBUG
void CModulatorCarrierForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CModulatorCarrierForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

void CModulatorCarrierForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();
	m_FrequencySpin.SetBuddy(&m_FrequencyCtrl);
	m_ShiftSpin.SetBuddy(&m_ShiftEdit);
	m_ShiftSpin.SetRange32(-100000, +100000);
	m_OutputLevelSpin.SetBuddy(&m_OutputLevelEdit);
	m_FineTuneSpin.SetBuddy(&m_FineTuneEdit);
}

BOOL CModulatorCarrierForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pModem = (CModem *)pDevice;

	m_FrequencySpin.SetRange32(m_pModem->GetMinTFrequency(), m_pModem->GetMaxTFrequency());
	m_OutputLevelSpin.SetRange32((int)m_pModem->GetMinOutputLevel(), (int)m_pModem->GetMaxOutputLevel());

	CModIfParams Params;
	MC_ErrorCode EC = m_pModem->GetTIfParams(Params, m_DeviceNumber);

	// frequency
	m_Frequency = Params.m_Frequency;
	m_FrequencyCtrl.EnableWindow(m_pModem->CanSetTFrequency());
	m_FrequencySpin.EnableWindow(m_pModem->CanSetTFrequency());
	m_SetFrequencyButton.EnableWindow(m_pModem->CanSetTFrequency());

	// Shift
	m_Shift = Params.m_Offset;
	m_ShiftSpin.SetPos(m_Shift);
	m_ShiftSpin.EnableWindow(m_pModem->CanSetModulatorShift());
	m_ShiftEdit.EnableWindow(m_pModem->CanSetModulatorShift());
	m_SetShiftButton.EnableWindow(m_pModem->CanSetModulatorShift());

	// Ref. generator fine tune
	m_pModem->GetModulatorRefFineTune(m_FineTune, m_DeviceNumber);
	m_FineTuneSpin.SetPos(m_FineTune);
	m_FineTuneSpin.SetRange32(m_pModem->GetModulatorRefFineTuneMinValue(), m_pModem->GetModulatorRefFineTuneMaxValue());
	m_FineTuneSpin.EnableWindow(m_pModem->CanModulatorRefFineTune());
	m_FineTuneEdit.EnableWindow(m_pModem->CanModulatorRefFineTune());
	m_SetFineTuneButton.EnableWindow(m_pModem->CanModulatorRefFineTune());

	// output
	m_OutputOnOffCheck.EnableWindow(m_pModem->CanOutputOnOff());
	GetDescendantWindow(IDC_OUTPUT_LEVEL_EDIT)->EnableWindow(m_pModem->CanOutputLevel());
	GetDescendantWindow(IDC_OUTPUT_LEVEL_SPIN)->EnableWindow(m_pModem->CanOutputLevel());
	GetDescendantWindow(IDC_SET_OUTPUT_LEVEL_BUTTON)->EnableWindow(m_pModem->CanOutputLevel());
	m_OutputLevel = Params.m_OutputLevel;
	m_bOutputOn = Params.m_bOutputEnabled;
	
	// Power supply
	m_PowerSupplyCombo.initT(m_pModem, m_DeviceNumber);
	UpdateBucStatus();
	
	// 10 MHz Reference (internal/external)
	m_Buc10MHzCombo.initT(m_pModem, m_DeviceNumber);
	m_Buc10MHzCombo.EnableWindow(m_pModem->CanT10MHzSupply());

	// Modulation type
	m_ModulationTypeCombo.initT(m_pModem, m_DeviceNumber);

	m_bContiniousWaveOn = Params.m_bContiniousWave;
	m_ContiniousWaveCtrl.EnableWindow(m_pModem->CanContiniousWave());

	// Spectral Inversion
	m_SpectralInvCheck.EnableWindow(m_pModem->CanTSpectralInv());
	m_bSpectralInvEnabled = Params.m_bSpectrumInversion;

	UpdateData(FALSE);

	return TRUE;
}

BOOL CModulatorCarrierForm::DoTimer()
{
	UpdateBucStatus();
	return TRUE;
}

void CModulatorCarrierForm::UpdateBucStatus()
{
	CBucStatus status;
	m_pModem->GetBucStatus(status, m_DeviceNumber);
	CString str; str.Format("%.3f", status.m_Current);
	GetDescendantWindow(IDC_CURRENT_EDIT)->SetWindowText(str);

	int PowerSupplyMode = 0;
	m_pModem->GetTPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierForm message handlers

void CModulatorCarrierForm::OnSetFrequencyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	MC_ErrorCode EC = m_pModem->SetTFrequency(m_Frequency, m_DeviceNumber);
	ReportCommandExecution(EC, m_pModem);

	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnSetOutputLevelButton() 
{
	CWaitCursor cursor;
	UpdateData();
	MC_ErrorCode EC = m_pModem->SetOutputLevel(m_OutputLevel, m_DeviceNumber);
	ReportCommandExecution(EC, m_pModem);

	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnOutputOnOffCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->TurnOutputOn(m_bOutputOn, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnSpectralInvCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->EnableTSpectralInv(m_bSpectralInvEnabled, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnDeltaPosOutputLevelSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateData();
	m_OutputLevel += pNMUpDown->iDelta/10.;
	if (m_OutputLevel > m_pModem->GetMaxOutputLevel())
		m_OutputLevel = m_pModem->GetMaxOutputLevel();
	if (m_OutputLevel < m_pModem->GetMinOutputLevel())
		m_OutputLevel = m_pModem->GetMinOutputLevel();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CModulatorCarrierForm::OnCwCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->TurnContiniousWaveOn(m_bContiniousWaveOn, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnSetShiftButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->SetModulatorShift(m_Shift, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnSetFineTuneButton() 
{
	CWaitCursor cursor;
	UpdateData();

	if (m_FineTune > m_pModem->GetModulatorRefFineTuneMaxValue())
		m_FineTune = m_pModem->GetModulatorRefFineTuneMaxValue();
	if (m_FineTune < m_pModem->GetModulatorRefFineTuneMinValue())
		m_FineTune = m_pModem->GetModulatorRefFineTuneMinValue();

	m_pModem->SetModulatorRefFineTune(m_FineTune, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorCarrierForm::OnSelChangeModulationTypeCombo() 
{
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	m_pModem->SetTModulationType(ModulationType, m_DeviceNumber);
	m_ModulationTypeCombo.SelectByDataValue(ModulationType);
}

void CModulatorCarrierForm::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetTPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	UpdateBucStatus();
}

void CModulatorCarrierForm::OnSelChangeBuc10MHzCombo() 
{
	int mode = m_Buc10MHzCombo.getSelectedMode();
	m_pModem->SetT10MHzMode(mode, m_DeviceNumber);
	m_Buc10MHzCombo.SelectByDataValue(mode);
	UpdateBucStatus();
}
