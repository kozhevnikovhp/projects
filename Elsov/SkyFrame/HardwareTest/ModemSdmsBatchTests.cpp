
#include "stdafx.h"
#include "resource.h"
#include "PowerMeter.h"
#include "FrequencyCounter.h"
#include "ModemSdmsBatchTests.h"
#include "MiscEthernet.h"
#include "ProgressDlg.h"
#include "StelaM.h"
#include "IcmpSockets.h"

/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsDevicePage property page

IMPLEMENT_DYNCREATE(CModemSdmsBatchTestsDevicePage, CAutoTestPropertyPage)

CModemSdmsBatchTestsDevicePage::CModemSdmsBatchTestsDevicePage()
	: CAutoTestPropertyPage(CModemSdmsBatchTestsDevicePage::IDD)
{
	//{{AFX_DATA_INIT(CModemSdmsBatchTestsDevicePage)
	m_FilePath = _T("");
	//}}AFX_DATA_INIT
}

CModemSdmsBatchTestsDevicePage::~CModemSdmsBatchTestsDevicePage()
{
}

void CModemSdmsBatchTestsDevicePage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemSdmsBatchTestsDevicePage)
	DDX_Control(pDX, IDC_ETALON_STELAM_COMBO, m_EtalonSdmsCombo);
	DDX_Control(pDX, IDC_ETALON_MODEM_COMBO, m_EtalonModemCombo);
	DDX_Control(pDX, IDC_FREQUENCY_COUNTER_COMBO, m_FrequencyCounterCombo);
	DDX_Control(pDX, IDC_POWER_METER_COMBO, m_PowerMeterCombo);
	DDX_Control(pDX, IDC_SDMS_COMBO, m_StelaMCombo);
	DDX_Control(pDX, IDC_MODEM_COMBO, m_ModemCombo);
	DDX_Text(pDX, IDC_FILE_PATH_EDIT, m_FilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemSdmsBatchTestsDevicePage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CModemSdmsBatchTestsDevicePage)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsDevicePage message handlers

BOOL CModemSdmsBatchTestsDevicePage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	for (int i=0; i<m_nDevices; i++)
	{
		CDevice *pDevice=m_ppDevices[i];
		if (pDevice == 0)
			continue;
		if (pDevice->IsModem() && !pDevice->IsStation())
		{
			// Device to be tested
			CString Name = pDevice->GetName();
			Name += " (";
			Name += pDevice->GetConnectionName();
			Name += ")";
			int index = m_ModemCombo.AddString(Name);
			m_ModemCombo.SetItemDataPtr(index, pDevice);
			index = m_EtalonModemCombo.AddString(Name);
			m_EtalonModemCombo.SetItemDataPtr(index, pDevice);
		}
		if (pDevice->IsStation())
		{
			// Station to be tested
			CString Name = pDevice->GetName();
			Name += " (";
			Name += pDevice->GetConnectionName();
			Name += ")";
			int index = m_StelaMCombo.AddString(Name);
			m_StelaMCombo.SetItemDataPtr(index, pDevice);
			index = m_EtalonSdmsCombo.AddString(Name);
			m_EtalonSdmsCombo.SetItemDataPtr(index, pDevice);
		}
		if (pDevice->IsMeasureDevice())
		{
			CMeasureDevice *pMeasureDevice = (CMeasureDevice *)pDevice;
			if (pMeasureDevice->IsPowerMeter())
			{
				CString Name = pDevice->GetName();
				Name += " (";
				Name += pDevice->GetConnectionName();
				int index = m_PowerMeterCombo.AddString(Name + ")");
				m_PowerMeterCombo.SetItemDataPtr(index, pDevice);
			}
			if (pMeasureDevice->IsFrequencyCounter())
			{
				CString Name = pDevice->GetName();
				Name += " (";
				Name += pDevice->GetConnectionName();
				int index = m_FrequencyCounterCombo.AddString(Name + ")");
				m_FrequencyCounterCombo.SetItemDataPtr(index, pDevice);
			}
		}
	}
	if (m_StelaMCombo.GetCount() == 1)
		m_StelaMCombo.SetCurSel(0);
	if (m_ModemCombo.GetCount() == 1)
		m_ModemCombo.SetCurSel(0);
	if (m_EtalonSdmsCombo.GetCount() == 1)
		m_EtalonSdmsCombo.SetCurSel(0);
	if (m_EtalonModemCombo.GetCount() == 1)
		m_EtalonModemCombo.SetCurSel(0);
	if (m_PowerMeterCombo.GetCount() == 1)
		m_PowerMeterCombo.SetCurSel(0);
	if (m_FrequencyCounterCombo.GetCount() == 1)
		m_FrequencyCounterCombo.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CModemSdmsBatchTestsDevicePage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CModemSdmsBatchTestsDevicePage::OnKillActive() 
{
	return CAutoTestPropertyPage::OnKillActive();
}

LRESULT CModemSdmsBatchTestsDevicePage::OnWizardNext() 
{
	UpdateData(FALSE);
	if (m_FilePath.IsEmpty())
		return -1;	// to prevent going ahead
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));

	// Modem to be tested
	int index = m_ModemCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CModem *pModem = (CModem *)m_ModemCombo.GetItemDataPtr(index);
	ASSERT(pModem);
	m_pAutoTestConfig->m_pTestModem = pModem;
	pModem->Connect();
	
	// Station to be tested
	m_pAutoTestConfig->m_pTestSDMS = NULL;
	index = m_StelaMCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CSdmsAPI *pSDMS = (CSdmsAPI *)m_StelaMCombo.GetItemDataPtr(index);
	ASSERT(pSDMS);
	m_pAutoTestConfig->m_pTestSDMS = pSDMS;
	if (pSDMS->GetControlType() != MC_SERIAL_PORT)
	{
		MessageBox("This test requires SDMS-card to be connected through SERIAL PORT, sorry.\nPlease reconnect that or choose another one.",
			"Error", MB_OK | MB_ICONEXCLAMATION);
		return -1; // to prevent going ahead
	}

	// Etalon modem
	index = m_EtalonModemCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CModem *pEtalonModem = (CModem *)m_EtalonModemCombo.GetItemDataPtr(index);
	ASSERT(pEtalonModem);
	m_pAutoTestConfig->m_pEtalonModem = pEtalonModem;
	pEtalonModem->Connect();
	
	// Etalon SDMS
	m_pAutoTestConfig->m_pEtalonSDMS = NULL;
	index = m_EtalonSdmsCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	pSDMS = (CSdmsAPI *)m_EtalonSdmsCombo.GetItemDataPtr(index);
	ASSERT(pSDMS);
	m_pAutoTestConfig->m_pEtalonSDMS = pSDMS;

	// Power meter
	m_pAutoTestConfig->m_pPowerMeter = NULL;
	index = m_PowerMeterCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CPowerMeter *pPowerMeter = (CPowerMeter *)m_PowerMeterCombo.GetItemDataPtr(index);
	ASSERT(pPowerMeter);
	m_pAutoTestConfig->m_pPowerMeter = pPowerMeter;
	if (!pPowerMeter->IsHuman())
		pPowerMeter->Connect();

	if (m_pAutoTestConfig->m_pEtalonModem == m_pAutoTestConfig->m_pTestModem ||
		m_pAutoTestConfig->m_pEtalonSDMS == m_pAutoTestConfig->m_pTestSDMS)
	{
		if (MessageBox("Please check devices to be tested and etalon ones.\nThey should be different. If they are equal you will not be able to perform some tests.\nDo you want to continue with that choice?",
			"Attention!", MB_YESNO | MB_ICONQUESTION) == IDNO)
				return -1; // to prevent going ahead
	}

	// Frequency counter
	m_pAutoTestConfig->m_pFrequencyCounter = NULL;
	index = m_FrequencyCounterCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CFrequencyCounter *pFrequencyCounter = (CFrequencyCounter *)m_FrequencyCounterCombo.GetItemDataPtr(index);
	ASSERT(pFrequencyCounter);
	m_pAutoTestConfig->m_pFrequencyCounter = pFrequencyCounter;
	if (!pFrequencyCounter->IsHuman())
		pFrequencyCounter->Connect();

	// Open file and write header
	CStdioFile *pFile = GetReportFile();
	if (pFile)
	{
		CString ReportHeaderStr = pModem->GetName();
		ReportHeaderStr += " + ";
		ReportHeaderStr += pSDMS->GetName();
		ReportHeaderStr += " investigation\n\n";

		WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(ReportHeaderStr));

		// Time
		CTime time = CTime::GetCurrentTime();
		CString string = time.Format("Date/Time: %d-%m-%Y %H:%M:%S\n");
		WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
	}

	return CAutoTestPropertyPage::OnWizardNext();
}

void CModemSdmsBatchTestsDevicePage::OnBrowseButton() 
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
// CModemSdmsBatchTestsDataThroughPage property page

IMPLEMENT_DYNCREATE(CModemSdmsBatchTestsDataThroughPage, CAutoTestPropertyPage)

CModemSdmsBatchTestsDataThroughPage::CModemSdmsBatchTestsDataThroughPage()
	: CAutoTestPropertyPage(CModemSdmsBatchTestsDataThroughPage::IDD)
{
	//{{AFX_DATA_INIT(CModemSdmsBatchTestsDataThroughPage)
	m_Protocol = _T("");
	m_EtalonEthernetIP = _T("");
	m_EtalonSerialIP = _T("");
	m_LocalHostIP = _T("");
	m_TestEthernetIP = _T("");
	m_TestSerialIP = _T("");
	//}}AFX_DATA_INIT
}

CModemSdmsBatchTestsDataThroughPage::~CModemSdmsBatchTestsDataThroughPage()
{
}

void CModemSdmsBatchTestsDataThroughPage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemSdmsBatchTestsDataThroughPage)
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_ETALON_ETHERNET_IP_EDIT, m_EtalonEthernetIP);
	DDX_Text(pDX, IDC_ETALON_SERIAL_IP_EDIT, m_EtalonSerialIP);
	DDX_Text(pDX, IDC_LOCAL_HOST_IP_EDIT, m_LocalHostIP);
	DDX_Text(pDX, IDC_TEST_ETHERNET_IP_EDIT, m_TestEthernetIP);
	DDX_Text(pDX, IDC_TEST_SERIAL_IP_EDIT, m_TestSerialIP);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemSdmsBatchTestsDataThroughPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CModemSdmsBatchTestsDataThroughPage)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsDataThroughPage message handlers

const char *pszSection = "ModemSdmsBatchTest";
const char *pszTestSerialIP = "TestSerialIPAddress";
const char *pszEtalonSerialIP = "EtalonSerialIPAddress";
const char *pszEtalonEthernetIP = "EtalonEthernetIPAddress";

BOOL CModemSdmsBatchTestsDataThroughPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CModemSdmsBatchTestsDataThroughPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	if (m_pAutoTestConfig->m_pEtalonModem == m_pAutoTestConfig->m_pTestModem ||
		m_pAutoTestConfig->m_pEtalonSDMS == m_pAutoTestConfig->m_pTestSDMS)
	{
		MessageBox("Data full loopback is skipped due to absent etalon device",
			"Attention!", MB_ICONEXCLAMATION);
		return 0; // skip this page
	}

	char szSdmsIpAddress[32], szSdmsIpSubnetMask[32];
	BOOL bSuccess = MiscGetSdmsEthernetIP(m_pAutoTestConfig->m_pTestSDMS, szSdmsIpAddress, szSdmsIpSubnetMask);
	if (!bSuccess)
		return TRUE;
	char szLocalHostIpAddress[32];
	bSuccess = MiscFindIpAddressForSDMS(szSdmsIpAddress, szSdmsIpSubnetMask, szLocalHostIpAddress);
	if (!bSuccess)
		return TRUE;

	m_LocalHostIP = szLocalHostIpAddress;
	m_TestEthernetIP = szSdmsIpAddress;
	m_TestSerialIP = AfxGetApp()->GetProfileString(pszSection, pszTestSerialIP, "");
	m_EtalonSerialIP = AfxGetApp()->GetProfileString(pszSection, pszEtalonSerialIP, "");
	m_EtalonEthernetIP = AfxGetApp()->GetProfileString(pszSection, pszEtalonEthernetIP, "");
	UpdateData(FALSE);

	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CModemSdmsBatchTestsDataThroughPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CAutoTestPropertyPage::OnWizardNext();
}

void CModemSdmsBatchTestsDataThroughPage::OnRunButton() 
{
	DoTest();
	UpdateData(FALSE);
}

void CModemSdmsBatchTestsDataThroughPage::DoTest()
{
	// Store all addresses
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileString(pszSection, pszTestSerialIP, m_TestSerialIP);
	AfxGetApp()->WriteProfileString(pszSection, pszEtalonSerialIP, m_EtalonSerialIP);
	AfxGetApp()->WriteProfileString(pszSection, pszEtalonEthernetIP, m_EtalonEthernetIP);

	CString str;
	// 0. Add route to etalon SDMS

	char szSdmsIpAddress[32], szSdmsIpSubnetMask[32];
	BOOL bSuccess = MiscGetSdmsEthernetIP(m_pAutoTestConfig->m_pTestSDMS, szSdmsIpAddress, szSdmsIpSubnetMask);
	if (!bSuccess)
		return;
	DWORD dwEtalonEthernet = inet_addr(m_EtalonEthernetIP);
	DWORD dwMask = inet_addr(szSdmsIpSubnetMask);
	DWORD dwRemoteNet = dwEtalonEthernet & dwMask;
	struct in_addr in;
	in.S_un.S_addr = dwRemoteNet;
	CString strRemoteNet = inet_ntoa(in);
	str.Format("route delete %s", strRemoteNet);
	::WinExec(str, SW_HIDE);
	Sleep(1000);
	str.Format("route add %s mask %s %s metric 1", strRemoteNet, szSdmsIpSubnetMask, m_TestEthernetIP);
	::WinExec(str, SW_HIDE);
	
	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 4);
	dlg.SetPos(0);
	m_Protocol = "Data full loopback test\r\n";
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return;

	CModem *pTestModem = m_pAutoTestConfig->m_pTestModem;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	CSdmsAPI *pTestSDMS = m_pAutoTestConfig->m_pTestSDMS;
	CSdmsAPI *pEtalonSDMS = m_pAutoTestConfig->m_pEtalonSDMS;

	// 1. Configure radio connection
	dlg.SetStatus("Setting radio connection parameters...");
	// 1.1 Check modem connections
	unsigned int Frequency;
	MC_ErrorCode EC = pTestModem->GetTFrequency(Frequency, 1);
	if (EC != MC_OK)
	{
		MessageBox("Cannot control modem to be tested!\nTesting skipped.", "Attention!", MB_ICONSTOP);
		return;
	}
	EC = pEtalonModem->GetTFrequency(Frequency, 1);
	if (EC != MC_OK)
	{
		MessageBox("Cannot control etalon modem!\nTesting skipped.", "Attention!", MB_ICONSTOP);
		return;
	}
	if (dlg.CheckCancelButton())
		return;
	// 1.2 Frequencies first
	Frequency = 1000000;
	pTestModem->SetRFrequency(Frequency, 1);
	pEtalonModem->SetTFrequency(Frequency, 1);
	Frequency = 1100000;
	pTestModem->SetTFrequency(Frequency, 1);
	pEtalonModem->SetRFrequency(Frequency, 1);
	if (dlg.CheckCancelButton())
		return;
	// 1.3 QPSK
	int ModulationType = 1; //QPSK
	pTestModem->SetTModulationType(ModulationType, 1);
	pEtalonModem->SetRModulationType(ModulationType, 1);
	pTestModem->SetRModulationType(ModulationType, 1);
	pEtalonModem->SetTModulationType(ModulationType, 1);
	if (dlg.CheckCancelButton())
		return;
	// 1.4 DataRate = 32 KBit/sec in one direction, 384 KBit/sec in another one
	unsigned int DataRate = 64000;
	pTestModem->SetTDataRate(DataRate, 1);
	pEtalonModem->SetRDataRate(DataRate, 1);
	DataRate = 384000;
	pTestModem->SetRDataRate(DataRate, 1);
	pEtalonModem->SetTDataRate(DataRate, 1);
	if (dlg.CheckCancelButton())
		return;
	// 1.5 Code rate = 1/2 in one direction and 3/4 in another one
	int FecMode = 1;
	pTestModem->SetTFecMode(FecMode, 1);
	pEtalonModem->SetRFecMode(FecMode, 1);
	FecMode = 2;
	pTestModem->SetRFecMode(FecMode, 1);
	pEtalonModem->SetTFecMode(FecMode, 1);
	if (dlg.CheckCancelButton())
		return;
	// 1.6 Turn both modulators on
	BOOL bOn = TRUE;
	pTestModem->TurnOutputOn(bOn, 1);
	pEtalonModem->TurnOutputOn(bOn, 1);
	if (dlg.CheckCancelButton())
		return;
	// 1.7 Radio report
	// 1.8 Adjust signals
	AdjustSignal(pEtalonModem, pTestModem, 7, 20, &dlg);
	if (dlg.CheckCancelButton())
		return;
	AdjustSignal(pTestModem, pEtalonModem, 7, 20, &dlg);
	if (dlg.CheckCancelButton())
		return;

	dlg.StepIt();
	UpdateData(FALSE);

	// 2. Configure serial interfaces
	// 2.1 Check SDMS connections
	EC = pTestSDMS->IsMCConnectionAlive();
	if (EC != MC_OK)
	{
		MessageBox("Cannot control SDMS to be tested!\nTesting skipped.", "Attention!", MB_ICONSTOP);
		return;
	}
	EC = pEtalonSDMS->IsMCConnectionAlive();
	if (EC != MC_OK)
	{
		MessageBox("Cannot control etalon SDMS!\nTesting skipped.", "Attention!", MB_ICONSTOP);
		return;
	}
	if (dlg.CheckCancelButton())
		return;
	dlg.SetStatus("Configuring serial interface of SDMS to be tested...");
	pTestSDMS->SetSerialIpAddress(m_TestSerialIP, szSdmsIpSubnetMask);
	if (dlg.CheckCancelButton())
		return;
	dlg.SetStatus("Configuring serial interface of etalon SDMS...");
	pEtalonSDMS->SetSerialIpAddress(m_EtalonSerialIP, szSdmsIpSubnetMask);
	if (dlg.CheckCancelButton())
		return;
	dlg.StepIt();
	UpdateData(FALSE);

	// 3. Configure etalon SDMS ethernet interface
	dlg.SetStatus("Configuring ethernet interface of etalon SDMS...");
	pEtalonSDMS->SetEthernetIpAddress(m_EtalonEthernetIP, szSdmsIpSubnetMask);

	// 3. Ping
	dlg.SetStatus("Waiting for SDMS routing set up itself (RIP etc.)...");
	CIcmpSocket ping;
	for (int i = 0; i<30; i++)
	{
		ping.SendEchoRequest(m_EtalonEthernetIP);
		ping.ReceiveEchoReply();
		if (!ping.IsTimedOut())
			break;
		if (dlg.CheckCancelButton())
			break;
		Sleep(1000);
	}
	dlg.SetStatus("Pinging remote etalon SDMS through radio channel...");
	for (i = 0; i<3; i++)
	{
		m_Protocol += "Ping echo reply: ";
		ping.SendEchoRequest(m_EtalonEthernetIP);
		ping.ReceiveEchoReply();
		if (ping.IsTimedOut())
			m_Protocol += "TIMEOUT!\r\n";
		else
		{
			CString str;
			str.Format("SUCCESS, time delay = %d msec\r\n", ping.GetDelayTime());
			m_Protocol += str;
		}
		UpdateData(FALSE);
		if (dlg.CheckCancelButton())
			break;
		Sleep(1000);
	}
	dlg.StepIt();

	// 4. Connecting to remote etalon SDMS
	// UNCOMMENT THIS!!!!!
/*	dlg.SetStatus("Connecting to etalon SDMS...");
	CSdmsVsat4900L TelnetEtalonSDMS;
	TelnetEtalonSDMS.SetControlType(MC_TELNET);
	if (!TelnetEtalonSDMS.Connect(m_EtalonEthernetIP))
	{
		MessageBox("Cannot connect to etalon SDMS by telnet!", "Attention!", MB_ICONSTOP);
		return;
	}

	dlg.SetStatus("Getting etalon SDMS status...");
	CDemIfStatus status;
	TelnetEtalonSDMS.GetRIfStatus(status, 1);
	m_Protocol += TelnetEtalonSDMS.GetMCReply();
	UpdateData(FALSE);*/
}

/////////////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsPropertySheet implementation

IMPLEMENT_DYNAMIC(CModemSdmsBatchTestsPropertySheet, CAutoTestSheet)

CModemSdmsBatchTestsPropertySheet::CModemSdmsBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	m_DevicePage.m_nDevices = nDevices;
	m_DevicePage.m_ppDevices = ppDevices;
	AddPage(&m_DevicePage);
	AddPage(&m_ModemPortPage);
	AddPage(&m_SdmsShellSerialPortPage);
	AddPage(&m_PowerSupplyModulator0VoltsPage);
	AddPage(&m_PowerSupplyModulator24VoltsPage);
	AddPage(&m_PowerSupplyDemodulator0VoltsPage);
	AddPage(&m_PowerSupplyDemodulator13VoltsPage);
	AddPage(&m_PowerSupplyDemodulator18VoltsPage);
	AddPage(&m_Modulator10MHzOnPage);
	AddPage(&m_Modulator10MHzOffPage);
	AddPage(&m_Demodulator10MHzOnPage);
	AddPage(&m_Demodulator10MHzOffPage);
	AddPage(&m_OutputLevelPage);
	AddPage(&m_OutputTurnOnOffPage);
	AddPage(&m_WideFreqRangePage);
	AddPage(&m_SecondaryLevelsPage);
	AddPage(&m_VariousOutputPage);
	AddPage(&m_SearchRangePage);
	AddPage(&m_CarrierAcquisitionPage);
	AddPage(&m_BerTestPage);
	AddPage(&m_SdmsPortsPage);
	AddPage(&m_EthernetPage);
	AddPage(&m_SoftwareVersionPage);
	AddPage(&m_SdmsShellTelnetPage);
	AddPage(&m_DataThroughPage);
	AddPage(&m_StartupConfigPage);
	AddPage(&m_CongratulationPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig = &m_AutoTestConfig;
}

CModemSdmsBatchTestsPropertySheet::~CModemSdmsBatchTestsPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CModemSdmsBatchTestsPropertySheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CModemSdmsBatchTestsPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsPropertySheet message handlers


