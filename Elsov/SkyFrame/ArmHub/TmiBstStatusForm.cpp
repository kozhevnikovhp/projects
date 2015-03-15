// TmiBpchStatusForm.cpp : implementation file
//

#include "stdafx.h"
#include "armhub.h"
#include "TmiBstStatusForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiBpchStatusForm

IMPLEMENT_DYNCREATE(CTmiBstStatusForm, CAbstractForm)

CTmiBstStatusForm::CTmiBstStatusForm()
	: CAbstractForm(CTmiBstStatusForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiBstStatusForm)
	m_AdcChannel = 0;
	m_AdcValue = 0;
	m_AdcHexValue = _T("");
	//}}AFX_DATA_INIT
}

CTmiBstStatusForm::~CTmiBstStatusForm()
{
}

void CTmiBstStatusForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiBstStatusForm)
	DDX_Control(pDX, IDC_BST_RECEIVE_STATIC, m_BstTransmitterPowerCtrl);
	DDX_Control(pDX, IDC_BST_TRANSMIT_STATIC, m_BstReceiverPowerCtrl);
	DDX_Text(pDX, IDC_ADC_CHANNEL_EDIT, m_AdcChannel);
	DDX_Text(pDX, IDC_ADC_VALUE_MW_EDIT, m_AdcValue);
	DDX_Text(pDX, IDC_ADC_VALUE_HEX_EDIT, m_AdcHexValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTmiBstStatusForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTmiBstStatusForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
BOOL CTmiBstStatusForm::SetupControls()
{
	UpdateAll();
	return TRUE;
}

void CTmiBstStatusForm::UpdateAll()
{
	m_AdcChannel = m_pStation->BstGetAdcChannel();
	m_AdcValue = m_pStation->BstGetAdcValue();
	m_AdcHexValue.Format("0x%X", m_pStation->BstGetAdcHexValue());

	if (m_pStation->BstIsTransmitterPowerOn())
		m_BstTransmitterPowerCtrl.ShowWindow(SW_SHOW);
	else
		m_BstTransmitterPowerCtrl.ShowWindow(SW_HIDE);

	if (m_pStation->BstIsReceiverPowerOn())
		m_BstReceiverPowerCtrl.ShowWindow(SW_SHOW);
	else
		m_BstReceiverPowerCtrl.ShowWindow(SW_HIDE);

	UpdateData(FALSE);
}

//virtual
BOOL CTmiBstStatusForm::DoTimer()
{
	if (m_pStation->IsTmiChangedSince(m_LastTimerTime))
		UpdateAll();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTmiBstStatusForm diagnostics

#ifdef _DEBUG
void CTmiBstStatusForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTmiBstStatusForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTmiBpchStatusForm message handlers
