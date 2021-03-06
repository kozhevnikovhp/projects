#ifndef __ABSTRACT_FORM_H_INCLUDED__
#define __ABSTRACT_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AbstractForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "Resource.h"

// Forward declarations
class CControlObject;
class CTargetInstance;
class CAttachedBlock;
class CControlSystemDoc;
class CControlSystemView;

class CAbstractForm : public CFormView
{
protected:
	CAbstractForm();           // protected constructor used by dynamic creation
	CAbstractForm(UINT nIDTemplate);
	DECLARE_DYNCREATE(CAbstractForm)

// Form Data
public:
	//{{AFX_DATA(CAbstractForm)
	enum { IDD = IDD_ABSTRACT_FORM };
	//}}AFX_DATA
	CAttachedBlock *m_pAttached;

// Attributes
public:

// Operations
public:
	void SetDocument(CControlSystemDoc *pDocument) { m_pDoc = pDocument; }
	void Timer();
// Overrides
	//{{AFX_VIRTUAL(CAbstractForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL CanBeDeleted() { return FALSE; }
	virtual CTargetInstance *DeleteSelectedObject(CAttachedBlock *pAttached) { return NULL; }
	virtual BOOL SetupControls() { return TRUE; }
	virtual BOOL BeforeHiding() { return TRUE; }
	virtual BOOL DoTimer() { return TRUE; }
	virtual int DeltaTimerTime() { return 1; }
	virtual BOOL CanAddChild() { return FALSE; }
	virtual CControlObject *CreateNewObject(CTargetInstance *pTargetInstance) { return NULL; }

// Implementation
protected:
	virtual ~CAbstractForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	CControlSystemView *GetTreeView();
	CControlSystemDoc *GetDocument();

// Protected members
protected:
	CControlSystemDoc *m_pDoc;

	//{{AFX_MSG(CAbstractForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CTime m_LastTimerTime;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif __ABSTRACT_FORM_H_INCLUDED__