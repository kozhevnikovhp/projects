// DemodulatorBerTestForm.cpp : implementation file
//

#include "stdafx.h"
#include "DemodulatorBerTestForm.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorBerTestForm

IMPLEMENT_DYNCREATE(CDemodulatorBerTestForm, CAbstractForm)

CDemodulatorBerTestForm::CDemodulatorBerTestForm()
	: CAbstractForm(CDemodulatorBerTestForm::IDD)
{
	//{{AFX_DATA_INIT(CDemodulatorBerTestForm)
	m_HeaderString = _T("");
	m_SyncLosses = 0;
	m_TotalBits = 0.0;
	m_Duration = 0;
	m_Errors = 0.0;
	m_ErroredSeconds = 0;
	m_BER = 0.0;
	m_ErrorFreePercentage = 0.0;
	//}}AFX_DATA_INIT
	m_pModem = NULL;
}

CDemodulatorBerTestForm::~CDemodulatorBerTestForm()
{
}

void CDemodulatorBerTestForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodulatorBerTestForm)
	DDX_Control(pDX, IDC_INSERT_1ERROR_BUTTON, m_Insert1ErrorButton);
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	DDX_Text(pDX, IDC_SYNC_LOSSES_EDIT, m_SyncLosses);
	DDX_Text(pDX, IDC_TOTAL_BITS_EDIT, m_TotalBits);
	DDX_Text(pDX, IDC_DURATION_EDIT, m_Duration);
	DDX_Text(pDX, IDC_ERRORED_BITS_EDIT, m_Errors);
	DDX_Text(pDX, IDC_ERRORED_SECONDS_EDIT, m_ErroredSeconds);
	DDX_Text(pDX, IDC_BER_EDIT, m_BER);
	DDX_Text(pDX, IDC_ERROR_FREE_PERCENTAGE_EDIT, m_ErrorFreePercentage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemodulatorBerTestForm, CAbstractForm)
	//{{AFX_MSG_MAP(CDemodulatorBerTestForm)
	ON_BN_CLICKED(IDC_DISABLED_RADIO, OnDisabledRadio)
	ON_BN_CLICKED(IDC_11_RADIO, On11Radio)
	ON_BN_CLICKED(IDC_23_RADIO, On23Radio)
	ON_BN_CLICKED(IDC_RESTART_BUTTON, OnRestartButton)
	ON_BN_CLICKED(IDC_INSERT_1ERROR_BUTTON, OnInsert1errorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDemodulatorBerTestForm diagnostics

#ifdef _DEBUG
void CDemodulatorBerTestForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CDemodulatorBerTestForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CDemodulatorBerTestForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pModem=(CModem *)pDevice;

	// Header string
	m_HeaderString.Format("Specify BER-Test properties for Demodulator%d here", m_DeviceNumber);

	ChoosePower();
	
	GetDescendantWindow(IDC_11_RADIO)->EnableWindow(m_pModem->CanBerTest(11));
	GetDescendantWindow(IDC_23_RADIO)->EnableWindow(m_pModem->CanBerTest(23));
	m_Insert1ErrorButton.EnableWindow(m_pModem->CanInsert1ErrorForBerTest());

	UpdateStatistics();
	UpdateData(FALSE);
	return TRUE;
}

BOOL CDemodulatorBerTestForm::DoTimer()
{
	UpdateStatistics();
	return TRUE;
}

void CDemodulatorBerTestForm::UpdateStatistics()
{
	m_pModem->GetBerTestStatus(m_BER, m_SyncLosses, m_Errors, m_TotalBits, m_ErrorFreePercentage, m_ErroredSeconds, m_Duration);
	m_TotalBits /= 1.e+06; // megabits are displayed
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorBerTestForm message handlers


void CDemodulatorBerTestForm::OnDisabledRadio() 
{
	CWaitCursor cursor;
	int Power = 0;
	m_pModem->SetRBerTest(Power, m_DeviceNumber);
	ChoosePower();
}

void CDemodulatorBerTestForm::On11Radio() 
{
	CWaitCursor cursor;
	int Power = 11;
	m_pModem->SetRBerTest(Power, m_DeviceNumber);
	ChoosePower();
}

void CDemodulatorBerTestForm::On23Radio() 
{
	CWaitCursor cursor;
	int Power = 23;
	m_pModem->SetRBerTest(Power, m_DeviceNumber);
	ChoosePower();
}

void CDemodulatorBerTestForm::ChoosePower()
{
	int Power = 0;
	m_pModem->GetRBerTest(Power, m_DeviceNumber);
	UINT ID = IDC_DISABLED_RADIO;
	switch (Power)
	{
		case 0: ID = IDC_DISABLED_RADIO; break;
		case 11: ID = IDC_11_RADIO; break;
		case 23: ID = IDC_23_RADIO; break;
		default: ID = IDC_DISABLED_RADIO; break;
	}
	CheckRadioButton(IDC_DISABLED_RADIO, IDC_23_RADIO, ID);
}


void CDemodulatorBerTestForm::OnRestartButton() 
{
	m_pModem->RestartBerTest(m_DeviceNumber);
	UpdateStatistics();
}

void CDemodulatorBerTestForm::OnInsert1errorButton() 
{
	m_pModem->Insert1ErrorForBerTest();
}
