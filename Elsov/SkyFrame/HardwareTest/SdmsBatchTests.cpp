// SdmsBatchTests.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SdmsBatchTests.h"
#include "StelaM.h"
#include "PowerMeter.h"
#include "ProgressDlg.h"
#include "Misc_SoftwareVersions.h"
#include "MiscSdmsStartupConfig.h"
#include "MiscEthernet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsDevicePage property page

IMPLEMENT_DYNCREATE(CSdmsBatchTestsDevicePage, CAutoTestPropertyPage)

CSdmsBatchTestsDevicePage::CSdmsBatchTestsDevicePage()
	: CAutoTestPropertyPage(CSdmsBatchTestsDevicePage::IDD)
{
	//{{AFX_DATA_INIT(CSdmsBatchTestsDevicePage)
	m_FilePath = _T("");
	//}}AFX_DATA_INIT
}

CSdmsBatchTestsDevicePage::~CSdmsBatchTestsDevicePage()
{
}

void CSdmsBatchTestsDevicePage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsBatchTestsDevicePage)
	DDX_Control(pDX, IDC_SDMS_COMBO, m_StelaMCombo);
	DDX_Text(pDX, IDC_FILE_PATH_EDIT, m_FilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsDevicePage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSdmsBatchTestsDevicePage)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsDevicePage message handlers

BOOL CSdmsBatchTestsDevicePage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	for (int i=0; i<m_nDevices; i++)
	{
		CDevice *pDevice=m_ppDevices[i];
		if (pDevice == 0)
			continue;
		if (pDevice->IsStation())
		{
			// Station to be tested
			CString Name = pDevice->GetName();
			Name += " (";
			Name += pDevice->GetConnectionName();
			int index = m_StelaMCombo.AddString(Name+")");
			m_StelaMCombo.SetItemDataPtr(index, pDevice);
		}
	}
	if (m_StelaMCombo.GetCount() == 1)
		m_StelaMCombo.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSdmsBatchTestsDevicePage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CSdmsBatchTestsDevicePage::OnKillActive() 
{
	m_pAutoTestConfig->m_pEtalonModem = NULL;
	return CAutoTestPropertyPage::OnKillActive();
}

LRESULT CSdmsBatchTestsDevicePage::OnWizardNext() 
{
	UpdateData(FALSE);
	if (m_FilePath.IsEmpty())
		return -1;	// to prevent going ahead
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));

	// Station to be tested
	m_pAutoTestConfig->m_pTestSDMS = NULL;
	int index = m_StelaMCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CSdmsAPI *pSDMS = (CSdmsAPI *)m_StelaMCombo.GetItemDataPtr(index);
	ASSERT(pSDMS);
	m_pAutoTestConfig->m_pTestSDMS = pSDMS;
	if (pSDMS->GetControlType() != MC_SERIAL_PORT)
	{
		MessageBox("This test requires SDMS-card to be connected through SERIAL PORT, sorry.\nPlease reconnect that or choose another one.",
			"Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;	// to prevent going ahead
	}

	// Open file and write header
	CStdioFile *pFile = GetReportFile();
	if (pFile)
	{
		CString ReportHeaderStr;
		ReportHeaderStr += pSDMS->GetName();
		ReportHeaderStr += " investigation\n\n";

		WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(ReportHeaderStr));

		// Time
		CTime time=CTime::GetCurrentTime();
		CString string=time.Format("Date/Time: %d-%m-%Y %H:%M:%S\n");
		WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
	}

	return CAutoTestPropertyPage::OnWizardNext();
}

void CSdmsBatchTestsDevicePage::OnBrowseButton() 
{
	UpdateData();
	CFileDialog fd(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Text files(*txt)|*txt|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Specify file to write report";
	if (fd.DoModal()==IDCANCEL) return;
	m_FilePath=fd.GetPathName();
	UpdateData(FALSE);
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));
}


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsPortsPage property page

IMPLEMENT_DYNCREATE(CSdmsBatchTestsPortsPage, CAutoTestPropertyPage)

CSdmsBatchTestsPortsPage::CSdmsBatchTestsPortsPage()
	: CAutoTestPropertyPage(CSdmsBatchTestsPortsPage::IDD)
{
	//{{AFX_DATA_INIT(CSdmsBatchTestsPortsPage)
	m_Protocol = _T("");
	//}}AFX_DATA_INIT
}

CSdmsBatchTestsPortsPage::~CSdmsBatchTestsPortsPage()
{
}

void CSdmsBatchTestsPortsPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsBatchTestsPortsPage)
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsPortsPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSdmsBatchTestsPortsPage)
	ON_BN_CLICKED(IDC_REPEAT_BUTTON, OnRepeatButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsPortsPage message handlers

BOOL CSdmsBatchTestsPortsPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSdmsBatchTestsPortsPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CSdmsBatchTestsPortsPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CAutoTestPropertyPage::OnWizardNext();
}

void CSdmsBatchTestsPortsPage::DoTest()
{
	CWaitCursor cursor;
	MC_ErrorCode EC1, EC2, EC3;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 3);
	dlg.SetPos(0);

	// SDMS console port
	char *pszTestStatus = "SDMS console port testing...";;
	m_Protocol = pszTestStatus;
	m_Protocol += "\r\n";
	dlg.SetStatus(pszTestStatus);
	char *pszString;
	EC1 = m_pAutoTestConfig->m_pTestSDMS->GetDeviceVersionString(pszString);
	m_Protocol += pszString;
	dlg.StepIt();
	UpdateData(FALSE);

	if (!dlg.CheckCancelButton())
	{
		// Startup config
		m_Protocol += "\r\nStartup config....\r\n";
		EC2 = m_pAutoTestConfig->m_pTestSDMS->Enable();
		EC2 = m_pAutoTestConfig->m_pTestSDMS->Command("show startup-config\r\n");
		pszString = m_pAutoTestConfig->m_pTestSDMS->GetMCReply();
		m_Protocol += pszString;
		// Status string (system uptime for SDMS)
		m_Protocol += "\r\n";
		EC3 = m_pAutoTestConfig->m_pTestSDMS->GetDeviceStatusString(pszString);
		m_Protocol += pszString;
		if (EC1 == MC_OK && EC2 == MC_OK && EC3 == MC_OK)
			m_Protocol += "Test passed\r\n";
		else
			m_Protocol += "Test FAILED !!!\r\n";
	}
	dlg.StepIt();
	UpdateData(FALSE);
	

	/*if (!dlg.CheckCancelButton())
	{
		// RS-485 M&C port
		pszTestStatus = "SDMS RS-485 port testing...";
		m_Protocol += "\r\n\r\n";
		m_Protocol += pszTestStatus;
		m_Protocol += "\r\n";
		dlg.SetStatus(pszTestStatus);
		UpdateData(FALSE);
		EC1 = m_pAutoTestConfig->m_pTestSDMS->GetTranceiverStatus();
		if (EC1 == MC_OK)
		{
			CString RS485Report;
			double T, P;
			m_pAutoTestConfig->m_pTestSDMS->GetTranceiverTemperature(T);
			m_pAutoTestConfig->m_pTestSDMS->GetTranceiverPower(P);
			RS485Report.Format("Temperature = %5.1f\r\nPower = %5.1f\r\n", T, P);
			m_Protocol += RS485Report + "Test passed\r\n";
		}
		else
			m_Protocol += "Test FAILED !!!\r\n";
	}*/
	dlg.StepIt();
	UpdateData(FALSE);
}

void CSdmsBatchTestsPortsPage::OnRepeatButton() 
{
	DoTest();	
}


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsEthernetPage property page

IMPLEMENT_DYNCREATE(CSdmsBatchTestsEthernetPage, CAutoTestPropertyPage)

CSdmsBatchTestsEthernetPage::CSdmsBatchTestsEthernetPage()
	: CAutoTestPropertyPage(CSdmsBatchTestsEthernetPage::IDD)
{
	//{{AFX_DATA_INIT(CSdmsBatchTestsEthernetPage)
	m_DesiredIpAddress = _T("");
	m_Protocol = _T("");
	m_DesiredIpSubnetMask = _T("");
	m_DesiredMacAddress = _T("");
	//}}AFX_DATA_INIT
}

CSdmsBatchTestsEthernetPage::~CSdmsBatchTestsEthernetPage()
{
}

void CSdmsBatchTestsEthernetPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsBatchTestsEthernetPage)
	DDX_Control(pDX, IDC_LOCAL_HOST_COMBO, m_LocalHostCombo);
	DDX_Text(pDX, IDC_DESIRED_IP_ADDRESS_EDIT, m_DesiredIpAddress);
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_DESIRED_SUBNET_MASK, m_DesiredIpSubnetMask);
	DDX_Text(pDX, IDC_DESIRED_MAC_ADDRESS_EDIT, m_DesiredMacAddress);
	DDV_MaxChars(pDX, m_DesiredMacAddress, 17);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsEthernetPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSdmsBatchTestsEthernetPage)
	ON_BN_CLICKED(IDC_SET_IP_ADDRESS_BUTTON, OnSetIpAddressButton)
	ON_BN_CLICKED(IDC_RUN_TEST_BUTTON, OnRunTestButton)
	ON_BN_CLICKED(IDC_COPY_BUTTON, OnCopyButton)
	ON_BN_CLICKED(IDC_SET_MAC_ADDRESS_BUTTON, OnSetMacAddressButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsEthernetPage message handlers

BOOL CSdmsBatchTestsEthernetPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSdmsBatchTestsEthernetPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	CWaitCursor cursor;

	MiscInitEthernetAdaptersCombo(&m_LocalHostCombo);

	char szSdmsIpAddress[32], szSdmsIpSubnetMask[32];
	MiscGetSdmsEthernetIP(m_pAutoTestConfig->m_pTestSDMS, szSdmsIpAddress, szSdmsIpSubnetMask);
	m_DesiredIpAddress = szSdmsIpAddress;
	m_DesiredIpSubnetMask = szSdmsIpSubnetMask;
	char szMacAddress[32];
	MiscGetSdmsEthernetMAC(m_pAutoTestConfig->m_pTestSDMS, szMacAddress);
	m_DesiredMacAddress = szMacAddress;
	UpdateData(FALSE);

	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString("\nEthernet port testing...\n\n");

	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CSdmsBatchTestsEthernetPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CAutoTestPropertyPage::OnWizardNext();
}

void CSdmsBatchTestsEthernetPage::OnSetIpAddressButton() 
{
	UpdateData();
	MiscSetSdmsEthernetIP(m_pAutoTestConfig->m_pTestSDMS, m_DesiredIpAddress, m_DesiredIpSubnetMask);
}


void CSdmsBatchTestsEthernetPage::OnCopyButton() 
{
	MiscCopyAddressAndMask(&m_LocalHostCombo, m_DesiredIpAddress, m_DesiredIpSubnetMask);
	UpdateData(FALSE);
}

void CSdmsBatchTestsEthernetPage::OnRunTestButton() 
{
	MiscTestSdmsEthernetPort(m_pAutoTestConfig->m_pTestSDMS, m_Protocol, this);
}

void CSdmsBatchTestsEthernetPage::OnSetMacAddressButton() 
{
	UpdateData();
	MiscSetSdmsEthernetMAC(m_pAutoTestConfig->m_pTestSDMS, m_DesiredMacAddress);
}

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsSoftwareVersionPage property page

IMPLEMENT_DYNCREATE(CSdmsBatchTestsSoftwareVersionPage, CAutoTestPropertyPage)

CSdmsBatchTestsSoftwareVersionPage::CSdmsBatchTestsSoftwareVersionPage()
	: CAutoTestPropertyPage(CSdmsBatchTestsSoftwareVersionPage::IDD)
{
	//{{AFX_DATA_INIT(CSdmsBatchTestsSoftwareVersionPage)
	m_SoftwareVersion = _T("");
	//}}AFX_DATA_INIT
}

CSdmsBatchTestsSoftwareVersionPage::~CSdmsBatchTestsSoftwareVersionPage()
{
}

void CSdmsBatchTestsSoftwareVersionPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsBatchTestsSoftwareVersionPage)
	DDX_Text(pDX, IDC_SOFTWARE_VERSION_EDIT, m_SoftwareVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsSoftwareVersionPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSdmsBatchTestsSoftwareVersionPage)
	ON_BN_CLICKED(IDC_SETUP_NEW_VERSION_BUTTON, OnSetupNewVersionButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsSoftwareVersionPage message handlers

BOOL CSdmsBatchTestsSoftwareVersionPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSdmsBatchTestsSoftwareVersionPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	CWaitCursor cursor;
	char *pszString;
	MC_ErrorCode EC = m_pAutoTestConfig->m_pTestSDMS->GetDeviceVersionString(pszString);
	m_SoftwareVersion = pszString;
	UpdateData(FALSE);
	
	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CSdmsBatchTestsSoftwareVersionPage::OnWizardNext() 
{
	return CAutoTestPropertyPage::OnWizardNext();
}


void CSdmsBatchTestsSoftwareVersionPage::OnSetupNewVersionButton() 
{
	CFileDialog fd(TRUE, _T("bin"), NULL, 0,
		_T("Ôàéëû ïðîøèâîê (*bin)|*bin|Âñå ôàéëû (*.*)|*.*||"));
	if (fd.DoModal()==IDCANCEL) return;
	CString SoftwareFileName = fd.GetPathName();
	
	CWaitCursor cursor;

	// find IP-address of ftp-server
	unsigned long SdmsIpAddress, SdmsIpSubnetMask;
	BOOL bSuccess = MiscGetSdmsEthernetIP(m_pAutoTestConfig->m_pTestSDMS, SdmsIpAddress, SdmsIpSubnetMask);
	if (!bSuccess)
		return;
	char szFtpServerIpAddress[32];
	bSuccess = MiscFindIpAddressForSDMS(SdmsIpAddress, SdmsIpSubnetMask, szFtpServerIpAddress);
	if (!bSuccess)
		return;
	
	bSuccess = Misc_SetNewSdmsSoftware(m_pAutoTestConfig->m_pTestSDMS, SoftwareFileName, szFtpServerIpAddress);
	if (!bSuccess)
		return;

	CString str;
	str.Format("\nCommunication software (file %s) is downloaded from FTP-server %s\n",  SoftwareFileName, szFtpServerIpAddress);
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(str);

	bSuccess = Misc_ReloadSDMS(m_pAutoTestConfig->m_pTestSDMS);
	if (!bSuccess)
		return;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetStatus("Restoring IP-address and subnet mask");
	MC_ErrorCode EC = m_pAutoTestConfig->m_pTestSDMS->SetEthernetIpAddress(SdmsIpAddress, SdmsIpSubnetMask);
	if (EC != MC_OK)
	{
		MessageBox("IP-address setting was FAILED !", pszError, MB_ICONEXCLAMATION);
	}

	dlg.SetStatus("Getting version...");
	char *pszString;
	EC = m_pAutoTestConfig->m_pTestSDMS->GetDeviceVersionString(pszString);
	m_SoftwareVersion = pszString;
	UpdateData(FALSE);
	if (pFile)
	{
		pFile->WriteString("Current software version is :\n");
		pFile->WriteString(m_SoftwareVersion);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsShellPage property page

IMPLEMENT_DYNCREATE(CSdmsBatchTestsShellPage, CAutoTestPropertyPage)

CSdmsBatchTestsShellPage::CSdmsBatchTestsShellPage()
	: CAutoTestPropertyPage(CSdmsBatchTestsShellPage::IDD)
{
	//{{AFX_DATA_INIT(CSdmsBatchTestsShellPage)
	m_Protocol = _T("");
	//}}AFX_DATA_INIT
}

CSdmsBatchTestsShellPage::~CSdmsBatchTestsShellPage()
{
}

void CSdmsBatchTestsShellPage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsBatchTestsShellPage)
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsShellPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSdmsBatchTestsShellPage)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsShellPage message handlers

BOOL CSdmsBatchTestsShellPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSdmsBatchTestsShellPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	pSheet->SetWindowText(GetTestDescription());

	m_pSDMS = m_pAutoTestConfig->m_pTestSDMS;

	m_Protocol = GetTestDescription()+"\r\n";

	UpdateData(FALSE);
	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CSdmsBatchTestsShellPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CAutoTestPropertyPage::OnWizardNext();
}

void CSdmsBatchTestsShellPage::OnRunButton() 
{
	m_Protocol = GetTestDescription()+"\r\n";
	if (BeforeTest())
	{
		m_Protocol += "connected\r\n";
		UpdateData(FALSE);
		DoTest();
	}
	else
	{
		m_Protocol += "not connected\r\n";
	}
	AfterTest();
	UpdateData(FALSE);
}

void CSdmsBatchTestsShellPage::DoTest()
{
	CString str;
	char *pszStepDescription;
	MC_ErrorCode EC;
	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 4);
	dlg.SetPos(0);

	// Getting demodulator IF status
	pszStepDescription = "Getting demodulator IF status...";
	m_Protocol += pszStepDescription;
	dlg.SetStatus(pszStepDescription);
	CDemIfStatus status;
	EC = m_pSDMS->GetRIfStatus(status, 1);
	if (EC == MC_OK)
	{
		m_Protocol += "OK";
	}
	else
	{
		m_Protocol += "ERROR!\r\n";
		m_Protocol += m_pSDMS->GetMCReply();
	}
	m_Protocol += "\r\n";
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return;

	// Getting modulator IF status
	pszStepDescription = "Getting modulator IF status...";
	m_Protocol += pszStepDescription;
	dlg.SetStatus(pszStepDescription);
	CModIfParams TParams;
	EC = m_pSDMS->GetTIfParams(TParams, 1);
	if (EC == MC_OK)
	{
		m_Protocol += "OK";
		str.Format(" ( Output level = %4.1f dB·mW )", TParams.m_OutputLevel);
		m_Protocol += str;
	}
	else
	{
		m_Protocol += "ERROR!\r\n";
		m_Protocol += m_pSDMS->GetMCReply();
	}
	m_Protocol += "\r\n";
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return;

	// Setting modulator IF parameter
	double Level = TParams.m_OutputLevel;
	Level += 1;
	pszStepDescription = "Setting modulator IF parameter (increment output level)...";
	m_Protocol += pszStepDescription;
	dlg.SetStatus(pszStepDescription);
	EC = m_pSDMS->SetOutputLevel(Level, 1);
	if (EC == MC_OK)
	{
		m_Protocol += "OK";
	}
	else
	{
		m_Protocol += "ERROR!\r\n";
		m_Protocol += m_pSDMS->GetMCReply();
	}
	m_Protocol += "\r\n";
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return;

	// Getting modulator IF status again
	pszStepDescription = "Getting modulator IF status again...";
	m_Protocol += pszStepDescription;
	dlg.SetStatus(pszStepDescription);
	EC = m_pSDMS->GetTIfParams(TParams, 1);
	if (EC == MC_OK)
	{
		m_Protocol += "OK";
		str.Format(" ( Output level = %4.1f dB·mW )", TParams.m_OutputLevel);
		m_Protocol += str;
	}
	else
	{
		m_Protocol += "ERROR!\r\n";
		m_Protocol += m_pSDMS->GetMCReply();
	}
	m_Protocol += "\r\n";
	m_pSDMS->GetOutputLevel(Level, 1);
	dlg.StepIt();
	UpdateData(FALSE);
}

//virtual
BOOL CSdmsBatchTestsShellSerialPortPage::BeforeTest()
{
	CWaitCursor cursor;
	m_Protocol += "Connecting to SDMS console...";
	UpdateData(FALSE);
	if (m_pSDMS->IsControllable())
		return TRUE;
	return m_pSDMS->Connect();
}

//virtual
BOOL CSdmsBatchTestsShellSerialPortPage::AfterTest()
{
	return TRUE;
}

//virtual
BOOL CSdmsBatchTestsShellTelnetPage::BeforeTest()
{
	CWaitCursor cursor;
	m_Protocol += "Connecting to shell by telnet...";
	UpdateData(FALSE);
	char szSdmsIpAddress[32], szSdmsIpSubnetMask[32];
	BOOL bSuccess = MiscGetSdmsEthernetIP(m_pAutoTestConfig->m_pTestSDMS, szSdmsIpAddress, szSdmsIpSubnetMask);
	if (!bSuccess)
		return FALSE;
	// store com-port connection
	m_StoredConnectionName = m_pSDMS->GetConnectionName();
	m_pSDMS->Disconnect();
	m_pSDMS->SetControlType(MC_TELNET);
	return m_pSDMS->Connect(szSdmsIpAddress);
}

//virtual
BOOL CSdmsBatchTestsShellTelnetPage::AfterTest()
{
	CWaitCursor cursor;
	m_pSDMS->SetControlType(MC_SERIAL_PORT);
	return m_pSDMS->Connect(m_StoredConnectionName);
}

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsStartupConfigPage property page

IMPLEMENT_DYNCREATE(CSdmsBatchTestsStartupConfigPage, CAutoTestPropertyPage)

CSdmsBatchTestsStartupConfigPage::CSdmsBatchTestsStartupConfigPage()
	: CAutoTestPropertyPage(CSdmsBatchTestsStartupConfigPage::IDD)
{
	//{{AFX_DATA_INIT(CSdmsBatchTestsStartupConfigPage)
	m_StartupConfig = _T("");
	//}}AFX_DATA_INIT
}

CSdmsBatchTestsStartupConfigPage::~CSdmsBatchTestsStartupConfigPage()
{
}

void CSdmsBatchTestsStartupConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsBatchTestsStartupConfigPage)
	DDX_Control(pDX, IDC_STARTUP_CONFIG_EDIT, m_StartupConfigCtrl);
	DDX_Text(pDX, IDC_STARTUP_CONFIG_EDIT, m_StartupConfig);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsStartupConfigPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSdmsBatchTestsStartupConfigPage)
	ON_BN_CLICKED(IDC_GET_CURRENT_CONFIG_BUTTON, OnGetCurrentConfigButton)
	ON_BN_CLICKED(IDC_LOAD_FROM_FILE_BUTTON, OnLoadFromFileButton)
	ON_BN_CLICKED(IDC_SAVE_CONFIG_TO_FILE_BUTTON, OnSaveConfigToFileButton)
	ON_BN_CLICKED(IDC_SET_STARTUP_CONFIG_BUTTON, OnSetStartupConfigButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsStartupConfigPage message handlers

BOOL CSdmsBatchTestsStartupConfigPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSdmsBatchTestsStartupConfigPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	CWaitCursor cursor;
	OnGetCurrentConfigButton();

	UpdateData(FALSE);
	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CSdmsBatchTestsStartupConfigPage::OnWizardNext() 
{
	return CAutoTestPropertyPage::OnWizardNext();
}


void CSdmsBatchTestsStartupConfigPage::OnGetCurrentConfigButton() 
{
	m_StartupConfig = Misc_GetSdmsStartupConfig(m_pAutoTestConfig->m_pTestSDMS);
	UpdateData(FALSE);
}

void CSdmsBatchTestsStartupConfigPage::OnLoadFromFileButton() 
{
	Misc_LoadStartupConfigFromFile(m_StartupConfig);
	UpdateData(FALSE);	
}

void CSdmsBatchTestsStartupConfigPage::OnSaveConfigToFileButton() 
{
	UpdateData();
	Misc_SaveStartupConfigToFile(m_StartupConfig);
}

void CSdmsBatchTestsStartupConfigPage::OnSetStartupConfigButton() 
{
	UpdateData(TRUE);
	Misc_SetStartupConfig(m_StartupConfigCtrl, m_pAutoTestConfig->m_pTestSDMS);
	OnGetCurrentConfigButton();
}

/////////////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsPropertySheet implementation

IMPLEMENT_DYNAMIC(CSdmsBatchTestsPropertySheet, CAutoTestSheet)

CSdmsBatchTestsPropertySheet::CSdmsBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	m_DevicePage.m_nDevices = nDevices;
	m_DevicePage.m_ppDevices = ppDevices;
	AddPage(&m_DevicePage);
	AddPage(&m_PortsPage);
	AddPage(&m_EthernetPage);
	AddPage(&m_SoftwareVersionPage);
	AddPage(&m_SdmsShellSerialPortPage);
	AddPage(&m_SdmsShellTelnetPage);
	AddPage(&m_StartupConfigPage);
	AddPage(&m_CongratulationPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig = &m_AutoTestConfig;
}

CSdmsBatchTestsPropertySheet::~CSdmsBatchTestsPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CSdmsBatchTestsPropertySheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CSdmsBatchTestsPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSdmsBatchTestsPropertySheet message handlers



