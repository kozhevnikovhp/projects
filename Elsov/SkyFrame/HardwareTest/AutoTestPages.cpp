// CarrierCaptureAutoTestPages.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AutoTestPages.h"
#include "Modem.h"
#include "ProgressDlg.h"
#include "CommonTests.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int DataRates[]={	9600,	14400,	19200,	28800,	33600,
					48000,	64000,	128000,	192000,	256000,
					320000,	384000,	448000,	512000,	576000,
					640000,	704000,	768000,	732000,	896000,
					960000,	1024000,1088000,1152000,1216000,
					1280000,1344000,1408000,1472000,1536000,
					1600000,1664000,1728000,1792000,1856000,
					1920000,1984000,2048000					};

char *pszMeasurementsSectionName = "Measurements";

double CalcCorrection(unsigned int Freq1, double Correction1, unsigned int Freq2, double Correction2, unsigned int Freq)
{
	double a = (Correction2-Correction1)/(Freq2-Freq1);
	double b = (Correction1*Freq2-Correction2*Freq1)/(Freq2-Freq1);
	double Correction = a*Freq + b;
	return Correction;
}

/////////////////////////////////////////////////////////////////////////////
// CAutoTestPropertyPage property page

IMPLEMENT_DYNCREATE(CAutoTestPropertyPage, CPropertyPage)

SAutoTestConfig *CAutoTestPropertyPage::m_pAutoTestConfig=NULL;

CAutoTestPropertyPage::CAutoTestPropertyPage(int nIDD) : CPropertyPage(nIDD)
{
	//{{AFX_DATA_INIT(CAutoTestPropertyPage)
	//}}AFX_DATA_INIT
}

CAutoTestPropertyPage::~CAutoTestPropertyPage()
{
}

void CAutoTestPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoTestPropertyPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoTestPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAutoTestPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CAutoTestPropertyPage::ReadRegistryInt(const char *pszParamName, int nDefaultValue)
{
	return AfxGetApp()->GetProfileInt(GetRegistrySectionName(), pszParamName, nDefaultValue);
}

void CAutoTestPropertyPage::WriteRegistryInt(const char *pszParamName, int nValue)
{
	AfxGetApp()->WriteProfileInt(GetRegistrySectionName(), pszParamName, nValue);
}

void CAutoTestPropertyPage::WriteString(CStdioFile *pFile, const char *pszStr)
{
	int len = strlen(pszStr);
	if (!len)
		return;
	pFile->Write(pszStr, len);
}

BOOL CAutoTestPropertyPage::AdjustSignal(CModem *pEtalonModem, CModem *pModem, double DesiredSNFrom, double DesiredSNTo, CProgressDlg *pDlg/* = NULL*/)
{
	BOOL bSuccess = FALSE;
	double OutputLevel = pEtalonModem->GetMinOutputLevel();
	pEtalonModem->SetOutputLevel(OutputLevel, 1);
	BOOL bOn = TRUE;
	pEtalonModem->TurnOutputOn(bOn, 1);
	Sleep(2000);
	double InputLevel, SN;

	pModem->GetInputLevel(InputLevel, 1);
	pModem->GetEbNo(SN, 1);
	pEtalonModem->GetOutputLevel(OutputLevel, 1);
	do 
	{
		OnUpdateLevels(OutputLevel, InputLevel, SN);
		if (pDlg)
		{
			CString str;
			str.Format("Adjusting signal (output = %5.1f dBm, input  = %5.1f dBm, S/N = %4.1f dB)", OutputLevel, InputLevel, SN);
			pDlg->SetStatus(str);
			if (pDlg->CheckCancelButton())
			{
				if (DoYouReallyWantToCancel(this))
					return FALSE;
			}
		}
		double DesiredSN = (DesiredSNFrom + DesiredSNTo)/2;
		double DeltaLevel = DesiredSN - SN;
		if (DeltaLevel > 1)
			DeltaLevel = 1;
		if (DeltaLevel < -1)
			DeltaLevel = -1;
		if (OutputLevel >= pEtalonModem->GetMaxOutputLevel()-0.01 && DeltaLevel > 0)
			return FALSE;
//		if (InputLevel > pModem->GetMaxComfortLevel() && DeltaLevel > 0)
//			return FALSE;
//		if (OutputLevel <= pEtalonModem->GetMinOutputLevel()+0.01 && DeltaLevel < 0)
//			return FALSE;
//		if (InputLevel < pModem->GetMinComfortLevel() && DeltaLevel < 0)
//			return FALSE;
		if (pEtalonModem->ChangeOutputLevel(DeltaLevel, 1) != MC_OK)
			break;
		Sleep(3000);
		if (pModem->GetInputLevel(InputLevel, 1) != MC_OK)
			break;
		if (pModem->GetEbNo(SN, 1) != MC_OK)
			break;
		if (pEtalonModem->GetOutputLevel(OutputLevel, 1) != MC_OK)
			break;
		bSuccess = (SN >= DesiredSNFrom && SN < DesiredSNTo);
		if (SN > (DesiredSNFrom - 0.5) && OutputLevel <= pEtalonModem->GetMinOutputLevel()+0.01)
			bSuccess = TRUE;
	} while (!bSuccess);

	OnUpdateLevels(OutputLevel, InputLevel, SN);
	return bSuccess;
}

CStdioFile *CAutoTestPropertyPage::GetReportFile()
{
	if (m_pAutoTestConfig->m_pReportFile->m_pStream == NULL)
	{
		if (!m_pAutoTestConfig->m_pReportFile->Open(m_pAutoTestConfig->m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
			return NULL;
		}
	}
	return m_pAutoTestConfig->m_pReportFile;
}

BOOL CAutoTestPropertyPage::ChangeCorrection(double &Correction)
{
	CCorrectionDialog dlg;
	dlg.m_Correction = Correction;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	Correction = dlg.m_Correction;

	UpdateData(FALSE);
	return TRUE;
}

BOOL CAutoTestPropertyPage::ChangeFrequencyDependentCorrection(unsigned int &Freq1, double &Correction1, unsigned int &Freq2, double &Correction2)
{
	CFrequencyDependentCorrectionDialog dlg;
	dlg.m_Correction1 = Correction1;
	dlg.m_Freq1 = Freq1;
	dlg.m_Correction2 = Correction2;
	dlg.m_Freq2 = Freq2;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	Correction1 = dlg.m_Correction1;
	Freq1 = dlg.m_Freq1;
	Correction2 = dlg.m_Correction2;
	Freq2 = dlg.m_Freq2;

	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDevicesPage property page

IMPLEMENT_DYNCREATE(CDevicesPage, CAutoTestPropertyPage)

CDevicesPage::CDevicesPage() : CAutoTestPropertyPage(CDevicesPage::IDD)
{
	//{{AFX_DATA_INIT(CDevicesPage)
	m_DemodulatorNo = 1;
	m_ModulatorNo = 1;
	//}}AFX_DATA_INIT
	m_MaxModulator=m_MaxDemodulator=1;
}

CDevicesPage::~CDevicesPage()
{
}

void CDevicesPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevicesPage)
	DDX_Control(pDX, IDC_RECEIVER_TAB, m_ReceiverTab);
	DDX_Control(pDX, IDC_TRANSMITTER_TAB, m_TransmitterTab);
	DDX_Control(pDX, IDC_MODULATOR_NO_SPIN, m_ModulatorNoSpin);
	DDX_Control(pDX, IDC_DEMODULATOR_NO_SPIN, m_DemodulatorNoSpin);
	DDX_Control(pDX, IDC_RECEIVER_COMBO, m_ReceiverCombo);
	DDX_Control(pDX, IDC_TRANSMITTER_COMBO, m_TransmitterCombo);
	DDX_Text(pDX, IDC_DEMODULATOR_NO_EDIT, m_DemodulatorNo);
//	DDV_MinMaxInt(pDX, m_DemodulatorNo, 1, m_MaxDemodulator);
	DDX_Text(pDX, IDC_MODULATOR_NO_EDIT, m_ModulatorNo);
//	DDV_MinMaxInt(pDX, m_ModulatorNo, 1, m_MaxModulator);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevicesPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CDevicesPage)
	ON_CBN_SELCHANGE(IDC_TRANSMITTER_COMBO, OnSelChangeTransmitterCombo)
	ON_CBN_SELCHANGE(IDC_RECEIVER_COMBO, OnSelChangeReceiverCombo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TRANSMITTER_TAB, OnSelChangeTransmitterTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TRANSMITTER_TAB, OnSelChangingTransmitterTab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_RECEIVER_TAB, OnSelChangeReceiverTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_RECEIVER_TAB, OnSelChangingReceiverTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDevicesPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	for (int i=0; i<m_nDevices; i++)
	{
		CDevice *pDevice=m_ppDevices[i];
		if (pDevice==0)
			continue;
		if (pDevice->IsModem())
		{
			CModem *pModem=(CModem *)pDevice;
			CString Name=pDevice->GetName();
			Name+=" (";
			Name+=pDevice->GetConnectionName();
			Name+=")";
			if (pModem->GetDemodulatorsNumber()>0)
			{
				int index=m_ReceiverCombo.AddString(Name);
				m_ReceiverCombo.SetItemDataPtr(index, pModem);
			}
			if (pModem->GetModulatorsNumber()>0)
			{
				int index=m_TransmitterCombo.AddString(Name);
				m_TransmitterCombo.SetItemDataPtr(index, pModem);
			}
		}
	}

	char szText[64];
	for (i=0; i<m_pAutoTestConfig->m_nNumberOfTransmitters; i++)
	{
		sprintf(szText, "Transmitter %d", i+1);
		m_TransmitterTab.InsertItem(i, szText, 0);
		m_TransmitterCombo.SetCurSel(i);
		m_pAutoTestConfig->m_CurrentTransmitter=i;
		SaveTransmitter();
	}
	for (i=0; i<m_pAutoTestConfig->m_nNumberOfReceivers; i++)
	{
		sprintf(szText, "Receiver %d", i+1);
		m_ReceiverTab.InsertItem(i, szText, 0);
		m_ReceiverCombo.SetCurSel(i);
		m_pAutoTestConfig->m_CurrentReceiver=i;
		SaveReceiver();
	}

	m_TransmitterCombo.SetCurSel(0);
	OnSelChangeTransmitterCombo();
	m_ReceiverCombo.SetCurSel(0);
	OnSelChangeReceiverCombo();

	m_TransmitterCombo.EnableWindow(m_pAutoTestConfig->m_nNumberOfTransmitters);
	m_ReceiverCombo.EnableWindow(m_pAutoTestConfig->m_nNumberOfReceivers);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDevicesPage::OnSelChangeTransmitterCombo() 
{
	int index=m_TransmitterCombo.GetCurSel();
	if (index==CB_ERR)
		return;
	CModem *pModem=(CModem *)m_TransmitterCombo.GetItemDataPtr(index);
	ASSERT(pModem);
	m_MaxModulator=pModem->GetModulatorsNumber();
	m_ModulatorNoSpin.SetRange(1, m_MaxModulator);
}

void CDevicesPage::OnSelChangeReceiverCombo() 
{
	int index=m_ReceiverCombo.GetCurSel();
	if (index==CB_ERR)
		return;
	CModem *pModem=(CModem *)m_ReceiverCombo.GetItemDataPtr(index);
	ASSERT(pModem);
	m_MaxDemodulator=pModem->GetDemodulatorsNumber();
	m_DemodulatorNoSpin.SetRange(1, m_MaxDemodulator);
}

BOOL CDevicesPage::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT);
	
	return CAutoTestPropertyPage::OnSetActive();
}

void CDevicesPage::SaveReceiver()
{
	UpdateData();
	int index=m_ReceiverCombo.GetCurSel();
	if (index!=CB_ERR)
		m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver]=(CModem *)m_ReceiverCombo.GetItemDataPtr(index);
	m_pAutoTestConfig->m_nDemodulator[m_pAutoTestConfig->m_CurrentReceiver]=m_DemodulatorNo;
}

void CDevicesPage::SaveTransmitter()
{
	UpdateData();
	int index=m_TransmitterCombo.GetCurSel();
	if (index!=CB_ERR)
		m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]=(CModem *)m_TransmitterCombo.GetItemDataPtr(index);
	m_pAutoTestConfig->m_nModulator[m_pAutoTestConfig->m_CurrentTransmitter]=m_ModulatorNo;
}

void CDevicesPage::RestoreReceiver()
{
	m_pAutoTestConfig->m_CurrentReceiver=m_ReceiverTab.GetCurSel();
	CModem *pThisReceiver=m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver];
	for (int index=0; index<m_ReceiverCombo.GetCount(); index++)
	{
		CModem *pReceiver=(CModem *)m_ReceiverCombo.GetItemDataPtr(index);
		if (pThisReceiver==pReceiver)
		{
			m_ReceiverCombo.SetCurSel(index);
			break;
		}
	}
	OnSelChangeReceiverCombo();
	m_DemodulatorNo=m_pAutoTestConfig->m_nDemodulator[m_pAutoTestConfig->m_CurrentReceiver];
	UpdateData(FALSE);
}

void CDevicesPage::RestoreTransmitter()
{
	m_pAutoTestConfig->m_CurrentTransmitter=m_TransmitterTab.GetCurSel();
	CModem *pThisTransmitter=m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter];
	for (int index=0; index<m_TransmitterCombo.GetCount(); index++)
	{
		CModem *pTransmitter=(CModem *)m_TransmitterCombo.GetItemDataPtr(index);
		if (pThisTransmitter==pTransmitter)
		{
			m_TransmitterCombo.SetCurSel(index);
			break;
		}
	}
	OnSelChangeTransmitterCombo();
	m_ModulatorNo=m_pAutoTestConfig->m_nModulator[m_pAutoTestConfig->m_CurrentTransmitter];
	UpdateData(FALSE);
}

BOOL CDevicesPage::OnKillActive() 
{
	SaveTransmitter();
	SaveReceiver();
	return CAutoTestPropertyPage::OnKillActive();
}

void CDevicesPage::OnSelChangeTransmitterTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	RestoreTransmitter();
	*pResult = 0;
}

void CDevicesPage::OnSelChangingTransmitterTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SaveTransmitter();
	*pResult = 0;
}

void CDevicesPage::OnSelChangeReceiverTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	RestoreReceiver();
	*pResult = 0;
}

void CDevicesPage::OnSelChangingReceiverTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SaveReceiver();
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CCarrierPage property page

IMPLEMENT_DYNCREATE(CCarrierPage, CAutoTestPropertyPage)

CCarrierPage::CCarrierPage() : CAutoTestPropertyPage(CCarrierPage::IDD)
{
	//{{AFX_DATA_INIT(CCarrierPage)
	m_ReceiverFreq = 0;
	m_ReceiverSpareFreq = 0;
	m_TransmitterFreq = 0;
	//}}AFX_DATA_INIT
	m_MinTransmitterFreq=m_MaxTransmitterFreq=0;
	m_MinReceiverFreq=m_MaxReceiverFreq=0;
}

CCarrierPage::~CCarrierPage()
{
}

void CCarrierPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCarrierPage)
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Control(pDX, IDC_TRANSMITTER_FREQ_SPIN, m_TransmitterFreqSpin);
	DDX_Control(pDX, IDC_RECEIVER_FREQ_SPIN2, m_ReceiverFreq2Spin);
	DDX_Control(pDX, IDC_RECEIVER_FREQ_SPIN, m_ReceiverFreqSpin);
	DDX_Text(pDX, IDC_RECEIVER_FREQ_EDIT, m_ReceiverFreq);
	DDV_MinMaxInt(pDX, m_ReceiverFreq, m_MinReceiverFreq, m_MaxReceiverFreq);
	DDX_Text(pDX, IDC_RECEIVER_FREQ_EDIT2, m_ReceiverSpareFreq);
	DDV_MinMaxInt(pDX, m_ReceiverSpareFreq, m_MinReceiverFreq, m_MaxReceiverFreq);
	DDX_Text(pDX, IDC_TRANSMITTER_FREQ_EDIT, m_TransmitterFreq);
	DDV_MinMaxInt(pDX, m_TransmitterFreq, m_MinTransmitterFreq, m_MaxTransmitterFreq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCarrierPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CCarrierPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CCarrierPage::OnSetActive() 
{
	if (m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver]==NULL ||
		m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]==NULL)
		return FALSE;

	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	m_MinTransmitterFreq=m_pAutoTestConfig->m_pModulator[0]->GetMinTFrequency();
	m_MaxTransmitterFreq=m_pAutoTestConfig->m_pModulator[0]->GetMaxTFrequency();
	m_MinReceiverFreq=m_pAutoTestConfig->m_pDemodulator[0]->GetMinRFrequency();
	m_MaxReceiverFreq=m_pAutoTestConfig->m_pDemodulator[0]->GetMaxRFrequency();

	m_TransmitterFreqSpin.SetRange32(m_MinTransmitterFreq, m_MaxTransmitterFreq);
	m_ReceiverFreqSpin.SetRange32(m_MinReceiverFreq, m_MaxReceiverFreq);
	m_ReceiverFreq2Spin.SetRange32(m_MinReceiverFreq, m_MaxReceiverFreq);

	m_ModulationTypeCombo.ResetContent();
	for (int i = 0; i < m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetTModulationTypeCount(); i++)
	{
		int index = m_ModulationTypeCombo.AddString(m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetTModulationTypeName(i));
		m_ModulationTypeCombo.SetItemData(index, i);
	}
	int TModulationType = 0;
	m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetTModulationType(TModulationType, m_pAutoTestConfig->m_nModulator[m_pAutoTestConfig->m_CurrentTransmitter]);
	m_ModulationTypeCombo.SetCurSel(TModulationType);

	unsigned int freq;
	m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetTFrequency(freq, m_pAutoTestConfig->m_nModulator[m_pAutoTestConfig->m_CurrentTransmitter]);
	m_TransmitterFreq=freq;
	m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver]->GetRFrequency(freq, m_pAutoTestConfig->m_nDemodulator[m_pAutoTestConfig->m_CurrentReceiver]);
	m_ReceiverFreq=freq;
	m_ReceiverSpareFreq=m_MinReceiverFreq;
	UpdateData(FALSE);

	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CCarrierPage::OnKillActive() 
{
	UpdateData();
	m_pAutoTestConfig->m_ModulatorFreq=m_TransmitterFreq;
	m_pAutoTestConfig->m_DemodulatorFreq=m_ReceiverFreq;
	m_pAutoTestConfig->m_DemodulatorSpareFreq=m_ReceiverSpareFreq;
	m_pAutoTestConfig->m_ModulationType = m_ModulationTypeCombo.GetItemData(m_ModulationTypeCombo.GetCurSel());
	return CAutoTestPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
// CDataPage property page

IMPLEMENT_DYNCREATE(CDataPage, CAutoTestPropertyPage)

CDataPage::CDataPage() : CAutoTestPropertyPage(CDataPage::IDD)
{
	//{{AFX_DATA_INIT(CDataPage)
	//}}AFX_DATA_INIT
}

CDataPage::~CDataPage()
{
}

void CDataPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataPage)
	DDX_Control(pDX, IDC_START_DATERATE_COMBO, m_StartDataRateCombo);
	DDX_Control(pDX, IDC_END_DATERATE_COMBO, m_EndDataRateCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDataPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CDataPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDataPage::OnSetActive() 
{
	if (m_pAutoTestConfig->m_pDemodulator[0]==NULL ||
		m_pAutoTestConfig->m_pModulator[0]==NULL)
		return FALSE;

	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CDataPage::OnKillActive() 
{
	int index=m_StartDataRateCombo.GetCurSel();
	if (index!=CB_ERR)
		m_pAutoTestConfig->m_StartDataRate=m_StartDataRateCombo.GetItemData(index);
	index=m_EndDataRateCombo.GetCurSel();
	if (index!=CB_ERR)
		m_pAutoTestConfig->m_EndDataRate=m_EndDataRateCombo.GetItemData(index);
	return CAutoTestPropertyPage::OnKillActive();
}

BOOL CDataPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	for (int i=0; i<sizeof(DataRates)/sizeof(DataRates[0]); i++)
	{
		int DataRate=DataRates[i];
		char pszDataRate[32];
		sprintf(pszDataRate, "%d", DataRate);
		int index=m_StartDataRateCombo.AddString(pszDataRate);
		m_StartDataRateCombo.SetItemData(index, DataRate);
		index=m_EndDataRateCombo.AddString(pszDataRate);
		m_EndDataRateCombo.SetItemData(index, DataRate);
	}
	m_StartDataRateCombo.SetCurSel(0);
	m_EndDataRateCombo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CLevelsPage property page

IMPLEMENT_DYNCREATE(CLevelsPage, CAutoTestPropertyPage)

CLevelsPage::CLevelsPage() : CAutoTestPropertyPage(CLevelsPage::IDD)
{
	//{{AFX_DATA_INIT(CLevelsPage)
	m_EndLevel = 0.0;
	m_StartLevel = 0.0;
	m_StepLevel = 0.0;
	//}}AFX_DATA_INIT
	m_MinLevel=m_MaxLevel=1;
}

CLevelsPage::~CLevelsPage()
{
}

void CLevelsPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLevelsPage)
	DDX_Control(pDX, IDC_STEP_LEVEL_SPIN, m_StepLevelSpin);
	DDX_Control(pDX, IDC_START_LEVEL_SPIN, m_StartLevelSpin);
	DDX_Control(pDX, IDC_END_LEVEL_SPIN, m_EndLevelSpin);
	DDX_Text(pDX, IDC_END_LEVEL_EDIT, m_EndLevel);
	DDX_Text(pDX, IDC_START_LEVEL_EDIT, m_StartLevel);
	DDX_Text(pDX, IDC_STEP_LEVEL_EDIT, m_StepLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLevelsPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CLevelsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLevelsPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	return TRUE;
}

BOOL CLevelsPage::OnSetActive() 
{
	if (m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver]==NULL ||
		m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]==NULL)
		return FALSE;

	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	m_MaxLevel=m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetMaxOutputLevel();
	m_MinLevel=m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetMinOutputLevel();
	m_StartLevelSpin.SetRange((int)m_MinLevel, (int)m_MaxLevel);
	m_EndLevelSpin.SetRange((int)m_MinLevel, (int)m_MaxLevel);
	m_StepLevelSpin.SetRange(-10, 10);
	if (m_StartLevel<m_MinLevel || m_StartLevel>m_MaxLevel)
		m_StartLevel=m_MinLevel;
	if (m_EndLevel<m_MinLevel || m_EndLevel>m_MaxLevel)
		m_EndLevel=m_MaxLevel;
	UpdateData(FALSE);

	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CLevelsPage::OnKillActive() 
{
	UpdateData();
	if (m_StepLevel==0)
		m_StepLevel=1;
	if (m_StartLevel<m_EndLevel && m_StepLevel<0)
		m_StepLevel=-m_StepLevel;
	if (m_StartLevel>m_EndLevel && m_StepLevel>0)
		m_StepLevel=-m_StepLevel;
	m_pAutoTestConfig->m_StartLevel=m_StartLevel;
	m_pAutoTestConfig->m_EndLevel=m_EndLevel;
	m_pAutoTestConfig->m_StepLevel=m_StepLevel;
	UpdateData(FALSE);
	return CAutoTestPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
// COtherPage property page

IMPLEMENT_DYNCREATE(COtherPage, CAutoTestPropertyPage)

COtherPage::COtherPage() : CAutoTestPropertyPage(COtherPage::IDD)
{
	//{{AFX_DATA_INIT(COtherPage)
	m_NumTests = 10;
	m_FilePath = _T("");
	m_bTurnTransmitterOff = FALSE;
	//}}AFX_DATA_INIT
}

COtherPage::~COtherPage()
{
}

void COtherPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherPage)
	DDX_Control(pDX, IDC_NUM_TESTS_SPIN, m_NumTestsSpin);
	DDX_Text(pDX, IDC_NUM_TESTS_EDIT, m_NumTests);
	DDV_MinMaxInt(pDX, m_NumTests, 1, 1000);
	DDX_Text(pDX, IDC_FILE_PATH_EDIT, m_FilePath);
	DDX_Check(pDX, IDC_TURN_TRANSMITTER_OFF_CHECK, m_bTurnTransmitterOff);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COtherPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(COtherPage)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COtherPage::OnKillActive() 
{
	UpdateData();
	m_pAutoTestConfig->m_nTests = m_NumTests;
	m_pAutoTestConfig->m_bTurnModOffOn = m_bTurnTransmitterOff;
	strcpy(m_pAutoTestConfig->m_pszFilePath, LPCTSTR(m_FilePath));
	return CAutoTestPropertyPage::OnKillActive();
}

BOOL COtherPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	
	m_NumTestsSpin.SetRange(1, 1000);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COtherPage::OnBrowseButton() 
{
	UpdateData();
	CFileDialog fd(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Text files(*txt)|*txt|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Specify file to write results";
	if (fd.DoModal()==IDCANCEL) return;
	m_FilePath=fd.GetPathName();
	UpdateData(FALSE);
}

BOOL COtherPage::OnSetActive() 
{
	if (m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver]==NULL ||
		m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]==NULL)
		return FALSE;

	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	
	return CAutoTestPropertyPage::OnSetActive();
}

BOOL COtherPage::OnWizardFinish() 
{
	OnKillActive();	
	return CAutoTestPropertyPage::OnWizardFinish();
}

/////////////////////////////////////////////////////////////////////////////
// CSecondSignalPage property page

IMPLEMENT_DYNCREATE(CSecondSignalPage, CAutoTestPropertyPage)

CSecondSignalPage::CSecondSignalPage() : CAutoTestPropertyPage(CSecondSignalPage::IDD)
{
	//{{AFX_DATA_INIT(CSecondSignalPage)
	m_StartFreq = 0;
	m_EndFreq = 0;
	m_StepFreq = 1;
	//}}AFX_DATA_INIT
}

CSecondSignalPage::~CSecondSignalPage()
{
}

void CSecondSignalPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecondSignalPage)
	DDX_Control(pDX, IDC_MODULATION_TYPE_COMBO, m_ModulationTypeCombo);
	DDX_Control(pDX, IDC_END_DATERATE_COMBO, m_EndDataRateCombo);
	DDX_Control(pDX, IDC_START_DATERATE_COMBO, m_StartDataRateCombo);
	DDX_Control(pDX, IDC_STEP_FREQ_SPIN, m_StepFreqSpin);
	DDX_Control(pDX, IDC_START_FREQ_SPIN, m_StartFreqSpin);
	DDX_Control(pDX, IDC_END_FREQ_SPIN, m_EndFreqSpin);
	DDX_Text(pDX, IDC_START_FREQ_EDIT, m_StartFreq);
	DDX_Text(pDX, IDC_END_FREQ_EDIT, m_EndFreq);
	DDX_Text(pDX, IDC_STEP_FREQ_EDIT, m_StepFreq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSecondSignalPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CSecondSignalPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecondSignalPage message handlers


BOOL CSecondSignalPage::OnSetActive() 
{
	if (m_pAutoTestConfig->m_pDemodulator[m_pAutoTestConfig->m_CurrentReceiver]==NULL ||
		m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]==NULL)
		return FALSE;
	
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	int MinFreq=m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetMinTFrequency();
	int MaxFreq=m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetMaxTFrequency();
	m_StartFreqSpin.SetRange32(MinFreq, MaxFreq);
	m_EndFreqSpin.SetRange32(MinFreq, MaxFreq);
	m_StepFreqSpin.SetRange32(-100, 100);

	m_StartFreq=m_pAutoTestConfig->m_StartFreq2;	
	if (m_StartFreq==0)
		m_StartFreq=m_pAutoTestConfig->m_DemodulatorFreq-20;
	m_EndFreq=m_pAutoTestConfig->m_EndFreq2;
	if (m_EndFreq==0)
		m_EndFreq=m_pAutoTestConfig->m_DemodulatorFreq+20;
	m_StepFreq=m_pAutoTestConfig->m_StepFreq2;
	if (m_StepFreq==0)
		m_StepFreq=1;

	if (m_StartFreq<MinFreq || m_StartFreq>MaxFreq)
		m_StartFreq=MinFreq;
	if (m_EndFreq<MinFreq || m_EndFreq>MaxFreq)
		m_EndFreq=MaxFreq;
	UpdateData(FALSE);

	m_ModulationTypeCombo.ResetContent();
	for (int i = 0; i < m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetTModulationTypeCount(); i++)
	{
		int index = m_ModulationTypeCombo.AddString(m_pAutoTestConfig->m_pModulator[m_pAutoTestConfig->m_CurrentTransmitter]->GetTModulationTypeName(i));
		m_ModulationTypeCombo.SetItemData(index, i);
	}
	m_ModulationTypeCombo.SetCurSel(m_pAutoTestConfig->m_ModulationType2);

	return CAutoTestPropertyPage::OnSetActive();
}

BOOL CSecondSignalPage::OnKillActive() 
{
	UpdateData();
	m_pAutoTestConfig->m_StartFreq2=m_StartFreq;
	m_pAutoTestConfig->m_EndFreq2=m_EndFreq;
	if (m_StepFreq==0)
		m_StepFreq=1;
	m_pAutoTestConfig->m_StepFreq2=m_StepFreq;

	m_pAutoTestConfig->m_ModulationType2 = m_ModulationTypeCombo.GetItemData(m_ModulationTypeCombo.GetCurSel());

	int index=m_StartDataRateCombo.GetCurSel();
	if (index!=CB_ERR)
		m_pAutoTestConfig->m_StartDataRate2=m_StartDataRateCombo.GetItemData(index);
	index=m_EndDataRateCombo.GetCurSel();
	if (index!=CB_ERR)
		m_pAutoTestConfig->m_EndDataRate2=m_EndDataRateCombo.GetItemData(index);

	return CAutoTestPropertyPage::OnKillActive();
}

BOOL CSecondSignalPage::OnInitDialog() 
{
	CAutoTestPropertyPage::OnInitDialog();
	for (int i=0; i<sizeof(DataRates)/sizeof(DataRates[0]); i++)
	{
		int DataRate=DataRates[i];
		char pszDataRate[32];
		sprintf(pszDataRate, "%d", DataRate);
		int index=m_StartDataRateCombo.AddString(pszDataRate);
		m_StartDataRateCombo.SetItemData(index, DataRate);
		index=m_EndDataRateCombo.AddString(pszDataRate);
		m_EndDataRateCombo.SetItemData(index, DataRate);
	}
	m_StartDataRateCombo.SetCurSel(0);
	m_EndDataRateCombo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////
// misc

BOOL DoYouReallyWantToCancel(CWnd *pParentWnd)
{
	return pParentWnd->MessageBox("Do you really want to cancel?", "Attention !!!", MB_YESNO | MB_ICONQUESTION) == IDYES;
}

