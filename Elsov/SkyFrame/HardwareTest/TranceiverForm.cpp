// TranceiverForm.cpp : implementation file
//

#include "stdafx.h"
#include "TranceiverForm.h"
#include "Attached.h"
#include "Tranceiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranceiverForm

IMPLEMENT_DYNCREATE(CTranceiverForm, CAbstractForm)

CTranceiverForm::CTranceiverForm()
	: CAbstractForm(CTranceiverForm::IDD)
{
	//{{AFX_DATA_INIT(CTranceiverForm)
	m_bOutputEnabled = FALSE;
	m_Frequency = 0;
	m_Gain = 0.0;
	//}}AFX_DATA_INIT
}

CTranceiverForm::~CTranceiverForm()
{
}

void CTranceiverForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverForm)
	DDX_Control(pDX, IDC_GAIN_EDIT, m_GainEdit);
	DDX_Control(pDX, IDC_TEMPERATURE_ALARM_STATIC, m_AlarmWnd);
	DDX_Control(pDX, IDC_FREQUENCY_EDIT, m_FrequencyEdit);
	DDX_Control(pDX, IDC_ONOFF_CHECK, m_bOutputControl);
	DDX_Check(pDX, IDC_ONOFF_CHECK, m_bOutputEnabled);
	DDX_Text(pDX, IDC_FREQUENCY_EDIT, m_Frequency);
	DDX_Text(pDX, IDC_GAIN_EDIT, m_Gain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranceiverForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTranceiverForm)
	ON_BN_CLICKED(IDC_ONOFF_CHECK, OnOutputOnOffCheck)
	ON_BN_CLICKED(IDC_SET_BUTTON, OnSetButton)
	ON_BN_CLICKED(IDC_SET_GAIN_BUTTON, OnSetGainButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranceiverForm diagnostics

#ifdef _DEBUG
void CTranceiverForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTranceiverForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CTranceiverForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pTranceiver = (CTranceiver *)pDevice;

	m_pTranceiver->IsOutputOn(m_bOutputEnabled);
	m_bOutputControl.EnableWindow(m_pTranceiver->CanOutputOnOff());

	// Frequency
	m_pTranceiver->GetFrequency(m_Frequency);
	m_FrequencyEdit.EnableWindow(m_pTranceiver->CanSetFrequency());
	GetDescendantWindow(IDC_SET_BUTTON)->EnableWindow(m_pTranceiver->CanSetFrequency());

	// Gain
	m_pTranceiver->GetGain(m_Gain);
	m_GainEdit.EnableWindow(m_pTranceiver->CanSetGain());
	GetDescendantWindow(IDC_SET_GAIN_BUTTON)->EnableWindow(m_pTranceiver->CanSetGain());

	UpdateAllFields();
	UpdateData(FALSE);
	return TRUE;
}

BOOL CTranceiverForm::DoTimer()
{
	UpdateAllFields();
	return TRUE;
}

void CTranceiverForm::UpdateAllFields()
{
	CString str;
	// Temperature
	if (m_pTranceiver->CanGetTemperature())
	{
		double T = -273.;
		m_pTranceiver->GetTemperature(T);
		str.Format("Temperature = %4.1f °C", T);
		GetDescendantWindow(IDC_TEMPERATURE_STATIC)->SetWindowText(str);
	}

	// Temperature alarm
	BOOL bAlarm = FALSE;
	m_pTranceiver->IsTemperatureAlarm(bAlarm);
	if (bAlarm)
		m_AlarmWnd.ShowWindow(SW_SHOW);
	else
		m_AlarmWnd.ShowWindow(SW_HIDE);
	
	// Power
	if (m_pTranceiver->CanGetPower())
	{
		double P = 0.;
		m_pTranceiver->GetPower(P);
		str.Format("Power = %6.2f dBm", P);
		GetDescendantWindow(IDC_POWER_STATIC)->SetWindowText(str);
	}
	else
		GetDescendantWindow(IDC_POWER_STATIC)->ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverForm message handlers

void CTranceiverForm::OnOutputOnOffCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pTranceiver->TurnOutputOn(m_bOutputEnabled);
	UpdateData(FALSE);
}

void CTranceiverForm::OnSetButton() 
{
	UpdateData();
	m_pTranceiver->SetFrequency(m_Frequency);	
	UpdateData(FALSE);
}

void CTranceiverForm::OnSetGainButton() 
{
	UpdateData();
	m_pTranceiver->SetGain(m_Gain);	
	UpdateData(FALSE);
}
