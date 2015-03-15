#ifndef __DEMODULATOR_BERTEST_FORM_H__INCLUDED__
#define __DEMODULATOR_BERTEST_FORM_H__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DemodulatorBerTestForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorBerTestForm form view

#include "Modem.h"

class CDemodulatorBerTestForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CDemodulatorBerTestForm)

// Form Data
public:
	//{{AFX_DATA(CDemodulatorBerTestForm)
	enum { IDD = IDD_DEMODULATOR_BERTEST_FORM };
	CButton	m_Insert1ErrorButton;
	CString	m_HeaderString;
	UINT	m_SyncLosses;
	double	m_TotalBits;
	UINT	m_Duration;
	double	m_Errors;
	UINT	m_ErroredSeconds;
	double	m_BER;
	double	m_ErrorFreePercentage;
	//}}AFX_DATA

// Attributes
public:
	CDemodulatorBerTestForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDemodulatorBerTestForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

	// Implementation
protected:
	virtual ~CDemodulatorBerTestForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	void UpdateStatistics();
	void ChoosePower();

	// Protected members
protected:
	CModem *m_pModem;

	// Generated message map functions
	//{{AFX_MSG(CDemodulatorBerTestForm)
	afx_msg void OnDisabledRadio();
	afx_msg void On11Radio();
	afx_msg void On23Radio();
	afx_msg void OnRestartButton();
	afx_msg void OnInsert1errorButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DEMODULATOR_BERTEST_FORM_H__INCLUDED__
