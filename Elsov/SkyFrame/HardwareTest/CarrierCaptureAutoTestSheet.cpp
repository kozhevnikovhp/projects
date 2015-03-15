// CarrierCaptureAutoTestSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CarrierCaptureAutoTestSheet.h"
#include "Modem.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char pszString[1024];
extern int DataRates[38];
/////////////////////////////////////////////////////////////////////////////
// CCarrierCaptureAutoTestSheet

IMPLEMENT_DYNAMIC(CCarrierCaptureAutoTestSheet, CAutoTestSheet)

CCarrierCaptureAutoTestSheet::CCarrierCaptureAutoTestSheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	AddPage(&m_DevicesPage);
	m_DevicesPage.m_nDevices=nDevices;
	m_DevicesPage.m_ppDevices=ppDevices;
	AddPage(&m_CarrierPage);
	AddPage(&m_DataPage);
	AddPage(&m_LevelPage);
	AddPage(&m_OtherPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig=&m_AutoTestConfig;
}

CCarrierCaptureAutoTestSheet::~CCarrierCaptureAutoTestSheet()
{
}

BEGIN_MESSAGE_MAP(CCarrierCaptureAutoTestSheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CCarrierCaptureAutoTestSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCarrierCaptureAutoTestSheet::RunAutoTest()
{
	// 1. Open file and write header
	CStdioFile file;
	if (!file.Open(m_AutoTestConfig.m_pszFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		MessageBox("Cannot open file!", "Attention!!!", MB_ICONEXCLAMATION);
		return;
	}
	WriteHeader(&file);

	CProgressDlg dlg;
	dlg.Create();

	// Write legend
	strcpy(pszString, "\n\t\t");
	WriteString(&file, pszString);
	// цикл по мощностям
	double Level=m_AutoTestConfig.m_StartLevel;
	do
	{
		strcpy(pszString, "Time, sec");
		WriteString(&file, pszString);
		if (m_AutoTestConfig.m_nTests==2)
			WriteString(&file, "(max/min)");
		if (m_AutoTestConfig.m_nTests>=3)
			WriteString(&file, "(max/min/ave)");
		strcpy(pszString, "\tOffset, Hz\tS/N, dB\tInput Level, dBm\t\t");
		WriteString(&file, pszString);
		Level += m_AutoTestConfig.m_StepLevel;
	} while ( (Level>m_AutoTestConfig.m_StartLevel && Level<=m_AutoTestConfig.m_EndLevel) ||
					(Level<m_AutoTestConfig.m_StartLevel && Level>=m_AutoTestConfig.m_EndLevel) );
	WriteString(&file, "\n");

	strcpy(pszString, "DataRate, baud\t\\Output Level, dB\t");
	WriteString(&file, pszString);
	// цикл по мощностям
	int nLevels=0;
	Level=m_AutoTestConfig.m_StartLevel;
	do
	{
		nLevels++;
		sprintf(pszString, "%5.1f\t\t\t\t\t", Level);
		WriteString(&file, pszString);
		Level+=m_AutoTestConfig.m_StepLevel;
	} while ( (Level>m_AutoTestConfig.m_StartLevel && Level<=m_AutoTestConfig.m_EndLevel) ||
					(Level<m_AutoTestConfig.m_StartLevel && Level>=m_AutoTestConfig.m_EndLevel) );
	WriteString(&file, "\n");
	
	// Set transmitter freq
	unsigned int freq=m_AutoTestConfig.m_ModulatorFreq;
	m_AutoTestConfig.m_pModulator[0]->SetTFrequency(freq, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
	BOOL bOnOff=TRUE;
	m_AutoTestConfig.m_pModulator[0]->TurnOutputOn(bOnOff, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);

	// Set modulation type
	unsigned int DataRate=64000;
	m_AutoTestConfig.m_pModulator[0]->SetTDataRate(DataRate, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
	m_AutoTestConfig.m_pDemodulator[0]->SetRDataRate(DataRate, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
	BOOL bBPSK=m_AutoTestConfig.m_bBPSK;
	m_AutoTestConfig.m_pModulator[0]->SetTBPSK(bBPSK, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
	bBPSK=m_AutoTestConfig.m_bBPSK;
	m_AutoTestConfig.m_pDemodulator[0]->SetRBPSK(bBPSK, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);

	int StartDataRateIndex=0;
	for (int i=0; i<sizeof(DataRates)/sizeof(DataRates[0]); i++)
		if (DataRates[i]==m_AutoTestConfig.m_StartDataRate)
			StartDataRateIndex=i;
	int EndDataRateIndex=0;
	for (i=0; i<sizeof(DataRates)/sizeof(DataRates[0]); i++)
		if (DataRates[i]==m_AutoTestConfig.m_EndDataRate)
			EndDataRateIndex=i;

	int DeltaDataRateIndex=1;
	if (EndDataRateIndex<StartDataRateIndex)
		DeltaDataRateIndex=-1;

	int nDataRates=abs(EndDataRateIndex-StartDataRateIndex)+1;
	int nMeasurements=nDataRates*nLevels;
	dlg.SetRange(0, nMeasurements);

	BOOL bCancel=FALSE;
	int j=StartDataRateIndex;
	// Цикл по скоростям передачи
	for (i=0; i<(abs(EndDataRateIndex-StartDataRateIndex))+1; i++)
	{
		int DataRate=DataRates[j];
		sprintf(pszString, "%d\t\t", DataRate);
		WriteString(&file, pszString);

		// цикл по мощностям
		double Level=m_AutoTestConfig.m_StartLevel;
		do
		{
			Measure(DataRate, Level, &file, &dlg);
			bCancel=dlg.CheckCancelButton();
			if (bCancel)
				break;
			Level+=m_AutoTestConfig.m_StepLevel;
		} while ( (Level>m_AutoTestConfig.m_StartLevel && Level<=m_AutoTestConfig.m_EndLevel) ||
					(Level<m_AutoTestConfig.m_StartLevel && Level>=m_AutoTestConfig.m_EndLevel) );
		WriteString(&file, "\n");
		j+=DeltaDataRateIndex;
		if (bCancel)
			break;
	}
	
	// Close file
	file.Close();
}

void CCarrierCaptureAutoTestSheet::WriteHeader(CStdioFile *pFile)
{
	char *pszStr="Carrier acquisition investigation:\n";
	WriteString(pFile, pszStr);

	// Time
	CTime time=CTime::GetCurrentTime();
	CString string=time.Format("Time: %d-%m-%Y %H:%M:%S\n");
	WriteString(pFile, (char *)LPCTSTR(string));

	// Modulator version
	WriteString(pFile, "\nModulator:\n");
	WriteString(pFile, m_AutoTestConfig.m_pModulator[0]->GetName());
	WriteString(pFile, "\n");
	char *pszModulatorVersion;
	m_AutoTestConfig.m_pModulator[0]->GetModulatorVersionString(pszModulatorVersion, m_AutoTestConfig.m_nModulator[0]);
	WriteString(pFile, pszModulatorVersion);
	sprintf(pszString, "Unit %d\n", m_AutoTestConfig.m_nModulator[0]);
	WriteString(pFile, pszString);

	// Demodulator version
	WriteString(pFile, "\nDemodulator:\n");
	WriteString(pFile, m_AutoTestConfig.m_pDemodulator[0]->GetName());
	WriteString(pFile, "\n");
	char *pszDemodulatorVersion;
	m_AutoTestConfig.m_pDemodulator[0]->GetDemodulatorVersionString(pszDemodulatorVersion, m_AutoTestConfig.m_nDemodulator[0]);
	WriteString(pFile, pszDemodulatorVersion);
	sprintf(pszString, "Unit %d\n", m_AutoTestConfig.m_nModulator[0]);
	WriteString(pFile, pszString);

	// Modulation and frequencies
	WriteString(pFile, "\nModulation type:");
	if (m_AutoTestConfig.m_bBPSK)
		WriteString(pFile, "BPSK\n");
	else
		WriteString(pFile, "QPSK\n");

	sprintf(pszString, "Modulator frequency=%d KHz\nDemodulator frequency=%d KHz\nSpare demodulator frequency=%d KHz\n",
			m_AutoTestConfig.m_ModulatorFreq, m_AutoTestConfig.m_DemodulatorFreq, m_AutoTestConfig.m_DemodulatorSpareFreq);
	WriteString(pFile, pszString);

	// Sweep range and sweep type
	unsigned int sweep;
	m_AutoTestConfig.m_pDemodulator[0]->GetSearchRange(sweep, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
	sprintf(pszString, "Sweep range=%d Hz\n", sweep);
	WriteString(pFile, pszString);

	// Num tests
	sprintf(pszString, "%d measurements for each parameter set\n", m_AutoTestConfig.m_nTests);
	WriteString(pFile, pszString);
}

void CCarrierCaptureAutoTestSheet::Measure(unsigned int DataRate, double Level,
										   CStdioFile *pFile, CProgressDlg *pDlg)
{
	const DWORD MAX_DELAY=20000; // 20 sec
	pDlg->StepIt();
	sprintf(pszString, "DataRate=%d baud, Output Level=%5.1f dB", DataRate, Level);
	pDlg->SetStatus(pszString);

	// Set date rates
	unsigned int _DataRate=DataRate;
	m_AutoTestConfig.m_pDemodulator[0]->SetRDataRate(_DataRate, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
	if (DataRate!=_DataRate)
	{
		WriteString(pFile, "\t");
		return;
	}
	
	_DataRate=DataRate;
	m_AutoTestConfig.m_pModulator[0]->SetTDataRate(_DataRate, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
	if (DataRate!=_DataRate)
	{
		WriteString(pFile, "\t");
		return;
	}

	if (pDlg->CheckCancelButton())
		return;

	double nDelay=0.;
	DWORD nMinDelay=MAX_DELAY;
	DWORD nMaxDelay=0;
	double nOffset=0;
	double nSN=0.;
	double nLevel=0.;

	sprintf(pszString, "%d,%5.0f.txt", DataRate, Level);
	FILE *fd = fopen(pszString, "a+");
	if (fd)
		fprintf(fd, "Ntest\tDelay\tS/N\tOffset\tLevel\n");
	for (int i=0; i<m_AutoTestConfig.m_nTests; i++)
	{
		sprintf(pszString, "DataRate=%d baud, Output Level=%5.1f dB Test %d", DataRate, Level, i+1);
		pDlg->SetStatus(pszString);
		// Turn transmitter off
		BOOL bOnOff=FALSE;
		if (m_AutoTestConfig.m_bTurnModOffOn)
		{
			m_AutoTestConfig.m_pModulator[0]->TurnOutputOn(bOnOff, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
		}
		Sleep(1000);
		
		if (pDlg->CheckCancelButton())
			return;

		// Move receiver far from test frequency
		unsigned int SpareFreq=m_AutoTestConfig.m_DemodulatorSpareFreq;
		m_AutoTestConfig.m_pDemodulator[0]->SetRFrequency(SpareFreq, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
		BOOL bIsCarrier=TRUE;
		int n=0;
		do
		{
			if (pDlg->CheckCancelButton())
				return;

			Sleep(20);
			n++;
			m_AutoTestConfig.m_pDemodulator[0]->IsRCarrier(bIsCarrier, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
		} while (bIsCarrier && n<100);
		Sleep(1000);

		// Set output level
		m_AutoTestConfig.m_pModulator[0]->SetOutputLevel(Level, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
		if (pDlg->CheckCancelButton())
			return;

		// Turn transmitter on
		bOnOff=TRUE;
		if (m_AutoTestConfig.m_bTurnModOffOn)
		{
			m_AutoTestConfig.m_pModulator[0]->TurnOutputOn(bOnOff, m_AutoTestConfig.m_nModulator[m_AutoTestConfig.m_CurrentTransmitter]);
			Sleep(1000);
		}
		
		if (pDlg->CheckCancelButton())
			return;

		// Move receiver to the test frequency
		unsigned int freq=m_AutoTestConfig.m_DemodulatorFreq;
		DWORD DeltaTime = 0, StartTime = timeGetTime();
		m_AutoTestConfig.m_pDemodulator[0]->SetRFrequency(freq, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
		bIsCarrier=FALSE;
		do
		{
			if (pDlg->CheckCancelButton())
				return;

			m_AutoTestConfig.m_pDemodulator[0]->IsRCarrier(bIsCarrier, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
			DeltaTime=timeGetTime()-StartTime;
		} while (!bIsCarrier && DeltaTime<MAX_DELAY);
		nDelay += DeltaTime;
		if (DeltaTime > nMaxDelay)
			nMaxDelay = DeltaTime;
		if (DeltaTime < nMinDelay)
			nMinDelay = DeltaTime;
		Sleep(10000);
		CDemIfStatus DemStatus;
		m_AutoTestConfig.m_pDemodulator[0]->GetRIfStatus(DemStatus, m_AutoTestConfig.m_nDemodulator[m_AutoTestConfig.m_CurrentReceiver]);
		nOffset += DemStatus.m_Offset;
		nSN += DemStatus.m_SN;
		nLevel += DemStatus.m_InputLevel;
		if (fd)
			fprintf(fd, "%d\t%6.2f\t%5.1f\t%+d\t%5.1f\n", i+1, DeltaTime/1000., DemStatus.m_SN, DemStatus.m_Offset, DemStatus.m_InputLevel);
	}
	fclose(fd);

	static char szMax[16], szMin[16], szAve[16];
	switch (m_AutoTestConfig.m_nTests)
	{
	case 1:
		FormatDelayString(nDelay, szAve);
		sprintf(pszString, "%s\t", szAve);
		break;
	case 2:
		FormatDelayString(nMaxDelay, szMax);
		FormatDelayString(nMinDelay, szMin);
		sprintf(pszString, "%s/%s\t", szMax, szMin);
		break;
	default:
		FormatDelayString(nDelay/m_AutoTestConfig.m_nTests, szAve);
		FormatDelayString(nMaxDelay, szMax);
		FormatDelayString(nMinDelay, szMin);
		sprintf(pszString, "%s/%s/%s\t", szMax, szMin, szAve);
//		sprintf(pszString, "%s\t", szAve); // ave only
		break;
	}
	WriteString(pFile, pszString);
	sprintf(pszString, "%5.1f\t%5.2f\t%g\t\t", nOffset/m_AutoTestConfig.m_nTests,
		nSN/m_AutoTestConfig.m_nTests, nLevel/m_AutoTestConfig.m_nTests);
	WriteString(pFile, pszString);
}

/////////////////////////////////////////////////////////////////////////////
// CCarrierCaptureAutoTestSheet message handlers


