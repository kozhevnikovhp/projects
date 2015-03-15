// TransmitterBerTestForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModulatorBerTestForm.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModulatorBerTestForm

IMPLEMENT_DYNCREATE(CModulatorBerTestForm, CAbstractForm)

CModulatorBerTestForm::CModulatorBerTestForm()
	: CAbstractForm(CModulatorBerTestForm::IDD)
{
	//{{AFX_DATA_INIT(CModulatorBerTestForm)
	m_HeaderString = _T("");
	//}}AFX_DATA_INIT
	m_pModem = NULL;
}

CModulatorBerTestForm::~CModulatorBerTestForm()
{
}

void CModulatorBerTestForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransmitterBerTestForm)
	DDX_Control(pDX, IDC_INSERT_1ERROR_BUTTON, m_Insert1ErrorButton);
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModulatorBerTestForm, CAbstractForm)
	//{{AFX_MSG_MAP(CModulatorBerTestForm)
	ON_BN_CLICKED(IDC_DISABLED_RADIO, OnDisabledRadio)
	ON_BN_CLICKED(IDC_11_RADIO, On11Radio)
	ON_BN_CLICKED(IDC_23_RADIO, On23Radio)
	ON_BN_CLICKED(IDC_INSERT_1ERROR_BUTTON, OnInsert1ErrorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTransmitterBerTestForm diagnostics

#ifdef _DEBUG
void CModulatorBerTestForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CModulatorBerTestForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CModulatorBerTestForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pModem=(CModem *)pDevice;

	// Header string
	m_HeaderString.Format("Specify BER-Test properties for Modulator%d here", m_DeviceNumber);

	ChoosePower();

	GetDescendantWindow(IDC_11_RADIO)->EnableWindow(m_pModem->CanBerTest(11));
	GetDescendantWindow(IDC_23_RADIO)->EnableWindow(m_pModem->CanBerTest(23));
	m_Insert1ErrorButton.EnableWindow(m_pModem->CanInsert1ErrorForBerTest());
	
	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTransmitterBerTestForm message handlers


void CModulatorBerTestForm::OnDisabledRadio() 
{
	CWaitCursor cursor;
	int Power = 0;
	m_pModem->SetTBerTest(Power, m_DeviceNumber);
	ChoosePower();
}

void CModulatorBerTestForm::On11Radio() 
{
	CWaitCursor cursor;
	int Power = 11;
	m_pModem->SetTBerTest(Power, m_DeviceNumber);
	ChoosePower();
}

void CModulatorBerTestForm::On23Radio() 
{
	CWaitCursor cursor;
	int Power = 23;
	m_pModem->SetTBerTest(Power, m_DeviceNumber);
	ChoosePower();
}

void CModulatorBerTestForm::ChoosePower()
{
	int Power = 0;
	m_pModem->GetTBerTest(Power, m_DeviceNumber);
	UINT ID = IDC_DISABLED_RADIO;
	switch (Power)
	{
		case 0: ID = IDC_DISABLED_RADIO; break;
		case 11: ID=IDC_11_RADIO; break;
		case 23: ID=IDC_23_RADIO; break;
		default: ID = IDC_DISABLED_RADIO; break;
	}
	CheckRadioButton(IDC_DISABLED_RADIO, IDC_23_RADIO, ID);
}


void CModulatorBerTestForm::OnInsert1ErrorButton() 
{
	m_pModem->Insert1ErrorForBerTest();
}
