#if !defined(AFX_NSGSCRIPTFORM_H__445AB600_3BB2_11D4_BBDF_FB7CE9B7235F__INCLUDED_)
#define AFX_NSGSCRIPTFORM_H__445AB600_3BB2_11D4_BBDF_FB7CE9B7235F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NsgScriptForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNsgScriptForm form view

class CNsgFrad;

class CNsgScriptForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CNsgScriptForm)

// Form Data
public:
	//{{AFX_DATA(CNsgScriptForm)
	enum { IDD = IDD_NSG_SCRIPT_FORM };
	CString	m_ScriptBody;
	//}}AFX_DATA

// Attributes
public:
	CNsgScriptForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CNsgScriptForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CNsgScriptForm();
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
	//{{AFX_MSG(CNsgScriptForm)
	afx_msg void OnApplyButton();
	afx_msg void OnMakeScriptButton();
	afx_msg void OnSaveAsButton();
	afx_msg void OnLoadButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NSGSCRIPTFORM_H__445AB600_3BB2_11D4_BBDF_FB7CE9B7235F__INCLUDED_)
