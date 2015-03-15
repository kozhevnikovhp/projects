// AutoTestSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AutoTestSheet.h"
#include "ProgressDlg.h"
#include "CommonTests.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoTestSheet

IMPLEMENT_DYNAMIC(CAutoTestSheet, CPropertySheet)

CAutoTestSheet::CAutoTestSheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	ZeroMemory(&m_AutoTestConfig, sizeof(m_AutoTestConfig));
	for (int i=0; i<=MAX_TRANSMITTER; i++)
		m_AutoTestConfig.m_nModulator[i]=1;
	for (i=0; i<=MAX_RECEIVER; i++)
		m_AutoTestConfig.m_nDemodulator[i]=1;
	CAutoTestPropertyPage::m_pAutoTestConfig=&m_AutoTestConfig;
	m_AutoTestConfig.m_pReportFile = &m_ReportFile;
}

CAutoTestSheet::~CAutoTestSheet()
{
	if (m_AutoTestConfig.m_pReportFile && m_AutoTestConfig.m_pReportFile->m_pStream)
		m_AutoTestConfig.m_pReportFile->Close();
}

BEGIN_MESSAGE_MAP(CAutoTestSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CAutoTestSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAutoTestSheet::FormatDelayString(double nDelay, char *pszStr)
{
	if (nDelay >= MAX_DELAY)
		strcpy(pszStr, "##");
	else
		sprintf(pszStr, "%5.2f", nDelay/1000.);
}

void CAutoTestSheet::WriteString(CStdioFile *pFile, char *pszStr)
{
	pFile->Write(pszStr, strlen(pszStr));
}

/////////////////////////////////////////////////////////////////////////////
// CIntervalAutoTestSheet message handlers

