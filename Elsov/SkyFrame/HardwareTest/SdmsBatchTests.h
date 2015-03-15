#ifndef __SDMS_BATCH_TESTS_H_INCLUDED__
#define __SDMS_BATCH_TESTS_H_INCLUDED__

#include "CommonTests.h"
#include "IpHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsDevicePage property page

class CSdmsBatchTestsDevicePage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSdmsBatchTestsDevicePage)

// Construction
public:
	CSdmsBatchTestsDevicePage();
	~CSdmsBatchTestsDevicePage();

// Dialog Data
	//{{AFX_DATA(CSdmsBatchTestsDevicePage)
	enum { IDD = IDD_SDMS_BATCH_TESTS_DEVICE_PROPPAGE };
	CComboBox	m_StelaMCombo;
	CString	m_FilePath;
	//}}AFX_DATA
	CDevice **m_ppDevices;
	int m_nDevices;

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsDevicePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSdmsBatchTestsDevicePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsPortsPage property page

class CSdmsBatchTestsPortsPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSdmsBatchTestsPortsPage)

// Construction
public:
	CSdmsBatchTestsPortsPage();
	~CSdmsBatchTestsPortsPage();

// Dialog Data
	//{{AFX_DATA(CSdmsBatchTestsPortsPage)
	enum { IDD = IDD_SDMS_BATCH_TESTS_PORTS_PROPPAGE };
	CString	m_Protocol;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsPortsPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoTest();
	//{{AFX_MSG(CSdmsBatchTestsPortsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRepeatButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsEthernetPage property page

class CSdmsBatchTestsEthernetPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSdmsBatchTestsEthernetPage)

// Construction
public:
	CSdmsBatchTestsEthernetPage();
	~CSdmsBatchTestsEthernetPage();

// Dialog Data
	//{{AFX_DATA(CSdmsBatchTestsEthernetPage)
	enum { IDD = IDD_SDMS_BATCH_TESTS_ETHERNET_PROPPAGE };
	CComboBox	m_LocalHostCombo;
	CString	m_DesiredIpAddress;
	CString	m_Protocol;
	CString	m_DesiredIpSubnetMask;
	CString	m_DesiredMacAddress;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsEthernetPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Protected members

// Implementation
protected:
	//{{AFX_MSG(CSdmsBatchTestsEthernetPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetIpAddressButton();
	afx_msg void OnRunTestButton();
	afx_msg void OnCopyButton();
	afx_msg void OnSetMacAddressButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsSoftwareVersionPage property page

class CSdmsBatchTestsSoftwareVersionPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSdmsBatchTestsSoftwareVersionPage)

// Construction
public:
	CSdmsBatchTestsSoftwareVersionPage();
	~CSdmsBatchTestsSoftwareVersionPage();

// Dialog Data
	//{{AFX_DATA(CSdmsBatchTestsSoftwareVersionPage)
	enum { IDD = IDD_SDMS_BATCH_TESTS_SOFTWARE_VERSION_PROPPAGE };
	CString	m_SoftwareVersion;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsSoftwareVersionPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSdmsBatchTestsSoftwareVersionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetupNewVersionButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// protected methods
protected:
};


/////////////////////////////////////////////////////////////////////////////
// CStelaMBatchTestsSdmsShellPage property page

class CSdmsBatchTestsShellPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSdmsBatchTestsShellPage)

// Construction
public:
	CSdmsBatchTestsShellPage();
	~CSdmsBatchTestsShellPage();

// Protected overridables
protected:
	virtual CString GetTestDescription() { return ""; }
	virtual BOOL BeforeTest() { return TRUE; }
	virtual BOOL AfterTest() { return TRUE; }

// Dialog Data
	//{{AFX_DATA(CSdmsBatchTestsShellPage)
	enum { IDD = IDD_SDMS_BATCH_TESTS_SHELL_PROPPAGE };
	CString	m_Protocol;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsShellPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoTest();
	//{{AFX_MSG(CSdmsBatchTestsShellPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRunButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
// Protected members
	CSdmsAPI *m_pSDMS;
	CString m_StoredConnectionName;
};

class CSdmsBatchTestsShellSerialPortPage : public CSdmsBatchTestsShellPage
{
// Protected overridables
protected:
	virtual CString GetTestDescription() { return "SDMS shell test (serial port Console SDMS)"; }
	virtual BOOL BeforeTest();
	virtual BOOL AfterTest();
};

class CSdmsBatchTestsShellTelnetPage : public CSdmsBatchTestsShellPage
{
// Protected overridables
protected:
	virtual CString GetTestDescription() { return "SDMS shell test (telnet protocol)"; }
	virtual BOOL BeforeTest();
	virtual BOOL AfterTest();
};

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsStartupConfigPage property page

class CSdmsBatchTestsStartupConfigPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSdmsBatchTestsStartupConfigPage)

// Construction
public:
	CSdmsBatchTestsStartupConfigPage();
	~CSdmsBatchTestsStartupConfigPage();

// Dialog Data
	//{{AFX_DATA(CSdmsBatchTestsStartupConfigPage)
	enum { IDD = IDD_SDMS_BATCH_TESTS_STARTUP_CONFIG_PROPPAGE };
	CEdit	m_StartupConfigCtrl;
	CString	m_StartupConfig;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsStartupConfigPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSdmsBatchTestsStartupConfigPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetCurrentConfigButton();
	afx_msg void OnLoadFromFileButton();
	afx_msg void OnSaveConfigToFileButton();
	afx_msg void OnSetStartupConfigButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// protected members
protected:
};


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsPropertySheet

class CSdmsBatchTestsPropertySheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CSdmsBatchTestsPropertySheet)

// Construction
public:
	CSdmsBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent = NULL);

// Attributes
public:
	CSdmsBatchTestsDevicePage m_DevicePage;
	CSdmsBatchTestsPortsPage m_PortsPage;
	CSdmsBatchTestsEthernetPage m_EthernetPage;
	CSdmsBatchTestsSoftwareVersionPage m_SoftwareVersionPage;
	CSdmsBatchTestsStartupConfigPage m_StartupConfigPage;
	CSdmsBatchTestsShellSerialPortPage m_SdmsShellSerialPortPage;
	CSdmsBatchTestsShellTelnetPage m_SdmsShellTelnetPage;
	CCommonTestsCongratulationsPage m_CongratulationPage;
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSdmsBatchTestsPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSdmsBatchTestsPropertySheet();

protected:
	//{{AFX_MSG(CSdmsBatchTestsPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__SDMS_BATCH_TESTS_H_INCLUDED__
