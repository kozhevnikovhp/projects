#if !defined(AFX_PHYSICALPORTSFORM_H__FF6354C0_3C7F_11D4_BBDF_D1D3E6F46725__INCLUDED_)
#define AFX_PHYSICALPORTSFORM_H__FF6354C0_3C7F_11D4_BBDF_D1D3E6F46725__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhysicalPortsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhysicalPortsForm form view

class CNsgFrad;

class CPhysicalPortsForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CPhysicalPortsForm)

// Form Data
public:
	//{{AFX_DATA(CPhysicalPortsForm)
	enum { IDD = IDD_PHYSICAL_PORTS_FORM };
	CComboBox	m_PortNumberCombo;
	CString	m_PortConfigString;
	CString	m_StatusString;
	//}}AFX_DATA

// Attributes
public:
	CPhysicalPortsForm();

// Operations
public:
	void RefreshStatus();

// Overrides
	//{{AFX_VIRTUAL(CPhysicalPortsForm)
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
	virtual ~CPhysicalPortsForm();
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
	//{{AFX_MSG(CPhysicalPortsForm)
	afx_msg void OnSelChangePortNumberCombo();
	afx_msg void OnSetPortConfigButton();
	afx_msg void OnWarmButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSICALPORTSFORM_H__FF6354C0_3C7F_11D4_BBDF_D1D3E6F46725__INCLUDED_)
