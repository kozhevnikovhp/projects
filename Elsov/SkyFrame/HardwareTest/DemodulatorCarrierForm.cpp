// DemodulatorCarrierForm.cpp : implementation file
//

#include "stdafx.h"
#include "DemodulatorCarrierForm.h"
#include "ProgressDlg.h"
#include "Attached.h"
#include "Modem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorCarrierForm

IMPLEMENT_DYNCREATE(CDemodulatorCarrierForm, CAbstractForm)

CDemodulatorCarrierForm::CDemodulatorCarrierForm()
	: CAbstractForm(CDemodulatorCarrierForm::IDD)
{
	//{{AFX_DATA_INIT(CDemodulatorCarrierForm)
	m_Frequency = 0;
	m_SearchRange = 0;
	m_bSpectralInvEnabled = FALSE;
	m_Shift = 0;
	m_FineTune = 0;
	//}}AFX_DATA_INIT
	m_pModem=NULL;
	m_bTimerEnabled=TRUE;
}

CDemodulatorCarrierForm::~CDemodulatorCarrierForm()
{
}

void CDemodulatorCarrierForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodulatorCarrierForm)
	DDX_Control(pDX, IDC_LNB_10MHZ_COMBO, m_Lnb10MHzCombo);
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Control(pDX, IDC_SET_FINE_TUNE_BUTTON, m_SetFineTuneButton);
	DDX_Control(pDX, IDC_FINE_TUNE_SPIN, m_FineTuneSpin);
	DDX_Control(pDX, IDC_FINE_TUNE_EDIT, m_FineTuneEdit);
	DDX_Control(pDX, IDC_SET_SHIFT_BUTTON, m_SetShiftButton);
	DDX_Control(pDX, IDC_SHIFT_EDIT, m_ShiftEdit);
	DDX_Control(pDX, IDC_SHIFT_SPIN, m_ShiftSpin);
	DDX_Control(pDX, IDC_SET_RANGE_BUTTON, m_SetSearchRangeButton);
	DDX_Control(pDX, IDC_SEARCH_RANGE_EDIT, m_SearchRangeCtrl);
	DDX_Control(pDX, IDC_MEASURE_BUTTON, m_MeasureTimeButton);
	DDX_Control(pDX, IDC_SET_FREQUENCY_BUTTON, m_SetFrequencyButton);
	DDX_Control(pDX, IDC_RECEIVER_FREQUENCY_EDIT, m_FrequencyCtrl);
	DDX_Control(pDX, IDC_SPECTRAL_INV_CHECK, m_SpectralInvCheck);
	DDX_Control(pDX, IDC_SEARCH_RANGE_SPIN, m_SearchRangeSpin);
	DDX_Control(pDX, IDC_RECEIVER_FREQUENCY_SPIN, m_ReceiverFrequencySpin);
	DDX_Text(pDX, IDC_RECEIVER_FREQUENCY_EDIT, m_Frequency);
	DDV_MinMaxUInt(pDX, m_Frequency, 0, 2000000);
	DDX_Text(pDX, IDC_SEARCH_RANGE_EDIT, m_SearchRange);
	DDX_Check(pDX, IDC_SPECTRAL_INV_CHECK, m_bSpectralInvEnabled);
	DDX_Text(pDX, IDC_SHIFT_EDIT, m_Shift);
	DDX_Text(pDX, IDC_FINE_TUNE_EDIT, m_FineTune);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemodulatorCarrierForm, CAbstractForm)
	//{{AFX_MSG_MAP(CDemodulatorCarrierForm)
	ON_BN_CLICKED(IDC_SET_FREQUENCY_BUTTON, OnSetFrequencyButton)
	ON_BN_CLICKED(IDC_SET_SHIFT_BUTTON, OnSetShiftButton)
	ON_BN_CLICKED(IDC_SET_RANGE_BUTTON, OnSetRangeButton)
	ON_BN_CLICKED(IDC_SPECTRAL_INV_CHECK, OnSpectralInvCheck)
	ON_BN_CLICKED(IDC_MEASURE_BUTTON, OnMeasureButton)
	ON_BN_CLICKED(IDC_SET_FINE_TUNE_BUTTON, OnSetFineTuneButton)
	ON_CBN_SELCHANGE(IDC_MODULATION_TYPE_COMBO, OnSelChangeModulationTypeCombo)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	ON_CBN_SELCHANGE(IDC_LNB_10MHZ_COMBO, OnSelChangeLnb10MHzCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorCarrierForm diagnostics

#ifdef _DEBUG
void CDemodulatorCarrierForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CDemodulatorCarrierForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

void CDemodulatorCarrierForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();
	m_ShiftSpin.SetBuddy(&m_ShiftEdit);
	m_ShiftSpin.SetRange32(-100000, +100000);

	m_FineTuneSpin.SetBuddy(&m_FineTuneEdit);
}

BOOL CDemodulatorCarrierForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pModem = (CModem *)pDevice;

	m_ReceiverFrequencySpin.SetRange32(m_pModem->GetMinRFrequency(), m_pModem->GetMaxRFrequency());
	m_SearchRangeSpin.SetRange32(0, 100000);
	
	CDemIfParams Params;
	m_pModem->GetRIfParams(Params, m_DeviceNumber);

	// Frequency
	m_Frequency = Params.m_Frequency;
	m_FrequencyCtrl.EnableWindow(m_pModem->CanSetRFrequency());
	m_ReceiverFrequencySpin.EnableWindow(m_pModem->CanSetRFrequency());
	m_SetFrequencyButton.EnableWindow(m_pModem->CanSetRFrequency());
	m_MeasureTimeButton.EnableWindow(m_pModem->CanSetRFrequency());
	
	// Search range
	m_SearchRange = Params.m_SearchRange;
	m_SearchRangeCtrl.EnableWindow(m_pModem->CanSetSearchRange());
	m_SearchRangeSpin.EnableWindow(m_pModem->CanSetSearchRange());
	m_SetSearchRangeButton.EnableWindow(m_pModem->CanSetSearchRange());

	// Shift
	m_pModem->GetDemodulatorShift(m_Shift, m_DeviceNumber);
	m_ShiftSpin.SetPos(m_Shift);
	m_ShiftSpin.EnableWindow(m_pModem->CanSetDemodulatorShift());
	m_ShiftEdit.EnableWindow(m_pModem->CanSetDemodulatorShift());
	m_SetShiftButton.EnableWindow(m_pModem->CanSetDemodulatorShift());

	// Ref. generator fine tune
	m_pModem->GetDemodulatorRefFineTune(m_FineTune, m_DeviceNumber);
	m_FineTuneSpin.SetPos(m_FineTune);
	m_FineTuneSpin.SetRange32(m_pModem->GetDemodulatorRefFineTuneMinValue(), m_pModem->GetDemodulatorRefFineTuneMaxValue());
	m_FineTuneSpin.EnableWindow(m_pModem->CanDemodulatorRefFineTune());
	m_FineTuneEdit.EnableWindow(m_pModem->CanDemodulatorRefFineTune());
	m_SetFineTuneButton.EnableWindow(m_pModem->CanDemodulatorRefFineTune());

	UpdateEbNoAndOffset();
	UpdateLnbStatus();

	// 10 MHz Reference (internal/external)
	m_Lnb10MHzCombo.initR(m_pModem, m_DeviceNumber);
	m_Lnb10MHzCombo.EnableWindow(m_pModem->CanR10MHzSupply());

	// Power supplier
	m_PowerSupplyCombo.initR(m_pModem, m_DeviceNumber);

	// Modulation type
	m_ModulationTypeCombo.initR(m_pModem, m_DeviceNumber);

	// Spectral Inversion
	m_SpectralInvCheck.EnableWindow(m_pModem->CanRSpectralInv());
	m_bSpectralInvEnabled = Params.m_bSpectrumInversion;
	
	UpdateData(FALSE);
	return TRUE;
}

BOOL CDemodulatorCarrierForm::DoTimer()
{
	if (!m_bTimerEnabled)
		return TRUE;
	UpdateEbNoAndOffset();
	UpdateLnbStatus();

	return TRUE;
}

void CDemodulatorCarrierForm::UpdateEbNoAndOffset()
{
	const char *pszLocked = "LOCKED";
	const char *pszNotLocked = "NOT LOCKED";
	CString str;
	CDemIfStatus status;
	m_pModem->GetRIfStatus(status, m_DeviceNumber);
	if (status.m_bCarrier)
		str.Format("Eb/No = %4.1f dB", status.m_SN);
	else
		str = pszNotLocked;
	GetDescendantWindow(IDC_CARRIER_STATIC)->SetWindowText(str);

	if (status.m_bDemLock)
		str = pszLocked;
	else
		str = pszNotLocked;
	GetDescendantWindow(IDC_DEMODULATOR_LOCK_STATIC)->SetWindowText(str);

	str.Format("Offset = %+5d Hz", status.m_Offset);
	GetDescendantWindow(IDC_OFFSET_STATIC)->SetWindowText(str);

	if (!m_pModem->CanGetInputLevel())
		GetDescendantWindow(IDC_INPUT_LEVEL_STATIC)->SetWindowText("");
	else
	{
		str.Format("Input level = %6.2f dBm", status.m_InputLevel);
		GetDescendantWindow(IDC_INPUT_LEVEL_STATIC)->SetWindowText(str);
	}


//	double ber;
//	m_pModem->GetBER(ber, m_DeviceNumber);
}

void CDemodulatorCarrierForm::UpdateLnbStatus()
{
	CLnbStatus status;
	m_pModem->GetLnbStatus(status, m_DeviceNumber);
	CString str; str.Format("%.3f", status.m_Current);
	GetDescendantWindow(IDC_CURRENT_EDIT)->SetWindowText(str);

	int PowerSupplyMode = 0;
	m_pModem->GetRPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorCarrierForm message handlers

void CDemodulatorCarrierForm::OnSetFrequencyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	MC_ErrorCode EC = m_pModem->SetRFrequency(m_Frequency, m_DeviceNumber);
	ReportCommandExecution(EC, m_pModem);
	UpdateData(FALSE);
}

void CDemodulatorCarrierForm::OnSetShiftButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->SetDemodulatorShift(m_Shift, m_DeviceNumber);
	UpdateData(FALSE);
}

void CDemodulatorCarrierForm::OnSetRangeButton() 
{
	CWaitCursor cursor;
	UpdateData();
	MC_ErrorCode EC = m_pModem->SetSearchRange(m_SearchRange, m_DeviceNumber);
	ReportCommandExecution(EC, m_pModem);
	UpdateData(FALSE);
}

void CDemodulatorCarrierForm::OnSpectralInvCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->EnableRSpectralInv(m_bSpectralInvEnabled, m_DeviceNumber);
	UpdateData(FALSE);
}

void CDemodulatorCarrierForm::OnMeasureButton() 
{
	BOOL bTimerEnabled=m_bTimerEnabled;
	m_bTimerEnabled=FALSE;

	CProgressDlg dlg;
	dlg.Create();
	UpdateData();
	DWORD DeltaTime=0, StartTime=timeGetTime();
	m_pModem->SetRFrequency(m_Frequency, m_DeviceNumber);
	BOOL bCarrier=FALSE;
	BOOL bCancel=FALSE;
	do
	{
		m_pModem->IsRCarrier(bCarrier, m_DeviceNumber);
		DeltaTime = timeGetTime()-StartTime;
		dlg.SetPos(DeltaTime);
		bCancel = dlg.CheckCancelButton();

	} while (!bCarrier && !bCancel);

	CString str;
	if (bCarrier)
		str.Format("Carrier captured in %d msec", DeltaTime);
	if (bCancel)
		str = "No carrier detected";
	MessageBox(str, "Information", MB_ICONINFORMATION);
	
	m_bTimerEnabled = bTimerEnabled;
}

void CDemodulatorCarrierForm::OnSetFineTuneButton() 
{
	CWaitCursor cursor;
	UpdateData();

	if (m_FineTune > m_pModem->GetDemodulatorRefFineTuneMaxValue())
		m_FineTune = m_pModem->GetDemodulatorRefFineTuneMaxValue();
	if (m_FineTune < m_pModem->GetDemodulatorRefFineTuneMinValue())
		m_FineTune = m_pModem->GetDemodulatorRefFineTuneMinValue();

	m_pModem->SetDemodulatorRefFineTune(m_FineTune, m_DeviceNumber);
	UpdateData(FALSE);
}

void CDemodulatorCarrierForm::OnSelChangeModulationTypeCombo() 
{
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	m_pModem->SetRModulationType(ModulationType, m_DeviceNumber);
	m_ModulationTypeCombo.SelectByDataValue(ModulationType);
}

void CDemodulatorCarrierForm::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetRPowerSupplyMode(PowerSupplyMode, m_DeviceNumber);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	UpdateLnbStatus();
}

void CDemodulatorCarrierForm::OnSelChangeLnb10MHzCombo() 
{
	int mode = m_Lnb10MHzCombo.getSelectedMode();
	m_pModem->SetR10MHzMode(mode, m_DeviceNumber);
	m_Lnb10MHzCombo.SelectByDataValue(mode);
	UpdateLnbStatus();
}
