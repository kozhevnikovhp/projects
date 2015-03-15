#if !defined __MODULATOR_BUC_STATUS_FORM_H_INCLUDED__
#define __MODULATOR_BUC_STATUS_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ModulatorBucStatusForm.h : header file
//

#include "ModemComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CModulatorBucStatusForm form view

class CModulatorBucStatusForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CModulatorBucStatusForm)

// Form Data
public:
	//{{AFX_DATA(CModulatorBucStatusForm)
	enum { IDD = IDD_MODULATOR_BUC_STATUS_FORM };
	cPowerSupplyCombo m_PowerSupplyCombo;
	double	m_Current;
	double	m_CurrentMax;
	double	m_CurrentMin;
	double	m_Voltage;
	CString	m_HeaderString;
	//}}AFX_DATA

// Attributes
public:
	CModulatorBucStatusForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CModulatorBucStatusForm)
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
	virtual ~CModulatorBucStatusForm();
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

	//{{AFX_MSG(CModulatorBucStatusForm)
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MODULATOR_BUC_STATUS_FORM_H_INCLUDED__
