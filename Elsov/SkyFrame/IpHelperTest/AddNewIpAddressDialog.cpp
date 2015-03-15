// AddNewIpAddressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IpHelperTest.h"
#include "AddNewIpAddressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddNewIpAddressDialog dialog


CAddNewIpAddressDialog::CAddNewIpAddressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewIpAddressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNewIpAddressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAddNewIpAddressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNewIpAddressDialog)
	DDX_Control(pDX, IDC_IPADDRESS_SUBNET_MASK, m_IpSubnetMaskCtrl);
	DDX_Control(pDX, IDC_IPADDRESS_ADDRESS, m_IpAddressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNewIpAddressDialog, CDialog)
	//{{AFX_MSG_MAP(CAddNewIpAddressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNewIpAddressDialog message handlers

void CAddNewIpAddressDialog::OnOK() 
{
	unsigned char b1, b2, b3, b4;
	m_IpAddressCtrl.GetAddress(b1, b2, b3, b4);
	m_IpAddress = MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4));
	m_IpSubnetMaskCtrl.GetAddress(b1, b2, b3, b4);
	m_IpSubnetMask = MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4));
	CDialog::OnOK();
}
