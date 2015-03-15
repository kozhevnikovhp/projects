#if !defined(AFX_IPADDRESSFORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
#define AFX_IPADDRESSFORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IpAddressForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIpAddressForm form view

#include "AbstractForm.h"

class CIpAddressForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CIpAddressForm)

// Form Data
public:
	//{{AFX_DATA(CIpAddressForm)
	enum { IDD = IDD_IPADDRESS_FORM };
	CSpinButtonCtrl	m_PingPeriodSpin;
	CString	m_IpAddress;
	CString	m_IpAddressRCh;
	UINT	m_PingPeriod;
	BOOL	m_bPingEnabled;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	CIpAddressForm();

	// Overrides
	//{{AFX_VIRTUAL(CIpAddressForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL DoTimer();
	virtual BOOL SetupControls();
	virtual BOOL IsAlarm(CStation *pStation);

	// Implementation
protected:
	virtual ~CIpAddressForm();
	void UpdateTexts();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CIpAddressForm)
	afx_msg void OnApplyButton();
	afx_msg void OnPingEnabledCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPADDRESSFORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
