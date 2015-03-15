// SerialPortForm.cpp : implementation file
//

#include "stdafx.h"
#include "SerialPortForm.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialPortForm

IMPLEMENT_DYNCREATE(CSerialPortForm, CAbstractForm)

CSerialPortForm::CSerialPortForm()
	: CAbstractForm(CSerialPortForm::IDD)
{
	//{{AFX_DATA_INIT(CSerialPortForm)
	m_DesiredIpAddress = _T("");
	m_DesiredIpSubnetMask = _T("");
	//}}AFX_DATA_INIT
	m_pSDMS = NULL;
}

CSerialPortForm::~CSerialPortForm()
{
}

void CSerialPortForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialPortForm)
	DDX_Text(pDX, IDC_DESIRED_IP_ADDRESS_EDIT, m_DesiredIpAddress);
	DDX_Text(pDX, IDC_DESIRED_SUBNET_MASK, m_DesiredIpSubnetMask);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSerialPortForm, CAbstractForm)
	//{{AFX_MSG_MAP(CSerialPortForm)
	ON_BN_CLICKED(IDC_SET_IP_ADDRESS_BUTTON, OnSetIpAddressButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSerialPortForm diagnostics

#ifdef _DEBUG
void CSerialPortForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CSerialPortForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CSerialPortForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pSDMS = (CSdmsAPI *)pDevice;

/*	MiscInitEthernetAdaptersCombo(&m_LocalHostCombo, &m_IpHelper);
	char szSdmsIpAddress[32], szSdmsIpSubnetMask[32];
	MiscGetSdmsEthernetIP(m_pSDMS, szSdmsIpAddress, szSdmsIpSubnetMask);
	m_DesiredIpAddress = szSdmsIpAddress;
	m_DesiredIpSubnetMask = szSdmsIpSubnetMask;*/
	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSerialPortForm message handlers


void CSerialPortForm::OnSetIpAddressButton() 
{
	UpdateData();

//	MiscSetSdmsEthernetIP(m_pSDMS, m_DesiredIpAddress, m_DesiredIpSubnetMask);
}

