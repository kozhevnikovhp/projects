#if !defined __DEMODULATOR_LNB_STATUS_FORM_H_INCLUDED__
#define __DEMODULATOR_LNB_STATUS_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DemodulatorLnbStatusForm.h : header file
//

#include "ModemComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorLnbStatusForm form view

class CDemodulatorLnbStatusForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CDemodulatorLnbStatusForm)

// Form Data
public:
	//{{AFX_DATA(CDemodulatorLnbStatusForm)
	enum { IDD = IDD_DEMODULATOR_LNB_STATUS_FORM };
	cPowerSupplyCombo m_PowerSupplyCombo;
	double	m_Current;
	double	m_CurrentMax;
	double	m_CurrentMin;
	CString	m_HeaderString;
	//}}AFX_DATA

// Attributes
public:
	CDemodulatorLnbStatusForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDemodulatorLnbStatusForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }

	// Implementation
protected:
	virtual ~CDemodulatorLnbStatusForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	void UpdateStatus();

	// Protected members
protected:
	CModem *m_pModem;

	//{{AFX_MSG(CDemodulatorLnbStatusForm)
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DEMODULATOR_LNB_STATUS_FORM_H_INCLUDED__
