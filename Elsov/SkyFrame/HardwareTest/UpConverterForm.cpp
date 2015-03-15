// UpConverterForm.cpp : implementation file
//

#include "stdafx.h"
#include "UpConverterForm.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpConverterForm

IMPLEMENT_DYNCREATE(CUpConverterForm, CAbstractForm)

CUpConverterForm::CUpConverterForm()
	: CAbstractForm(CUpConverterForm::IDD)
{
	//{{AFX_DATA_INIT(CUpConverterForm)
	m_bOn = FALSE;
	m_bSpectralInvEnabled = FALSE;
	m_Frequency = 0;
	m_InputAttenuator = 0.0;
	m_OutputAttenuator = 0.0;
	m_GainLoss = 0.0;
	//}}AFX_DATA_INIT
	m_pConverter = NULL;
}

CUpConverterForm::~CUpConverterForm()
{
}

void CUpConverterForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpConverterForm)
	DDX_Control(pDX, IDC_SET_GAIN_LOSS_BUTTON, m_SetGainLossButton);
	DDX_Control(pDX, IDC_GAIN_LOSS_EDIT, m_GainLossEdit);
	DDX_Control(pDX, IDC_OUTPUT_ATTENUATOR_EDIT, m_OutputAttenuatorEdit);
	DDX_Control(pDX, IDC_OUTPUT_ATT_SET_BUTTON, m_SetOutputAttenuatorButton);
	DDX_Control(pDX, IDC_INPUT_ATT_SET_BUTTON, m_SetInputAttenuatorButton);
	DDX_Control(pDX, IDC_INPUT_ATTENUATOR_EDIT, m_InputAttenuatorEdit);
	DDX_Control(pDX, IDC_SET_FREQUENCY_BUTTON, m_SetFrequencyButton);
	DDX_Control(pDX, IDC_ONOFF_CHECK, m_OnOffCheck);
	DDX_Control(pDX, IDC_SPECTRAL_INV_CHECK, m_SpectralInvCheck);
	DDX_Control(pDX, IDC_FREQUENCY_SPIN, m_FrequencySpin);
	DDX_Check(pDX, IDC_ONOFF_CHECK, m_bOn);
	DDX_Check(pDX, IDC_SPECTRAL_INV_CHECK, m_bSpectralInvEnabled);
	DDX_Text(pDX, IDC_FREQUENCY_EDIT, m_Frequency);
	DDX_Text(pDX, IDC_INPUT_ATTENUATOR_EDIT, m_InputAttenuator);
	DDX_Text(pDX, IDC_OUTPUT_ATTENUATOR_EDIT, m_OutputAttenuator);
	DDX_Text(pDX, IDC_GAIN_LOSS_EDIT, m_GainLoss);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUpConverterForm, CAbstractForm)
	//{{AFX_MSG_MAP(CUpConverterForm)
	ON_BN_CLICKED(IDC_ONOFF_CHECK, OnOutputOnOffCheck)
	ON_BN_CLICKED(IDC_SPECTRAL_INV_CHECK, OnSpectralInvCheck)
	ON_BN_CLICKED(IDC_SET_FREQUENCY_BUTTON, OnSetFrequencyButton)
	ON_BN_CLICKED(IDC_SET_GAIN_LOSS_BUTTON, OnSetGainLossButton)
	ON_BN_CLICKED(IDC_OUTPUT_ATT_SET_BUTTON, OnSetOutputAttButton)
	ON_BN_CLICKED(IDC_INPUT_ATT_SET_BUTTON, OnSetInputAttButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpConverterForm diagnostics

#ifdef _DEBUG
void CUpConverterForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CUpConverterForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

//virtual
BOOL CUpConverterForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pConverter=(CUpDownConverter *)pDevice;

	// frequency
	m_FrequencySpin.SetRange32(m_pConverter->GetMinUpOutputFrequency(), m_pConverter->GetMaxUpOutputFrequency());
	m_pConverter->GetUpOutputFrequency(m_Frequency);

	// output
	m_OnOffCheck.EnableWindow(m_pConverter->CanUpOnOff());
	m_pConverter->IsUpTurnedOn(m_bOn);
	
	// Spectral Inversion
	m_SpectralInvCheck.EnableWindow(m_pConverter->CanUpSpectralInv());
	m_pConverter->IsUpSpectralInvEnabled(m_bSpectralInvEnabled);

	// Input attenuator
	m_pConverter->GetUpInputAtt(m_InputAttenuator);
	m_InputAttenuatorEdit.EnableWindow(m_pConverter->CanSetUpInputAtt());
	m_SetInputAttenuatorButton.EnableWindow(m_pConverter->CanSetUpInputAtt());

	// Output attenuator
	m_pConverter->GetUpOutputAtt(m_OutputAttenuator);
	m_OutputAttenuatorEdit.EnableWindow(m_pConverter->CanSetUpOutputAtt());
	m_SetOutputAttenuatorButton.EnableWindow(m_pConverter->CanSetUpOutputAtt());

	// Gain/loss
	m_pConverter->GetUpGainLoss(m_GainLoss);
	m_GainLossEdit.EnableWindow(m_pConverter->CanSetUpGainLoss());
	m_SetGainLossButton.EnableWindow(m_pConverter->CanSetUpGainLoss());

	UpdateData(FALSE);

	return TRUE;
}

//virtual
BOOL CUpConverterForm::DoTimer()
{
	CString str;
	if (m_pConverter->CanGetUpTemperature())
	{
		double T = -273.15;
		m_pConverter->GetUpTemperature(T);
		str.Format("Temperature = %4.1f °C", T);
		GetDescendantWindow(IDC_TEMPERATURE_STATIC)->SetWindowText(str);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUpConverterForm message handlers

void CUpConverterForm::OnOutputOnOffCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->TurnUpOn(m_bOn);
	UpdateData(FALSE);
}

void CUpConverterForm::OnSpectralInvCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->EnableUpSpectralInv(m_bSpectralInvEnabled);
	UpdateData(FALSE);
}


void CUpConverterForm::OnSetFrequencyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetUpOutputFrequency(m_Frequency);
	UpdateData(FALSE);
}

void CUpConverterForm::OnSetGainLossButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetUpGainLoss(m_GainLoss);
	UpdateData(FALSE);
}

void CUpConverterForm::OnSetOutputAttButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetUpOutputAtt(m_OutputAttenuator);
	UpdateData(FALSE);
}

void CUpConverterForm::OnSetInputAttButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetUpInputAtt(m_InputAttenuator);
	UpdateData(FALSE);
}
