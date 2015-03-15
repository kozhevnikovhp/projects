// AutoTestSheet.h : header file
//
// This class defines custom modal property sheet 
// CAutoTestSheet.
 
#ifndef __AUTOTEST_SHEET_H__
#define __AUTOTEST_SHEET_H__

#include "AutoTestPages.h"
class CProgressDlg;

/////////////////////////////////////////////////////////////////////////////
// CIntervalAutoTestSheet

const DWORD MAX_DELAY=10000; // 20 sec

class CAutoTestSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAutoTestSheet)

// Construction
public:
	CAutoTestSheet(CWnd *pParentWnd=NULL);

// Attributes
public:
	SAutoTestConfig m_AutoTestConfig;

// Operations
public:
	void FormatDelayString(double nDelay, char *pszStr);
	void WriteString(CStdioFile *pFile, char *pszStr);

// Overrides
	//{{AFX_VIRTUAL(CAutoTestSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnWizard();
	virtual ~CAutoTestSheet();

protected:
	CStdioFile m_ReportFile;

	//{{AFX_MSG(CAutoTestSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __AUTOTEST_SHEET_H__
