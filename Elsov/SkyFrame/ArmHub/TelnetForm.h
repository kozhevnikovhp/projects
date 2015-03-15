#if !defined(AFX_TELNETFORM_H__777179A2_A3CC_4275_ACF1_1C1D2BA45FBC__INCLUDED_)
#define AFX_TELNETFORM_H__777179A2_A3CC_4275_ACF1_1C1D2BA45FBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TelnetForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTelnetForm form view

#include "AbstractForm.h"
class CTelnetSocket;

class CTelnetForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTelnetForm)

// Form Data
public:
	//{{AFX_DATA(CTelnetForm)
	enum { IDD = IDD_TELNET_FORM };
	CString	m_Command;
	CString	m_Telnet;
	//}}AFX_DATA

// Attributes
public:

	// Operations
public:
	CTelnetForm();

// Overrides
	//{{AFX_VIRTUAL(CTelnetForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL DoTimer();
	virtual BOOL SetupControls();

// Implementation
protected:
	virtual ~CTelnetForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CTelnetForm)
	afx_msg void OnConnectButton();
	afx_msg void OnDisconnectButton();
	afx_msg void OnRunButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CTelnetSocket *m_pTelnetSocket;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELNETFORM_H__777179A2_A3CC_4275_ACF1_1C1D2BA45FBC__INCLUDED_)
