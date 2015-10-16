// DataForm.cpp : implementation file
//

#include "stdafx.h"
#include "DataForm.h"
#include "RadioDevice.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataForm

IMPLEMENT_DYNCREATE(CDataForm, CAbstractForm)

CDataForm::CDataForm()
	: CAbstractForm(CDataForm::IDD)
{
	//{{AFX_DATA_INIT(CDataForm)
	m_DataRate = 0;
	m_bDataInvEnabled = FALSE;
	m_bDataClockInvEnabled = FALSE;
	m_HeaderString = _T("");
	m_DopplerBufferSize = 0;
	//}}AFX_DATA_INIT
}

CDataForm::CDataForm(UINT nIDTemplate)
	: CAbstractForm(nIDTemplate)
{
}

CDataForm::~CDataForm()
{
}

void CDataForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataForm)
	DDX_Control(pDX, IDC_FEC_CODERATE_COMBO, m_FecCodeRateCombo);
	DDX_Control(pDX, IDC_FEC_OPTION_COMBO, m_FecOptionCombo);
	DDX_Control(pDX, IDC_DIFFERENTIAL_MODE_COMBO, m_DifferentialCombo);
	DDX_Control(pDX, IDC_FEC_MODE_COMBO, m_FecModeCombo);
	DDX_Control(pDX, IDC_DATA_CLOCK_SOURCE_COMBO, m_ClockSourceCombo);
	DDX_Control(pDX, IDC_SCRAMBLER_MODE_COMBO, m_ScramblerModeCombo);
	DDX_Control(pDX, IDC_DATA_INVERSION_CHECK, m_DataInvCheck);
	DDX_Control(pDX, IDC_DATA_CLOCK_INV_CHECK, m_DataClockInvCheck);
	DDX_Control(pDX, IDC_DATA_RATE_SPIN, m_DataRateSpin);
	DDX_Text(pDX, IDC_DATA_RATE_EDIT, m_DataRate);
	DDX_Check(pDX, IDC_DATA_INVERSION_CHECK, m_bDataInvEnabled);
	DDX_Check(pDX, IDC_DATA_CLOCK_INV_CHECK, m_bDataClockInvEnabled);
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	DDX_Text(pDX, IDC_DOPPLER_BUFFER_SIZE_EDIT, m_DopplerBufferSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataForm, CAbstractForm)
	//{{AFX_MSG_MAP(CDataForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataForm diagnostics

#ifdef _DEBUG
void CDataForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CDataForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CDataForm::SetupControls()
{
	m_DataRateSpin.SetRange32(0, 8192000);

	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pModem=(CModem *)pDevice;

	return TRUE;
}

void CDataForm::SetControls(CModemDataParams &Params)
{
	// Data rate
	BOOL bCanDataRate = CanDataRate();
	GetDescendantWindow(IDC_DATA_RATE_EDIT)->EnableWindow(bCanDataRate);
	GetDescendantWindow(IDC_DATA_RATE_SPIN)->EnableWindow(bCanDataRate);
	GetDescendantWindow(IDC_SET_DATA_RATE_BUTTON)->EnableWindow(bCanDataRate);
	m_DataRate = Params.m_DataRate;

	// DataClock source
	m_DataClockInvCheck.EnableWindow(CanDataClockInversion());
	m_bDataClockInvEnabled = Params.m_bClockInverted;
	
	// Differential decoding
	m_DifferentialCombo.EnableWindow(CanDiff());
	m_DifferentialCombo.SelectByDataValue(Params.m_DifferentialCodingMode);

	// Descrambling
	m_ScramblerModeCombo.EnableWindow(CanScrambleDescramble());
	m_ScramblerModeCombo.SelectByDataValue(Params.m_ScramblingMode);

	// Data Inversion
	m_DataInvCheck.EnableWindow(CanDataInversion());
	m_bDataInvEnabled = Params.m_bDataInverted;

	m_DopplerBufferSize = Params.m_DopplerBufferSize;

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDataForm message handlers

