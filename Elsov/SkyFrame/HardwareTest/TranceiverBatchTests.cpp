// StelaMBatchTests.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PowerMeter.h"
#include "FrequencyCounter.h"
#include "TranceiverBatchTests.h"
#include "StelaM.h"
#include "Tranceiver.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const char *pszCorrection1 = "Power_Correction1";
const char *pszFrequency1 = "Power_Frequency1";
const char *pszCorrection2 = "Power_Correction2";
const char *pszFrequency2 = "Power_Frequency2";
const char *pszOutputCorrection = "Power_OutputCorrection";


CString GetDeviceFullName(CDevice *pDevice)
{
	CString str = pDevice->GetName();
	str += " (";
	str += pDevice->GetConnectionName();
	str += ")";
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsPage property page

//static
CModem *CTranceiverBatchTestsPage::m_pModem = NULL;
//static
CModem *CTranceiverBatchTestsPage::m_pModem2 = NULL;
//static
CTranceiver *CTranceiverBatchTestsPage::m_pTranceiver = NULL;
//static
CPowerMeter *CTranceiverBatchTestsPage::m_pPowerMeter = NULL;

IMPLEMENT_DYNCREATE(CTranceiverBatchTestsPage, CAutoTestPropertyPage)

CTranceiverBatchTestsPage::CTranceiverBatchTestsPage(int nIDD)
	: CAutoTestPropertyPage(nIDD)
{
	//{{AFX_DATA_INIT(CTranceiverBatchTestsPage)
	//}}AFX_DATA_INIT
}

CTranceiverBatchTestsPage::~CTranceiverBatchTestsPage()
{
}

void CTranceiverBatchTestsPage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverBatchTestsPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTranceiverBatchTestsPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CTranceiverBatchTestsPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTranceiverBatchTestsPage::TurnPureCarrier(CModem *pModem, BOOL bOnOff)
{
	if (pModem->TurnContiniousWaveOn(bOnOff, 1) != MC_OK)
	{
		CString str = "Cannot turn PURE CARRIER mode for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::Set10MHz(CModem *pModem, BOOL bOnOff)
{
	if (pModem->EnableT10MHzSupplier(bOnOff, 1) != MC_OK)
	{
		CString str = "Cannot set 10 MHz for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::Get10MHz(CModem *pModem, BOOL &bOnOff)
{
	if (pModem->IsT10MHzSupplierEnabled(bOnOff, 1) != MC_OK)
	{
		CString str = "Cannot get 10 MHz for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::SetDCVoltage(CModem *pModem, cPowerSupplyCombo &PowerSupplyCombo)
{
	int PowerSupplyMode = PowerSupplyCombo.getSelectedMode();
	BOOL bSuccess = SetDCVoltage(pModem, PowerSupplyMode);
	PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	return bSuccess;
}

BOOL CTranceiverBatchTestsPage::SetDCVoltage(CModem *pModem, int &PowerSupplyMode)
{
	if (pModem->SetTPowerSupplyMode(PowerSupplyMode, 1) != MC_OK)
	{
		CString str = "Cannot set DC OUTPUT for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::GetDCVoltage(CModem *pModem, cPowerSupplyCombo &PowerSupplyCombo)
{
	int PowerSupplyMode;
	if (pModem->GetTPowerSupplyMode(PowerSupplyMode, 1) != MC_OK)
	{
		CString str = "Cannot get DC OUTPUT for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::GetDCValues(CModem *pModem, double &voltage, double &current)
{
	CBucStatus status ;
	if (pModem->GetBucStatus(status, 1) != MC_OK)
	{
		CString str = "Cannot get DC voltage/current for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	voltage = status.m_Voltage;
	current = status.m_Current;
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::SetModemFrequency(CModem *pModem, unsigned int &Freq)
{
	if (pModem->SetTFrequency(Freq, 1) != MC_OK)
	{
		CString str = "Cannot set output frequency for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::GetModemFrequency(CModem *pModem, unsigned int &Freq)
{
	if (pModem->GetTFrequency(Freq, 1) != MC_OK)
	{
		CString str = "Cannot get output frequency for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::SetModemLevel(CModem *pModem, double &Level)
{
	if (pModem->SetOutputLevel(Level, 1) != MC_OK)
	{
		CString str = "Cannot set output level for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::GetModemLevel(CModem *pModem, double &Level)
{
	if (pModem->GetOutputLevel(Level, 1) != MC_OK)
	{
		CString str = "Cannot get output level for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::IsModemOutputOn(CModem *pModem, BOOL &bOn)
{
	if (pModem->IsOutputOn(bOn, 1) != MC_OK)
	{
		CString str = "Cannot get output state (ON/OFF) for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::TurnModemOutputOn(CModem *pModem, BOOL &bOn)
{
	if (pModem->TurnOutputOn(bOn, 1) != MC_OK)
	{
		CString str = "Cannot set output state (ON/OFF) for ";
		str += GetDeviceFullName(pModem);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::SetTranceiverFrequency(CTranceiver *pTranceiver, unsigned int &Freq)
{
	if (pTranceiver->SetFrequency(Freq) != MC_OK)
	{
		CString str = "Cannot set frequency for ";
		str += GetDeviceFullName(pTranceiver);
		MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

BOOL CTranceiverBatchTestsPage::MeasurePower(CPowerMeter *pPowerMeter, int channel, double &Power)
{
	if (pPowerMeter->MeasurePower(Power, channel) != MC_OK)
	{
		CString str = "Cannot get measured power from ";
		str += GetDeviceFullName(pPowerMeter);
		//MessageBox(str, pszError, MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsDevicePage property page

IMPLEMENT_DYNCREATE(CTranceiverBatchTestsDevicePage, CTranceiverBatchTestsPage)

CTranceiverBatchTestsDevicePage::CTranceiverBatchTestsDevicePage()
	: CTranceiverBatchTestsPage(CTranceiverBatchTestsDevicePage::IDD)
{
	//{{AFX_DATA_INIT(CTranceiverBatchTestsDevicePage)
	m_FilePath = _T("");
	//}}AFX_DATA_INIT
}

CTranceiverBatchTestsDevicePage::~CTranceiverBatchTestsDevicePage()
{
}

void CTranceiverBatchTestsDevicePage::DoDataExchange(CDataExchange *pDX)
{
	CTranceiverBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverBatchTestsDevicePage)
	DDX_Control(pDX, IDC_MODEM2_COMBO, m_Modem2Combo);
	DDX_Control(pDX, IDC_TRANCEIVER_COMBO, m_TranceiverCombo);
	DDX_Control(pDX, IDC_FREQUENCY_COUNTER_COMBO, m_FrequencyCounterCombo);
	DDX_Control(pDX, IDC_POWER_METER_COMBO, m_PowerMeterCombo);
	DDX_Control(pDX, IDC_MODEM_COMBO, m_ModemCombo);
	DDX_Text(pDX, IDC_FILE_PATH_EDIT, m_FilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTranceiverBatchTestsDevicePage, CTranceiverBatchTestsPage)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsDevicePage)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsDevicePage message handlers

BOOL CTranceiverBatchTestsDevicePage::OnInitDialog() 
{
	CTranceiverBatchTestsPage::OnInitDialog();
	
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
			index = m_Modem2Combo.AddString(Name);
			m_Modem2Combo.SetItemDataPtr(index, pDevice);
		}
		if (pDevice->IsTranceiver())
		{
			// Tranceiver to be tested
			CString Name = pDevice->GetName();
			Name += " (";
			Name += pDevice->GetConnectionName();
			int index = m_TranceiverCombo.AddString(Name + ")");
			m_TranceiverCombo.SetItemDataPtr(index, pDevice);
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
	if (m_TranceiverCombo.GetCount() == 1)
		m_TranceiverCombo.SetCurSel(0);
	if (m_ModemCombo.GetCount() == 1)
		m_ModemCombo.SetCurSel(0);
	if (m_Modem2Combo.GetCount() == 1)
		m_Modem2Combo.SetCurSel(0);
	if (m_PowerMeterCombo.GetCount() == 1)
		m_PowerMeterCombo.SetCurSel(0);
	if (m_FrequencyCounterCombo.GetCount() == 1)
		m_FrequencyCounterCombo.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTranceiverBatchTestsDevicePage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CTranceiverBatchTestsPage::OnSetActive();
}

BOOL CTranceiverBatchTestsDevicePage::OnKillActive() 
{
	return CTranceiverBatchTestsPage::OnKillActive();
}

LRESULT CTranceiverBatchTestsDevicePage::OnWizardNext() 
{
	UpdateData(FALSE);
	if (m_FilePath.IsEmpty())
	{
		MessageBox("Please specify report file", pszError, MB_ICONEXCLAMATION);
		return -1;	// to prevent going ahead
	}
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 5);
	dlg.SetPos(0);

	// Modem1
	dlg.SetStatus("Connecting to modems");
	int index = m_ModemCombo.GetCurSel();
	if (index == CB_ERR)
	{
		MessageBox("Please specify Modem1", pszError, MB_ICONEXCLAMATION);
		return -1;	// to prevent going ahead
	}
	m_pModem = (CModem *)m_ModemCombo.GetItemDataPtr(index);
	ASSERT(m_pModem);
	m_pModem->Connect();
	dlg.StepIt();
	
	int PowerSupplyMode = 0;
	// Modem2
	BOOL bOn = FALSE;
	index = m_Modem2Combo.GetCurSel();
	if (index == CB_ERR)
	{
		MessageBox("Secondary modem is not specified - intermodulation test will be skipped", pszWarning, MB_ICONINFORMATION);
		m_pModem2 = NULL;
	}
	else
	{
		m_pModem2 = (CModem *)m_Modem2Combo.GetItemDataPtr(index);
		ASSERT(m_pModem2);
		m_pModem2->Connect();
		// Turn secondary modem OFF
		Set10MHz(m_pModem2, FALSE);
		PowerSupplyMode = 0;
		SetDCVoltage(m_pModem2, PowerSupplyMode);
		TurnModemOutputOn(m_pModem2, bOn);
	}
	// Turn primary modem ON
	Set10MHz(m_pModem, TRUE);
	PowerSupplyMode = 1;
	SetDCVoltage(m_pModem, PowerSupplyMode);
	bOn = TRUE;
	TurnModemOutputOn(m_pModem, bOn);
	dlg.StepIt();
	
	// Tranceiver to be tested
	dlg.SetStatus("Connecting to tranceiver");
	index = m_TranceiverCombo.GetCurSel();
	if (index == CB_ERR)
	{
		MessageBox("Please specify tranceiver", pszError, MB_ICONEXCLAMATION);
		return -1;	// to prevent going ahead
	}
	m_pTranceiver = (CTranceiver *)m_TranceiverCombo.GetItemDataPtr(index);
	ASSERT(m_pTranceiver);
	m_pTranceiver->Connect();
	dlg.StepIt();

	// Power meter
	dlg.SetStatus("Connecting to power meter");
	index = m_PowerMeterCombo.GetCurSel();
	if (index == CB_ERR)
	{
		MessageBox("Please specify power meter", pszError, MB_ICONEXCLAMATION);
		return -1;	// to prevent going ahead
	}
	m_pPowerMeter = (CPowerMeter *)m_PowerMeterCombo.GetItemDataPtr(index);
	ASSERT(m_pPowerMeter);
	if (!m_pPowerMeter->IsHuman())
		m_pPowerMeter->Connect();
	dlg.StepIt();

	// Frequency counter
	dlg.SetStatus("Connecting to frequency counter");
	index = m_FrequencyCounterCombo.GetCurSel();
	if (index == CB_ERR)
	{
		MessageBox("Please specify frequency counter", pszError, MB_ICONEXCLAMATION);
		return -1;	// to prevent going ahead
	}
	CFrequencyCounter *pFrequencyCounter = (CFrequencyCounter *)m_FrequencyCounterCombo.GetItemDataPtr(index);
	ASSERT(pFrequencyCounter);
	m_pAutoTestConfig->m_pFrequencyCounter = pFrequencyCounter;
	if (!pFrequencyCounter->IsHuman())
		pFrequencyCounter->Connect();
	dlg.StepIt();

	// Open file and write header
	CStdioFile *pFile = GetReportFile();
	if (pFile)
	{
		char *pszStr="Tranceiver investigation\n";
		WriteString(m_pAutoTestConfig->m_pReportFile, pszStr);

		// Time
		CTime time = CTime::GetCurrentTime();
		CString string = time.Format("Date/Time: %d-%m-%Y %H:%M:%S\n\n");
		WriteString(m_pAutoTestConfig->m_pReportFile, (char *)LPCTSTR(string));
	}

	return CTranceiverBatchTestsPage::OnWizardNext();
}

void CTranceiverBatchTestsDevicePage::OnBrowseButton() 
{
	UpdateData();
	CFileDialog fd(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Text files(*txt)|*txt|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Specify file to write report";
	if (fd.DoModal()==IDCANCEL) return;
	m_FilePath = fd.GetPathName();
	UpdateData(FALSE);
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsRemoteControlPage property page

IMPLEMENT_DYNCREATE(CTranceiverBatchTestsRemoteControlPage, CTranceiverBatchTestsPage)

CTranceiverBatchTestsRemoteControlPage::CTranceiverBatchTestsRemoteControlPage()
	: CTranceiverBatchTestsPage(CTranceiverBatchTestsRemoteControlPage::IDD)
{
	//{{AFX_DATA_INIT(CTranceiverBatchTestsRemoteControlPage)
	m_Protocol = _T("");
	m_InputLevelFrom = 0.0;
	m_InputLevelStep = 0.0;
	m_InputLevelTo = 0.0;
	m_FreqFrom = 0;
	m_FreqStep = 0;
	m_FreqTo = 0;
	m_Correction1 = 0.0;
	m_Correction2 = 0.0;
	m_Frequency1 = 0;
	m_Frequency2 = 0;
	m_10MHzLevel = 0.0;
	m_b10MHzEnabled = FALSE;
	m_OutputCorrection = 0.0;
	//}}AFX_DATA_INIT
}

CTranceiverBatchTestsRemoteControlPage::~CTranceiverBatchTestsRemoteControlPage()
{
}

void CTranceiverBatchTestsRemoteControlPage::DoDataExchange(CDataExchange *pDX)
{
	CTranceiverBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverBatchTestsRemoteControlPage)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_AMPLITUDE_FREQ__PROTOCOL_EDIT, m_ProtocolCtrl);
	DDX_Text(pDX, IDC_AMPLITUDE_FREQ__PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_FROM_EDIT, m_InputLevelFrom);
	DDX_Text(pDX, IDC_STEP_EDIT, m_InputLevelStep);
	DDX_Text(pDX, IDC_TO_EDIT, m_InputLevelTo);
	DDX_Text(pDX, IDC_FREQ_FROM_EDIT, m_FreqFrom);
	DDX_Text(pDX, IDC_FREQ_STEP_EDIT, m_FreqStep);
	DDX_Text(pDX, IDC_FREQ_TO_EDIT, m_FreqTo);
	DDX_Text(pDX, IDC_CORRECTION1_EDIT, m_Correction1);
	DDX_Text(pDX, IDC_CORRECTION2_EDIT, m_Correction2);
	DDX_Text(pDX, IDC_FREQUENCY1_EDIT, m_Frequency1);
	DDX_Text(pDX, IDC_FREQUENCY2_EDIT, m_Frequency2);
	DDX_Text(pDX, IDC_10MHZ_LEVEL_EDIT, m_10MHzLevel);
	DDX_Check(pDX, IDC_10MHZ_ENABLED_CHECK, m_b10MHzEnabled);
	DDX_Text(pDX, IDC_OUTPUT_CORRECTION_EDIT, m_OutputCorrection);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTranceiverBatchTestsRemoteControlPage, CTranceiverBatchTestsPage)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsRemoteControlPage)
	ON_BN_CLICKED(IDC_RUN_TEST_BUTTON, OnRunTestButton)
	ON_BN_CLICKED(IDC_CHANGE_AMENDMENT_BUTTON, OnChangeAmendmentButton)
	ON_BN_CLICKED(IDC_10MHZ_ENABLED_CHECK, On10MHzEnabledCheck)
	ON_BN_CLICKED(IDC_CHANGE_OUTPUT_CORRECTION_BUTTON, OnChangeOutputCorrectionButton)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTranceiverBatchTestsRemoteControlPage::UpdateDCValues()
{
	CWaitCursor cursor;
	char szBuffer[16];
	double U,I;
	GetDCValues(m_pModem, U, I);
	sprintf(szBuffer, "U = %6.2f V", U);
	GetDescendantWindow(IDC_U_STATIC)->SetWindowText(szBuffer);
	sprintf(szBuffer, "I = %6.2f A", I);
	GetDescendantWindow(IDC_I_STATIC)->SetWindowText(szBuffer);
}

void CTranceiverBatchTestsRemoteControlPage::UpdateModemControls()
{
	CWaitCursor cursor;
	GetDCVoltage(m_pModem, m_PowerSupplyCombo);
	Get10MHz(m_pModem, m_b10MHzEnabled);
	UpdateDCValues();
	GetDescendantWindow(IDC_10MHZ_LEVEL_EDIT)->EnableWindow(!m_b10MHzEnabled);
	
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsRemoteControlPage message handlers


const char *pszRemoteCtrl_InputLevelFrom = "RemoteControl_InputLevelFrom";
const char *pszRemoteCtrl_InputLevelTo = "RemoteControl_InputLevelTo";
const char *pszRemoteCtrl_InputLevelStep = "RemoteControl_InputLevelStep";
const char *pszRemoteCtrl_FreqFrom = "RemoteControl_FrequencyFrom";
const char *pszRemoteCtrl_FreqTo = "RemoteControl_FrequencyTo";
const char *pszRemoteCtrl_FreqStep = "RemoteControl_FrequencyStep";

BOOL CTranceiverBatchTestsRemoteControlPage::OnInitDialog() 
{
	CTranceiverBatchTestsPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTranceiverBatchTestsRemoteControlPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	int iValue = ReadRegistryInt(pszRemoteCtrl_InputLevelFrom, (int)(m_pModem->GetMinOutputLevel()*10));
	m_InputLevelFrom = iValue/10.;
	iValue = ReadRegistryInt(pszRemoteCtrl_InputLevelTo, (int)(m_pModem->GetMaxOutputLevel()*10));
	m_InputLevelTo = iValue/10.;
	iValue = ReadRegistryInt(pszRemoteCtrl_InputLevelStep, 10*10);
	m_InputLevelStep = iValue/10.;

	iValue = ReadRegistryInt(pszCorrection1, 0*10);
	m_Correction1 = iValue/10.;
	m_Frequency1 = ReadRegistryInt(pszFrequency1, m_pModem->GetMinTFrequency()/1000);
	iValue = ReadRegistryInt(pszCorrection2, 0*10);
	m_Correction2 = iValue/10.;
	m_Frequency2 = ReadRegistryInt(pszFrequency2, m_pModem->GetMaxTFrequency()/1000);
	iValue = ReadRegistryInt(pszOutputCorrection, 0*10);
	m_OutputCorrection = iValue/10.;

	m_FreqFrom = ReadRegistryInt(pszRemoteCtrl_FreqFrom, m_pModem->GetMinTFrequency()/1000);
	m_FreqTo = ReadRegistryInt(pszRemoteCtrl_FreqTo, m_pModem->GetMaxTFrequency()/1000);
	m_FreqStep = ReadRegistryInt(pszRemoteCtrl_FreqStep, 50);

	m_PowerSupplyCombo.initT(m_pModem, 1);

	UpdateModemControls();
	UpdateData(FALSE);
	return CTranceiverBatchTestsPage::OnSetActive();
}


void CTranceiverBatchTestsRemoteControlPage::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

void CTranceiverBatchTestsRemoteControlPage::On10MHzEnabledCheck() 
{
	UpdateData(TRUE);
	Set10MHz(m_pModem, m_b10MHzEnabled);
	UpdateModemControls();
	UpdateData(FALSE);
}

BOOL CTranceiverBatchTestsRemoteControlPage::DoTest(unsigned int Frequency, CProgressDlg *pDlg)
{
	const char *pszFreqInputLevelFormat = "Frequency = %4d MHz, Input = %6.1f dBm";
	const char *pszFreqInputOutputLevelFormat = "Frequency = %d MHz, Input = %6.1f dBm, Output %6.1f/%6.1f dBm";

	// Tranceiver frequency
	unsigned int TranceiverFreq = Frequency;
	if (!SetTranceiverFrequency(m_pTranceiver, TranceiverFreq))
		return FALSE;

	Sleep(1000);
	
	char szReport[256];
	double Correction = CalcCorrection(m_Frequency1, m_Correction1, m_Frequency2, m_Correction2, Frequency);
	sprintf(szReport, "\r\nInput frequency = %d MHz\tOutput frequency = %d MHz\r\nCorrection = %6.2f dBm\r\n",
		Frequency, Frequency + m_pTranceiver->GetBandShift(), Correction);
	m_Protocol += szReport;
	// Buc voltage and current
	double voltage, current;
	GetDCValues(m_pModem, voltage, current);
	sprintf(szReport, "Ubuc = %6.2f V\t\tIbuc = %6.2f A\r\n", voltage, current);
	m_Protocol += szReport;
	UpdateDCValues();
	// 10 MHz
	CString str10Mhz;
	BOOL bOn = FALSE;
	Get10MHz(m_pModem, bOn);
	if (bOn)
		str10Mhz = "standard modem";
	else
		str10Mhz.Format("%6.1f dBm", m_10MHzLevel);
	m_Protocol += "10 MHz level : ";
	m_Protocol += str10Mhz;
	m_Protocol += "\r\n";


	sprintf(szReport, "\r\n Pinput\t Pmeter\t Ptmi\t Pdelta\t Gmeter\t Gtmi\t  t,°C\r\n");
	m_Protocol += szReport;
	UpdateData(FALSE);

	if (pDlg && pDlg->CheckCancelButton())
	{
		if (DoYouReallyWantToCancel(this))
			return FALSE;
	}

	unsigned int ModemFreq = Frequency*1000; // in KHz
	if (!SetModemFrequency(m_pModem, ModemFreq))
		return FALSE;

	if (pDlg)
		pDlg->SetStatus(szReport);

	double level = m_InputLevelFrom;
	while (level <= (m_InputLevelTo+0.001))
	{
		if (pDlg && pDlg->CheckCancelButton())
		{
			if (DoYouReallyWantToCancel(this))
				return FALSE;
		}
		
		sprintf(szReport, pszFreqInputLevelFormat, Frequency, level);
		if (pDlg)
			pDlg->SetStatus(szReport);

		if (!SetModemLevel(m_pModem, level))
			return FALSE;

		// 2 sec delay to get stable measurement
		for (int i = 0; i < 10; i++)
		{
			if (pDlg && pDlg->CheckCancelButton())
			{
				if (DoYouReallyWantToCancel(this))
					return FALSE;
			}
			Sleep(200);		
		}

		DWORD StartTime = GetTickCount();
		double PowerReadAverage = 0, PowerMeasuredAverage = 0;
		double GainReadAverage = 0, GainMeasuredAverage = 0;
		int nMeasurements = 0;
		do
		{
			if (pDlg && pDlg->CheckCancelButton())
			{
				if (DoYouReallyWantToCancel(this))
					return FALSE;
			}
		
			double PowerRead, PowerMeasured;
			if (m_pTranceiver->GetPower(PowerRead) != MC_OK)
			{
				MessageBox("Cannot get power from tranceiver. Tests interrupted.", pszError, MB_ICONSTOP);
				return FALSE;
			}
			PowerReadAverage += PowerRead;
			if (!MeasurePower(m_pPowerMeter, 1, PowerMeasured))
				return FALSE;
			PowerMeasured += m_OutputCorrection;
			PowerMeasuredAverage += PowerMeasured;

			double GainRead, GainMeasured;
			if (m_pTranceiver->GetGain(GainRead) != MC_OK)
			{
				MessageBox("Cannot get gain from tranceiver. Tests interrupted.", pszError, MB_ICONSTOP);
				return FALSE;
			}
			GainReadAverage += GainRead;
			GainMeasured = PowerMeasured - level - Correction;
			GainMeasuredAverage += GainMeasured;

			nMeasurements++;
			sprintf(szReport, pszFreqInputOutputLevelFormat, Frequency, level, PowerMeasured, PowerRead);
			if (pDlg)
				pDlg->SetStatus(szReport);
		} while ((GetTickCount() - StartTime) < 5000);

		PowerReadAverage /= nMeasurements;
		PowerMeasuredAverage /= nMeasurements;
		GainReadAverage /= nMeasurements;
		GainMeasuredAverage /= nMeasurements;
		double T = -273;
		m_pTranceiver->GetTemperature(T);
		
		sprintf(szReport, "%6.1f\t%6.1f\t%6.1f\t%6.1f\t%6.1f\t%6.1f\t%5.0f\r\n", level, PowerMeasuredAverage, PowerReadAverage, PowerMeasuredAverage-PowerReadAverage, GainMeasuredAverage, GainReadAverage, T);
		m_Protocol += szReport;
		UpdateData(FALSE);

		int nLines = m_ProtocolCtrl.GetLineCount();
		m_ProtocolCtrl.LineScroll(nLines);		
	
		if (pDlg)
		{
			sprintf(szReport, pszFreqInputOutputLevelFormat, Frequency, level, PowerMeasuredAverage, PowerReadAverage);
			pDlg->SetStatus(szReport);
			pDlg->StepIt();
			Sleep(1000);
		}
		level += m_InputLevelStep;
	}

	return TRUE;
}

void CTranceiverBatchTestsRemoteControlPage::OnRunTestButton() 
{
	UpdateData(TRUE);
	// Check input
	if (m_InputLevelFrom < m_pModem->GetMinOutputLevel())
		m_InputLevelFrom = m_pModem->GetMinOutputLevel();
	if (m_InputLevelFrom > m_pModem->GetMaxOutputLevel())
		m_InputLevelFrom = m_pModem->GetMaxOutputLevel();
	if (m_InputLevelTo < m_pModem->GetMinOutputLevel())
		m_InputLevelTo = m_pModem->GetMinOutputLevel();
	if (m_InputLevelTo > m_pModem->GetMaxOutputLevel())
		m_InputLevelTo = m_pModem->GetMaxOutputLevel();
	if (m_InputLevelStep < 0.1)
		m_InputLevelStep = 0.1;

	if (m_FreqFrom < m_pModem->GetMinTFrequency()/1000)
		m_FreqFrom = m_pModem->GetMinTFrequency()/1000;
	if (m_FreqTo > m_pModem->GetMaxTFrequency()/1000)
		m_FreqTo = m_pModem->GetMaxTFrequency()/1000;

	m_Protocol = "Remote control test:\r\n";
	UpdateData(FALSE);
	
	WriteRegistryInt(pszRemoteCtrl_InputLevelFrom, (int)(m_InputLevelFrom*10));
	WriteRegistryInt(pszRemoteCtrl_InputLevelTo, (int)(m_InputLevelTo*10));
	WriteRegistryInt(pszRemoteCtrl_InputLevelStep, (int)(m_InputLevelStep*10));

	WriteRegistryInt(pszRemoteCtrl_FreqFrom, m_FreqFrom);
	WriteRegistryInt(pszRemoteCtrl_FreqTo, m_FreqTo);
	WriteRegistryInt(pszRemoteCtrl_FreqStep, m_FreqStep);
	
	CProgressDlg dlg;
	dlg.Create();
	int nMeasurements = 0, nFreqs = 0;
	double level = m_InputLevelFrom;
	while (level <= m_InputLevelTo)
	{
		nMeasurements++;
		level += m_InputLevelStep;
	}
	unsigned int freq = m_FreqFrom;
	while (freq <= m_FreqTo)
	{
		nFreqs++;
		freq += m_FreqStep;
	}
	nMeasurements *= nFreqs;
	dlg.SetRange(0, nMeasurements);
	dlg.SetPos(0);

	UpdateData(FALSE);

	freq = m_FreqFrom;
	while (freq <= m_FreqTo)
	{
		if (!DoTest(freq, &dlg))
		{
			m_Protocol += "\r\nTest cancelled!\r\n";
			break;
		}
		freq += m_FreqStep;
	}

	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);
}

void CTranceiverBatchTestsRemoteControlPage::OnChangeAmendmentButton() 
{
	UpdateData();
	ChangeFrequencyDependentCorrection(m_Frequency1, m_Correction1, m_Frequency2, m_Correction2);
	WriteRegistryInt(pszCorrection1, (int)(m_Correction1*10));
	WriteRegistryInt(pszFrequency1, (int)(m_Frequency1));
	WriteRegistryInt(pszCorrection2, (int)(m_Correction2*10));
	WriteRegistryInt(pszFrequency2, (int)(m_Frequency2));
}


void CTranceiverBatchTestsRemoteControlPage::OnChangeOutputCorrectionButton() 
{
	UpdateData();
	ChangeCorrection(m_OutputCorrection);
	WriteRegistryInt(pszOutputCorrection, (int)(m_OutputCorrection*10));
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsAmplitudeFrequencyPage property page

IMPLEMENT_DYNCREATE(CTranceiverBatchTestsAmplitudeFrequencyPage, CTranceiverBatchTestsPage)

CTranceiverBatchTestsAmplitudeFrequencyPage::CTranceiverBatchTestsAmplitudeFrequencyPage()
	: CTranceiverBatchTestsPage(CTranceiverBatchTestsAmplitudeFrequencyPage::IDD)
{
	//{{AFX_DATA_INIT(CTranceiverBatchTestsAmplitudeFrequencyPage)
	m_Protocol = _T("");
	m_InputLevelFrom = 0.0;
	m_InputLevelStep = 0.0;
	m_InputLevelTo = 0.0;
	m_FreqFrom = 0;
	m_FreqStep = 0;
	m_FreqTo = 0;
	m_Correction1 = 0.0;
	m_Correction2 = 0.0;
	m_Frequency1 = 0;
	m_Frequency2 = 0;
	m_b10MHzEnabled = FALSE;
	m_10MHzLevel = 0.0;
	m_OutputCorrection = 0.0;
	//}}AFX_DATA_INIT
}

CTranceiverBatchTestsAmplitudeFrequencyPage::~CTranceiverBatchTestsAmplitudeFrequencyPage()
{
}

void CTranceiverBatchTestsAmplitudeFrequencyPage::DoDataExchange(CDataExchange *pDX)
{
	CTranceiverBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverBatchTestsAmplitudeFrequencyPage)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_AMPLITUDE_FREQ__PROTOCOL_EDIT, m_ProtocolCtrl);
	DDX_Text(pDX, IDC_AMPLITUDE_FREQ__PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_FROM_EDIT, m_InputLevelFrom);
	DDX_Text(pDX, IDC_STEP_EDIT, m_InputLevelStep);
	DDX_Text(pDX, IDC_TO_EDIT, m_InputLevelTo);
	DDX_Text(pDX, IDC_FREQ_FROM_EDIT, m_FreqFrom);
	DDX_Text(pDX, IDC_FREQ_STEP_EDIT, m_FreqStep);
	DDX_Text(pDX, IDC_FREQ_TO_EDIT, m_FreqTo);
	DDX_Text(pDX, IDC_CORRECTION1_EDIT, m_Correction1);
	DDX_Text(pDX, IDC_CORRECTION2_EDIT, m_Correction2);
	DDX_Text(pDX, IDC_FREQUENCY1_EDIT, m_Frequency1);
	DDX_Text(pDX, IDC_FREQUENCY2_EDIT, m_Frequency2);
	DDX_Check(pDX, IDC_10MHZ_ENABLED_CHECK, m_b10MHzEnabled);
	DDX_Text(pDX, IDC_10MHZ_LEVEL_EDIT, m_10MHzLevel);
	DDX_Text(pDX, IDC_OUTPUT_CORRECTION_EDIT, m_OutputCorrection);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTranceiverBatchTestsAmplitudeFrequencyPage, CTranceiverBatchTestsPage)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsAmplitudeFrequencyPage)
	ON_BN_CLICKED(IDC_RUN_TEST_BUTTON, OnRunTestButton)
	ON_BN_CLICKED(IDC_CHANGE_AMENDMENT_BUTTON, OnChangeAmendmentButton)
	ON_BN_CLICKED(IDC_10MHZ_ENABLED_CHECK, On10MHzEnabledCheck)
	ON_BN_CLICKED(IDC_CHANGE_OUTPUT_CORRECTION_BUTTON, OnChangeOutputCorrectionButton)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTranceiverBatchTestsAmplitudeFrequencyPage::UpdateDCValues()
{
	CWaitCursor cursor;
	char szBuffer[16];
	double U,I;
	GetDCValues(m_pModem, U, I);
	sprintf(szBuffer, "U = %6.2f V", U);
	GetDescendantWindow(IDC_U_STATIC)->SetWindowText(szBuffer);
	sprintf(szBuffer, "I = %6.2f A", I);
	GetDescendantWindow(IDC_I_STATIC)->SetWindowText(szBuffer);
}

void CTranceiverBatchTestsAmplitudeFrequencyPage::UpdateModemControls()
{
	CWaitCursor cursor;
	GetDCVoltage(m_pModem, m_PowerSupplyCombo);
	Get10MHz(m_pModem, m_b10MHzEnabled);
	UpdateDCValues();
	GetDescendantWindow(IDC_10MHZ_LEVEL_EDIT)->EnableWindow(!m_b10MHzEnabled);
	
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsAmplitudeFrequencyPage message handlers

const char *pszAmplitudeFreq_InputLevelFrom = "AmplitudeFrequency_InputLevelFrom";
const char *pszAmplitudeFreq_InputLevelTo = "AmplitudeFrequency_InputLevelTo";
const char *pszAmplitudeFreq_InputLevelStep = "AmplitudeFrequency_InputLevelStep";
const char *pszAmplitudeFreq_FreqFrom = "AmplitudeFrequency_FrequencyFrom";
const char *pszAmplitudeFreq_FreqTo = "AmplitudeFrequency_FrequencyTo";
const char *pszAmplitudeFreq_FreqStep = "AmplitudeFrequency_FrequencyStep";

BOOL CTranceiverBatchTestsAmplitudeFrequencyPage::OnInitDialog() 
{
	CTranceiverBatchTestsPage::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTranceiverBatchTestsAmplitudeFrequencyPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	int iValue = ReadRegistryInt(pszAmplitudeFreq_InputLevelFrom, (int)(m_pModem->GetMinOutputLevel()*10));
	m_InputLevelFrom = iValue/10.;
	iValue = ReadRegistryInt(pszAmplitudeFreq_InputLevelTo, (int)(m_pModem->GetMaxOutputLevel()*10));
	m_InputLevelTo = iValue/10.;
	iValue = ReadRegistryInt(pszAmplitudeFreq_InputLevelStep, 1*10);
	m_InputLevelStep = iValue/10.;

	m_FreqFrom = ReadRegistryInt(pszAmplitudeFreq_FreqFrom, m_pModem->GetMinTFrequency()/1000);
	m_FreqTo = ReadRegistryInt(pszAmplitudeFreq_FreqTo, m_pModem->GetMaxTFrequency()/1000);
	m_FreqStep = ReadRegistryInt(pszAmplitudeFreq_FreqStep, 50);

	iValue = ReadRegistryInt(pszCorrection1, 0*10);
	m_Correction1 = iValue/10.;
	m_Frequency1 = ReadRegistryInt(pszFrequency1, m_pModem->GetMinTFrequency()/1000);
	iValue = ReadRegistryInt(pszCorrection2, 0*10);
	m_Correction2 = iValue/10.;
	m_Frequency2 = ReadRegistryInt(pszFrequency2, m_pModem->GetMaxTFrequency()/1000);
	iValue = ReadRegistryInt(pszOutputCorrection, 0*10);
	m_OutputCorrection = iValue/10.;

	m_PowerSupplyCombo.initT(m_pModem, 1);

	UpdateModemControls();
	UpdateData(FALSE);
	return CTranceiverBatchTestsPage::OnSetActive();
}

void CTranceiverBatchTestsAmplitudeFrequencyPage::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

BOOL CTranceiverBatchTestsAmplitudeFrequencyPage::DoTest(double Level, CProgressDlg *pDlg)
{
	const char *pszFreqInputLevelFormat = "Frequency = %4d MHz, Input = %6.1f dBm";
	const char *pszFreqInputOutputLevelFormat = "Frequency = %d MHz, Input = %6.1f dBm, Output = %6.1f dBm, Gain = %6.1f dBm";

	// Modem output level
	if (!SetModemLevel(m_pModem, Level))
		return FALSE;

	Sleep(1000);
	
	char szReport[256];
	sprintf(szReport, "\r\nLevel = %6.2f dBm\r\n", Level);
	m_Protocol += szReport;
	// Buc voltage and current
	double voltage, current;
	GetDCValues(m_pModem, voltage, current);
	sprintf(szReport, "Ubuc = %6.2f V\t\tIbuc = %6.2f A\r\n", voltage, current);
	m_Protocol += szReport;
	UpdateDCValues();
	// 10 MHz
	CString str10Mhz;
	BOOL bOn = FALSE;
	Get10MHz(m_pModem, bOn);
	if (bOn)
		str10Mhz = "standard modem";
	else
		str10Mhz.Format("%6.1f dBm", m_10MHzLevel);
	m_Protocol += "10 MHz level : ";
	m_Protocol += str10Mhz;
	m_Protocol += "\r\n";

	sprintf(szReport, "\r\nFrIn\tFrOut\tCorr.\t Pmeter\t Gmeter\r\n");
	m_Protocol += szReport;
	UpdateData(FALSE);

	if (pDlg && pDlg->CheckCancelButton())
	{
		if (DoYouReallyWantToCancel(this))
			return FALSE;
	}

	if (pDlg)
		pDlg->SetStatus(szReport);

	unsigned int Frequency = m_FreqFrom;
	while (Frequency <= m_FreqTo)
	{
		if (pDlg && pDlg->CheckCancelButton())
		{
			if (DoYouReallyWantToCancel(this))
				return FALSE;
		}
		
		sprintf(szReport, pszFreqInputLevelFormat, Frequency, Level);
		if (pDlg)
			pDlg->SetStatus(szReport);

		// Frequencies
		unsigned int TranceiverFreq = Frequency;
		if (!SetTranceiverFrequency(m_pTranceiver, TranceiverFreq))
			return FALSE;
		unsigned int ModemFreq = Frequency*1000; // in KHz
		if (!SetModemFrequency(m_pModem, ModemFreq))
			return FALSE;


		double Correction = CalcCorrection(m_Frequency1, m_Correction1, m_Frequency2, m_Correction2, Frequency);
		// 2 sec delay to get stable measurement
		for (int i = 0; i < 10; i++)
		{
			if (pDlg && pDlg->CheckCancelButton())
			{
				if (DoYouReallyWantToCancel(this))
					return FALSE;
			}
			Sleep(200);		
		}

		DWORD StartTime = GetTickCount();
		double PowerMeasuredAverage = 0;
		double GainMeasuredAverage = 0;
		int nMeasurements = 0;
		do
		{
			if (pDlg && pDlg->CheckCancelButton())
			{
				if (DoYouReallyWantToCancel(this))
					return FALSE;
			}
		
			double PowerMeasured;
			if (!MeasurePower(m_pPowerMeter, 1, PowerMeasured))
				return FALSE;
			PowerMeasured += m_OutputCorrection;
			PowerMeasuredAverage += PowerMeasured;

			double GainMeasured = PowerMeasured - Level - Correction;
			GainMeasuredAverage += GainMeasured;

			nMeasurements++;
			sprintf(szReport, pszFreqInputOutputLevelFormat, Frequency, Level, PowerMeasured, GainMeasured);
			if (pDlg)
				pDlg->SetStatus(szReport);
		} while ((GetTickCount() - StartTime) < 5000);

		GainMeasuredAverage /= nMeasurements;
		PowerMeasuredAverage /= nMeasurements;
		sprintf(szReport, "%d\t%d\t%6.2f\t%6.1f\t%6.1f\r\n",
			Frequency, Frequency+m_pTranceiver->GetBandShift(), Correction, PowerMeasuredAverage, GainMeasuredAverage);
		m_Protocol += szReport;
		UpdateData(FALSE);

		int nLines = m_ProtocolCtrl.GetLineCount();
		m_ProtocolCtrl.LineScroll(nLines);		
	
		sprintf(szReport, pszFreqInputOutputLevelFormat, Frequency, Level, PowerMeasuredAverage, GainMeasuredAverage);
		if (pDlg)
		{
			pDlg->SetStatus(szReport);
			pDlg->StepIt();
			Sleep(1000);
		}
		Frequency += m_FreqStep;
	}

	return TRUE;
}

void CTranceiverBatchTestsAmplitudeFrequencyPage::OnRunTestButton() 
{
	UpdateData(TRUE);
	// Check input
	if (m_InputLevelFrom < m_pModem->GetMinOutputLevel())
		m_InputLevelFrom = m_pModem->GetMinOutputLevel();
	if (m_InputLevelFrom > m_pModem->GetMaxOutputLevel())
		m_InputLevelFrom = m_pModem->GetMaxOutputLevel();
	if (m_InputLevelTo < m_pModem->GetMinOutputLevel())
		m_InputLevelTo = m_pModem->GetMinOutputLevel();
	if (m_InputLevelTo > m_pModem->GetMaxOutputLevel())
		m_InputLevelTo = m_pModem->GetMaxOutputLevel();
	if (m_InputLevelStep < 0.1)
		m_InputLevelStep = 0.1;

	if (m_FreqFrom < m_pModem->GetMinTFrequency()/1000)
		m_FreqFrom = m_pModem->GetMinTFrequency()/1000;
	if (m_FreqTo > m_pModem->GetMaxTFrequency()/1000)
		m_FreqTo = m_pModem->GetMaxTFrequency()/1000;
	
	m_Protocol = "Amplitude-frequency test:\r\n";
	UpdateData(FALSE);
	
	WriteRegistryInt(pszAmplitudeFreq_InputLevelFrom, (int)(m_InputLevelFrom*10));
	WriteRegistryInt(pszAmplitudeFreq_InputLevelTo, (int)(m_InputLevelTo*10));
	WriteRegistryInt(pszAmplitudeFreq_InputLevelStep, (int)(m_InputLevelStep*10));

	WriteRegistryInt(pszAmplitudeFreq_FreqFrom, m_FreqFrom);
	WriteRegistryInt(pszAmplitudeFreq_FreqTo, m_FreqTo);
	WriteRegistryInt(pszAmplitudeFreq_FreqStep, m_FreqStep);
	
	CProgressDlg dlg;
	dlg.Create();
	int nMeasurements = 0, nFreqs = 0;
	double level = m_InputLevelFrom;
	while (level <= m_InputLevelTo)
	{
		nMeasurements++;
		level += m_InputLevelStep;
	}
	unsigned int freq = m_FreqFrom;
	while (freq <= m_FreqTo)
	{
		nFreqs++;
		freq += m_FreqStep;
	}
	nMeasurements *= nFreqs;
	dlg.SetRange(0, nMeasurements);
	dlg.SetPos(0);

	UpdateData(FALSE);

	level = m_InputLevelFrom;
	while (level <= m_InputLevelTo)
	{
		if (!DoTest(level, &dlg))
		{
			m_Protocol += "\r\nTest cancelled!\r\n";
			break;
		}
		level += m_InputLevelStep;
	}

	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);
}

void CTranceiverBatchTestsAmplitudeFrequencyPage::OnChangeAmendmentButton() 
{
	UpdateData();
	ChangeFrequencyDependentCorrection(m_Frequency1, m_Correction1, m_Frequency2, m_Correction2);
	WriteRegistryInt(pszCorrection1, (int)(m_Correction1*10));
	WriteRegistryInt(pszFrequency1, (int)(m_Frequency1));
	WriteRegistryInt(pszCorrection2, (int)(m_Correction2*10));
	WriteRegistryInt(pszFrequency2, (int)(m_Frequency2));
}


void CTranceiverBatchTestsAmplitudeFrequencyPage::OnChangeOutputCorrectionButton() 
{
	UpdateData();
	ChangeCorrection(m_OutputCorrection);
	WriteRegistryInt(pszOutputCorrection, (int)(m_OutputCorrection*10));
}

void CTranceiverBatchTestsAmplitudeFrequencyPage::On10MHzEnabledCheck() 
{
	UpdateData(TRUE);
	Set10MHz(m_pModem, m_b10MHzEnabled);
	UpdateModemControls();
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsSpuriousLevelPage dialog


CTranceiverBatchTestsSpuriousLevelPage::CTranceiverBatchTestsSpuriousLevelPage()
	: CTranceiverBatchTestsPage(CTranceiverBatchTestsSpuriousLevelPage::IDD)
{
	//{{AFX_DATA_INIT(CTranceiverBatchTestsSpuriousLevelPage)
	m_InputFreq = 0;
	m_InputLevel = 0.0;
	m_SpuriousLevel = 0.0;
	m_Protocol = _T("");
	m_OutputFreq = 0;
	m_OutputLevel = 0.0;
	m_b10MHzEnabled = FALSE;
	m_10MHzLevel = 0.0;
	//}}AFX_DATA_INIT
}


void CTranceiverBatchTestsSpuriousLevelPage::DoDataExchange(CDataExchange *pDX)
{
	CTranceiverBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverBatchTestsSpuriousLevelPage)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_INPUT_LEVEL_SPIN, m_InputLevelSpin);
	DDX_Text(pDX, IDC_INPUT_FREQ_EDIT, m_InputFreq);
	DDX_Text(pDX, IDC_INPUT_LEVEL_EDIT, m_InputLevel);
	DDX_Text(pDX, IDC_SPURIOUS_LEVEL_EDIT, m_SpuriousLevel);
	DDX_Text(pDX, IDC_AMPLITUDE_FREQ__PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_OUTPUT_FREQ_EDIT, m_OutputFreq);
	DDX_Text(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevel);
	DDX_Check(pDX, IDC_10MHZ_ENABLED_CHECK, m_b10MHzEnabled);
	DDX_Text(pDX, IDC_10MHZ_LEVEL_EDIT, m_10MHzLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranceiverBatchTestsSpuriousLevelPage, CTranceiverBatchTestsPage)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsSpuriousLevelPage)
	ON_BN_CLICKED(IDC_SET_BUTTON, OnSetButton)
	ON_BN_CLICKED(IDC_WRITE__BUTTON, OnWriteButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INPUT_LEVEL_SPIN, OnDeltaPosInputLevelSpin)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_10MHZ_ENABLED_CHECK, On10MHzEnabledCheck)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTranceiverBatchTestsSpuriousLevelPage::UpdatePowerMeter()
{
	if (!m_pPowerMeter->IsHuman())
	{
		CWaitCursor cursor;
		MeasurePower(m_pPowerMeter, 1, m_OutputLevel);
		char szBuffer[16];
		sprintf(szBuffer, "%6.2f", m_OutputLevel);
		GetDescendantWindow(IDC_OUTPUT_LEVEL_EDIT)->SetWindowText(szBuffer);
	}	
}

void CTranceiverBatchTestsSpuriousLevelPage::UpdateDCValues()
{
	CWaitCursor cursor;
	char szBuffer[16];
	double U,I;
	GetDCValues(m_pModem, U, I);
	sprintf(szBuffer, "U = %6.2f V", U);
	GetDescendantWindow(IDC_U_STATIC)->SetWindowText(szBuffer);
	sprintf(szBuffer, "I = %6.2f A", I);
	GetDescendantWindow(IDC_I_STATIC)->SetWindowText(szBuffer);
}

void CTranceiverBatchTestsSpuriousLevelPage::UpdateModemControls()
{
	CWaitCursor cursor;
	GetModemFrequency(m_pModem, m_InputFreq);
	m_InputFreq /= 1000; // KHz -> MHz
	m_OutputFreq = m_InputFreq + m_pTranceiver->GetBandShift();
	GetModemLevel(m_pModem, m_InputLevel);
	GetDCVoltage(m_pModem, m_PowerSupplyCombo);
	Get10MHz(m_pModem, m_b10MHzEnabled);
	UpdateDCValues();
	GetDescendantWindow(IDC_10MHZ_LEVEL_EDIT)->EnableWindow(!m_b10MHzEnabled);
	
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsSpuriousLevelPage message handlers

BOOL CTranceiverBatchTestsSpuriousLevelPage::OnInitDialog() 
{
	CTranceiverBatchTestsPage::OnInitDialog();
	m_InputLevelSpin.SetBuddy(GetDescendantWindow(IDC_INPUT_LEVEL_EDIT));
	m_InputLevelSpin.SetRange32((int)m_pModem->GetMinOutputLevel(), (int)m_pModem->GetMaxOutputLevel());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTranceiverBatchTestsSpuriousLevelPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	m_Protocol = "Spurious level test:\r\n";
	m_Protocol += "\r\nFrIn\tFrOut\tPinput\tPoutput\tUbuc\tIbuc\t10Mhz\tSpurLevel\r\n";

	CWaitCursor cursor;
	
	// Pure carrier
	TurnPureCarrier(m_pModem, TRUE);

	// Input and output freq
	unsigned int Freq;
	GetModemFrequency(m_pModem, Freq);
	// Round to integer number of MHz
	Freq /= 1000; // KHz -> MHz
	Freq *= 1000; // MHz again
	SetModemFrequency(m_pModem, Freq);
	m_InputFreq = Freq/1000; // MHz
	m_OutputFreq = m_InputFreq + m_pTranceiver->GetBandShift();

	UpdateModemControls();

	m_PowerSupplyCombo.initT(m_pModem, 1);

	UpdateData(FALSE);

	m_TimerID = SetTimer(1, 5000, NULL);
	return CTranceiverBatchTestsPage::OnSetActive();
}

BOOL CTranceiverBatchTestsSpuriousLevelPage::OnKillActive() 
{
	KillTimer(m_TimerID);

	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);

	return CTranceiverBatchTestsPage::OnKillActive();
}

void CTranceiverBatchTestsSpuriousLevelPage::OnSetButton() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	unsigned int Freq = m_InputFreq*1000; // MHz -> KHz
	if (SetModemFrequency(m_pModem, Freq) &&
		SetModemLevel(m_pModem, m_InputLevel))
	{
		Sleep(300);
		MeasurePower(m_pPowerMeter, 1, m_OutputLevel);
	}
	m_InputFreq = Freq/1000; // KHz -> MHz
	m_OutputFreq = m_InputFreq + m_pTranceiver->GetBandShift();
	UpdateData(FALSE);	
}

void CTranceiverBatchTestsSpuriousLevelPage::OnWriteButton() 
{
	UpdateData(TRUE);

	double U, I;
	GetDCValues(m_pModem, U, I);

	CString str10Mhz;
	BOOL bOn = FALSE;
	Get10MHz(m_pModem, bOn);
	if (bOn)
		str10Mhz = "modem";
	else
		str10Mhz.Format("%6.1f", m_10MHzLevel);

	char szBuffer[512];
	sprintf(szBuffer, "%d\t%d\t%6.1f\t%6.1f\t%6.2f\t%6.2f\t%s\t%6.1f\r\n",
		m_InputFreq, m_OutputFreq, m_InputLevel, m_OutputLevel, U, I, str10Mhz, m_SpuriousLevel);
	m_Protocol += szBuffer;
	UpdateData(FALSE);
}


void CTranceiverBatchTestsSpuriousLevelPage::OnDeltaPosInputLevelSpin(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_UPDOWN *pNMUpDown = (NM_UPDOWN *)pNMHDR;
	UpdateData();
	m_InputLevel += pNMUpDown->iDelta/10.;
	if (m_InputLevel > m_pModem->GetMaxOutputLevel())
		m_InputLevel = m_pModem->GetMaxOutputLevel();
	if (m_InputLevel < m_pModem->GetMinOutputLevel())
		m_InputLevel = m_pModem->GetMinOutputLevel();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CTranceiverBatchTestsSpuriousLevelPage::OnTimer(UINT nIDEvent) 
{
	UpdatePowerMeter();
	UpdateDCValues();
	
	CTranceiverBatchTestsPage::OnTimer(nIDEvent);
}

void CTranceiverBatchTestsSpuriousLevelPage::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

void CTranceiverBatchTestsSpuriousLevelPage::On10MHzEnabledCheck() 
{
	UpdateData(TRUE);
	Set10MHz(m_pModem, m_b10MHzEnabled);
	UpdateModemControls();
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsIntermodulationLevelTest property page

IMPLEMENT_DYNCREATE(CTranceiverBatchTestsIntermodulationLevelTest, CTranceiverBatchTestsPage)

CTranceiverBatchTestsIntermodulationLevelTest::CTranceiverBatchTestsIntermodulationLevelTest()
 : CTranceiverBatchTestsPage(CTranceiverBatchTestsIntermodulationLevelTest::IDD)
{
	//{{AFX_DATA_INIT(CTranceiverBatchTestsIntermodulationLevelTest)
	m_InputFreq = 0;
	m_InputLevel = 0.0;
	m_bOnOff = FALSE;
	m_OutputFreq = 0;
	m_OutputLevel = 0.0;
	m_Protocol = _T("");
	m_IntermodulationLevel = 0.0;
	m_CenterOutputFreq = 0.0;
	m_b10MhzEnabled = FALSE;
	m_10MHzLevel = 0.0;
	m_b10MhzEnabled2 = FALSE;
	m_InputFreq2 = 0;
	m_InputLevel2 = 0.0;
	m_bOnOff2 = FALSE;
	m_OutputFreq2 = 0;
	//}}AFX_DATA_INIT
}

CTranceiverBatchTestsIntermodulationLevelTest::~CTranceiverBatchTestsIntermodulationLevelTest()
{
}

void CTranceiverBatchTestsIntermodulationLevelTest::DoDataExchange(CDataExchange *pDX)
{
	CTranceiverBatchTestsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranceiverBatchTestsIntermodulationLevelTest)
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO2, m_PowerSupplyCombo2);
	DDX_Control(pDX, IDC_POWER_SUPPLY_COMBO, m_PowerSupplyCombo);
	DDX_Control(pDX, IDC_INPUT_LEVEL_SPIN2, m_InputLevelSpin2);
	DDX_Control(pDX, IDC_INPUT_LEVEL_SPIN, m_InputLevelSpin);
	DDX_Text(pDX, IDC_INPUT_FREQ_EDIT, m_InputFreq);
	DDX_Text(pDX, IDC_INPUT_LEVEL_EDIT, m_InputLevel);
	DDX_Check(pDX, IDC_ONOFF_CHECK, m_bOnOff);
	DDX_Text(pDX, IDC_OUTPUT_FREQ_EDIT, m_OutputFreq);
	DDX_Text(pDX, IDC_OUTPUT_LEVEL_EDIT, m_OutputLevel);
	DDX_Text(pDX, IDC_PROTOCOL_EDIT, m_Protocol);
	DDX_Text(pDX, IDC_INTERMODULATION_LEVEL_EDIT, m_IntermodulationLevel);
	DDX_Text(pDX, IDC_CENTER_OTPUT_FREQ_EDIT, m_CenterOutputFreq);
	DDX_Check(pDX, IDC_10MHZ_ENABLED_CHECK, m_b10MhzEnabled);
	DDX_Text(pDX, IDC_10MHZ_LEVEL_EDIT, m_10MHzLevel);
	DDX_Check(pDX, IDC_10MHZ_ENABLED_CHECK2, m_b10MhzEnabled2);
	DDX_Text(pDX, IDC_INPUT_FREQ_EDIT2, m_InputFreq2);
	DDX_Text(pDX, IDC_INPUT_LEVEL_EDIT2, m_InputLevel2);
	DDX_Check(pDX, IDC_ONOFF_CHECK2, m_bOnOff2);
	DDX_Text(pDX, IDC_OUTPUT_FREQ_EDIT2, m_OutputFreq2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranceiverBatchTestsIntermodulationLevelTest, CTranceiverBatchTestsPage)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsIntermodulationLevelTest)
	ON_BN_CLICKED(IDC_SET_BUTTON, OnSetButton)
	ON_BN_CLICKED(IDC_WRITE__BUTTON, OnWriteButton)
	ON_BN_CLICKED(IDC_ONOFF_CHECK, OnOnOffCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INPUT_LEVEL_SPIN, OnDeltaPosInputLevelSpin)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_10MHZ_ENABLED_CHECK, On10MhzEnabledCheck)
	ON_BN_CLICKED(IDC_SET_BUTTON2, OnSetButton2)
	ON_BN_CLICKED(IDC_10MHZ_ENABLED_CHECK2, On10mhzEnabledCheck2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INPUT_LEVEL_SPIN2, OnDeltaPosInputLevelSpin2)
	ON_BN_CLICKED(IDC_ONOFF_CHECK2, OnOnOffCheck2)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO, OnSelChangePowerSupplyCombo)
	ON_CBN_SELCHANGE(IDC_POWER_SUPPLY_COMBO2, OnSelChangePowerSupplyCombo2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTranceiverBatchTestsIntermodulationLevelTest::UpdateModemControls()
{
	CWaitCursor cursor;
	GetModemFrequency(m_pModem, m_InputFreq);
	m_InputFreq /= 1000; // KHz -> MHz
	m_OutputFreq = m_InputFreq + m_pTranceiver->GetBandShift();
	GetModemLevel(m_pModem, m_InputLevel);
	IsModemOutputOn(m_pModem, m_bOnOff);
	GetDCVoltage(m_pModem, m_PowerSupplyCombo);
	Get10MHz(m_pModem, m_b10MhzEnabled);
	UpdateDCValues();
	
	UpdateData(FALSE);
}

void CTranceiverBatchTestsIntermodulationLevelTest::UpdateModemControls2()
{
	CWaitCursor cursor;
	GetModemFrequency(m_pModem2, m_InputFreq2);
	m_InputFreq2 /= 1000; // KHz -> MHz
	m_OutputFreq2 = m_InputFreq2 + m_pTranceiver->GetBandShift();
	GetModemLevel(m_pModem2, m_InputLevel2);
	IsModemOutputOn(m_pModem2, m_bOnOff2);
	GetDCVoltage(m_pModem2, m_PowerSupplyCombo2);
	Get10MHz(m_pModem2, m_b10MhzEnabled2);
	UpdateDCValues2();
	
	UpdateData(FALSE);
}


double CTranceiverBatchTestsIntermodulationLevelTest::CalcCenterOutputFreq()
{
	unsigned int Freq1, Freq2;
	GetModemFrequency(m_pModem, Freq1);
	Freq1 /= 1000;
	if (m_pModem2)
	{
		GetModemFrequency(m_pModem2, Freq2);
		Freq2 /= 1000;
	}
	else
		Freq2 = Freq1;
	return (Freq1+Freq2+0.)/2+m_pTranceiver->GetBandShift();
}

void CTranceiverBatchTestsIntermodulationLevelTest::UpdatePowerMeter()
{
	if (!m_pPowerMeter->IsHuman())
	{
		CWaitCursor cursor;
		MeasurePower(m_pPowerMeter, 1, m_OutputLevel);
		char szBuffer[16];
		sprintf(szBuffer, "%8.2f", m_OutputLevel);
		GetDescendantWindow(IDC_OUTPUT_LEVEL_EDIT)->SetWindowText(szBuffer);
	}	
}

void CTranceiverBatchTestsIntermodulationLevelTest::UpdateDCValues()
{
	CWaitCursor cursor;
	char szBuffer[16];
	double U,I;
	GetDCValues(m_pModem, U, I);
	sprintf(szBuffer, "U = %6.2f V", U);
	GetDescendantWindow(IDC_U_STATIC)->SetWindowText(szBuffer);
	sprintf(szBuffer, "I = %6.2f A", I);
	GetDescendantWindow(IDC_I_STATIC)->SetWindowText(szBuffer);
}

void CTranceiverBatchTestsIntermodulationLevelTest::UpdateDCValues2()
{
	CWaitCursor cursor;
	char szBuffer[16];
	double U,I;
	GetDCValues(m_pModem2, U, I);
	sprintf(szBuffer, "U = %6.2f V", U);
	GetDescendantWindow(IDC_U_STATIC2)->SetWindowText(szBuffer);
	sprintf(szBuffer, "I = %6.2f A", I);
	GetDescendantWindow(IDC_I_STATIC2)->SetWindowText(szBuffer);
}


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsIntermodulationLevelTest message handlers

BOOL CTranceiverBatchTestsIntermodulationLevelTest::OnSetActive() 
{
	if (!m_pModem2)
	{
		MessageBox("Intermodulation test is skipped because of secondary modem is not specified", pszWarning, MB_ICONINFORMATION);
		return 0; // skip this page
	}

	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	m_Protocol = "Intermodulation level test:\r\n";
	m_Protocol += "\r\nFr1in\tFr1out\tFr2in\tFr2out\tPoutput\tUbuc\tIbuc\t10Mhz\tIntSpurLevel\r\n";

	CWaitCursor cursor;
	
	// Pure carrier
	TurnPureCarrier(m_pModem, TRUE);
	TurnPureCarrier(m_pModem2, TRUE);

	m_CenterOutputFreq = CalcCenterOutputFreq();

	UpdateModemControls();
	UpdateModemControls2();
	UpdatePowerMeter();

	m_PowerSupplyCombo.initT(m_pModem, 1);
	m_PowerSupplyCombo2.initT(m_pModem2, 1);

	UpdateData(FALSE);

	m_TimerID = SetTimer(1, 5000, NULL);
	return CTranceiverBatchTestsPage::OnSetActive();
}

BOOL CTranceiverBatchTestsIntermodulationLevelTest::OnKillActive() 
{
	KillTimer(m_TimerID);

	CStdioFile *pFile = GetReportFile();
	if (pFile)
		pFile->WriteString(m_Protocol);
	
	return CTranceiverBatchTestsPage::OnKillActive();
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnSetButton() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	unsigned int Freq = m_InputFreq*1000; // MHz -> KHz
	if (SetModemFrequency(m_pModem, Freq) &&
		SetModemLevel(m_pModem, m_InputLevel) &&
		TurnModemOutputOn(m_pModem, m_bOnOff))
	{
		Sleep(300);
		MeasurePower(m_pPowerMeter, 1, m_OutputLevel);
	}
	m_InputFreq = Freq/1000;
	m_OutputFreq = m_InputFreq + m_pTranceiver->GetBandShift();
	m_CenterOutputFreq = CalcCenterOutputFreq();
	UpdateData(FALSE);	
}


void CTranceiverBatchTestsIntermodulationLevelTest::OnSetButton2() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	unsigned int Freq = m_InputFreq2*1000; // MHz -> KHz
	if (SetModemFrequency(m_pModem2, Freq) &&
		SetModemLevel(m_pModem2, m_InputLevel2) &&
		TurnModemOutputOn(m_pModem2, m_bOnOff2))
	{
		Sleep(300);
		MeasurePower(m_pPowerMeter, 1, m_OutputLevel);
	}
	m_InputFreq2 = Freq/1000;
	m_OutputFreq2 = m_InputFreq2 + m_pTranceiver->GetBandShift();
	m_CenterOutputFreq = CalcCenterOutputFreq();
	UpdateData(FALSE);	
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnWriteButton() 
{
	UpdateData(TRUE);
	char szBuffer[512];

	unsigned int Freq1, Freq2;
	GetModemFrequency(m_pModem, Freq1);
	Freq1 /= 1000;
	GetModemFrequency(m_pModem2, Freq2);
	Freq2 /= 1000;

	double U, I;
	GetDCValues(m_pModem, U, I);

	CString str10Mhz;
	BOOL bOn = FALSE;
	Get10MHz(m_pModem, bOn);
	if (bOn)
		str10Mhz = "modem";
	else
		str10Mhz.Format("%6.1f", m_10MHzLevel);

	sprintf(szBuffer, "%d\t%d\t%d\t%d\t%6.1f\t%6.2f\t%6.2f\t%s\t%6.1f\r\n",
		Freq1, Freq1+m_pTranceiver->GetBandShift(),
		Freq2, Freq2+m_pTranceiver->GetBandShift(),
		m_OutputLevel, U, I, str10Mhz, m_IntermodulationLevel);
	m_Protocol += szBuffer;

	UpdateData(FALSE);
}

BOOL CTranceiverBatchTestsIntermodulationLevelTest::OnInitDialog() 
{
	CTranceiverBatchTestsPage::OnInitDialog();
	if (!m_pModem2)
		return TRUE; // skip this page

	m_InputLevelSpin.SetBuddy(GetDescendantWindow(IDC_INPUT_LEVEL_EDIT));
	m_InputLevelSpin.SetRange32((int)m_pModem->GetMinOutputLevel(), (int)m_pModem->GetMaxOutputLevel());
	m_InputLevelSpin2.SetBuddy(GetDescendantWindow(IDC_INPUT_LEVEL_EDIT2));
	m_InputLevelSpin2.SetRange32((int)m_pModem2->GetMinOutputLevel(), (int)m_pModem2->GetMaxOutputLevel());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnOnOffCheck() 
{
	UpdateData(TRUE);
	TurnModemOutputOn(m_pModem, m_bOnOff);
	UpdateData(FALSE);
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnOnOffCheck2() 
{
	UpdateData(TRUE);
	TurnModemOutputOn(m_pModem2, m_bOnOff2);
	UpdateData(FALSE);
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnDeltaPosInputLevelSpin(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_UPDOWN *pNMUpDown = (NM_UPDOWN *)pNMHDR;
	UpdateData();
	m_InputLevel += pNMUpDown->iDelta/10.;
	if (m_InputLevel > m_pModem->GetMaxOutputLevel())
		m_InputLevel = m_pModem->GetMaxOutputLevel();
	if (m_InputLevel < m_pModem->GetMinOutputLevel())
		m_InputLevel = m_pModem->GetMinOutputLevel();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnDeltaPosInputLevelSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN *pNMUpDown = (NM_UPDOWN *)pNMHDR;
	UpdateData();
	m_InputLevel2 += pNMUpDown->iDelta/10.;
	if (m_InputLevel2 > m_pModem2->GetMaxOutputLevel())
		m_InputLevel2 = m_pModem2->GetMaxOutputLevel();
	if (m_InputLevel2 < m_pModem2->GetMinOutputLevel())
		m_InputLevel2 = m_pModem2->GetMinOutputLevel();
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnTimer(UINT nIDEvent) 
{
	UpdatePowerMeter();
	UpdateDCValues();
	UpdateDCValues2();
	CTranceiverBatchTestsPage::OnTimer(nIDEvent);
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnSelChangePowerSupplyCombo() 
{
	int PowerSupplyMode = m_PowerSupplyCombo.getSelectedMode();
	m_pModem->SetTPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo.SelectByDataValue(PowerSupplyMode);
}

void CTranceiverBatchTestsIntermodulationLevelTest::OnSelChangePowerSupplyCombo2() 
{
	int PowerSupplyMode = m_PowerSupplyCombo2.getSelectedMode();
	m_pModem2->SetTPowerSupplyMode(PowerSupplyMode, 1);
	m_PowerSupplyCombo2.SelectByDataValue(PowerSupplyMode);
}

void CTranceiverBatchTestsIntermodulationLevelTest::On10MhzEnabledCheck() 
{
	UpdateData(TRUE);
	Set10MHz(m_pModem, m_b10MhzEnabled);
	UpdateModemControls();
	UpdateData(FALSE);
}

void CTranceiverBatchTestsIntermodulationLevelTest::On10mhzEnabledCheck2() 
{
	UpdateData(TRUE);
	Set10MHz(m_pModem2, m_b10MhzEnabled2);
	UpdateModemControls2();
	UpdateData(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsPropertySheet implementation

IMPLEMENT_DYNAMIC(CTranceiverBatchTestsPropertySheet, CAutoTestSheet)

CTranceiverBatchTestsPropertySheet::CTranceiverBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	AddPage(&m_DevicePage);
	m_DevicePage.m_nDevices = nDevices;
	m_DevicePage.m_ppDevices = ppDevices;

	AddPage(&m_RemoteControlPage);
	AddPage(&m_AmplitudeFrequencyPage);
	AddPage(&m_SpuriousLevelPage);
	AddPage(&m_IntermodulationLevelPage);
	AddPage(&m_CongratulationPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig = &m_AutoTestConfig;
}

CTranceiverBatchTestsPropertySheet::~CTranceiverBatchTestsPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CTranceiverBatchTestsPropertySheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CTranceiverBatchTestsPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsPropertySheet message handlers

