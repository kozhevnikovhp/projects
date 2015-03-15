#ifndef __SUBSTITUTOR_FORM_H_INCLUDED__
#define __SUBSTITUTOR_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SubstitutorForm.h : header file
//

// Forward declarations
class CDeviceSubstitutor;

/////////////////////////////////////////////////////////////////////////////
// CSubstitutorForm form view


class CSubstitutorForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CSubstitutorForm)

// Form Data
public:
	//{{AFX_DATA(CSubstitutorForm)
	enum { IDD = IDD_SUBSTITUTOR_FORM };
	BOOL	m_bEnabled;
	//}}AFX_DATA

// Attributes
public:
	CSubstitutorForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSubstitutorForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CSubstitutorForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:
	CDeviceSubstitutor *m_pSubstitutor;

	// Generated message map functions
	//{{AFX_MSG(CSubstitutorForm)
	afx_msg void OnEnableCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SUBSTITUTOR_FORM_H_INCLUDED__
