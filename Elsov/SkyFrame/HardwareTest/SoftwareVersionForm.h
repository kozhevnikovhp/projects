#ifndef __SDMS_SOFTWARE_VERSION_FORM_H_INCLUDED__
#define __SDMS_SOFTWARE_VERSION_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SdmsSoftwareVersionForm.h : header file
//

#include "StelaM.h"

/////////////////////////////////////////////////////////////////////////////
// CSdmsSoftwareVersionForm form view

class CSdmsSoftwareVersionForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CSdmsSoftwareVersionForm)

// Form Data
public:
	//{{AFX_DATA(CSdmsSoftwareVersionForm)
	enum { IDD = IDD_SDMS_SOFTWARE_VERSION_FORM };
	CString	m_SoftwareVersion;
	//}}AFX_DATA

// Attributes
public:
	CSdmsSoftwareVersionForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSdmsSoftwareVersionForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL BeforeHiding();

	// Implementation
protected:
	virtual ~CSdmsSoftwareVersionForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:
	CSdmsAPI *m_pSDMS;

	//{{AFX_MSG(CSdmsSoftwareVersionForm)
	afx_msg void OnSetupNewVersionButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SDMS_SOFTWARE_VERSION_FORM_H_INCLUDED__
