#ifndef __MONITOR_AND_CONTROL_FORM_H_INCLUDED__
#define __MONITOR_AND_CONTROL_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MonitorAndControlForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonitorAndControlForm form view

#include "AbstractForm.h"

// Forward declarations
//class IControlTargetAPI;

class CMonitorAndControlForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CMonitorAndControlForm)

// Form Data
public:
	//{{AFX_DATA(CMonitorAndControlForm)
	enum { IDD = IDD_MONITOR_AND_CONTROL_FORM };
	CSpinButtonCtrl	m_SerialPortBusAddressSpin;
	CSpinButtonCtrl	m_TelnetPortSpin;
	CButton	m_UseParentConnectionCheck;
	CComboBox	m_PortComboBox;
	BOOL	m_bUseParentConnection;
	CString	m_strTelnetIpAddress;
	int		m_TelnetPortNo;
	long	m_SerialPortBusAddress;
	//}}AFX_DATA

// Attributes
public:
	CMonitorAndControlForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CMonitorAndControlForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL BeforeHiding();

	// Implementation
protected:
	virtual ~CMonitorAndControlForm();
	void EnableControls();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CControlObject *m_pObject;
//	IControlTargetAPI *m_pTarget;
//	CTargetInstance *m_pTargetInstance;

	//{{AFX_MSG(CMonitorAndControlForm)
	afx_msg void OnUseParentConnectionCheck();
	afx_msg void OnSerialPortRadio();
	afx_msg void OnGpibRadio();
	afx_msg void OnSnmpRadio();
	afx_msg void OnTelnetRadio();
	afx_msg void OnUnknownRadio();
	afx_msg void OnApplyButton();
	afx_msg void OnSerialPortConfigureButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MONITOR_AND_CONTROL_FORM_H_INCLUDED__
