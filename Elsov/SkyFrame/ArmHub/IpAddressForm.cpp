// IpAddressForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "IpAddressForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"
/////////////////////////////////////////////////////////////////////////////
// CIpAddressForm

IMPLEMENT_DYNCREATE(CIpAddressForm, CAbstractForm)

CIpAddressForm::CIpAddressForm()
	: CAbstractForm(CIpAddressForm::IDD)
{
	//{{AFX_DATA_INIT(CIpAddressForm)
	m_IpAddress = _T("");
	m_IpAddressRCh = _T("");
	m_PingPeriod = 0;
	m_bPingEnabled = FALSE;
	//}}AFX_DATA_INIT
}

CIpAddressForm::~CIpAddressForm()
{
}

void CIpAddressForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIpAddressForm)
	DDX_Control(pDX, IDC_PING_PERIOD_SPIN, m_PingPeriodSpin);
	DDX_Text(pDX, IDC_IP_ADDRESS_EDIT, m_IpAddress);
	DDX_Text(pDX, IDC_IP_ADDRESS_RCH_EDIT, m_IpAddressRCh);
	DDX_Text(pDX, IDC_PING_PERIOD_EDIT, m_PingPeriod);
	DDV_MinMaxUInt(pDX, m_PingPeriod, 5, 3600);
	DDX_Check(pDX, IDC_PING_ENABLED_CHECK, m_bPingEnabled);
	//}}AFX_DATA_MAP
}

//virtual
BOOL CIpAddressForm::DoTimer()
{
	UpdateTexts();
	return TRUE;
}

void CIpAddressForm::UpdateTexts()
{
	static CString NotConnectedStr = "Not connected!";
	CString DisplayString;

	int delay = m_pStation->GetPingDelaySP();
	if (delay >= 0)
		DisplayString.Format("Delay = %d msec", delay);
	else
		DisplayString = NotConnectedStr;
	GetDescendantWindow(IDC_DELAY_SP_STATIC)->SetWindowText(DisplayString);

	delay = m_pStation->GetPingDelayRCh();
	if (delay >= 0)
		DisplayString.Format("Delay = %d msec", delay);
	else
		DisplayString = NotConnectedStr;
	GetDescendantWindow(IDC_DELAY_RCH_STATIC)->SetWindowText(DisplayString);
}

//virtual
BOOL CIpAddressForm::SetupControls()
{
	unsigned long IpAddressOfSP = m_pStation->GetIpAddressOfSP();
	m_IpAddress.Format("%d.%d.%d.%d",
		LOBYTE(LOWORD(IpAddressOfSP)),
		HIBYTE(LOWORD(IpAddressOfSP)),
		LOBYTE(HIWORD(IpAddressOfSP)),
		HIBYTE(HIWORD(IpAddressOfSP)));
	unsigned long IpAddressOfRCh = m_pStation->GetIpAddressOfRCh();
	m_IpAddressRCh.Format("%d.%d.%d.%d",
		LOBYTE(LOWORD(IpAddressOfRCh)),
		HIBYTE(LOWORD(IpAddressOfRCh)),
		LOBYTE(HIWORD(IpAddressOfRCh)),
		HIBYTE(HIWORD(IpAddressOfRCh)));
	m_bPingEnabled = m_pStation->IsPingEnabled();
	m_PingPeriod = m_pStation->GetPingPeriod().GetTotalSeconds();
	m_PingPeriodSpin.SetRange(5, 3600);
	UpdateData(FALSE);
	UpdateTexts();
	return TRUE;
}

//virtual
BOOL CIpAddressForm::IsAlarm(CStation *pStation)
{
	return !pStation->IsSpConnected() || !pStation->IsRChConnected();
}

BEGIN_MESSAGE_MAP(CIpAddressForm, CAbstractForm)
	//{{AFX_MSG_MAP(CIpAddressForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_PING_ENABLED_CHECK, OnPingEnabledCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpAddressForm diagnostics

#ifdef _DEBUG
void CIpAddressForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CIpAddressForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIpAddressForm message handlers

void CIpAddressForm::OnApplyButton() 
{
	UpdateData();
	unsigned char b1=0, b2=0, b3=0, b4=0;

	sscanf(LPCTSTR(m_IpAddress), "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
	m_pStation->SetIpAddressOfSP(MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4)));
	m_IpAddress.Format("%d.%d.%d.%d", b1, b2, b3, b4);

	sscanf(LPCTSTR(m_IpAddressRCh), "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
	m_pStation->SetIpAddressOfRCh(MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4)));
	m_IpAddressRCh.Format("%d.%d.%d.%d", b1, b2, b3, b4);

	m_pStation->SetPingPeriod(m_PingPeriod);
	m_pStation->EnablePing(m_bPingEnabled);

	UpdateData(FALSE);
}

void CIpAddressForm::OnPingEnabledCheck() 
{
	UpdateData();
	m_pStation->EnablePing(m_bPingEnabled);
}
