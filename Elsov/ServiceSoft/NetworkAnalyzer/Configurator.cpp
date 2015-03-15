/* Configurator.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	16 May 2007 - context menu
*/


#include "stdafx.h"
#include "NetworkAnalyzer.h"
#include "Network.h"
#include "Configurator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHostConfigurePage property page

CHostConfigurePage::CHostConfigurePage(UINT ID) 
	: CPropertyPage(ID)
{
	//{{AFX_DATA_INIT(CHostConfigurePage)
	//}}AFX_DATA_INIT
	m_pHost = NULL;
}

CHostConfigurePage::~CHostConfigurePage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHostConfigureCommonPage property page

IMPLEMENT_DYNCREATE(CHostConfigureCommonPage, CPropertyPage)

CHostConfigureCommonPage::CHostConfigureCommonPage() 
	: CHostConfigurePage(CHostConfigureCommonPage::IDD)
{
	//{{AFX_DATA_INIT(CHostConfigureCommonPage)
	m_GivenName = _T("");
	m_NETBIOS_Name = _T("");
	//}}AFX_DATA_INIT
}

CHostConfigureCommonPage::~CHostConfigureCommonPage()
{
}

void CHostConfigureCommonPage::DoDataExchange(CDataExchange *pDX)
{
	CHostConfigurePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostConfigureCommonPage)
	DDX_Text(pDX, IDC_GIVEN_NAME_EDIT, m_GivenName);
	DDX_Text(pDX, IDC_NETBIOS_NAME_EDIT, m_NETBIOS_Name);
	//}}AFX_DATA_MAP
}

//virtual
BOOL CHostConfigureCommonPage::OnInitDialog()
{
	m_GivenName = m_pHost->GetGivenName();
	m_NETBIOS_Name = m_pHost->GetName();
	if (m_NETBIOS_Name.IsEmpty())
	{
		GetDescendantWindow(IDC_NETBIOS_NAME_STATIC)->ShowWindow(SW_HIDE);
		GetDescendantWindow(IDC_NETBIOS_NAME_EDIT)->ShowWindow(SW_HIDE);
	}
	return CHostConfigurePage::OnInitDialog();
}

//virtual
void CHostConfigureCommonPage::OnOK()
{
	UpdateData();
	m_pHost->SetGivenName(m_GivenName);
	CHostConfigurePage::OnOK();
}

BEGIN_MESSAGE_MAP(CHostConfigureCommonPage, CHostConfigurePage)
	//{{AFX_MSG_MAP(CHostConfigureCommonPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostConfigureCommonPage message handlers


/////////////////////////////////////////////////////////////////////////////
// CHostConfigurator

IMPLEMENT_DYNAMIC(CHostConfigurator, CPropertySheet)

CHostConfigurator::CHostConfigurator(CHost *pHost, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pHost = pHost;
	m_CommonPage.SetHost(pHost);
	AddPage(&m_CommonPage);
}

CHostConfigurator::~CHostConfigurator()
{
}


BEGIN_MESSAGE_MAP(CHostConfigurator, CPropertySheet)
	//{{AFX_MSG_MAP(CHostConfigurator)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostConfigurator message handlers


/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigurePage property page

CSubnetConfigurePage::CSubnetConfigurePage(UINT ID) 
	: CPropertyPage(ID)
{
	//{{AFX_DATA_INIT(CSubnetConfigurePage)
	//}}AFX_DATA_INIT
	m_pSubnet = NULL;
}

CSubnetConfigurePage::~CSubnetConfigurePage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigureCommonPage property page

IMPLEMENT_DYNCREATE(CSubnetConfigureCommonPage, CPropertyPage)

CSubnetConfigureCommonPage::CSubnetConfigureCommonPage() 
	: CSubnetConfigurePage(CSubnetConfigureCommonPage::IDD)
{
	//{{AFX_DATA_INIT(CSubnetConfigureCommonPage)
	m_GivenName = _T("");
	//}}AFX_DATA_INIT
}

CSubnetConfigureCommonPage::~CSubnetConfigureCommonPage()
{
}

void CSubnetConfigureCommonPage::DoDataExchange(CDataExchange *pDX)
{
	CSubnetConfigurePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubnetConfigureCommonPage)
	DDX_Text(pDX, IDC_GIVEN_NAME_EDIT, m_GivenName);
	//}}AFX_DATA_MAP
}

//virtual
BOOL CSubnetConfigureCommonPage::OnInitDialog()
{
	m_GivenName = m_pSubnet->GetGivenName();
	return CSubnetConfigurePage::OnInitDialog();
}

//virtual
void CSubnetConfigureCommonPage::OnOK()
{
	UpdateData();
	m_pSubnet->SetGivenName(m_GivenName);
	CSubnetConfigurePage::OnOK();
}

BEGIN_MESSAGE_MAP(CSubnetConfigureCommonPage, CSubnetConfigurePage)
	//{{AFX_MSG_MAP(CSubnetConfigureCommonPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigureCommonPage message handlers


/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigurator

IMPLEMENT_DYNAMIC(CSubnetConfigurator, CPropertySheet)

CSubnetConfigurator::CSubnetConfigurator(CSubnet *pSubnet, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pSubnet = pSubnet;
	m_CommonPage.SetSubnet(pSubnet);
	AddPage(&m_CommonPage);
}

CSubnetConfigurator::~CSubnetConfigurator()
{
}


BEGIN_MESSAGE_MAP(CSubnetConfigurator, CPropertySheet)
	//{{AFX_MSG_MAP(CSubnetConfigurator)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostConfigurator message handlers
