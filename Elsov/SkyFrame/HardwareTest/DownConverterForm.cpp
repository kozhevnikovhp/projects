// DownConverterForm.cpp : implementation file
//

#include "stdafx.h"
#include "DownConverterForm.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierForm

IMPLEMENT_DYNCREATE(CDownConverterForm, CAbstractForm)

CDownConverterForm::CDownConverterForm()
	: CAbstractForm(CDownConverterForm::IDD)
{
	//{{AFX_DATA_INIT(CDownConverterForm)
	m_bOn = FALSE;
	m_bSpectralInvEnabled = FALSE;
	m_Frequency = 0;
	m_InputAttenuator = 0.0;
	m_OutputAttenuator = 0.0;
	m_GainLoss = 0.0;
	//}}AFX_DATA_INIT
	m_pConverter = NULL;
}

CDownConverterForm::~CDownConverterForm()
{
}

void CDownConverterForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownConverterForm)
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

BEGIN_MESSAGE_MAP(CDownConverterForm, CAbstractForm)
	//{{AFX_MSG_MAP(CDownConverterForm)
	ON_BN_CLICKED(IDC_ONOFF_CHECK, OnOnOffCheck)
	ON_BN_CLICKED(IDC_SPECTRAL_INV_CHECK, OnSpectralInvCheck)
	ON_BN_CLICKED(IDC_SET_FREQUENCY_BUTTON, OnSetFrequencyButton)
	ON_BN_CLICKED(IDC_SET_GAIN_LOSS_BUTTON, OnSetGainLossButton)
	ON_BN_CLICKED(IDC_OUTPUT_ATT_SET_BUTTON, OnSetOutputAttButton)
	ON_BN_CLICKED(IDC_INPUT_ATT_SET_BUTTON, OnSetInputAttButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDownConverterForm diagnostics

#ifdef _DEBUG
void CDownConverterForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CDownConverterForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

//virtual
BOOL CDownConverterForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pConverter=(CUpDownConverter *)pDevice;

	// frequency
	m_FrequencySpin.SetRange32(m_pConverter->GetMinDownInputFrequency(), m_pConverter->GetMaxDownInputFrequency());
	m_pConverter->GetDownInputFrequency(m_Frequency);

	// output
	m_OnOffCheck.EnableWindow(m_pConverter->CanDownOnOff());
	m_pConverter->IsDownTurnedOn(m_bOn);
	
	// Spectral Inversion
	m_SpectralInvCheck.EnableWindow(m_pConverter->CanDownSpectralInv());
	m_pConverter->IsDownSpectralInvEnabled(m_bSpectralInvEnabled);

	// Input attenuator
	m_pConverter->GetDownInputAtt(m_InputAttenuator);
	m_InputAttenuatorEdit.EnableWindow(m_pConverter->CanSetDownInputAtt());
	m_SetInputAttenuatorButton.EnableWindow(m_pConverter->CanSetDownInputAtt());

	// Output attenuator
	m_pConverter->GetDownOutputAtt(m_OutputAttenuator);
	m_OutputAttenuatorEdit.EnableWindow(m_pConverter->CanSetDownOutputAtt());
	m_SetOutputAttenuatorButton.EnableWindow(m_pConverter->CanSetDownOutputAtt());

	// Gain/loss
	m_pConverter->GetDownGainLoss(m_GainLoss);
	m_GainLossEdit.EnableWindow(m_pConverter->CanSetDownGainLoss());
	m_SetGainLossButton.EnableWindow(m_pConverter->CanSetDownGainLoss());

	UpdateData(FALSE);

	return TRUE;
}

//virtual
BOOL CDownConverterForm::DoTimer()
{
	CString str;
	if (m_pConverter->CanGetDownTemperature())
	{
		double T = -273.15;
		m_pConverter->GetDownTemperature(T);
		str.Format("Temperature = %4.1f °C", T);
		GetDescendantWindow(IDC_TEMPERATURE_STATIC)->SetWindowText(str);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDownConverterForm message handlers

void CDownConverterForm::OnOnOffCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->TurnDownOn(m_bOn);
	UpdateData(FALSE);
}

void CDownConverterForm::OnSpectralInvCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->EnableDownSpectralInv(m_bSpectralInvEnabled);
	UpdateData(FALSE);
}


void CDownConverterForm::OnSetFrequencyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetDownInputFrequency(m_Frequency);
	UpdateData(FALSE);
}

void CDownConverterForm::OnSetGainLossButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetDownGainLoss(m_GainLoss);
	UpdateData(FALSE);
}

void CDownConverterForm::OnSetOutputAttButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetDownOutputAtt(m_OutputAttenuator);
	UpdateData(FALSE);
}

void CDownConverterForm::OnSetInputAttButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pConverter->SetDownInputAtt(m_InputAttenuator);
	UpdateData(FALSE);
}
