// StelaMBatchTests.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PowerMeter.h"
#include "FrequencyCounter.h"
#include "StelaMBatchTests.h"
#include "Modem.h"
#include "StelaM.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsDevicePage property page

IMPLEMENT_DYNCREATE(CVppuBatchTestsDevicePage, CAutoTestPropertyPage)

CVppuBatchTestsDevicePage::CVppuBatchTestsDevicePage()
	: CAutoTestPropertyPage(CVppuBatchTestsDevicePage::IDD)
{
	//{{AFX_DATA_INIT(CVppuBatchTestsDevicePage)
	m_FilePath = _T("");
	//}}AFX_DATA_INIT
}

CVppuBatchTestsDevicePage::~CVppuBatchTestsDevicePage()
{
}

void CVppuBatchTestsDevicePage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVppuBatchTestsDevicePage)
	DDX_Control(pDX, IDC_FREQUENCY_COUNTER_COMBO, m_FrequencyCounterCombo);
	DDX_Control(pDX, IDC_POWER_METER_COMBO, m_PowerMeterCombo);
	DDX_Control(pDX, IDC_SDMS_COMBO, m_SdmsCombo);
	DDX_Control(pDX, IDC_MODEM_COMBO, m_ModemCombo);
	DDX_Text(pDX, IDC_FILE_PATH_EDIT, m_FilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVppuBatchTestsDevicePage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CVppuBatchTestsDevicePage)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsDevicePage message handlers

BOOL CVppuBatchTestsDevicePage::OnInitDialog() 
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
			int index = m_ModemCombo.AddString(Name+")");
			m_ModemCombo.SetItemDataPtr(index, pDevice);
		}
		if (pDevice->IsStation())
		{
			// Station to be tested
			CString Name = pDevice->GetName();
			Name += " (";
			Name += pDevice->GetConnectionName();
			int index = m_SdmsCombo.AddString(Name + ")");
			m_SdmsCombo.SetItemDataPtr(index, pDevice);
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
	if (m_SdmsCombo.GetCount() == 1)
		m_SdmsCombo.SetCurSel(0);
	if (m_ModemCombo.GetCount() == 1)
		m_ModemCombo.SetCurSel(0);
	if (m_PowerMeterCombo.GetCount() == 1)
		m_PowerMeterCombo.SetCurSel(0);
	if (m_FrequencyCounterCombo.GetCount() == 1)
		m_FrequencyCounterCombo.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CVppuBatchTestsDevicePage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CVppuBatchTestsDevicePage::OnKillActive() 
{
	return CAutoTestPropertyPage::OnKillActive();
}

LRESULT CVppuBatchTestsDevicePage::OnWizardNext() 
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
	m_pAutoTestConfig->m_pTestModem = m_pAutoTestConfig->m_pEtalonModem = pModem;
	pModem->Connect();
	
	// Tranceiver to be tested
	m_pAutoTestConfig->m_pTestSDMS = NULL;
	index = m_SdmsCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CSdmsAPI *pSDMS = (CSdmsAPI *)m_SdmsCombo.GetItemDataPtr(index);
	ASSERT(pSDMS);
	m_pAutoTestConfig->m_pTestSDMS = pSDMS;

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
		char *pszStr="VSAT StelaM investigation\n\n";
		WriteString(m_pAutoTestConfig->m_pReportFile, pszStr);

		// Time
		CTime time = CTime::GetCurrentTime();
		CString string = time.Format("Date/Time: %d-%m-%Y %H:%M:%S\n");
		WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
	}

	return CAutoTestPropertyPage::OnWizardNext();
}

void CVppuBatchTestsDevicePage::OnBrowseButton() 
{
	UpdateData();
	CFileDialog fd(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Text files(*txt)|*txt|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Specify file to write report";
	if (fd.DoModal() == IDCANCEL) return;
	m_FilePath = fd.GetPathName();
	UpdateData(FALSE);
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));
}


/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsRadioPage property page

IMPLEMENT_DYNCREATE(CVppuBatchTestsRadioPage, CAutoTestPropertyPage)

CVppuBatchTestsRadioPage::CVppuBatchTestsRadioPage()
	: CAutoTestPropertyPage(CVppuBatchTestsRadioPage::IDD)
{
	//{{AFX_DATA_INIT(CVppuBatchTestsRadioPage)
	m_Protocol = _T("");
	//}}AFX_DATA_INIT
}

CVppuBatchTestsRadioPage::~CVppuBatchTestsRadioPage()
{
}

void CVppuBatchTestsRadioPage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVppuBatchTestsRadioPage)
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVppuBatchTestsRadioPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CVppuBatchTestsRadioPage)
	ON_BN_CLICKED(IDC_REPEAT_BUTTON, OnRepeatButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsRadioPage message handlers

BOOL CVppuBatchTestsRadioPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CVppuBatchTestsRadioPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	MC_ErrorCode EC;
	CString str;
	
	m_Protocol = "Radio connections testing...\r\n\r\nTest parameters:\r\nParameter\tModulator\tDemodulator\r\n";
	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 10);
	dlg.SetPos(0);

	// Frequency
	m_Protocol += "Frequency\t";
	unsigned int freq = 1000000;
	EC = pModem->SetTFrequency(freq, 1);
	if (EC == MC_OK)
	{
		str.Format("%d KHz\t", freq);
		m_Protocol += str;
	}
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	freq = 1000000;
	EC = pModem->SetRFrequency(freq, 1);
	if (EC == MC_OK)
	{
		str.Format("%d KHz\r\n", freq);
		m_Protocol += str;
	}
	dlg.StepIt();
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	// Modulation
	m_Protocol += "Modulation\t";
	int ModulationType = 1; // QPSK
	EC = pModem->SetTModulationType(ModulationType, 1);
	if (EC == MC_OK)
	{
		str = pModem->GetTModulationTypeName(ModulationType);
		str += "\t\t";
		m_Protocol += str;
	}
	dlg.StepIt();
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	EC = pModem->SetRModulationType(ModulationType, 1);
	if (EC == MC_OK)
	{
		str = pModem->GetRModulationTypeName(ModulationType);
		str += "\t\t";
		m_Protocol += str;
	}
	dlg.StepIt();
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	// Data rate
	m_Protocol += "Data rate\t";
	unsigned int rate = 64000;
	EC = pModem->SetTDataRate(rate, 1);
	if (EC == MC_OK)
	{
		str.Format("%d baud\t", rate);
		m_Protocol += str;
	}
	dlg.StepIt();
	EC = pModem->SetRDataRate(rate, 1);
	if (EC == MC_OK)
	{
		str.Format("%d baud\r\n", rate);
		m_Protocol += str;
	}
	dlg.StepIt();
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	// Code rate
	m_Protocol += "FEC mode\t";
	int FecMode = 1;
	EC = pModem->SetTFecMode(FecMode, 1);
	if (EC == MC_OK)
	{
		m_Protocol += pModem->GetTFecModeName(FecMode);
		m_Protocol += "\t\t";
	}
	dlg.StepIt();

	EC = pModem->SetRFecMode(FecMode, 1);
	if (EC == MC_OK)
	{
		m_Protocol += pModem->GetRFecModeName(FecMode);
		m_Protocol += "\r\n";
	}
	dlg.SetPos(8);
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	// Turn modulator ON
	BOOL bOn = TRUE;
	pModem->TurnOutputOn(bOn, 1);
	m_Protocol += "Modulator output is ";
	if (bOn)
		m_Protocol += "ON\r\n";
	else
		m_Protocol += "OFF\r\n";
	dlg.StepIt();
	if (dlg.CheckCancelButton())
		return CAutoTestPropertyPage::OnSetActive();

	// Demodulator search range
	unsigned int range = 20000;
	EC = pModem->SetSearchRange(range, 1);
	if (EC == MC_OK)
	{
		str.Format("Demodulator search range = %d Hz\r\n", range);
		m_Protocol += str;
	}

	UpdateData(FALSE);
	return CAutoTestPropertyPage::OnSetActive();
}

LRESULT CVppuBatchTestsRadioPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CAutoTestPropertyPage::OnWizardNext();
}

void CVppuBatchTestsRadioPage::DoTest()
{
	m_Protocol += "\r\nTransmit -> receive loopback test...\r\nOutput level, dBm\tInput level, dBm\tS/N, dB\t\tOffset, Hz\r\n";
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CString str;

	double real_level, desired_level = pModem->GetMinOutputLevel();
	double SN, InputLevel;
	int Offset;
	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, (int)(pModem->GetMaxOutputLevel()-pModem->GetMinOutputLevel()));
	while (desired_level <= pModem->GetMaxOutputLevel())
	{
		real_level = desired_level;
		MC_ErrorCode EC = pModem->SetOutputLevel(real_level, 1);
		if (EC != MC_OK)
		{
			m_Protocol += "Cannot set modulator output level! Test canceled.\r\n";
			break;
		}
		Sleep(1000);
		// S/N
		pModem->GetEbNo(SN, 1);
		// Input Level
		pModem->GetInputLevel(InputLevel, 1);
		// Offset
		pModem->GetDemodulatorOffset(Offset, 1);
		str.Format("    %4.1f\t\t   %4.1f\t\t  %4.1f\t\t%+4d\r\n", real_level, InputLevel, SN, Offset);
		m_Protocol += str;
		dlg.SetStatus(str);
		if (dlg.CheckCancelButton())
			break;
		desired_level += 1;
		dlg.StepIt();
	}

	UpdateData(FALSE);
}

void CVppuBatchTestsRadioPage::OnRepeatButton() 
{
	DoTest();	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsPropertySheet implementation

IMPLEMENT_DYNAMIC(CVppuBatchTestsPropertySheet, CAutoTestSheet)

CVppuBatchTestsPropertySheet::CVppuBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	AddPage(&m_DevicePage);
	m_DevicePage.m_nDevices=nDevices;
	m_DevicePage.m_ppDevices=ppDevices;
	AddPage(&m_ModemPortPage);
	AddPage(&m_SdmsPortsPage);
	AddPage(&m_PowerSupplyModulator24VoltsPage);
	AddPage(&m_PowerSupplyDemodulator13VoltsPage);
	AddPage(&m_PowerSupplyDemodulator18VoltsPage);
	AddPage(&m_Modulator10MHzOnPage);
	AddPage(&m_Demodulator10MHzOnPage);
	AddPage(&m_OutputTurnOnOffPage);
	AddPage(&m_RadioPage);
	AddPage(&m_CarrierAcquisitionPage);
	AddPage(&m_BerTestPage);
	AddPage(&m_EthernetPage);
	AddPage(&m_SoftwareVersionPage);
	AddPage(&m_SdmsShellSerialPortPage);
	AddPage(&m_SdmsShellTelnetPage);
	AddPage(&m_StartupConfigPage);
	AddPage(&m_CongratulationPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig=&m_AutoTestConfig;
}

CVppuBatchTestsPropertySheet::~CVppuBatchTestsPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CVppuBatchTestsPropertySheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CVppuBatchTestsPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsPropertySheet message handlers


