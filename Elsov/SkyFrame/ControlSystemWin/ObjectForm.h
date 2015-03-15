#ifndef __OBJECT_FORM_H_INCLUDED__
#define __OBJECT_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ObjectForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectForm form view

#include "AbstractForm.h"

// Forward declarations
class IControlTargetAPI;

class CObjectForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CObjectForm)

// Form Data
public:
	//{{AFX_DATA(CObjectForm)
	enum { IDD = IDD_OBJECT_FORM };
	CTabCtrl	m_DisplayTab;
	CComboBox	m_TypeComboBox;
	CString	m_NameEdit;
	CString	m_NameStatic;
	CString	m_TypeStatic;
	//}}AFX_DATA

// Attributes
public:
	CObjectForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CObjectForm)
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
	virtual ~CObjectForm();
	void LoadDisplaySwitcher();
	void TurnToAbstractPane();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CControlObject *m_pObject;
	IControlTargetAPI *m_pTarget;
	CTargetInstance *m_pTargetInstance;

	//{{AFX_MSG(CObjectForm)
	afx_msg void OnApplyButton();
	afx_msg void OnSelChangeDisplayTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangingDisplayTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __OBJECT_FORM_H_INCLUDED__
