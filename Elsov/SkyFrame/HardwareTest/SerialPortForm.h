#ifndef __SERIAL_PORT_FORM_H_INCLUDED__
#define __SERIAL_PORT_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SerialPortForm.h : header file
//

// Forward declarations
class CSdmsAPI;

/////////////////////////////////////////////////////////////////////////////
// CSerialPortForm form view

class CSerialPortForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CSerialPortForm)

// Form Data
public:
	//{{AFX_DATA(CSerialPortForm)
	enum { IDD = IDD_SERIAL_PORT_FORM };
	CString	m_DesiredIpAddress;
	CString	m_DesiredIpSubnetMask;
	//}}AFX_DATA

// Attributes
public:
	CSerialPortForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSerialPortForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CSerialPortForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:

	// Protected members
	CSdmsAPI *m_pSDMS;
protected:

	//{{AFX_MSG(CSerialPortForm)
	afx_msg void OnSetIpAddressButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SERIAL_PORT_FORM_H_INCLUDED__
