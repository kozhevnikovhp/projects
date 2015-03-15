#ifndef __TRANSMITTER_BERTEST_FORM_H__INCLUDED__
#define __TRANSMITTER_BERTEST_FORM_H__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TransmitterBerTestForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransmitterBerTestForm form view

#include "Modem.h"

class CModulatorBerTestForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CModulatorBerTestForm)

// Form Data
public:
	//{{AFX_DATA(CModulatorBerTestForm)
	enum { IDD = IDD_MODULATOR_BERTEST_FORM };
	CButton	m_Insert1ErrorButton;
	CString	m_HeaderString;
	//}}AFX_DATA

// Attributes
public:
	CModulatorBerTestForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CModulatorBerTestForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CModulatorBerTestForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	void ChoosePower();

	// Protected members
protected:
	CModem *m_pModem;

	//{{AFX_MSG(CModulatorBerTestForm)
	afx_msg void OnDisabledRadio();
	afx_msg void On11Radio();
	afx_msg void On23Radio();
	afx_msg void OnInsert1ErrorButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __TRANSMITTER_BERTEST_FORM_H__INCLUDED__
