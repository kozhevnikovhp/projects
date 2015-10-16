// DemodulatorDataForm.cpp : implementation file
//

#include "stdafx.h"
#include "DemodulatorDataForm.h"
#include "Attached.h"
#include "Modem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorDataForm

IMPLEMENT_DYNCREATE(CDemodulatorDataForm, CDataForm)

CDemodulatorDataForm::CDemodulatorDataForm()
	: CDataForm(CDataForm::IDD)
{
	//{{AFX_DATA_INIT(CDemodulatorDataForm)
	m_DataRate = 0;
	//}}AFX_DATA_INIT
}

CDemodulatorDataForm::~CDemodulatorDataForm()
{
}

void CDemodulatorDataForm::DoDataExchange(CDataExchange* pDX)
{
	CDataForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodulatorDataForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDemodulatorDataForm, CDataForm)
	//{{AFX_MSG_MAP(CDemodulatorDataForm)
	ON_BN_CLICKED(IDC_SET_DATA_RATE_BUTTON, OnSetDataRateButton)
	ON_BN_CLICKED(IDC_DATA_INVERSION_CHECK, OnDataInversionCheck)
	ON_BN_CLICKED(IDC_DATA_CLOCK_INV_CHECK, OnDataClockInvCheck)
	ON_CBN_SELCHANGE(IDC_FEC_OPTION_COMBO, OnSelChangeFecOptionCombo)
	ON_CBN_SELCHANGE(IDC_FEC_CODERATE_COMBO, OnSelChangeFecCodeRateCombo)
	ON_CBN_SELCHANGE(IDC_FEC_MODE_COMBO, OnSelChangeFecModeCombo)
	ON_CBN_SELCHANGE(IDC_SCRAMBLER_MODE_COMBO, OnSelChangeScramblerModeCombo)
	ON_CBN_SELCHANGE(IDC_DATA_CLOCK_SOURCE_COMBO, OnSelChangeDataClockSourceCombo)
	ON_CBN_SELCHANGE(IDC_DIFFERENTIAL_MODE_COMBO, OnSelChangeDifferentialModeCombo)
	ON_BN_CLICKED(IDC_SET_DOPPLER_BUFFER_SIZE_BUTTON, OnSetDopplerBufferSizeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorDataForm diagnostics

#ifdef _DEBUG
void CDemodulatorDataForm::AssertValid() const
{
	CDataForm::AssertValid();
}

void CDemodulatorDataForm::Dump(CDumpContext& dc) const
{
	CDataForm::Dump(dc);
}
#endif //_DEBUG

BOOL CDemodulatorDataForm::SetupControls()
{
	CDataForm::SetupControls();

	// Header string
	m_HeaderString.Format("Specify data properties for Demodulator%d here", m_DeviceNumber);

	// FEC mode
	m_FecModeCombo.initR(m_pModem, m_DeviceNumber);
	m_FecOptionCombo.initR(m_pModem, m_DeviceNumber);
	m_FecCodeRateCombo.initR(m_pModem, m_DeviceNumber);

	// Clock source
	m_ClockSourceCombo.initR(m_pModem, m_DeviceNumber);

	// Differential decoding
	m_DifferentialCombo.initR(m_pModem, m_DeviceNumber);

	// Descrambler
	m_ScramblerModeCombo.initR(m_pModem, m_DeviceNumber);

	CModemDataParams Params;
	MC_ErrorCode EC = m_pModem->GetRDataParams(Params, m_DeviceNumber);
	SetControls(Params);
	m_ClockSourceCombo.EnableWindow(m_pModem->CanSetRDataClockSource());

	GetDescendantWindow(IDC_DIFFERENTIAL_CODING_STATIC)->SetWindowText("Differential decoder On/Off");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorDataForm message handlers

void CDemodulatorDataForm::OnSetDataRateButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->SetRDataRate(m_DataRate, m_DeviceNumber);
	UpdateData(FALSE);
}

void CDemodulatorDataForm::OnDataInversionCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->EnableRDataInv(m_bDataInvEnabled, m_DeviceNumber);
	UpdateData(FALSE);
}

void CDemodulatorDataForm::OnDataClockInvCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->EnableRDataClockInv(m_bDataClockInvEnabled, m_DeviceNumber);
	UpdateData(FALSE);
}

void CDemodulatorDataForm::OnSelChangeFecModeCombo() 
{
	int mode = m_FecModeCombo.getSelectedMode();
	m_pModem->SetRFecMode(mode, m_DeviceNumber);
	m_FecModeCombo.SelectByDataValue(mode);
}

void CDemodulatorDataForm::OnSelChangeFecOptionCombo() 
{
	int option = m_FecOptionCombo.getSelectedMode();
	m_pModem->SetRFecOption(option, m_DeviceNumber);
	m_FecOptionCombo.SelectByDataValue(option);
}

void CDemodulatorDataForm::OnSelChangeFecCodeRateCombo() 
{
	int mode = m_FecCodeRateCombo.getSelectedMode();
	m_pModem->SetRFecCodeRate(mode, m_DeviceNumber);
	m_FecCodeRateCombo.SelectByDataValue(mode);
}

void CDemodulatorDataForm::OnSelChangeScramblerModeCombo() 
{
	int mode = m_ScramblerModeCombo.getSelectedMode();
	m_pModem->SetDescramblerMode(mode, m_DeviceNumber);
	m_ScramblerModeCombo.SelectByDataValue(mode);
}

void CDemodulatorDataForm::OnSelChangeDataClockSourceCombo() 
{
	int ClockSource = m_ClockSourceCombo.getSelectedMode();
	m_pModem->SetRDataClockSource(ClockSource, m_DeviceNumber);
	m_ClockSourceCombo.SelectByDataValue(ClockSource);
}

void CDemodulatorDataForm::OnSelChangeDifferentialModeCombo() 
{
	int mode = m_DifferentialCombo.getSelectedMode();
	m_pModem->SetDiffDecoderMode(mode, m_DeviceNumber);
	m_DifferentialCombo.SelectByDataValue(mode);
}

void CDemodulatorDataForm::OnSetDopplerBufferSizeButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pModem->SetDemodulatorDopplerBufferSize(m_DopplerBufferSize, m_DeviceNumber);
	UpdateData(FALSE);
}
