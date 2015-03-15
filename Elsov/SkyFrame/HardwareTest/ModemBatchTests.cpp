// ModemBatchTests.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ModemBatchTests.h"
#include "Modem.h"
#include "PowerMeter.h"
#include "FrequencyCounter.h"
#include "ProgressDlg.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char pszTmpBuffer[1024];

const char *pszCorrection = "Power_Correction";

BOOL InputToOutputConnectionWarning(const char *pszTestDesc, CWnd *pParentWnd)
{
	CString caption = "Please check all the connections for ";
	caption += pszTestDesc;
	return pParentWnd->MessageBox("Demodulator jack of modem MUST be connected to signal source", caption, MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK;
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsPage, CAutoTestPropertyPage)

CModemBatchTestsPage::CModemBatchTestsPage(int nIDD)
	: CAutoTestPropertyPage(nIDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsPage)
	//}}AFX_DATA_INIT
}

CModemBatchTestsPage::~CModemBatchTestsPage()
{
}

void CModemBatchTestsPage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CModemBatchTestsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsDevicePage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsDevicePage, CModemBatchTestsPage)

CModemBatchTestsDevicePage::CModemBatchTestsDevicePage()
	: CModemBatchTestsPage(CModemBatchTestsDevicePage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsDevicePage)
	m_FilePath = _T("");
	//}}AFX_DATA_INIT
}

CModemBatchTestsDevicePage::~CModemBatchTestsDevicePage()
{
}

void CModemBatchTestsDevicePage::DoDataExchange(CDataExchange* pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsDevicePage)
	DDX_Control(pDX, IDC_FREQUENCY_COUNTER_COMBO, m_FrequencyCounterCombo);
	DDX_Control(pDX, IDC_POWER_METER_COMBO, m_PowerMeterCombo);
	DDX_Control(pDX, IDC_ETALON_MODEM_COMBO, m_EtalonModemCombo);
	DDX_Control(pDX, IDC_DEVICE_COMBO, m_DeviceCombo);
	DDX_Text(pDX, IDC_FILE_PATH_EDIT, m_FilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsDevicePage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsDevicePage)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsDevicePage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();
	for (int i = 0; i < m_nDevices; i++)
	{
		CDevice *pDevice = m_ppDevices[i];
		if (pDevice == NULL)
			continue;
		if (pDevice->IsModem() && !pDevice->IsStation())
		{
			CModem *pModem = (CModem *)pDevice;
			// Device to be tested
			CString Name=pDevice->GetName();
			Name+=" (";
			Name+=pDevice->GetConnectionName();
			int index=m_DeviceCombo.AddString(Name+")");
			m_DeviceCombo.SetItemDataPtr(index, pDevice);

			// Etalon modem (must have modulators)
			if (pModem->GetModulatorsNumber())
			{
				Name=pDevice->GetName();
				Name+=" (";
				Name+=pDevice->GetConnectionName();
				int index=m_EtalonModemCombo.AddString(Name+")");
				m_EtalonModemCombo.SetItemDataPtr(index, pDevice);
			}
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
				int index=m_FrequencyCounterCombo.AddString(Name + ")");
				m_FrequencyCounterCombo.SetItemDataPtr(index, pDevice);
			}
		}
	}
	if (m_DeviceCombo.GetCount() == 1)
		m_DeviceCombo.SetCurSel(0);
	if (m_EtalonModemCombo.GetCount() == 1)
		m_EtalonModemCombo.SetCurSel(0);
	if (m_PowerMeterCombo.GetCount() == 1)
		m_PowerMeterCombo.SetCurSel(0);
	if (m_FrequencyCounterCombo.GetCount() == 1)
		m_FrequencyCounterCombo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CModemBatchTestsDevicePage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CModemBatchTestsPage::OnSetActive();
}

LRESULT CModemBatchTestsDevicePage::OnWizardNext() 
{
	CWaitCursor cursor;
	UpdateData(FALSE);
	if (m_FilePath.IsEmpty())
		return -1;	// to prevent going ahead
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));

	// Device to be tested
	int index = m_DeviceCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CModem *pModem = (CModem *)m_DeviceCombo.GetItemDataPtr(index);
	ASSERT(pModem);
	m_pAutoTestConfig->m_pTestModem = pModem;
	pModem->Connect();
	
	// Etalon device
	m_pAutoTestConfig->m_pEtalonModem = NULL;
	index = m_EtalonModemCombo.GetCurSel();
	if (index == CB_ERR)
		return -1;	// to prevent going ahead
	CModem *pEtalonModem = (CModem *)m_EtalonModemCombo.GetItemDataPtr(index);
	ASSERT(pEtalonModem);
	m_pAutoTestConfig->m_pEtalonModem = pModem;
	if (pEtalonModem != pModem)
		pEtalonModem->Connect();

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

	// Human meters
	for (int i=0; i<m_nDevices; i++)
	{
		CDevice *pDevice=m_ppDevices[i];
		if (pDevice == NULL)
			continue;
		if (!pDevice->IsHuman())
			continue;
		if (pDevice->IsMeasureDevice())
		{
			CMeasureDevice *pMeasureDevice = (CMeasureDevice *)pDevice;
			if (pMeasureDevice->IsPowerMeter())
			{
				m_pAutoTestConfig->m_pHumanPowerMeter = (CPowerMeter *)pDevice;
			}
			else if (pMeasureDevice->IsFrequencyCounter())
			{
				m_pAutoTestConfig->m_pHumanFrequencyCounter = (CFrequencyCounter *)pDevice;
			}
		}
	}


	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
	}
	if (m_pAutoTestConfig->m_pReportFile->m_pStream != NULL)
	{
		CString ReportHeaderStr = pModem->GetName();
		ReportHeaderStr += " investigation\n\n";

		WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(ReportHeaderStr));


		// Time
		CTime time=CTime::GetCurrentTime();
		CString string=time.Format("Date/Time: %d-%m-%Y %H:%M:%S\n");
		WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
	}

	// Turn all 10 MHz suppliers OFF
	BOOL bOn = FALSE;
	pModem->EnableR10MHzSupplier(bOn, 1);
	bOn = FALSE;
	pModem->EnableT10MHzSupplier(bOn, 1);
	bOn = FALSE;
	pEtalonModem->EnableR10MHzSupplier(bOn, 1);
	bOn = FALSE;
	pEtalonModem->EnableT10MHzSupplier(bOn, 1);

	// Turn all power suppliers off
	int PowerSupplyMode = 0;
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	PowerSupplyMode = 0;
	pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);
	PowerSupplyMode = 0;
	pEtalonModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	PowerSupplyMode = 0;
	pEtalonModem->SetTPowerSupplyMode(PowerSupplyMode, 1);

	// Turn continious wave mode off
	BOOL bContiniousWave = FALSE;
	pModem->TurnContiniousWaveOn(bContiniousWave, 1);
	bContiniousWave = FALSE;
	pEtalonModem->TurnContiniousWaveOn(bContiniousWave, 1);

	return CModemBatchTestsPage::OnWizardNext();
}

void CModemBatchTestsDevicePage::OnBrowseButton() 
{
	UpdateData();
	CFileDialog fd(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Text files(*txt)|*txt|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Specify file to write results";
	if (fd.DoModal()==IDCANCEL) return;
	m_FilePath=fd.GetPathName();
	UpdateData(FALSE);
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPortPage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsPortPage, CModemBatchTestsPage)

CModemBatchTestsPortPage::CModemBatchTestsPortPage()
	: CModemBatchTestsPage(CModemBatchTestsPortPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsPortPage)
	m_ConnectionStatus = _T("");
	m_Protocol = _T("");
	//}}AFX_DATA_INIT
}

CModemBatchTestsPortPage::~CModemBatchTestsPortPage()
{
}

void CModemBatchTestsPortPage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsPortPage)
	DDX_Text(pDX, IDC_CONNECTION_STATUS_STATIC, m_ConnectionStatus);
	DDX_Text(pDX, IDC_PORT_TEST_PROTOCOL_EDIT, m_Protocol);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsPortPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsPortPage)
	ON_BN_CLICKED(IDC_REPEAT_BUTTON, OnRepeatButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsPortPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	m_ConnectionStatus = m_pAutoTestConfig->m_pTestModem->GetConnectionStatus();
	UpdateData(FALSE);
	
	return CModemBatchTestsPage::OnSetActive();
}

LRESULT CModemBatchTestsPortPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CModemBatchTestsPage::OnWizardNext();
}

void CModemBatchTestsPortPage::OnRepeatButton() 
{
	CWaitCursor cursor;
	char *pszTestStatus = "\r\nModem M&C port testing...\r\n";;
	m_Protocol = pszTestStatus;
	m_Protocol += "\r\n";
	char *pszString;
	MC_ErrorCode EC = m_pAutoTestConfig->m_pTestModem->GetDeviceVersionString(pszString);
	m_Protocol += pszString;
	if (EC == MC_OK)
		m_Protocol += "Test passed\r\n";
	else
		m_Protocol += "Test FAILED !!!\r\n";
	
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsWideFreqRangePage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsWideFreqRangePage, CModemBatchTestsPage)

CModemBatchTestsWideFreqRangePage::CModemBatchTestsWideFreqRangePage()
	: CModemBatchTestsPage(CModemBatchTestsWideFreqRangePage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsWideFreqRangePage)
	m_HoldTime = 0;
	m_bHold = FALSE;
	m_bMute = FALSE;
	m_MuteTime = 0;
	m_EndFreq = 0;
	m_StartFreq = 0;
	m_Step = 0;
	//}}AFX_DATA_INIT
}

CModemBatchTestsWideFreqRangePage::~CModemBatchTestsWideFreqRangePage()
{
}

void CModemBatchTestsWideFreqRangePage::DoDataExchange(CDataExchange* pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsWideFreqRangePage)
	DDX_Control(pDX, IDC_MUTE_TIME_EDIT, m_MuteTimeCtrl);
	DDX_Control(pDX, IDC_WAIT_TIME_EDIT, m_HoldTimeCtrl);
	DDX_Control(pDX, IDC_PROBLEM_FREQUENCIES_LISTBOX, m_ProblemFreqsListBox);
	DDX_Text(pDX, IDC_WAIT_TIME_EDIT, m_HoldTime);
	DDV_MinMaxUInt(pDX, m_HoldTime, 0, 5000);
	DDX_Check(pDX, IDC_WAIT_CHECK, m_bHold);
	DDX_Check(pDX, IDC_MUTE_OUTPUT_CHECK, m_bMute);
	DDX_Text(pDX, IDC_MUTE_TIME_EDIT, m_MuteTime);
	DDV_MinMaxUInt(pDX, m_MuteTime, 0, 5000);
	DDX_Text(pDX, IDC_END_FREQ_EDIT, m_EndFreq);
	DDX_Text(pDX, IDC_START_FREQ_EDIT, m_StartFreq);
	DDX_Text(pDX, IDC_STEP_EDIT, m_Step);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsWideFreqRangePage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsWideFreqRangePage)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	ON_BN_CLICKED(IDC_WAIT_CHECK, OnWaitCheck)
	ON_BN_CLICKED(IDC_MUTE_OUTPUT_CHECK, OnMuteOutputCheck)
	ON_BN_CLICKED(IDC_RESTORE_DEFAULTS_BUTTON, OnRestoreDefaultsButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CModemBatchTestsWideFreqRangePage::SaveAllParameters()
{
	UpdateData();
	WriteRegistryInt("StartFreqForWideFreqTest", m_StartFreq);
	WriteRegistryInt("EndFreqForWideFreqTest", m_EndFreq);
	WriteRegistryInt("FreqStepForWideFreqTest", m_Step);
	WriteRegistryInt("MuteTimeForWideFreqTest", m_MuteTime);
	WriteRegistryInt("MutingForWideFreqEnabled", m_bMute);
	WriteRegistryInt("HoldTimeForWideFreqTest", m_HoldTime);
	WriteRegistryInt("HoldingForWideFreqEnabled", m_bHold);
}

BOOL CModemBatchTestsWideFreqRangePage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bOn = TRUE;
	pModem->TurnContiniousWaveOn(bOn, 1);
	m_StartFreq = ReadRegistryInt("StartFreqForWideFreqTest", pModem->GetMinTFrequency()/1000);
	m_EndFreq = ReadRegistryInt("EndFreqForWideFreqTest", pModem->GetMaxTFrequency()/1000);
	m_Step = ReadRegistryInt("FreqStepForWideFreqTest", 100);
	m_HoldTime = ReadRegistryInt("HoldTimeForWideFreqTest", 250);
	m_bHold = ReadRegistryInt("HoldingForWideFreqEnabled", 1);
	m_MuteTime = ReadRegistryInt("MuteTimeForWideFreqTest", 250);
	m_bMute = ReadRegistryInt("MutingForWideFreqEnabled", 1);
	UpdateData(FALSE);
	OnMuteOutputCheck();
	OnWaitCheck();
	
	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnSetActive();
		}
	}
	char *pszStr="Modulator output investigation (frequency range)\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, pszStr);
	
	return CModemBatchTestsPage::OnSetActive();
}


BOOL CModemBatchTestsWideFreqRangePage::OnKillActive() 
{
	SaveAllParameters();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bOn = FALSE;
	pModem->TurnContiniousWaveOn(bOn, 1);
	return CModemBatchTestsPage::OnKillActive();
}

void CModemBatchTestsWideFreqRangePage::OnWaitCheck() 
{
	UpdateData();
	m_HoldTimeCtrl.EnableWindow(m_bHold);
}

void CModemBatchTestsWideFreqRangePage::OnMuteOutputCheck() 
{
	UpdateData();
	m_MuteTimeCtrl.EnableWindow(m_bMute);
}


void CModemBatchTestsWideFreqRangePage::OnRestoreDefaultsButton() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	m_bMute = TRUE;
	m_bHold = TRUE;
	m_MuteTime = 250;
	m_HoldTime = 250;
	m_StartFreq = pModem->GetMinTFrequency()/1000;
	m_EndFreq = pModem->GetMaxTFrequency()/1000;
	m_Step = 100;
	UpdateData(FALSE);
	OnMuteOutputCheck();
	OnWaitCheck();
}
LRESULT CModemBatchTestsWideFreqRangePage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

BOOL CModemBatchTestsWideFreqRangePage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModemBatchTestsWideFreqRangePage::OnRunButton() 
{
	SaveAllParameters();

	// Time
	CTime CurrentTime = CTime::GetCurrentTime();
	CString string = CurrentTime.Format("Started at: %d-%m-%Y %H:%M:%S\n");
	WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));

	const int modulator = 1;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;

	m_ProblemFreqsListBox.ResetContent();
	CProgressDlg dlg;
	dlg.Create();
	int nMeasurements = (m_EndFreq - m_StartFreq)*1000/m_Step;
	dlg.SetRange(0, nMeasurements);
	
	BOOL bOn = TRUE;
	pModem->TurnContiniousWaveOn(bOn, 1);
	
	// Set level
	double Level = (pModem->GetMinOutputLevel()+pModem->GetMaxOutputLevel())/2.;
	pModem->SetOutputLevel(Level, modulator);
	// QPSK
	int ModulationType = 1; // QPSK
	pModem->SetTModulationType(ModulationType, modulator);
	// 64000 baud
	unsigned int DataRate = 64000;
	pModem->SetTDataRate(DataRate, modulator);
	
	// Start tests
	CString str;
	unsigned int real_freq = m_StartFreq*1000;

	for (unsigned int freq = m_StartFreq*1000; freq <= m_EndFreq*1000; freq += m_Step)
	{
		real_freq = freq;
		str.Format("Frequency = %d KHz", freq);
		dlg.SetStatus(str);
		bOn = FALSE;
		if (m_bMute)
			pModem->TurnOutputOn(bOn, modulator);
		pModem->SetTFrequency(real_freq, modulator);
		bOn = TRUE;
		if (m_bMute)
		{
			Sleep(m_MuteTime);
			pModem->TurnOutputOn(bOn, modulator);
		}
		
		CFrequencyCounter *pFrequencyCounter = m_pAutoTestConfig->m_pFrequencyCounter;
		double MeasuredFreq = real_freq;
		BOOL bFreqMeasurementOK = FALSE;
		if (pFrequencyCounter->IsHuman())
			bFreqMeasurementOK = TRUE;
		else
		{
			Sleep(2000);
			pFrequencyCounter->MeasureFrequency(MeasuredFreq, 1);
			MeasuredFreq /= 1000; // Hz to KHz
			if (fabs(MeasuredFreq - freq) < 2.)
				bFreqMeasurementOK = TRUE;			
			str.Format("Frequency = %ld KHz, measured value %ld KHz", freq, (unsigned int)(MeasuredFreq+0.4999));
			dlg.SetStatus(str);
			Sleep(500);
		}

		if (real_freq != freq || !bFreqMeasurementOK)
		{ // problem!
			str.Format("Desired:%ld   Set:%ld   Measured:%ld", freq, real_freq, (unsigned int)(MeasuredFreq+0.4999));
			m_ProblemFreqsListBox.AddString(str);
		}
		dlg.StepIt();
		if (dlg.CheckCancelButton())
		{
			WriteString(m_pAutoTestConfig->m_pReportFile, "Canceled\n");
			break;
		}
		if (m_bHold)
			Sleep(m_HoldTime);
	}
	// Time
	CurrentTime = CTime::GetCurrentTime();
	string = CurrentTime.Format("Ended at: %d-%m-%Y %H:%M:%S\n");
	WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));

	// Freqs
	string.Format("Tested frequencies from %d KHz to %d KHz (step %d KHz)\n",
		m_StartFreq*1000, real_freq, m_Step);
	WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
	WriteString(m_pAutoTestConfig->m_pReportFile, "Problem frequencies:\n");
	if (m_ProblemFreqsListBox.GetCount())
	{ 
		for (int index = 0; index<m_ProblemFreqsListBox.GetCount(); index++)
		{
			m_ProblemFreqsListBox.GetText(index, string);
			string += "KHz\n";
			WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
		}
	}
	else
		WriteString(m_pAutoTestConfig->m_pReportFile, "none\n");
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsOutputTurnOnOffPage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsOutputTurnOnOffPage, CModemBatchTestsPage)

CModemBatchTestsOutputTurnOnOffPage::CModemBatchTestsOutputTurnOnOffPage()
	: CModemBatchTestsPage(CModemBatchTestsOutputTurnOnOffPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsOutputTurnOnOffPage)
	//}}AFX_DATA_INIT
}

CModemBatchTestsOutputTurnOnOffPage::~CModemBatchTestsOutputTurnOnOffPage()
{
}

void CModemBatchTestsOutputTurnOnOffPage::DoDataExchange(CDataExchange* pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsOutputTurnOnOffPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsOutputTurnOnOffPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsOutputTurnOnOffPage)
	ON_BN_CLICKED(IDC_TURN_ON_OFF_BUTTON, OnTurnOnOffButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsOutputTurnOnOffPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnSetActive();
		}
	}
	char *pszHeader = "\nTurning output on/off\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, pszHeader);

	// Turn tested modulator OFF
	BOOL bOn = FALSE;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->TurnOutputOn(bOn, 1);
	if (bOn)
		GetDescendantWindow(IDC_TURN_ON_OFF_BUTTON)->SetWindowText("Turn off");
	else
		GetDescendantWindow(IDC_TURN_ON_OFF_BUTTON)->SetWindowText("Turn on");

	// Turn ContinuousWave mode ON
	bOn = TRUE;
	pModem->TurnContiniousWaveOn(bOn, 1);

	unsigned int freq = (pModem->GetMinTFrequency() + pModem->GetMaxTFrequency())/2;
	pModem->SetTFrequency(freq, 1);
	unsigned int DataRate = 64000;
	pModem->SetTDataRate(DataRate, 1);
	UpdateData(FALSE);

	// Turn etalon modulator OFF
	bOn = FALSE;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	pEtalonModem->TurnOutputOn(bOn, 1);
	
	return CModemBatchTestsPage::OnSetActive();
}

void CModemBatchTestsOutputTurnOnOffPage::OnTurnOnOffButton() 
{
	const int modulator = 1;
	BOOL bOn = FALSE;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bContinuousWaveOn = TRUE;
	pModem->TurnContiniousWaveOn(bContinuousWaveOn, modulator);

	pModem->IsOutputOn(bOn, modulator);
	bOn = !bOn;
	if (bOn)
		WriteString(m_pAutoTestConfig->m_pReportFile, "\tTurn output on");
	else
		WriteString(m_pAutoTestConfig->m_pReportFile, "\tTurn output off");

	pModem->TurnOutputOn(bOn, modulator);
	if (bOn)
		GetDescendantWindow(IDC_TURN_ON_OFF_BUTTON)->SetWindowText("Turn off");
	else
		GetDescendantWindow(IDC_TURN_ON_OFF_BUTTON)->SetWindowText("Turn on");
	BOOL bSignalOn = (MessageBox("Do you see signal on spectrum analyzer?",
		"Output on/off", MB_ICONQUESTION | MB_YESNO) == IDYES);
	if (bOn == bSignalOn)
		WriteString(m_pAutoTestConfig->m_pReportFile, "\tPASSED\n");
	else
		WriteString(m_pAutoTestConfig->m_pReportFile, "\tFAILED!!!\n");
}

LRESULT CModemBatchTestsOutputTurnOnOffPage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

BOOL CModemBatchTestsOutputTurnOnOffPage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTests10MHzPage property page

IMPLEMENT_DYNCREATE(CModemBatchTests10MHzPage, CModemBatchTestsPage)

CModemBatchTests10MHzPage::CModemBatchTests10MHzPage()
	: CModemBatchTestsPage(CModemBatchTests10MHzPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTests10MHzPage)
	m_10MhzLevel = 0.0;
	m_DeviceName = _T("");
	m_Correction = 0.0;
	m_Frequency = 0.0;
	//}}AFX_DATA_INIT
}

CModemBatchTests10MHzPage::~CModemBatchTests10MHzPage()
{
}

void CModemBatchTests10MHzPage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTests10MHzPage)
	DDX_Text(pDX, IDC_10MHZ_EDIT, m_10MhzLevel);
	DDX_Text(pDX, IDC_DEVICE_STATIC, m_DeviceName);
	DDX_Text(pDX, IDC_CORRECTION_EDIT, m_Correction);
	DDX_Text(pDX, IDC_MEASURED_FREQUENCY_EDIT, m_Frequency);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTests10MHzPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTests10MHzPage)
	ON_BN_CLICKED(IDC_CHANGE_SHIFT_BUTTON, OnChangeShiftButton)
	ON_BN_CLICKED(IDC_MEASURE_BUTTON, OnMeasureButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTests10MHzPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	// Turn tested modulator OFF
	BOOL bOn = FALSE;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->TurnOutputOn(bOn, 1);
	// Turn etalon modulator OFF
	bOn = FALSE;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	pEtalonModem->TurnOutputOn(bOn, 1);

	CPowerMeter *pPowerMeter = m_pAutoTestConfig->m_pPowerMeter;
	pPowerMeter->ResetMeasurements();

	int iValue = ReadRegistryInt(pszCorrection, 0*10);
	m_Correction = iValue/10.;

	bOn = NeedT10MHz();
	pModem->EnableT10MHzSupplier(bOn, 1);
	bOn = NeedR10MHz();
	pModem->EnableR10MHzSupplier(bOn, 1);

	CString Caption = GetDeviceDescription();
	m_DeviceName = Caption;
	Caption += " 10 MHz ";
	Caption += GetRegimeDescription();
	Caption += " test";
	pSheet->SetWindowText(Caption);
	UpdateData(FALSE);

	unsigned int style = SW_SHOW;
	if (strcmp(GetRegimeDescription(), "ON"))
		style = SW_HIDE;
	GetDescendantWindow(IDC_MEASURED_FREQUENCY_STATIC)->ShowWindow(style);
	GetDescendantWindow(IDC_MEASURED_FREQUENCY_HZ_STATIC)->ShowWindow(style);
	GetDescendantWindow(IDC_MEASURED_FREQUENCY_EDIT)->ShowWindow(style);
	
	return CModemBatchTestsPage::OnSetActive();
}

void CModemBatchTests10MHzPage::OnMeasureButton() 
{
	if (!strcmp(GetRegimeDescription(), "ON"))
	{ // measure frequency
		m_pAutoTestConfig->m_pFrequencyCounter->MeasureFrequency(m_Frequency, 1);
	}

	m_pAutoTestConfig->m_pPowerMeter->ResetMeasurements();
	m_pAutoTestConfig->m_pPowerMeter->MeasurePower(m_10MhzLevel, 1);
	m_10MhzLevel = (m_10MhzLevel*100)/100.;
	UpdateData(FALSE);
}

BOOL CModemBatchTests10MHzPage::OnKillActive() 
{
	CWaitCursor cursor;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	// Turn all 10 MHz supplyers off	
	BOOL bOn = FALSE;
	pModem->EnableT10MHzSupplier(bOn, 1);
	bOn = FALSE;
	pModem->EnableR10MHzSupplier(bOn, 1);

	return CModemBatchTestsPage::OnKillActive();
}

LRESULT CModemBatchTests10MHzPage::OnWizardNext() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTests10MHzPage::OnWizardNext();
		}
	}
	CString strReportEntry;
	strReportEntry.Format("\n%s 10 MHz turn %s test\n", GetDeviceDescription(), GetRegimeDescription());
	WriteString(m_pAutoTestConfig->m_pReportFile, strReportEntry);
	if (!strcmp(GetRegimeDescription(), "ON"))
	{
		WriteString(m_pAutoTestConfig->m_pReportFile, "Measured signal frequency = ");
		strReportEntry.Format("%d Hz (%+d)\n", (unsigned int)m_Frequency, (unsigned int)(m_Frequency - 10000000));
		WriteString(m_pAutoTestConfig->m_pReportFile, strReportEntry);
	}

	WriteString(m_pAutoTestConfig->m_pReportFile, "Measured signal level = ");
	strReportEntry.Format("%7.2f dBm\n\n", m_10MhzLevel+m_Correction);
	WriteString(m_pAutoTestConfig->m_pReportFile, strReportEntry);
	
	return CModemBatchTestsPage::OnWizardNext();
}

void CModemBatchTests10MHzPage::OnChangeShiftButton() 
{
	ChangeCorrection(m_Correction);
	WriteRegistryInt(pszCorrection, (int)(m_Correction*10));
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyPage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsPowerSupplyPage, CModemBatchTestsPage)

CModemBatchTestsPowerSupplyPage::CModemBatchTestsPowerSupplyPage()
	: CModemBatchTestsPage(CModemBatchTestsPowerSupplyPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsPowerSupplyPage)
	m_MeasuredValue = 0.0;
	m_Current = 0.0;
	//}}AFX_DATA_INIT
}

CModemBatchTestsPowerSupplyPage::~CModemBatchTestsPowerSupplyPage()
{
}

void CModemBatchTestsPowerSupplyPage::DoDataExchange(CDataExchange* pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsPowerSupplyPage)
	DDX_Text(pDX, IDC_MEASURED_VALUE_EDIT, m_MeasuredValue);
	DDX_Text(pDX, IDC_CURRENT_VALUE_EDIT, m_Current);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsPowerSupplyPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsPowerSupplyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsPowerSupplyPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;

	CString Caption = GetDeviceDescription();
	Caption += " ";
	Caption += GetVoltageDescription();
	Caption += " test";
	pSheet->SetWindowText(Caption);

	CString WarningStr;
	WarningStr.Format("Please check connections:\nDC %s will be appeared on modem %s jack",
		GetVoltageDescription(), GetDeviceDescription());
	if (MessageBox(WarningStr, "Warning : power supply will be turned ON", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
		return 0;
	
	GetDescendantWindow(IDC_DEVICE_DESCRIPTION_STATIC)->SetWindowText(GetDeviceDescription());
	GetDescendantWindow(IDC_VOLTAGE_DESCRIPTION_STATIC)->SetWindowText(GetVoltageDescription());

	int PowerSupplyMode = GetRPowerSupplyMode();
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	PowerSupplyMode = GetTPowerSupplyMode();
	pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);

	Sleep(1000);
	m_Current = GetCurrent();
	UpdateData(FALSE);

	return CModemBatchTestsPage::OnSetActive();
}

BOOL CModemBatchTestsPowerSupplyPage::OnKillActive() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int PowerSupplyMode = 0;
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	PowerSupplyMode = 0;
	pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);

	return CModemBatchTestsPage::OnKillActive();
}

LRESULT CModemBatchTestsPowerSupplyPage::OnWizardNext() 
{
	UpdateData(TRUE);
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnWizardNext();
		}
	}
	CString strReportEntry = "\n";
	strReportEntry += GetDeviceDescription();
	strReportEntry += "  ";
	strReportEntry += GetVoltageDescription();
	strReportEntry += " test:\n";
	char pszValue[64];
	sprintf(pszValue, "Measured voltage = %.1f Volts\n", m_MeasuredValue);
	strReportEntry += pszValue;
	sprintf(pszValue, "Current = %.3f A\n", m_Current);
	strReportEntry += pszValue;

	WriteString(m_pAutoTestConfig->m_pReportFile, (LPCTSTR)strReportEntry);
	
	return CModemBatchTestsPage::OnWizardNext();
}

//virtual
double CModemBatchTestsPowerSupplyModulator0VoltsPage::GetCurrent()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CBucStatus status;
	pModem->GetBucStatus(status, 1);
	return status.m_Current;
}

//virtual
double CModemBatchTestsPowerSupplyModulator24VoltsPage::GetCurrent()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CBucStatus status;
	pModem->GetBucStatus(status, 1);
	return status.m_Current;
}

//virtual
double CModemBatchTestsPowerSupplyDemodulator0VoltsPage::GetCurrent()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	return status.m_Current;
}

//virtual
double CModemBatchTestsPowerSupplyDemodulator13VoltsPage::GetCurrent()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	return status.m_Current;
}

//virtual
double CModemBatchTestsPowerSupplyDemodulator18VoltsPage::GetCurrent()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	return status.m_Current;
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsVariousOutputPage property page


IMPLEMENT_DYNCREATE(CModemBatchTestsVariousOutputPage, CModemBatchTestsPage)

CModemBatchTestsVariousOutputPage::CModemBatchTestsVariousOutputPage()
	: CModemBatchTestsPage(CModemBatchTestsVariousOutputPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsVariousOutputPage)
	m_Frequency = 0;
	m_bOutputOn = FALSE;
	m_OutputLevel = 0.0;
	m_DataRate = 0;
	m_Report = _T("");
	//}}AFX_DATA_INIT
	m_TimerID = 0;
}

CModemBatchTestsVariousOutputPage::~CModemBatchTestsVariousOutputPage()
{
}

void CModemBatchTestsVariousOutputPage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsVariousOutputPage)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_FEC_MODE_COMBO, m_FecModeCombo);
	DDX_Control(pDX, IDC_SET_DATA_RATE_BUTTON, m_SetDataRateButton);
	DDX_Control(pDX, IDC_DATA_RATE_SPIN, m_DataRateSpin);
	DDX_Control(pDX, IDC_DATA_RATE_EDIT, m_DataRateEdit);
	DDX_Control(pDX, IDC_SET_OUTPUT_LEVEL_BUTTON, m_SetOutputLevelButton);
	DDX_Control(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevelEdit);
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Control(pDX, IDC_ONOFF_CHECK, m_OutputOnOffCheck);
	DDX_Control(pDX, IDC_SET_FREQUENCY_BUTTON, m_SetFrequencyButton);
	DDX_Control(pDX, IDC_TRANSMITTER_FREQUENCY_EDIT, m_FrequencyCtrl);
	DDX_Control(pDX, IDC_OUTPUT_LEVEL_SPIN, m_OutputLevelSpin);
	DDX_Control(pDX, IDC_TRANSMITTER_FREQUENCY_SPIN, m_FrequencySpin);
	DDX_Text(pDX, IDC_TRANSMITTER_FREQUENCY_EDIT, m_Frequency);
	DDX_Check(pDX, IDC_ONOFF_CHECK, m_bOutputOn);
	DDX_Text(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevel);
	DDX_Text(pDX, IDC_DATA_RATE_EDIT, m_DataRate);
	DDX_Text(pDX, IDC_REPORT_EDIT, m_Report);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsVariousOutputPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsVariousOutputPage)
	ON_BN_CLICKED(IDC_SET_FREQUENCY_BUTTON, OnSetFrequencyButton)
	ON_BN_CLICKED(IDC_ONOFF_CHECK, OnOnoffCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_OUTPUT_LEVEL_SPIN, OnDeltaposOutputLevelSpin)
	ON_BN_CLICKED(IDC_SET_OUTPUT_LEVEL_BUTTON, OnSetOutputLevelButton)
	ON_CBN_SELCHANGE(IDC_MODULATION_TYPE_COMBO, OnSelChangeModulationTypeCombo)
	ON_BN_CLICKED(IDC_SET_DATA_RATE_BUTTON, OnSetDataRateButton)
	ON_CBN_SELCHANGE(IDC_FEC_MODE_COMBO, OnSelChangeFecModeCombo)
	ON_BN_CLICKED(IDC_CLEAR_REPORT_BUTTON, OnClearReportButton)
	ON_BN_CLICKED(IDC_TEST_OK_BUTTON, OnTestOkButton)
	ON_BN_CLICKED(IDC_TEST_FAILED_BUTTON, OnTestFailedButton)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsVariousOutputPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;

	m_FrequencySpin.SetRange32(pModem->GetMinTFrequency(), pModem->GetMaxTFrequency());
	m_OutputLevelSpin.SetRange32((int)pModem->GetMinOutputLevel(), (int)pModem->GetMaxOutputLevel());
	m_DataRateSpin.SetRange32(0, 8192000);

	CModIfParams Params;
	MC_ErrorCode EC = pModem->GetTIfParams(Params, 1);

	// frequency
	m_Frequency = Params.m_Frequency;
	m_FrequencyCtrl.EnableWindow(pModem->CanSetTFrequency());
	m_FrequencySpin.EnableWindow(pModem->CanSetTFrequency());
	m_SetFrequencyButton.EnableWindow(pModem->CanSetTFrequency());

	// output
	m_OutputOnOffCheck.EnableWindow(pModem->CanOutputOnOff());
	m_OutputLevelEdit.EnableWindow(pModem->CanOutputLevel());
	m_OutputLevelSpin.EnableWindow(pModem->CanOutputLevel());
	m_SetOutputLevelButton.EnableWindow(pModem->CanOutputLevel());
	m_OutputLevel = Params.m_OutputLevel;
	m_bOutputOn = Params.m_bOutputEnabled;
	
	// Modulation type
	m_ModulationTypeCombo.initT(pModem, 1);

	// Data Rate
	m_DataRateEdit.EnableWindow(pModem->CanDataRate());
	m_DataRateSpin.EnableWindow(pModem->CanDataRate());
	m_SetDataRateButton.EnableWindow(pModem->CanDataRate());
	pModem->GetTDataRate(m_DataRate, 1);

	// Fec
	m_FecModeCombo.initT(pModem, 1);

	// PowerSupply
	m_PowerSupplyCombo.initR(pModem, 1);
	UpdateLnbStatus();

	UpdateData(FALSE);

	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream)
	{
		char *pszHeader = "\nVarious signals\n";
		WriteString(m_pAutoTestConfig->m_pReportFile, pszHeader);
	}
	else
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
		}
	}
	
	if (!m_TimerID)
		m_TimerID = SetTimer(1, 2000, NULL);

	return CModemBatchTestsPage::OnSetActive();
}

BOOL CModemBatchTestsVariousOutputPage::OnKillActive() 
{
	KillTimer(m_TimerID);
	m_TimerID = 0;
	return CModemBatchTestsPage::OnSetActive();
}

LRESULT CModemBatchTestsVariousOutputPage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

void CModemBatchTestsVariousOutputPage::OnTimer(UINT nIDEvent) 
{
	UpdateLnbStatus();	
	CModemBatchTestsPage::OnTimer(nIDEvent);
}

BOOL CModemBatchTestsVariousOutputPage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();

	m_FrequencySpin.SetBuddy(&m_FrequencyCtrl);
	m_OutputLevelSpin.SetBuddy(&m_OutputLevelEdit);
	m_DataRateSpin.SetBuddy(&m_DataRateEdit);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModemBatchTestsVariousOutputPage::OnSetFrequencyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->SetTFrequency(m_Frequency, 1);

	UpdateData(FALSE);
}

void CModemBatchTestsVariousOutputPage::OnOnoffCheck() 
{
	CWaitCursor cursor;
	UpdateData();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->TurnOutputOn(m_bOutputOn, 1);
	UpdateData(FALSE);
}

void CModemBatchTestsVariousOutputPage::OnDeltaposOutputLevelSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateData();
	m_OutputLevel += pNMUpDown->iDelta/10.;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	if (m_OutputLevel > pModem->GetMaxOutputLevel())
		m_OutputLevel = pModem->GetMaxOutputLevel();
	if (m_OutputLevel < pModem->GetMinOutputLevel())
		m_OutputLevel = pModem->GetMinOutputLevel();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CModemBatchTestsVariousOutputPage::OnSetOutputLevelButton() 
{
	CWaitCursor cursor;
	UpdateData();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	MC_ErrorCode EC = pModem->SetOutputLevel(m_OutputLevel, 1);

	UpdateData(FALSE);
}

void CModemBatchTestsVariousOutputPage::OnSelChangeModulationTypeCombo() 
{
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->SetTModulationType(ModulationType, 1);
	m_ModulationTypeCombo.SelectByDataValue(ModulationType);
	m_FecModeCombo.initT(pModem, 1);
}

void CModemBatchTestsVariousOutputPage::OnSetDataRateButton() 
{
	CWaitCursor cursor;
	UpdateData();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->SetTDataRate(m_DataRate, 1);
	UpdateData(FALSE);
}

void CModemBatchTestsVariousOutputPage::OnSelChangeFecModeCombo() 
{
	int FecMode = m_FecModeCombo.getSelectedMode();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->SetTFecMode(FecMode, 1);
	m_FecModeCombo.SelectByDataValue(FecMode);
}

void CModemBatchTestsVariousOutputPage::OnSelChangePowerSupplyCombo() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	UpdateLnbStatus();
}

void CModemBatchTestsVariousOutputPage::UpdateLnbStatus()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	CString str; str.Format("%.3f", status.m_Current);
	GetDescendantWindow(IDC_CURRENT_EDIT)->SetWindowText(str);
}

CString CModemBatchTestsVariousOutputPage::GetSignalTextDescription(CModem *pModem)
{
	CString str;

	unsigned int Freq = 0;
	pModem->GetTFrequency(Freq, 1);

	unsigned int DataRate = 0;
	pModem->GetTDataRate(DataRate, 1);

	double level;
	pModem->GetOutputLevel(level, 1);

	int ModulationType;
	pModem->GetTModulationType(ModulationType, 1);

	int FecMode;
	pModem->GetTFecMode(FecMode, 1);

	str.Format("%d KHz, %s, %.1lf dB, %d baud, %s : ", Freq, pModem->GetTModulationTypeName(ModulationType), level, DataRate, pModem->GetTFecModeName(FecMode));
	return str;
}

void CModemBatchTestsVariousOutputPage::OnClearReportButton() 
{
	m_Report.Empty();
	UpdateData(FALSE);
}

void CModemBatchTestsVariousOutputPage::OnTestOkButton() 
{
	CString str = GetSignalTextDescription(m_pAutoTestConfig->m_pTestModem);
	str += "OK\r\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(str));
	m_Report += str;
	UpdateData(FALSE);
}

void CModemBatchTestsVariousOutputPage::OnTestFailedButton() 
{
	CString str = GetSignalTextDescription(m_pAutoTestConfig->m_pTestModem);
	str += "FAILED!!!\r\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(str));
	m_Report += str;
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsOutputLevelPage property page

static SOutputLevelTestDesc OutputLevelTests[] = {
//	Freq		Span	RefLevel	dB/div
{	950000,		300,	0,		10		},
{	1000000,	300,	0,		10		},
{	1100000,	300,	0,		10		},
{	1200000,	300,	0,		10,		},
{	1300000,	300,	0,		10,		},
{	1400000,	300,	0,		10,		},
{	1450000,	300,	0,		10,		}
};

IMPLEMENT_DYNCREATE(CModemBatchTestsOutputLevelPage, CModemBatchTestsPage)

CModemBatchTestsOutputLevelPage::CModemBatchTestsOutputLevelPage()
	: CModemBatchTestsPage(CModemBatchTestsOutputLevelPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsOutputLevelPage)
	m_bAutomaticFlow = FALSE;
	m_FromLevel = 0.0;
	m_StepLevel = 0.0;
	m_ToLevel = 0.0;
	m_WaitTime = 0;
	m_bIntegralCheck = FALSE;
	//}}AFX_DATA_INIT
}

CModemBatchTestsOutputLevelPage::~CModemBatchTestsOutputLevelPage()
{
}

void CModemBatchTestsOutputLevelPage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsOutputLevelPage)
	DDX_Control(pDX, IDC_SIGNAL_PARAMETERS_LIST, m_SignalListBox);
	DDX_Check(pDX, IDC_AUTOMATIC_FLOW_CHECK, m_bAutomaticFlow);
	DDX_Text(pDX, IDC_FROM_LEVEL, m_FromLevel);
	DDX_Text(pDX, IDC_STEP_LEVEL, m_StepLevel);
	DDX_Text(pDX, IDC_TO_LEVEL, m_ToLevel);
	DDX_Text(pDX, IDC_WAIT_TIME, m_WaitTime);
	DDX_Check(pDX, IDC_INTEGRAL_CHECK, m_bIntegralCheck);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsOutputLevelPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsOutputLevelPage)
	ON_LBN_SELCHANGE(IDC_SIGNAL_PARAMETERS_LIST, OnSelChangeSignalParametersList)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsOutputLevelPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bOn = TRUE;
	pModem->TurnContiniousWaveOn(bOn, 1);
	CPowerMeter *pPowerMeter = m_pAutoTestConfig->m_pPowerMeter;
	pPowerMeter->ResetMeasurements();

	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnSetActive();
		}
	}
	char *pszHeader = "\nOutput level test\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, pszHeader);
	
	return CModemBatchTestsPage::OnSetActive();
}

BOOL CModemBatchTestsOutputLevelPage::OnKillActive() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bOn = FALSE;
	pModem->TurnContiniousWaveOn(bOn, 1);
	return CModemBatchTestsPage::OnKillActive();
}

LRESULT CModemBatchTestsOutputLevelPage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

BOOL CModemBatchTestsOutputLevelPage::OnInitDialog() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	m_FromLevel = pModem->GetMinOutputLevel();
	m_StepLevel = 1;
	m_ToLevel = pModem->GetMaxOutputLevel();

	m_WaitTime = 1;

	CModemBatchTestsPage::OnInitDialog();
	char pszTestShortDesc[64];
	for (int i = 0; i < sizeof(OutputLevelTests)/sizeof(OutputLevelTests[0]); i++)
	{
		SOutputLevelTestDesc *pTestDesc = OutputLevelTests + i;
		pTestDesc->FormatShortDesc(pszTestShortDesc);
		int index = m_SignalListBox.AddString(pszTestShortDesc);
		m_SignalListBox.SetItemDataPtr(index, pTestDesc);
	}
	m_SignalListBox.SetCurSel(0);
	OnSelChangeSignalParametersList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CModemBatchTestsOutputLevelPage::OnNextButton() 
{
	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	index++;
	m_SignalListBox.SetCurSel(index);
	OnSelChangeSignalParametersList();
}

void CModemBatchTestsOutputLevelPage::OnRunButton() 
{
	UpdateData(TRUE);
	
	BOOL bCancel = FALSE;

	while (1)
	{
		bCancel = oneFrequency();
		if (bCancel)
			break;
		
		UpdateData(TRUE);
		if (m_bAutomaticFlow)
		{
			int index = m_SignalListBox.GetCurSel();
			if (index == m_SignalListBox.GetCount()-1)
				break;
			else
				OnNextButton();
		}
		else
			break;
	}
}

// returns TRUE if CANCEL
BOOL CModemBatchTestsOutputLevelPage::oneFrequency()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	double fromLevel = m_FromLevel;
	if (fromLevel < pModem->GetMinOutputLevel())
		fromLevel = pModem->GetMinOutputLevel();
	double toLevel = m_ToLevel;
	if (toLevel > pModem->GetMaxOutputLevel())
		toLevel = pModem->GetMinOutputLevel();
	double levelStep = m_StepLevel;
	if (levelStep < 0)
		levelStep = 1;

	CProgressDlg dlg;
	dlg.Create();

	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return FALSE;
	SOutputLevelTestDesc *pTestDesc = (SOutputLevelTestDesc *)m_SignalListBox.GetItemDataPtr(index);
	pTestDesc->m_RealFreq = pTestDesc->m_Freq;

	const int modulator = 1;
	CPowerMeter *pPowerMeter = m_pAutoTestConfig->m_pPowerMeter;

	// Turn ContiniousWave mode on
	BOOL bOn = TRUE;
	pModem->TurnContiniousWaveOn(bOn, 1);
	// Turn modulator on
	bOn = TRUE;
	pModem->TurnOutputOn(bOn, modulator);

	// Tune frequency
	pModem->SetTFrequency(pTestDesc->m_RealFreq, modulator);

	if (!m_bIntegralCheck)
	{
		sprintf(pszTmpBuffer, "Frequency = %d KHz\n", pTestDesc->m_Freq);
		WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);

		WriteString(m_pAutoTestConfig->m_pReportFile, "Level:");
	}

	double level = fromLevel;
	int nMeasurements = 0;
	while (level <= toLevel)
	{
		if (!m_bIntegralCheck)
		{
			sprintf(pszTmpBuffer, "\t%5.2f", level);
			WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);
		}
		level += levelStep;
		nMeasurements++;
	}
	if (!m_bIntegralCheck)
		WriteString(m_pAutoTestConfig->m_pReportFile, "\n");

	dlg.SetRange(0, nMeasurements);
	dlg.SetPos(0);

	if (!m_bIntegralCheck)
		WriteString(m_pAutoTestConfig->m_pReportFile, "Measured level:");
	level = fromLevel;
	while (level <= toLevel)
	{
		sprintf(pszTmpBuffer, "Frequency = %d, Level = %6.2f", pTestDesc->m_Freq, level);
		dlg.SetStatus(pszTmpBuffer);
		double real_level = level;
		pModem->SetOutputLevel(real_level, 1);
		Sleep(2000);
		MC_ErrorCode EC = MC_OK;
		if (!m_bIntegralCheck)
		{
			EC = pPowerMeter->MeasurePower(real_level, 1);
			if (EC != MC_OK)
			{
				if (MessageBox("Please check connection and controllability of power meter", "Power Meter", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
				{
					if (DoYouReallyWantToCancel(this))
					{
						WriteString(m_pAutoTestConfig->m_pReportFile, "\tCanceled\n");
						return TRUE; //cancel
					}
				}
			}
		}
		if (dlg.CheckCancelButton() || EC == MC_COMMAND_NOT_SUPPORTED)
		{
			if (DoYouReallyWantToCancel(this))
			{
				WriteString(m_pAutoTestConfig->m_pReportFile, "\tCanceled\n");
				return TRUE; // cancel
			}
		}
		dlg.StepIt();
		if (!m_bIntegralCheck)
		{
			sprintf(pszTmpBuffer, "\t%5.2f", real_level);
			WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);
		}
		level += levelStep;
		Sleep(1000*m_WaitTime);
	}
	if (m_bIntegralCheck)
	{
		int answer = MessageBox("Is everything OK with output level on this frequency?", "Output Level", MB_YESNO);
		sprintf(pszTmpBuffer, "Frequency = %d, Level check from %6.2f to %6.2f step %6.2f ", pTestDesc->m_Freq, m_FromLevel, m_ToLevel, m_StepLevel);
		WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);
		if (answer == IDYES)
			WriteString(m_pAutoTestConfig->m_pReportFile, "PASSED\n");
		else
			WriteString(m_pAutoTestConfig->m_pReportFile, "FAILED\n");
	}
	else
		WriteString(m_pAutoTestConfig->m_pReportFile, "\n");
	return FALSE; // not cancel
}

void CModemBatchTestsOutputLevelPage::OnSelChangeSignalParametersList() 
{
	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	SOutputLevelTestDesc *pTestDesc = (SOutputLevelTestDesc *)m_SignalListBox.GetItemDataPtr(index);
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsSecondaryLevelsPage property page

static SSecondaryLevelsTestDesc SecondaryLevelsTests[] = {
//	Freq		RefLevel	dB/div
{	950000,		10,			30		},
{	1000000,	10,			30		},
{	1100000,	10,			30		},
{	1200000,	10,			30,		},
{	1300000,	10,			30,		},
{	1400000,	10,			30,		},
{	1450000,	10,			30,		}
};

IMPLEMENT_DYNCREATE(CModemBatchTestsSecondaryLevelsPage, CModemBatchTestsPage)

CModemBatchTestsSecondaryLevelsPage::CModemBatchTestsSecondaryLevelsPage()
	: CModemBatchTestsPage(CModemBatchTestsSecondaryLevelsPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsSecondaryLevelsPage)
	//}}AFX_DATA_INIT
}

CModemBatchTestsSecondaryLevelsPage::~CModemBatchTestsSecondaryLevelsPage()
{
}

void CModemBatchTestsSecondaryLevelsPage::DoDataExchange(CDataExchange* pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsSecondaryLevelsPage)
	DDX_Control(pDX, IDC_SIGNAL_PARAMETERS_LIST, m_SignalListBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsSecondaryLevelsPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CModemBatchTestsSecondaryLevelsPage)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_RESTART_TESTS_BUTTON, OnRestartTestsButton)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsSecondaryLevelsPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	//if (MessageBox("Please switch SWEEP mode of your spectrum analyser to AUTO", "Spurious output test", MB_OKCANCEL) == IDCANCEL)
	//	return 0;
	
	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnSetActive();
		}
	}

	// Turn Continuous Wave mode ON
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bContiniousWave = TRUE;
	pModem->TurnContiniousWaveOn(bContiniousWave, 1);

	char *pszHeader = "\nSpurious level test\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, pszHeader);
	
	return CModemBatchTestsPage::OnSetActive();
}

BOOL CModemBatchTestsSecondaryLevelsPage::OnKillActive() 
{
	// Turn Continuous Wave mode OFF
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	BOOL bContiniousWave = FALSE;
	pModem->TurnContiniousWaveOn(bContiniousWave, 1);

	return CModemBatchTestsPage::OnKillActive();
}

LRESULT CModemBatchTestsSecondaryLevelsPage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

BOOL CModemBatchTestsSecondaryLevelsPage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();
	char pszTestShortDesc[64];
	for (int i = 0; i < sizeof(SecondaryLevelsTests)/sizeof(SecondaryLevelsTests[0]); i++)
	{
		SSecondaryLevelsTestDesc *pTestDesc = SecondaryLevelsTests + i;
		pTestDesc->FormatShortDesc(pszTestShortDesc);
		int index = m_SignalListBox.AddString(pszTestShortDesc);
		m_SignalListBox.SetItemDataPtr(index, pTestDesc);
	}
	OnRestartTestsButton();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CModemBatchTestsSecondaryLevelsPage::OnNextButton() 
{
	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	index++;
	m_SignalListBox.SetCurSel(index);
}

void CModemBatchTestsSecondaryLevelsPage::OnRestartTestsButton() 
{
	m_SignalListBox.SetCurSel(0);
}

void CModemBatchTestsSecondaryLevelsPage::OnRunButton() 
{
	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	SSecondaryLevelsTestDesc *pTestDesc = (SSecondaryLevelsTestDesc *)m_SignalListBox.GetItemDataPtr(index);
	CWaitCursor cursor;

	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	// Turn Continuous Wave mode ON
	BOOL bOn = TRUE;
	pModem->TurnContiniousWaveOn(bOn, 1);

	unsigned int real_freq = pTestDesc->m_Freq;
	pModem->SetTFrequency(real_freq, 1);
	bOn = TRUE;
	pModem->TurnOutputOn(bOn, 1);
	sprintf(pszTmpBuffer, "Frequency = %d KHz\n", pTestDesc->m_Freq);
	WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);

	double level = pModem->GetMaxOutputLevel();
	pModem->SetOutputLevel(level, 1);
	if (MessageBox("Do you see L-band spurious output above -40 dBm?",
		"Question", MB_YESNO | MB_ICONQUESTION) == IDYES)
		WriteString(m_pAutoTestConfig->m_pReportFile, "\tSpurious output take place\n");
	else
		WriteString(m_pAutoTestConfig->m_pReportFile, "\tNo spurious output\n");
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsSearchRangePage property page

static SSearchRangeTestDesc SearchRangeTests[] = {
//	Freq		DataRate	SearchRange	span		RefLevel	dB/Div
{	950010,		64000,		5000,		300,		0,			10		},
{	1000000,	64000,		10000,		300,		0,			10		},
{	1100000,	64000,		20000,		300,		0,			10		},
{	1200000,	19200,		5000,		300,		0,			10		},
{	1300000,	19200,		10000,		300,		0,			10		},
{	1400000,	19200,		20000,		300,		0,			10		},
{	1450000,	19200,		30000,		300,		0,			10		}
};

IMPLEMENT_DYNCREATE(CModemBatchTestsSearchRangePage, CModemBatchTestsPage)

CModemBatchTestsSearchRangePage::CModemBatchTestsSearchRangePage()
	: CModemBatchTestsPage(CModemBatchTestsSearchRangePage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsSearchRangePage)
	m_bAutomaticFlow = FALSE;
	//}}AFX_DATA_INIT
	m_TimerID = 0;
}

CModemBatchTestsSearchRangePage::~CModemBatchTestsSearchRangePage()
{
}

void CModemBatchTestsSearchRangePage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsSearchRangePage)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_SIGNAL_PARAMETERS_LIST, m_SignalListBox);
	DDX_Check(pDX, IDC_AUTOMATIC_FLOW_CHECK, m_bAutomaticFlow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsSearchRangePage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsSearchRangePage)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CModemBatchTestsSearchRangePage::OnTimer(UINT nIDEvent) 
{
	UpdateLnbStatus();	
	
	CModemBatchTestsPage::OnTimer(nIDEvent);
}

BOOL CModemBatchTestsSearchRangePage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	if (!InputToOutputConnectionWarning(" search range test", this))
		return 0;
	
	// Turn Continuous Wave mode OFF
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	BOOL bOn = FALSE;
	pEtalonModem->TurnContiniousWaveOn(bOn, 1);

	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnSetActive();
		}
	}
	char *pszHeader = "\nSearch range test\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, pszHeader);

	if (!m_TimerID)
		m_TimerID = SetTimer(1, 2000, NULL);
	
	return CModemBatchTestsPage::OnSetActive();
}

BOOL CModemBatchTestsSearchRangePage::OnKillActive() 
{
	KillTimer(m_TimerID);
	m_TimerID = 0;
	
	return CModemBatchTestsPage::OnKillActive();
}

LRESULT CModemBatchTestsSearchRangePage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

BOOL CModemBatchTestsSearchRangePage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();
	char pszTestShortDesc[64];
	for (int i = 0; i < sizeof(SearchRangeTests)/sizeof(SearchRangeTests[0]); i++)
	{
		SSearchRangeTestDesc *pTestDesc = SearchRangeTests + i;
		pTestDesc->FormatShortDesc(pszTestShortDesc);
		int index = m_SignalListBox.AddString(pszTestShortDesc);
		m_SignalListBox.SetItemDataPtr(index, pTestDesc);
	}
	m_SignalListBox.SetCurSel(0);

	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	m_PowerSupplyCombo.initR(pModem, 1);
	UpdateLnbStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CModemBatchTestsSearchRangePage::OnNextButton() 
{
	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	index++;
	m_SignalListBox.SetCurSel(index);
}

void CModemBatchTestsSearchRangePage::OnRunButton() 
{
	int index = m_SignalListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	CProgressDlg dlg;
	dlg.Create();
	BOOL bCancel = FALSE;

	while (1)
	{
		if (bCancel || dlg.CheckCancelButton())
			break;
		dlg.SetPos(0);

		index = m_SignalListBox.GetCurSel();
		if (index == LB_ERR)
			break;
		SSearchRangeTestDesc *pTestDesc = (SSearchRangeTestDesc *)m_SignalListBox.GetItemDataPtr(index);

		CModem *pModem = m_pAutoTestConfig->m_pTestModem;
		CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;

		// Turn Continuous Wave mode OFF
		BOOL bOn = FALSE;
		pEtalonModem->TurnContiniousWaveOn(bOn, 1);

		GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText("Setting signal parameters...");
		unsigned int real_freq = pTestDesc->m_Freq;
		pEtalonModem->SetTFrequency(real_freq, 1);
		pModem->SetRFrequency(real_freq, 1);

		unsigned int real_datarate = pTestDesc->m_DataRate;
		pEtalonModem->SetTDataRate(real_datarate, 1);
		pModem->SetRDataRate(real_datarate, 1);

		int ModulationType = 1; // QPSK;
		pEtalonModem->SetTModulationType(ModulationType, 1);
		pModem->SetRModulationType(ModulationType, 1);

		int real_coderate = 1;
		pEtalonModem->SetTFecMode(real_coderate, 1);
		pModem->SetRFecMode(real_coderate, 1);

		unsigned int real_search_range = pTestDesc->m_SearchRange+10; // Fake! +10 Hz specially for Yartsev
		pModem->SetSearchRange(real_search_range, 1);
		real_search_range = pTestDesc->m_SearchRange; // Fake! Real value differ (+10 Hz) frm desired value but displayed correctly

		//	pModem->SetOutputLevel(pTestDesc->m_RealLevel, modulator);
		sprintf(pszTmpBuffer, "\nFrequency = %d KHz, Data Rate = %d baud, Search range = +/-%d KHz\n", real_freq, real_datarate, real_search_range/1000);
		WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);

		if (!AdjustSignal(pEtalonModem, pModem, 7, 20))
		{
			char *pszError = "Cannot complete this test because of carrier acquisition fault\n";
			MessageBox(pszError, "Error", MB_ICONEXCLAMATION);
			WriteString(m_pAutoTestConfig->m_pReportFile, pszError);
			return;
		}

		unsigned int nMeasurements = 0;
		int Offset;
		unsigned int DeltaFreqMax = 2*real_search_range;
		if (real_search_range > 20000)
			DeltaFreqMax = real_search_range + 20000;
		DeltaFreqMax /= 1000; // All freqs are in KHz
		unsigned int MinModFreq = pTestDesc->m_Freq - DeltaFreqMax;
		unsigned int MaxModFreq = pTestDesc->m_Freq + DeltaFreqMax;
		WriteString(m_pAutoTestConfig->m_pReportFile, "Mod. freq. :");
		for (unsigned int freq = MinModFreq; freq <= MaxModFreq; freq++)
		{
			int ThisDeltaFreq = ((int)freq - (int)(pTestDesc->m_Freq))*1000;
			sprintf(pszTmpBuffer, "\t%+d", ThisDeltaFreq);
			WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);
			nMeasurements++;
		}
		WriteString(m_pAutoTestConfig->m_pReportFile, "\n");
		
		dlg.SetRange(0, nMeasurements);
		
		WriteString(m_pAutoTestConfig->m_pReportFile, "Offset :");
		for (freq = MinModFreq; freq <= MaxModFreq; freq++)
		{
			real_freq = freq;
			int ThisDeltaFreq = ((int)freq - (int)(pTestDesc->m_Freq));
			sprintf(pszTmpBuffer, "Frequency = %d (%+d) KHz", freq, ThisDeltaFreq);
			dlg.SetStatus(pszTmpBuffer);
			pEtalonModem->SetTFrequency(real_freq, 1);
			Sleep(5000);
			if (dlg.CheckCancelButton())
			{
				WriteString(m_pAutoTestConfig->m_pReportFile, "\tCanceled\n");
				bCancel = TRUE;
				break;
			}
			dlg.StepIt();
			BOOL bIsCarrier = FALSE;
			pModem->IsRCarrier(bIsCarrier, 1);
			if (bIsCarrier)
			{
				pModem->GetDemodulatorOffset(Offset, 1);
				sprintf(pszTmpBuffer, "\t%+d", Offset);
			}
			else
				strcpy(pszTmpBuffer, "\t#####");
			WriteString(m_pAutoTestConfig->m_pReportFile, pszTmpBuffer);
		}
		pModem->SetSearchRange(real_search_range, 1); // restore search range (end of fake)
		WriteString(m_pAutoTestConfig->m_pReportFile, "\n");
		if (bCancel)
			break;
		
		UpdateData(TRUE);
		if (m_bAutomaticFlow)
		{
			if (index == m_SignalListBox.GetCount()-1)
				break;
			else
				OnNextButton();
		}
		else
			break;
	}
}

void CModemBatchTestsSearchRangePage::OnSelChangePowerSupplyCombo() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	UpdateLnbStatus();
}

//virtual
void CModemBatchTestsSearchRangePage::OnUpdateLevels(double OutputLevel, double InputLevel, double SN)
{
	CString str;
	str.Format("Adjusting signal (output = %5.1f dBm, input  = %5.1f dBm, S/N = %4.1f dB)", OutputLevel, InputLevel, SN);
	GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText(str);
}

void CModemBatchTestsSearchRangePage::UpdateLnbStatus()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	CString str; str.Format("%.3f", status.m_Current);
	GetDescendantWindow(IDC_CURRENT_EDIT2)->SetWindowText(str);
}

/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsCarrierAcquisitionPage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsCarrierAcquisitionPage, CModemBatchTestsPage)

CModemBatchTestsCarrierAcquisitionPage::CModemBatchTestsCarrierAcquisitionPage()
	: CModemBatchTestsPage(CModemBatchTestsCarrierAcquisitionPage::IDD)
{
	//{{AFX_DATA_INIT(CModemBatchTestsCarrierAcquisitionPage)
	m_Frequency = 0;
	m_NTests = 5;
	m_Protocol = _T("");
	//}}AFX_DATA_INIT
}

CModemBatchTestsCarrierAcquisitionPage::~CModemBatchTestsCarrierAcquisitionPage()
{
}

void CModemBatchTestsCarrierAcquisitionPage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsCarrierAcquisitionPage)
	DDX_Control(pDX, IDC_NTESTS_EDIT, m_NTestsEdit);
	DDX_Control(pDX, IDC_FEC_MODE_COMBO, m_FecModeCombo);
	DDX_Control(pDX, IDC_DATA_RATES_LIST, m_DataRatesList);
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_NTESTS_SPIN, m_NTestsSpin);
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Text(pDX, IDC_FREQUENCY_EDIT, m_Frequency);
	DDX_Text(pDX, IDC_NTESTS_EDIT, m_NTests);
	DDV_MinMaxUInt(pDX, m_NTests, 1, 100);
	DDX_Text(pDX, IDC_TEST_PROTOCOL_EDIT, m_Protocol);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsCarrierAcquisitionPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsCarrierAcquisitionPage)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	ON_BN_CLICKED(IDC_SET_FREQUENCY_BUTTON, OnSetFrequencyButton)
	ON_CBN_SELCHANGE(IDC_FEC_MODE_COMBO, OnSelChangeFecModeCombo)
	ON_CBN_SELCHANGE(IDC_MODULATION_TYPE_COMBO, OnSelChangeModulationTypeCombo)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CModemBatchTestsCarrierAcquisitionPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	if (!InputToOutputConnectionWarning("carrier acquisition test", this))
		return 0;

	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	m_FecModeCombo.initR(pModem, 1);
	m_PowerSupplyCombo.initR(pModem, 1);
	m_ModulationTypeCombo.initR(pModem, 1);

	pModem->GetRFrequency(m_Frequency, 1);

	UpdateData(FALSE);

	UpdateLnbStatus();

	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnSetActive();
		}
	}
	char *pszHeader = "\nDemodulator acquisition time investigation\n";
	WriteString(m_pAutoTestConfig->m_pReportFile, pszHeader);

	BOOL bOn = FALSE;
	pModem->TurnOutputOn(bOn, 1);
	bOn = TRUE;
	pEtalonModem->TurnOutputOn(bOn, 1);

//	MessageBox("Please turn noise generator ON now and\nturn demodulator to FFT acquisition mode (if it possible)", "Directions", MB_OK | MB_ICONEXCLAMATION);
	
	return CModemBatchTestsPage::OnSetActive();
}

LRESULT CModemBatchTestsCarrierAcquisitionPage::OnWizardNext() 
{
	return CModemBatchTestsPage::OnWizardNext();
}

BOOL CModemBatchTestsCarrierAcquisitionPage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();

	m_NTestsSpin.SetBuddy(&m_NTestsEdit);
	m_NTestsSpin.SetRange(1, 100);

	UINT DataRates[] = {
		9600,
		14400,
		19200,
		28800,
		33600,
		48000,
		64000,
		128000,
		192000,
		256000,
		320000,
		384000,
		448000,
		512000,
		1024000,
		2048000
	};
	for (int i = 0; i < sizeof(DataRates)/sizeof(DataRates[0]); i++)
	{
		char szDataRate[32];
		sprintf(szDataRate, "%d", DataRates[i]);
		int index = m_DataRatesList.AddString(szDataRate);
		m_DataRatesList.SetItemData(index, DataRates[i]);
		m_DataRatesList.SetCheck(index, TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModemBatchTestsCarrierAcquisitionPage::OnSetFrequencyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	pModem->SetRFrequency(m_Frequency, 1);
	pEtalonModem->SetTFrequency(m_Frequency, 1);

	UpdateData(FALSE);
}

void CModemBatchTestsCarrierAcquisitionPage::OnSelChangeFecModeCombo() 
{
	int FecMode = m_FecModeCombo.getSelectedMode();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	pModem->SetRFecMode(FecMode, 1);
	pEtalonModem->SetTFecMode(FecMode, 1);
	m_FecModeCombo.SelectByDataValue(FecMode);
}

void CModemBatchTestsCarrierAcquisitionPage::OnSelChangeModulationTypeCombo() 
{
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	pModem->SetRModulationType(ModulationType, 1);
	pEtalonModem->SetTModulationType(ModulationType, 1);
	m_ModulationTypeCombo.SelectByDataValue(ModulationType);
	m_FecModeCombo.initT(pModem, 1);
}

void CModemBatchTestsCarrierAcquisitionPage::OnSelChangePowerSupplyCombo() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	UpdateLnbStatus();
}

void CModemBatchTestsCarrierAcquisitionPage::UpdateLnbStatus()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	CString str; str.Format("%.3f", status.m_Current);
	GetDescendantWindow(IDC_CURRENT_EDIT)->SetWindowText(str);
}

BOOL CModemBatchTestsCarrierAcquisitionPage::DoTest(CProgressDlg *pDlg) 
{
	UpdateData();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CModem *pEtalonModem = m_pAutoTestConfig->m_pEtalonModem;
	
	// Frequencies
	pEtalonModem->SetTFrequency(m_Frequency, 1);
	pModem->SetRFrequency(m_Frequency, 1);
	if (pDlg->CheckCancelButton())
		return FALSE;

	// Modulation
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	pEtalonModem->SetTModulationType(ModulationType, 1);
	pModem->SetRModulationType(ModulationType, 1);
	if (pDlg->CheckCancelButton())
		return FALSE;

	// FEC mode
	int FecMode = m_FecModeCombo.getSelectedMode();
	pEtalonModem->SetTFecMode(FecMode, 1);
	pModem->SetRFecMode(FecMode, 1);
	if (pDlg->CheckCancelButton())
		return FALSE;

	CString str;
	str.Format("%d KHz, %s, %s :\r\n", m_Frequency, pModem->GetRModulationTypeName(ModulationType),
			pModem->GetRFecModeName(FecMode));
	WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(str));
	m_Protocol += str;
	UpdateData(FALSE);

	int nCycles = 0;
	for (int iDataRate = 0; iDataRate < m_DataRatesList.GetCount(); iDataRate++)
	{
		UpdateLnbStatus();
		// Data rates
		if (!m_DataRatesList.GetCheck(iDataRate))
			continue;
		m_DataRatesList.SetCurSel(iDataRate);
		unsigned int DataRate = m_DataRatesList.GetItemData(iDataRate);;
		pModem->SetRDataRate(DataRate, 1);
		pEtalonModem->SetTDataRate(DataRate, 1);
		if (pDlg->CheckCancelButton())
			return FALSE;
		
		// Output level
		if (!AdjustSignal(pEtalonModem, pModem, 7., 20., pDlg))
		{
			if (MessageBox("Cannot adjust signal level. Please check radio connection.\nInterrupt test?", "Error!", MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
				return FALSE;
			else
				return TRUE;
		}
		
		BOOL bIsCarrier = FALSE;
		double SumTimes = 0;
		double MaxTime = 0;
		int nOK = 0;
		int nFailed = 0;
		double SumInputLevel = 0.;
		double SumSN = 0.;
		for (unsigned int iTest = 0; iTest < m_NTests; iTest++)
		{
			// Set RFrequency far from working frequency
			GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText("Loosing carrier...");
			unsigned int FarFreq;
			pModem->GetRFrequency(FarFreq, 1);
			FarFreq += 1000; // Shifted up by 1 MHz
			if (FarFreq >= pModem->GetMaxRFrequency())
				FarFreq -= 2000; // Shifted down by 2 MHz
			pModem->SetRFrequency(FarFreq, 1);
		
			int iLoop=0;
			while (TRUE)
			{
				if (pDlg->CheckCancelButton())
					return FALSE;
				Sleep(100);
				pModem->IsRCarrier(bIsCarrier, 1);
				if (!bIsCarrier)
					break;
				++iLoop;
				if (iLoop>100)
					break; // to avoid infinite loop if we cannot loose carrier
			}
			if (bIsCarrier)
			{
				if (MessageBox("Cannot loose the carrier.\nInterrupt test?", "Error!", MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
					return FALSE;
				else
					return TRUE;
			}
			Sleep(500);

			// Move receiver to the test frequency
			GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText("Carrier acquisition...");
			DWORD DeltaTime = 0, StartTime = GetTickCount();
			pModem->SetRFrequency(m_Frequency, 1);
			bIsCarrier=FALSE;
			do
			{
				pModem->IsRCarrier(bIsCarrier, 1);
				DeltaTime=GetTickCount()-StartTime;
			} while (!bIsCarrier && DeltaTime < MAX_DELAY);

			if (bIsCarrier)
			{
				nOK++;
				SumTimes += DeltaTime;
				if (DeltaTime > MaxTime)
					MaxTime = DeltaTime;
				Sleep(2000); // to get stable S/N
				double SN;
				pModem->GetEbNo(SN, 1);
				SumSN += SN;
				double InputLevel;
				pModem->GetInputLevel(InputLevel, 1);
				SumInputLevel += InputLevel;
			}
			else
			{
				nFailed++;
			}
			pDlg->StepIt();
		} // end of NTests loop
		double AveTime = 0;
		double AveInputLevel = 0;
		double AveSN = 0;
		if (nOK)
		{
			AveTime = SumTimes/nOK;
			AveInputLevel = SumInputLevel/nOK;
			AveSN = SumSN/nOK;
		}
		str.Format("   %.1lf kbit/s, OK/FAIL %d/%d, S/N %.1lf dB, %.1lf dBm, Ave/Max = %.2lf/%.2lf s\r\n",
			DataRate/1000.,
			nOK, nFailed,
			AveSN, AveInputLevel,
			AveTime/1000., MaxTime/1000.);
		WriteString(m_pAutoTestConfig->m_pReportFile, LPCTSTR(str));
		m_Protocol += str;
		UpdateData(FALSE);
		//int nLines = m_Protocol.GetLineCount();
		//int iFirstLine = m_Protocol.GetFirstVisibleLine();
	}

	return TRUE;
}

//virtual
void CModemBatchTestsCarrierAcquisitionPage::OnUpdateLevels(double OutputLevel, double InputLevel, double SN)
{
	CString str;
	str.Format("Adjusting signal (output = %5.1f dBm, input  = %5.1f dBm, S/N = %4.1f dB)", OutputLevel, InputLevel, SN);
	GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText(str);
}

BOOL CModemBatchTestsCarrierAcquisitionPage::OnKillActive() 
{
	// Open file and write header
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return CModemBatchTestsPage::OnKillActive();
		}
	}


	return CModemBatchTestsPage::OnKillActive();
}

void CModemBatchTestsCarrierAcquisitionPage::OnRunButton() 
{
	UpdateData(FALSE);
	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, m_NTests*m_DataRatesList.GetCount());
	DoTest(&dlg);
}


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsBerTestPage property page

IMPLEMENT_DYNCREATE(CModemBatchTestsBerTestPage, CModemBatchTestsPage)

CModemBatchTestsBerTestPage::CModemBatchTestsBerTestPage()
	: CModemBatchTestsPage(IDD_MODEM_BATCH_TESTS_BERTEST_PROPPAGE)
{
	//{{AFX_DATA_INIT(CModemBatchTestsBerTestPage)
	m_Protocol = _T("");
	m_Frequency = 0;
	//}}AFX_DATA_INIT
	m_TimerID = 0;
}

CModemBatchTestsBerTestPage::~CModemBatchTestsBerTestPage()
{
}

void CModemBatchTestsBerTestPage::DoDataExchange(CDataExchange *pDX)
{
	CModemBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemBatchTestsBerTestPage)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_FEC_MODE_COMBO, m_FecModeCombo);
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Control(pDX, IDC_DATA_RATE_COMBO, m_DataRateCombo);
	DDX_Control(pDX, IDC_FREQUENCY_SPIN, m_FrequencySpin);
	DDX_Text(pDX, IDC_TEST_PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_FREQUENCY_EDIT, m_Frequency);
	DDX_Text(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevel);
	DDX_Control(pDX, IDC_OUTPUT_LEVEL_SPIN, m_OutputLevelSpin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemBatchTestsBerTestPage, CModemBatchTestsPage)
	//{{AFX_MSG_MAP(CModemBatchTestsBerTestPage)
	ON_BN_CLICKED(IDC_REPEAT_BUTTON, OnRepeatButton)
	ON_WM_TIMER()
	ON_NOTIFY(UDN_DELTAPOS, IDC_OUTPUT_LEVEL_SPIN, OnDeltaPosOutputLevelSpin)
	ON_CBN_SELCHANGE(IDC_DATA_RATE_COMBO, OnSelChangeDataRateCombo)
	ON_CBN_SELCHANGE(IDC_FEC_MODE_COMBO, OnSelChangeFecModeCombo)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	ON_BN_CLICKED(IDC_FREQ_BUTTON, OnFreqButton)
	ON_BN_CLICKED(IDC_LEVEL_BUTTON, OnLevelButton)
	ON_CBN_SELCHANGE(IDC_MODULATION_TYPE_COMBO, OnSelChangeModulationTypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsBerTestPage message handlers

BOOL CModemBatchTestsBerTestPage::OnInitDialog() 
{
	CModemBatchTestsPage::OnInitDialog();

	CModem *pModem = m_pAutoTestConfig->m_pTestModem;

	m_FrequencySpin.SetBuddy(GetDescendantWindow(IDC_FREQUENCY_EDIT));
	m_OutputLevelSpin.SetBuddy(GetDescendantWindow(IDC_OUTPUT_LEVEL_EDIT));
	m_OutputLevelSpin.SetRange32((int)pModem->GetMinOutputLevel(), (int)pModem->GetMaxOutputLevel());

	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("9600"), 9600);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("19200"), 19200);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("32000"), 32000);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("64000"), 64000);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("128000"), 128000);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("256000"), 256000);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("384000"), 384000);
	m_DataRateCombo.SetItemData(m_DataRateCombo.AddString("1024000"), 1024000);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CModemBatchTestsBerTestPage::OnSetActive() 
{
	CWaitCursor cursor;
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;

	if (!InputToOutputConnectionWarning("BER-test", this))
		return 0;

	m_FrequencySpin.SetRange32(max(pModem->GetMinTFrequency(), pModem->GetMinRFrequency()),
								min(pModem->GetMaxTFrequency(), pModem->GetMaxRFrequency()));
	m_Frequency = 1000000;
	pModem->GetTFrequency(m_Frequency, 1);
	m_FrequencySpin.SetPos(m_Frequency);
	m_DataRateCombo.SetCurSel(CB_ERR);
	pModem->GetOutputLevel(m_OutputLevel, 1);
	
	m_PowerSupplyCombo.initR(pModem, 1);	
	m_ModulationTypeCombo.initT(pModem, 1);
	m_FecModeCombo.initT(pModem, 1);
	
	UpdateData(FALSE);
	UpdateSNandLevel();
	UpdateLnbStatus();
	
	if (!m_TimerID)
		m_TimerID = SetTimer(1, 2000, NULL);

	return CModemBatchTestsPage::OnSetActive();
}

BOOL CModemBatchTestsBerTestPage::OnKillActive() 
{
	KillTimer(m_TimerID);
	m_TimerID = 0;
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	// BER-test mode OFF
	int Power = 0;
	pModem->SetTBerTest(Power, 1);
	pModem->SetRBerTest(Power, 1);

	return CModemBatchTestsPage::OnKillActive();
}

LRESULT CModemBatchTestsBerTestPage::OnWizardNext() 
{
	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CModemBatchTestsPage::OnWizardNext();
}

void CModemBatchTestsBerTestPage::OnDeltaPosOutputLevelSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	NM_UPDOWN *pNMUpDown = (NM_UPDOWN *)pNMHDR;
	UpdateData();
	m_OutputLevel += pNMUpDown->iDelta/10.;
	if (m_OutputLevel > pModem->GetMaxOutputLevel())
		m_OutputLevel = pModem->GetMaxOutputLevel();
	if (m_OutputLevel < pModem->GetMinOutputLevel())
		m_OutputLevel = pModem->GetMinOutputLevel();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CModemBatchTestsBerTestPage::OnSelChangeDataRateCombo() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int DataRateSel = m_DataRateCombo.GetCurSel();
	unsigned int DataRate = m_DataRateCombo.GetItemData(DataRateSel);
	pModem->SetTDataRate(DataRate, 1);
	pModem->SetRDataRate(DataRate, 1);
}

void CModemBatchTestsBerTestPage::OnSelChangeFecModeCombo() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int FecMode = m_FecModeCombo.getSelectedMode();
	pModem->SetRFecMode(FecMode, 1);
	pModem->SetTFecMode(FecMode, 1);
	m_FecModeCombo.SelectByDataValue(FecMode);
}

void CModemBatchTestsBerTestPage::OnSelChangePowerSupplyCombo() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	pModem->SetRPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	UpdateSNandLevel();
	UpdateLnbStatus();
}

void CModemBatchTestsBerTestPage::OnSelChangeModulationTypeCombo() 
{
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	pModem->SetTModulationType(ModulationType, 1);
	pModem->SetRModulationType(ModulationType, 1);
	m_ModulationTypeCombo.SelectByDataValue(ModulationType);
	m_FecModeCombo.initT(pModem, 1);
}

void CModemBatchTestsBerTestPage::OnTimer(UINT nIDEvent) 
{
	UpdateSNandLevel();
	UpdateLnbStatus();
	
	CModemBatchTestsPage::OnTimer(nIDEvent);
}

void CModemBatchTestsBerTestPage::UpdateSNandLevel()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	double SN, InputLevel;
	pModem->GetEbNo(SN, 1);
	pModem->GetInputLevel(InputLevel, 1);
	CString str; str.Format("S/N = %5.1f dB, Input Level = %5.1f dBm", SN, InputLevel);
	GetDescendantWindow(IDC_SN_INPUT_LEVEL_STATIC)->SetWindowText(str);
}

void CModemBatchTestsBerTestPage::UpdateLnbStatus()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CLnbStatus status;
	pModem->GetLnbStatus(status, 1);
	CString str; str.Format("%.3f", status.m_Current);
	GetDescendantWindow(IDC_CURRENT_EDIT)->SetWindowText(str);
}

void CModemBatchTestsBerTestPage::DoTest()
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CString str;
	CProgressDlg dlg;
	dlg.Create();

	// BER-test mode ON
	dlg.SetStatus("Setting up BER-test options...");
	m_Protocol += "Pattern length\t";
	int Power = 11;
	MC_ErrorCode EC = pModem->SetTBerTest(Power, 1);
	if (EC == MC_OK)
		m_Protocol += "2047\t\t";
	EC = pModem->SetRBerTest(Power, 1);
	if (EC == MC_OK)
		m_Protocol += "2047\r\n";
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return;

	// BER-test restart
	double SN, InputLevel, OutputLevel;
	pModem->RestartBerTest(1);

	m_Protocol += "Bits\tLosses\tErrors\tErrSecs\tDuration\tS/N\tLevelIn\tLevelOut\r\n";
	UpdateData(FALSE);

	dlg.SetRange(0, 60*60);
	dlg.SetPos(0);
	CTime BeginTime = CTime::GetCurrentTime();
	CTime LastRecordTime = BeginTime;
	while (1)
	{
		CTime CurrentTime = CTime::GetCurrentTime();
		if (dlg.CheckCancelButton())
			break;
		CTimeSpan TimeFromBegin = CurrentTime - BeginTime;
		if (TimeFromBegin > CTimeSpan(0, 1, 0, 0))
			break;
		CTimeSpan TimeFromLastRecord = CurrentTime - LastRecordTime;
		double BER, Errors, TotalBits, ErrorFreeSecsPercentage;
		unsigned int SyncLoss, ErredSecs, TotalTestDuration;
		pModem->GetBerTestStatus(BER, SyncLoss, Errors, TotalBits, ErrorFreeSecsPercentage, ErredSecs, TotalTestDuration);
		pModem->GetEbNo(SN, 1);
		pModem->GetInputLevel(InputLevel, 1);
		pModem->GetOutputLevel(OutputLevel, 1);
		str.Format("%6.1e      %3d      %6.1e   %4d        %d    %5.1f dB   %5.1f dBm   %5.1f dBm", TotalBits, SyncLoss, Errors, ErredSecs, TotalTestDuration, SN, InputLevel, OutputLevel);
		dlg.SetStatus(str);
		if (TimeFromLastRecord >= CTimeSpan(0, 0, 5, 0))
		{
			m_Protocol += str;
			m_Protocol += "\r\n";
			LastRecordTime = CurrentTime;
			UpdateData(FALSE);
		}
		Sleep(500);
		dlg.SetPos(TimeFromBegin.GetTotalSeconds());
	} 

	UpdateData(FALSE);
}

void CModemBatchTestsBerTestPage::OnRepeatButton() 
{
	if (SetParams())
		DoTest();	
	UpdateData(FALSE);
}

void CModemBatchTestsBerTestPage::OnFreqButton() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	UpdateData();
	pModem->SetTFrequency(m_Frequency, 1);
	pModem->SetRFrequency(m_Frequency, 1);
	UpdateData(FALSE);
}

void CModemBatchTestsBerTestPage::OnLevelButton() 
{
	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	UpdateData();
	pModem->SetOutputLevel(m_OutputLevel, 1);
	UpdateData(FALSE);
}

BOOL CModemBatchTestsBerTestPage::SetParams()
{
	UpdateData();

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 11);
	dlg.SetPos(0);

	CModem *pModem = m_pAutoTestConfig->m_pTestModem;
	CString str;
	
	// Turn BER-Test OFF
	int Power = 0;
	MC_ErrorCode EC = pModem->SetTBerTest(Power, 1);
	EC = pModem->SetRBerTest(Power, 1);

	m_Protocol = "\r\nBER-test...\r\n\r\nTest parameters:\r\nParameter\tModulator\t\tDemodulator\r\n";

	// Frequency
	dlg.SetStatus("Setting up frequency...");
	m_Protocol += "Frequency\t";
	unsigned int freq = m_Frequency;
	EC = pModem->SetTFrequency(freq, 1);
	if (EC == MC_OK)
	{
		str.Format("%d KHz\t", freq);
		m_Protocol += str;
	}
	if (dlg.CheckCancelButton())
		return FALSE;

	freq = m_Frequency;
	EC = pModem->SetRFrequency(freq, 1);
	if (EC == MC_OK)
	{
		str.Format("%d KHz\r\n", freq);
		m_Protocol += str;
	}
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return FALSE;

	// Modulation
	dlg.SetStatus("Setting up modulation type...");
	m_Protocol += "Modulation\t";
	int ModulationType = m_ModulationTypeCombo.getSelectedMode();
	EC = pModem->SetTModulationType(ModulationType, 1);
	if (EC == MC_OK)
	{
		str = pModem->GetTModulationTypeName(ModulationType);
		str += "\t\t";
		m_Protocol += str;
	}
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return FALSE;
	EC = pModem->SetRModulationType(ModulationType, 1);
	if (EC == MC_OK)
	{
		str = pModem->GetRModulationTypeName(ModulationType);
		str += "\r\n";
		m_Protocol += str;
	}
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return FALSE;

	// Data rate
	dlg.SetStatus("Setting up data rate...");
	m_Protocol += "Data rate\t\t";
	int DataRateSel = m_DataRateCombo.GetCurSel();
	if (DataRateSel == CB_ERR)
	{
		MessageBox("Please set DATA RATE", "ERROR", MB_ICONSTOP);
		return FALSE;
	}
	unsigned int DataRate = m_DataRateCombo.GetItemData(DataRateSel);
	EC = pModem->SetTDataRate(DataRate, 1);
	if (EC == MC_OK)
	{
		str.Format("%d baud\t", DataRate);
		m_Protocol += str;
	}
	dlg.StepIt();
	EC = pModem->SetRDataRate(DataRate, 1);
	if (EC == MC_OK)
	{
		str.Format("%d baud\r\n", DataRate);
		m_Protocol += str;
	}
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return FALSE;

	// Fec mode
	dlg.SetStatus("Setting up code rate...");
	m_Protocol += "Fec mode\t\t";

	int FecMode = m_FecModeCombo.getSelectedMode();
	EC = pModem->SetTFecMode(FecMode, 1);
	m_Protocol += pModem->GetTFecModeName(FecMode);

	m_Protocol += "\t\t";
	dlg.StepIt();

	EC = pModem->SetRFecMode(FecMode, 1);
	m_Protocol += pModem->GetRFecModeName(FecMode);

	m_Protocol += "\r\n";
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return FALSE;

	// Output level
	dlg.SetStatus("Setting up output level...");
	m_Protocol += "Output level\t";
	double level = m_OutputLevel;
	EC = pModem->SetOutputLevel(level, 1);
	if (EC == MC_OK)
	{
		str.Format("%5.1f dBm\r\n", level);
		m_Protocol += str;
	}
	if (dlg.CheckCancelButton())
		return FALSE;

	// LNB voltage
	dlg.SetStatus("Setting up LNB voltage...");
	m_Protocol += "LNB Voltage \t";
	int LnbPowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	EC = pModem->SetRPowerSupplyMode(LnbPowerSupplyMode, 1);
	if (EC == MC_OK)
	{
		const char *pszModeName = pModem->GetRPowerSupplyModeName(LnbPowerSupplyMode);
		m_Protocol += pszModeName;
	}
	m_Protocol += "\r\n";

	// Turn modulator ON
	dlg.SetStatus("Turning modulator ON...");
	BOOL bOn = TRUE;
	pModem->TurnOutputOn(bOn, 1);
	m_Protocol += "Modulator output is ";
	if (bOn)
		m_Protocol += "ON\r\n";
	else
		m_Protocol += "OFF\r\n";
	dlg.StepIt();
	UpdateData(FALSE);
	if (dlg.CheckCancelButton())
		return FALSE;

	// Demodulator search range
	dlg.SetStatus("Setting up demodulator search range...");
	unsigned int range = 20000;
	EC = pModem->SetSearchRange(range, 1);
	if (EC == MC_OK)
	{
		str.Format("Demodulator search range = %d Hz\r\n", range);
		m_Protocol += str;
	}
	UpdateData(FALSE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPropertySheet implementation

IMPLEMENT_DYNAMIC(CModemBatchTestsPropertySheet, CAutoTestSheet)

CModemBatchTestsPropertySheet::CModemBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	AddPage(&m_DevicePage);
	AddPage(&m_MCPortPage);
	m_DevicePage.m_nDevices = nDevices;
	m_DevicePage.m_ppDevices = ppDevices;
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
	AddPage(&m_CongratulationPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig = &m_AutoTestConfig;
}

CModemBatchTestsPropertySheet::~CModemBatchTestsPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CModemBatchTestsPropertySheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CModemBatchTestsPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPropertySheet message handlers

