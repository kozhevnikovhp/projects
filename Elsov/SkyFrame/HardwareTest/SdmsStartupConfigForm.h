#ifndef __SDMS_STARTUP_CONFIG_FORM_H_INCLUDED__
#define __SDMS_STARTUP_CONFIG_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SdmsStartupConfigForm.h : header file
//

#include "StelaM.h"

/////////////////////////////////////////////////////////////////////////////
// CSdmsStartupConfigForm form view

class CSdmsStartupConfigForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CSdmsStartupConfigForm)

// Form Data
public:
	//{{AFX_DATA(CSdmsStartupConfigForm)
	enum { IDD = IDD_SDMS_STARTUP_CONFIG_FORM };
	CEdit	m_StartupConfigCtrl;
	CString	m_StartupConfig;
	//}}AFX_DATA

// Attributes
public:
	CSdmsStartupConfigForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSdmsStartupConfigForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL BeforeHiding();

	// Implementation
protected:
	virtual ~CSdmsStartupConfigForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:
	CSdmsAPI *m_pSDMS;

	//{{AFX_MSG(CSdmsStartupConfigForm)
	afx_msg void OnGetCurrentConfigButton();
	afx_msg void OnLoadFromFileButton();
	afx_msg void OnSaveConfigToFileButton();
	afx_msg void OnSetStartupConfigButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SDMS_STARTUP_CONFIG_FORM_H_INCLUDED__
