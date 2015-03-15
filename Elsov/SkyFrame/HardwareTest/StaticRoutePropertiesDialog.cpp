// StaticRoutePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "StaticRoutePropertiesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticRoutePropertiesDialog dialog


CStaticRoutePropertiesDialog::CStaticRoutePropertiesDialog(char *pszNet, char *pszMask, char *pszGateway, char *pszMetric, CWnd* pParent /*=NULL*/)
	: CDialog(CStaticRoutePropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStaticRoutePropertiesDialog)
	m_Metric = 0;
	m_GW = _T("");
	m_Mask = _T("");
	m_Net = _T("");
	//}}AFX_DATA_INIT
	m_pszNet=pszNet;
	m_pszMask=pszMask;
	m_pszGateway=pszGateway;
	m_pszMetric=pszMetric;
}


void CStaticRoutePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStaticRoutePropertiesDialog)
	DDX_Text(pDX, IDC_METRIC_EDIT, m_Metric);
	DDV_MinMaxUInt(pDX, m_Metric, 0, 16);
	DDX_Text(pDX, IDC_GW_EDIT, m_GW);
	DDX_Text(pDX, IDC_MASK_EDIT, m_Mask);
	DDX_Text(pDX, IDC_NET_EDIT, m_Net);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStaticRoutePropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CStaticRoutePropertiesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticRoutePropertiesDialog message handlers

BOOL CStaticRoutePropertiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

/*	BYTE byte0=0, byte1=0, byte2=0, byte3=0;
	sscanf(m_pszNet, "%d.%d.%d.%d", &byte0, &byte1, &byte2, &byte3);
	m_NetIPAddressCtrl.SetAddress(byte0, byte1, byte2, byte3);

	byte0=byte1=byte2=byte3=0;
	sscanf(m_pszMask, "%d.%d.%d.%d", &byte0, &byte1, &byte2, &byte3);
	m_MaskIPAddressCtrl.SetAddress(byte0, byte1, byte2, byte3);

	byte0=byte1=byte2=byte3=0;
	sscanf(m_pszGateway, "%d.%d.%d.%d", &byte0, &byte1, &byte2, &byte3);
	m_GatewayIPAddressCtrl.SetAddress(byte0, byte1, byte2, byte3);*/

	m_Net=m_pszNet;
	m_Mask=m_pszMask;
	m_GW=m_pszGateway;

	sscanf(m_pszMetric, "%d", &m_Metric);
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStaticRoutePropertiesDialog::OnOK() 
{
	UpdateData();

/*	BYTE byte0=0, byte1=0, byte2=0, byte3=0;
	m_NetIPAddressCtrl.GetAddress(byte0, byte1, byte2, byte3);
	sprintf(m_pszNet, "%d.%d.%d.%d", byte0, byte1, byte2, byte3);
		
	byte0=byte1=byte2=byte3=0;
	m_MaskIPAddressCtrl.GetAddress(byte0, byte1, byte2, byte3);
	sprintf(m_pszMask, "%d.%d.%d.%d", byte0, byte1, byte2, byte3);
		
	byte0=byte1=byte2=byte3=0;
	m_GatewayIPAddressCtrl.GetAddress(byte0, byte1, byte2, byte3);
	sprintf(m_pszGateway, "%d.%d.%d.%d", byte0, byte1, byte2, byte3);*/
	
	strcpy(m_pszNet, LPCTSTR(m_Net));
	strcpy(m_pszMask, LPCTSTR(m_Mask));
	strcpy(m_pszGateway, LPCTSTR(m_GW));
	sprintf(m_pszMetric, "%d", m_Metric);

	CDialog::OnOK();
}
