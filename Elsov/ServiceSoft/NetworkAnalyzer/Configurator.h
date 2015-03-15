/* Configurator.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	16 May 2007 - context menu
*/


#ifndef __HOST_CONFIGURATOR_H_INCLUDED__
#define __HOST_CONFIGURATOR_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHostConfigurePage page

class CHostConfigurePage : public CPropertyPage
{
// Construction
public:
	CHostConfigurePage(UINT ID);
	~CHostConfigurePage();
public:
	void SetHost(CHost *pHost) { m_pHost = pHost; }
protected:
	CHost *m_pHost;
};

/////////////////////////////////////////////////////////////////////////////
// CHostConfigureCommonPage page

class CHostConfigureCommonPage : public CHostConfigurePage
{
	DECLARE_DYNCREATE(CHostConfigureCommonPage)

// Construction
public:
	CHostConfigureCommonPage();
	~CHostConfigureCommonPage();

// Dialog Data
	//{{AFX_DATA(CHostConfigureCommonPage)
	enum { IDD = IDD_HOST_CONFIGURE_COMMON_PAGE };
	CString	m_GivenName;
	CString	m_NETBIOS_Name;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CHostConfigureCommonPage)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHostConfigureCommonPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CHostConfigurator

class CHostConfigurator : public CPropertySheet
{
	DECLARE_DYNAMIC(CHostConfigurator)

// Construction
public:
	CHostConfigurator(CHost *pHost, LPCTSTR pszCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
	CHost *GetHost() { return m_pHost; }

// Overrides
	//{{AFX_VIRTUAL(CHostConfigurator)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHostConfigurator();

protected:
	//{{AFX_MSG(CHostConfigurator)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CHostConfigureCommonPage m_CommonPage;
	CHost *m_pHost;
};

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigurePage page

class CSubnetConfigurePage : public CPropertyPage
{
// Construction
public:
	CSubnetConfigurePage(UINT ID);
	~CSubnetConfigurePage();
public:
	void SetSubnet(CSubnet *pSubnet) { m_pSubnet = pSubnet; }
protected:
	CSubnet *m_pSubnet;
};

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigureCommonPage page

class CSubnetConfigureCommonPage : public CSubnetConfigurePage
{
	DECLARE_DYNCREATE(CSubnetConfigureCommonPage)

// Construction
public:
	CSubnetConfigureCommonPage();
	~CSubnetConfigureCommonPage();

// Dialog Data
	//{{AFX_DATA(CSubnetConfigureCommonPage)
	enum { IDD = IDD_SUBNET_CONFIGURE_COMMON_PAGE };
	CString	m_GivenName;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CSubnetConfigureCommonPage)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSubnetConfigureCommonPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigurator

class CSubnetConfigurator : public CPropertySheet
{
	DECLARE_DYNAMIC(CSubnetConfigurator)

// Construction
public:
	CSubnetConfigurator(CSubnet *pSubnet, LPCTSTR pszCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSubnetConfigurator)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubnetConfigurator();

protected:
	//{{AFX_MSG(CSubnetConfigurator)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CSubnetConfigureCommonPage m_CommonPage;
	CSubnet *m_pSubnet;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __HOST_CONFIGURATOR_H_INCLUDED__
