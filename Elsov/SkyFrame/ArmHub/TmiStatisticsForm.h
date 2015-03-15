#if !defined(AFX_TMISTATISTICSFORM_H__5D0D9F41_DDC4_11D5_91BE_0050BA5D75FE__INCLUDED_)
#define AFX_TMISTATISTICSFORM_H__5D0D9F41_DDC4_11D5_91BE_0050BA5D75FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiStatisticsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiStatisticsForm form view

#include "AbstractForm.h"

class CTmiStatisticsForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTmiStatisticsForm)

// Form Data
public:
	//{{AFX_DATA(CTmiStatisticsForm)
	enum { IDD = IDD_TMI_STATISTICS_FORM };
	CEdit	m_StatisticsEditCtrl;
	CTabCtrl	m_Tab;
	CString	m_Statistics;
	//}}AFX_DATA

// Attributes
public:
	CTmiStatisticsForm();           // protected constructor used by dynamic creation
	CTmiStatisticsForm(UINT nIDTemplate);

// Operations
public:
	void UpdateStatisticsText();
	void UpdateTabbedStatisticsText();
	void UpdateUntabbedStatisticsText();

protected:
	CFont m_StatisticsFont;

// Overrides
	//{{AFX_VIRTUAL(CTmiStatisticsForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual CString GetStatisticsText(BOOL bIncremental) { return ""; }
	virtual CString GetStatisticsText(unsigned nTab, BOOL bIncremental) { return ""; }
	virtual BOOL IsTabVisible() { return FALSE; }

// Implementation
protected:
	virtual ~CTmiStatisticsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CTmiStatisticsForm)
	afx_msg void OnCumulativeRadio();
	afx_msg void OnIncrementalRadio();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFontButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMISTATISTICSFORM_H__5D0D9F41_DDC4_11D5_91BE_0050BA5D75FE__INCLUDED_)
