// TmiModemStatusForm.cpp : implementation file
//

#include "stdafx.h"
#include "armhub.h"
#include "TmiModemStatusForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiModemStatusForm

IMPLEMENT_DYNCREATE(CTmiModemStatusForm, CAbstractForm)

CTmiModemStatusForm::CTmiModemStatusForm()
	: CAbstractForm(CTmiModemStatusForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiModemStatusForm)
	m_ModemType = _T("");
	m_SignalNoise = _T("");
	m_AruValue = _T("");
	//}}AFX_DATA_INIT
}

CTmiModemStatusForm::~CTmiModemStatusForm()
{
}

void CTmiModemStatusForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiModemStatusForm)
	DDX_Control(pDX, IDC_BPCH_TRANSMIT_STATIC, m_BPCHTransmitterCtrl);
	DDX_Control(pDX, IDC_BPCH_RECEIVE_STATIC, m_BPCHReceiverCtrl);
	DDX_Text(pDX, IDC_MODEM_TYPE_EDIT, m_ModemType);
	DDX_Text(pDX, IDC_SIGNAL_NOISE_EDIT, m_SignalNoise);
	DDX_Text(pDX, IDC_ARU_VALUE_EDIT, m_AruValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTmiModemStatusForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTmiModemStatusForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
BOOL CTmiModemStatusForm::SetupControls()
{
	UpdateAll();
	return TRUE;
}

void CTmiModemStatusForm::UpdateAll()
{
	m_SignalNoise.Format("%4.1f", m_pStation->GetSignalNoise());
	m_AruValue.Format("%5.1f", m_pStation->GetAruValue());
	m_ModemType = m_pStation->GetModemType();

	if (m_pStation->IsBPCHTransmitterLoaded())
		m_BPCHTransmitterCtrl.ShowWindow(SW_SHOW);
	else
		m_BPCHTransmitterCtrl.ShowWindow(SW_HIDE);

	if (m_pStation->IsBPCHReceiverLoaded())
		m_BPCHReceiverCtrl.ShowWindow(SW_SHOW);
	else
		m_BPCHReceiverCtrl.ShowWindow(SW_HIDE);

	UpdateData(FALSE);
}

//virtual
BOOL CTmiModemStatusForm::DoTimer()
{
	if (m_pStation->IsTmiChangedSince(m_LastTimerTime))
		UpdateAll();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTmiModemStatusForm diagnostics

#ifdef _DEBUG
void CTmiModemStatusForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTmiModemStatusForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTmiModemStatusForm message handlers
