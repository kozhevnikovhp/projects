// EthernetPortForm.cpp : implementation file
//

#include "stdafx.h"
#include "EthernetPortForm.h"
#include "Attached.h"
#include "MiscEthernet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEthernetPortForm

IMPLEMENT_DYNCREATE(CEthernetPortForm, CAbstractForm)

CEthernetPortForm::CEthernetPortForm()
	: CAbstractForm(CEthernetPortForm::IDD)
{
	//{{AFX_DATA_INIT(CEthernetPortForm)
	m_DesiredIpAddress = _T("");
	m_DesiredIpSubnetMask = _T("");
	m_Protocol = _T("");
	m_DesiredMacAddress = _T("");
	//}}AFX_DATA_INIT
	m_pSDMS = NULL;
}

CEthernetPortForm::~CEthernetPortForm()
{
}

void CEthernetPortForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEthernetPortForm)
	DDX_Control(pDX, IDC_LOCAL_HOST_COMBO, m_LocalHostCombo);
	DDX_Text(pDX, IDC_DESIRED_IP_ADDRESS_EDIT, m_DesiredIpAddress);
	DDX_Text(pDX, IDC_DESIRED_SUBNET_MASK, m_DesiredIpSubnetMask);
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_DESIRED_MAC_ADDRESS_EDIT, m_DesiredMacAddress);
	DDV_MaxChars(pDX, m_DesiredMacAddress, 17);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEthernetPortForm, CAbstractForm)
	//{{AFX_MSG_MAP(CEthernetPortForm)
	ON_BN_CLICKED(IDC_COPY_BUTTON, OnCopyButton)
	ON_BN_CLICKED(IDC_SET_IP_ADDRESS_BUTTON, OnSetIpAddressButton)
	ON_BN_CLICKED(IDC_RUN_TEST_BUTTON, OnRunTestButton)
	ON_BN_CLICKED(IDC_SET_MAC_ADDRESS_BUTTON, OnSetMacAddressButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEthernetPortForm diagnostics

#ifdef _DEBUG
void CEthernetPortForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CEthernetPortForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CEthernetPortForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pSDMS = (CSdmsAPI *)pDevice;

	MiscInitEthernetAdaptersCombo(&m_LocalHostCombo);
	char szSdmsIpAddress[32], szSdmsIpSubnetMask[32];
	MiscGetSdmsEthernetIP(m_pSDMS, szSdmsIpAddress, szSdmsIpSubnetMask);
	m_DesiredIpAddress = szSdmsIpAddress;
	m_DesiredIpSubnetMask = szSdmsIpSubnetMask;
	char szMacAddress[32];
	MiscGetSdmsEthernetMAC(m_pSDMS, szMacAddress);
	m_DesiredMacAddress = szMacAddress;
	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEthernetPortForm message handlers


void CEthernetPortForm::OnCopyButton() 
{
	MiscCopyAddressAndMask(&m_LocalHostCombo, m_DesiredIpAddress, m_DesiredIpSubnetMask);
	UpdateData(FALSE);
}

void CEthernetPortForm::OnSetIpAddressButton() 
{
	UpdateData();
	MiscSetSdmsEthernetIP(m_pSDMS, m_DesiredIpAddress, m_DesiredIpSubnetMask);
}

void CEthernetPortForm::OnRunTestButton() 
{
	MiscTestSdmsEthernetPort(m_pSDMS, m_Protocol, this);
}

void CEthernetPortForm::OnSetMacAddressButton() 
{
	UpdateData();
	MiscSetSdmsEthernetMAC(m_pSDMS, m_DesiredMacAddress);
}
