// ModulatorCarrierAutoTestSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ModulatorCarrierAutoTestSheet.h"
#include "Modem.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char pszString[1024];

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierAutoTestSheet

IMPLEMENT_DYNAMIC(CModulatorCarrierAutoTestSheet, CAutoTestSheet)

CModulatorCarrierAutoTestSheet::CModulatorCarrierAutoTestSheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent)
	 : CAutoTestSheet(pWndParent)
{
	AddPage(&m_DevicesPage);
	m_DevicesPage.m_nDevices=nDevices;
	m_DevicesPage.m_ppDevices=ppDevices;

	AddPage(&m_CarrierPage);

	AddPage(&m_OtherPage);

	SetWizardMode();
	CAutoTestPropertyPage::m_pAutoTestConfig=&m_AutoTestConfig;
}

CModulatorCarrierAutoTestSheet::~CModulatorCarrierAutoTestSheet()
{
}

BEGIN_MESSAGE_MAP(CModulatorCarrierAutoTestSheet, CAutoTestSheet)
	//{{AFX_MSG_MAP(CModulatorCarrierAutoTestSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CModulatorCarrierAutoTestSheet::RunAutoTest()
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

	int nFreqs=abs((m_AutoTestConfig.m_EndFreq2-m_AutoTestConfig.m_StartFreq2)/m_AutoTestConfig.m_StepFreq2)+1;
	int nMeasurements=nFreqs;
	dlg.SetRange(0, nMeasurements);

	BOOL bCancel=FALSE;
	// Цикл по частотам
	for (unsigned freq=m_AutoTestConfig.m_StartFreq2; freq<=m_AutoTestConfig.m_EndFreq2; freq+=m_AutoTestConfig.m_StepFreq2)
	{
		sprintf(pszString, "%d\t\t", freq);
		WriteString(&file, pszString);

		Measure(freq, &file, &dlg);

		WriteString(&file, "\n");
		if (dlg.CheckCancelButton())
			break;
	}
	
	// Close file
	file.Close();
}

void CModulatorCarrierAutoTestSheet::WriteHeader(CStdioFile *pFile)
{
	char *pszStr="Modulator carrier investigation:\n";
	WriteString(pFile, pszStr);

	// Time
	CTime time=CTime::GetCurrentTime();
	CString string=time.Format("Time: %d-%m-%Y %H:%M:%S\n");
	WriteString(pFile, (char *)LPCTSTR(string));

	// Modulator version
	WriteString(pFile, "\nModulator:\n");
	WriteString(pFile, m_AutoTestConfig.m_pModulator[0]->GetName());
	WriteString(pFile, "\n");
	sprintf(pszString, "Unit %d\n", m_AutoTestConfig.m_nModulator[0]);
	WriteString(pFile, pszString);
	char *pszModulatorVersion;
	m_AutoTestConfig.m_pModulator[0]->GetModulatorVersionString(pszModulatorVersion, m_AutoTestConfig.m_nModulator[0]);
	WriteString(pFile, pszModulatorVersion);

	// Modulation and frequencies
	WriteString(pFile, "\nModulation type:");
	if (m_AutoTestConfig.m_bBPSK)
		WriteString(pFile, "BPSK\n");
	else
		WriteString(pFile, "QPSK\n");
}

void CModulatorCarrierAutoTestSheet::Measure(unsigned int freq,
										   CStdioFile *pFile, CProgressDlg *pDlg)
{
	pDlg->StepIt();
	sprintf(pszString, "Freq=%d KHz", freq);
	pDlg->SetStatus(pszString);

	if (pDlg->CheckCancelButton())
		return;

	unsigned int real_freq=freq;
	m_AutoTestConfig.m_pModulator[0]->SetTFrequency(real_freq, m_AutoTestConfig.m_nModulator[0]);

	sprintf(pszString, "%d\t%d\t%d", freq, real_freq, freq-real_freq);
	WriteString(pFile, pszString);
}

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierAutoTestSheet message handlers


