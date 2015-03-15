// CommonTests.h : header file
//

#ifndef __COMMONTESTS_H__
#define __COMMONTESTS_H__

#include "AutoTestSheet.h"
class CProgressDlg;

/////////////////////////////////////////////////////////////////////////////
// CCommonTestsEthernetPage dialog

class CCommonTestsEthernetPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CCommonTestsEthernetPage)

// Construction
public:
	CCommonTestsEthernetPage();
	~CCommonTestsEthernetPage();

// Dialog Data
	//{{AFX_DATA(CCommonTestsEthernetPage)
	enum { IDD = IDD_COMMON_TESTS_ETHERNET_PROPPAGE };
	CString	m_Report;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CCommonTestsEthernetPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCommonTestsEthernetPage)
	afx_msg void OnRunButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CCommonTestsCongratulationsPage dialog

class CCommonTestsCongratulationsPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CCommonTestsCongratulationsPage)

// Construction
public:
	CCommonTestsCongratulationsPage();
	~CCommonTestsCongratulationsPage();

// Dialog Data
	//{{AFX_DATA(CCommonTestsCongratulationsPage)
	enum { IDD = IDD_COMMON_TESTS_CONGRATULATIONS_PROPPAGE };
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CCommonTestsCongratulationsPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCommonTestsCongratulationsPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CFrequencyDependentCorrectionDialog dialog

class CFrequencyDependentCorrectionDialog : public CDialog
{
// Construction
public:
	CFrequencyDependentCorrectionDialog(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrequencyDependentCorrectionDialog)
	enum { IDD = IDD_FREQUENCY_DEPENDENT_CORRECTION_DIALOG };
	double	m_Correction1;
	double	m_Correction2;
	UINT	m_Freq1;
	UINT	m_Freq2;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CFrequencyDependentCorrectionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CFrequencyDependentCorrectionDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CCorrectionDialog dialog

class CCorrectionDialog : public CDialog
{
// Construction
public:
	CCorrectionDialog(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCorrectionDialog)
	enum { IDD = IDD_CORRECTION_DIALOG };
	double	m_Correction;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CCorrectionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CCorrectionDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __COMMONTESTS_H__
