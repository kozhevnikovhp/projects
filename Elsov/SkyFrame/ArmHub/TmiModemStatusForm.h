#if !defined(AFX_TMI_MODEM_STATUS_FORM_H__091E44C9_E003_11D5_91BE_0050BA5D75FE__INCLUDED_)
#define AFX_TMI_MODEM_STATUS_FORM_H__091E44C9_E003_11D5_91BE_0050BA5D75FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TmiModemStatusForm.h : header file
//

#include "AbstractForm.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiModemStatusForm form view

class CTmiModemStatusForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTmiModemStatusForm)

// Form Data
public:
	//{{AFX_DATA(CTmiModemStatusForm)
	enum { IDD = IDD_TMI_MODEM_STATUS_FORM };
	CStatic	m_BPCHTransmitterCtrl;
	CStatic	m_BPCHReceiverCtrl;
	CString	m_ModemType;
	CString	m_SignalNoise;
	CString	m_AruValue;
	//}}AFX_DATA

// Attributes
public:
	CTmiModemStatusForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTmiModemStatusForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

// Implementation
protected:
	virtual ~CTmiModemStatusForm();
	void UpdateAll();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CTmiModemStatusForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMI_MODEM_STATUS_FORM_H__091E44C9_E003_11D5_91BE_0050BA5D75FE__INCLUDED_)
