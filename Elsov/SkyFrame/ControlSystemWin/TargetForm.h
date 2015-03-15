#ifndef __TARGET_FORM_H_INCLUDED__
#define __TARGET_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TargetForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetForm form view

#include "AbstractForm.h"

class CTargetForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTargetForm)

// Form Data
public:
	//{{AFX_DATA(CTargetForm)
	enum { IDD = IDD_TARGET_FORM };
	CString	m_NameEdit;
	CString	m_NameStatic;
	CString	m_DbConnectionEdit;
	//}}AFX_DATA

// Attributes
public:
	CTargetForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTargetForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL CanBeDeleted() { return TRUE; }
	virtual CTargetInstance *DeleteSelectedObject(CAttachedBlock *pAttached);
	virtual BOOL SetupControls();
	virtual BOOL BeforeHiding();
	virtual BOOL CanAddChild() { return TRUE; }
	virtual CControlObject *CreateNewObject(CTargetInstance *pTargetInstance);

	// Implementation
protected:
	virtual ~CTargetForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CTargetForm)
	afx_msg void OnSetDbConnectionButton();
	afx_msg void OnSetNameButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __TARGET_FORM_H_INCLUDED__
