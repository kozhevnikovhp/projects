// ModulatorDataForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModulatorDataForm.h"
#include "Attached.h"
#include "Modem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModulatorDataForm

IMPLEMENT_DYNCREATE(CModulatorDataForm, CDataForm)

CModulatorDataForm::CModulatorDataForm()
	: CDataForm(CDataForm::IDD)
{
	//{{AFX_DATA_INIT(CModulatorDataForm)
	m_DataRate = 0;
	//}}AFX_DATA_INIT
}

CModulatorDataForm::~CModulatorDataForm()
{
}

void CModulatorDataForm::DoDataExchange(CDataExchange* pDX)
{
	CDataForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModulatorDataForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModulatorDataForm, CDataForm)
	//{{AFX_MSG_MAP(CModulatorDataForm)
	ON_BN_CLICKED(IDC_SET_DATA_RATE_BUTTON, OnSetDataRateButton)
	ON_BN_CLICKED(IDC_DATA_INVERSION_CHECK, OnDataInversionCheck)
	ON_BN_CLICKED(IDC_DATA_CLOCK_INV_CHECK, OnDataClockInvCheck)
	ON_CBN_SELCHANGE(IDC_FEC_MODE_COMBO, OnSelChangeFecModeCombo)
	ON_CBN_SELCHANGE(IDC_DATA_CLOCK_SOURCE_COMBO, OnSelChangeDataClockSourceCombo)
	ON_CBN_SELCHANGE(IDC_SCRAMBLER_MODE_COMBO, OnSelChangeScramblerModeCombo)
	ON_CBN_SELCHANGE(IDC_DIFFERENTIAL_MODE_COMBO, OnSelChangeDifferentialModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModulatorDataForm diagnostics

#ifdef _DEBUG
void CModulatorDataForm::AssertValid() const
{
	CDataForm::AssertValid();
}

void CModulatorDataForm::Dump(CDumpContext& dc) const
{
	CDataForm::Dump(dc);
}
#endif //_DEBUG

BOOL CModulatorDataForm::SetupControls()
{
	CDataForm::SetupControls();

	// Header string
	m_HeaderString.Format("Specify data properties for Modulator%d here", m_DeviceNumber);

	// Fec
	m_FecModeCombo.initT(m_pModem, m_DeviceNumber);

	// Clock source
	m_ClockSourceCombo.initT(m_pModem, m_DeviceNumber);

	// Differential encoding
	m_DifferentialCombo.initT(m_pModem, m_DeviceNumber);

	// Scrambler
	m_ScramblerModeCombo.initT(m_pModem, m_DeviceNumber);

	CModemDataParams Params;
	MC_ErrorCode EC = m_pModem->GetTDataParams(Params, m_DeviceNumber);
	SetControls(Params);
	m_ClockSourceCombo.EnableWindow(m_pModem->CanSetTDataClockSource());

	GetDescendantWindow(IDC_DIFFERENTIAL_CODING_STATIC)->SetWindowText("Differential encoder :");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CModulatorDataForm message handlers

void CModulatorDataForm::OnSetDataRateButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->SetTDataRate(m_DataRate, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorDataForm::OnDataInversionCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->EnableTDataInv(m_bDataInvEnabled, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorDataForm::OnDataClockInvCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->EnableTDataClockInv(m_bDataClockInvEnabled, m_DeviceNumber);
	UpdateData(FALSE);
}

void CModulatorDataForm::OnSelChangeFecModeCombo() 
{
	int FecMode = m_FecModeCombo.getSelectedMode();
	m_pModem->SetTFecMode(FecMode, m_DeviceNumber);
	m_FecModeCombo.SelectByDataValue(FecMode);
}

void CModulatorDataForm::OnSelChangeDataClockSourceCombo() 
{
	int ClockSource = m_ClockSourceCombo.getSelectedMode();
	m_pModem->SetTDataClockSource(ClockSource, m_DeviceNumber);
	m_ClockSourceCombo.SelectByDataValue(ClockSource);
}

void CModulatorDataForm::OnSelChangeScramblerModeCombo() 
{
	int mode = m_ScramblerModeCombo.getSelectedMode();
	m_pModem->SetScramblerMode(mode, m_DeviceNumber);
	m_ScramblerModeCombo.SelectByDataValue(mode);
}

void CModulatorDataForm::OnSelChangeDifferentialModeCombo() 
{
	int mode = m_DifferentialCombo.getSelectedMode();
	m_pModem->SetDiffEncoderMode(mode, m_DeviceNumber);
	m_DifferentialCombo.SelectByDataValue(mode);
}
