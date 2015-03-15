#if !defined(AFX_PVCFORM_H__3205DD60_486C_11D4_BBDF_F9A696E77E7C__INCLUDED_)
#define AFX_PVCFORM_H__3205DD60_486C_11D4_BBDF_F9A696E77E7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PvcForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPvcForm form view

class CNsgFrad;

class CPvcForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CPvcForm)

// Form Data
public:
	//{{AFX_DATA(CPvcForm)
	enum { IDD = IDD_PVC_FORM };
	CString	m_PvcString;
	//}}AFX_DATA

// Attributes
public:
	CPvcForm();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPvcForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

// Implementation
protected:
	virtual ~CPvcForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	void RefreshTable();

	// Protected members
protected:
	CNsgFrad *m_pNsgFrad;

	// Generated message map functions
	//{{AFX_MSG(CPvcForm)
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnModifyButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PVCFORM_H__3205DD60_486C_11D4_BBDF_F9A696E77E7C__INCLUDED_)
