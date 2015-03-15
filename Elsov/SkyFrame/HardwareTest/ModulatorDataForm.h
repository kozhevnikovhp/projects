#ifndef __MODULATOR_DATA_FORM_H_INCLUDED__
#define __MODULATOR_DATA_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ModulatorDataForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModulatorDataForm form view

#include "DataForm.h"

class  CModulatorDataForm: public CDataForm
{
protected:
	DECLARE_DYNCREATE(CModulatorDataForm)

// Form Data
public:
	//{{AFX_DATA(CModulatorDataForm)
	enum { IDD = IDD_DATA_FORM };
	//}}AFX_DATA

// Attributes
public:
	CModulatorDataForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CModulatorDataForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

// Implementation
protected:
	virtual ~CModulatorDataForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	virtual BOOL CanDataClockInversion() { return m_pModem->CanTDataClockInv(); }
	virtual BOOL CanDiff() { return m_pModem->CanDiffEncoder(); }
	virtual BOOL CanScrambleDescramble() { return m_pModem->CanScramble(); }
	virtual BOOL CanDataInversion() { return m_pModem->CanTDataInv(); }

	// Protected members
protected:

	// Generated message map functions
	//{{AFX_MSG(CModulatorDataForm)
	afx_msg void OnSetDataRateButton();
	afx_msg void OnDataInversionCheck();
	afx_msg void OnDataClockInvCheck();
	afx_msg void OnSelChangeFecModeCombo();
	afx_msg void OnSelChangeDataClockSourceCombo();
	afx_msg void OnSelChangeScramblerModeCombo();
	afx_msg void OnSelChangeDifferentialModeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MODULATOR_DATA_FORM_H_INCLUDED__
