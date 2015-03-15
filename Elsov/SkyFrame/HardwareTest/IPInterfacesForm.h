#if !defined(AFX_IPINTERFACESFORM_H__BC295080_3E35_11D4_BBDF_97C0EB5A055F__INCLUDED_)
#define AFX_IPINTERFACESFORM_H__BC295080_3E35_11D4_BBDF_97C0EB5A055F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IPInterfacesForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIPInterfacesForm form view

class CNsgFrad;

class CIPInterfacesForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CIPInterfacesForm)

// Form Data
public:
	//{{AFX_DATA(CIPInterfacesForm)
	enum { IDD = IDD_IP_INTERFACES_FORM };
	CButton	m_CanRipCheck;
	CEdit	m_NumEdit;
	CComboBox	m_NumberCombo;
	CString	m_ConfigString;
	UINT	m_NUM;
	BOOL	m_bCanRIP;
	CString	m_StatusString;
	//}}AFX_DATA

// Attributes
public:
	CIPInterfacesForm();

// Operations
public:
	void RefreshStatus();

// Overrides
	//{{AFX_VIRTUAL(CIPInterfacesForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 10; }

	// Implementation
protected:
	virtual ~CIPInterfacesForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:
	CNsgFrad *m_pNsgFrad;

	// Generated message map functions
	//{{AFX_MSG(CIPInterfacesForm)
	afx_msg void OnSetConfigButton();
	afx_msg void OnSelChangeNumberCombo();
	afx_msg void OnWarmButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPINTERFACESFORM_H__BC295080_3E35_11D4_BBDF_97C0EB5A055F__INCLUDED_)
