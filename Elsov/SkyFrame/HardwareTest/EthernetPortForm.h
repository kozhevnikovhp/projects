#ifndef __ETHERNET_PORT_FORM_H_INCLUDED__
#define __ETHERNET_PORT_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EthernetPortForm.h : header file
//

// Forward declarations
class CSdmsAPI;

#include "IpHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CEthernetPortForm form view

class CEthernetPortForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CEthernetPortForm)

// Form Data
public:
	//{{AFX_DATA(CEthernetPortForm)
	enum { IDD = IDD_ETHERNET_PORT_FORM };
	CComboBox	m_LocalHostCombo;
	CString	m_DesiredIpAddress;
	CString	m_DesiredIpSubnetMask;
	CString	m_Protocol;
	CString	m_DesiredMacAddress;
	//}}AFX_DATA

// Attributes
public:
	CEthernetPortForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CEthernetPortForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CEthernetPortForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:

	// Protected members
	CSdmsAPI *m_pSDMS;
	CIpHelper m_IpHelper;
protected:

	//{{AFX_MSG(CEthernetPortForm)
	afx_msg void OnCopyButton();
	afx_msg void OnSetIpAddressButton();
	afx_msg void OnRunTestButton();
	afx_msg void OnSetMacAddressButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __ETHERNET_PORT_FORM_H_INCLUDED__
