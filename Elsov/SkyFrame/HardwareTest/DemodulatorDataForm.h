#ifndef __DEMODULATOR_DATA_FORM_H_INCLUDED__
#define __DEMODULATOR_DATA_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DemodulatorDataForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorDataForm form view

#include "DataForm.h"

class  CDemodulatorDataForm: public CDataForm
{
protected:
	DECLARE_DYNCREATE(CDemodulatorDataForm)

// Form Data
public:
	//{{AFX_DATA(CDemodulatorDataForm)
	enum { IDD = IDD_DATA_FORM };
	//}}AFX_DATA

// Attributes
public:
	CDemodulatorDataForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDemodulatorDataForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

// Implementation
protected:
	virtual ~CDemodulatorDataForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	virtual BOOL CanDataRate() { return m_pModem->CanDataRate(); }
	virtual BOOL CanDataClockInversion() { return m_pModem->CanRDataClockInv(); }
	virtual BOOL CanDiff() { return m_pModem->CanDiffDecoder(); }
	virtual BOOL CanScrambleDescramble() { return m_pModem->CanDescramble(); }
	virtual BOOL CanDataInversion() { return m_pModem->CanRDataInv(); }

	// Protected members
protected:

	// Generated message map functions
	//{{AFX_MSG(CDemodulatorDataForm)
	afx_msg void OnSetDataRateButton();
	afx_msg void OnDataInversionCheck();
	afx_msg void OnDataClockInvCheck();
	afx_msg void OnSelChangeFecModeCombo();
	afx_msg void OnSelChangeFecOptionCombo();
	afx_msg void OnSelChangeFecCodeRateCombo();
	afx_msg void OnSelChangeScramblerModeCombo();
	afx_msg void OnSelChangeDataClockSourceCombo();
	afx_msg void OnSelChangeDifferentialModeCombo();
	afx_msg void OnSetDopplerBufferSizeButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DEMODULATOR_DATA_FORM_H_INCLUDED__
