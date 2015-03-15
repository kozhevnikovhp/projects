// AbstractForm.h : header file
//

#ifndef __ABSTRACT_FORM_H_INCLUDED__
#define __ABSTRACT_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "Resource.h"

// Forward declarations
class CStationsView;
class CCuvsDoc;

class CAbstractForm : public CFormView
{
	friend class CCuvsDoc;
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
	void SetDocument(CCuvsDoc *pDocument) { m_pDoc = pDocument; }

// Overrides
	//{{AFX_VIRTUAL(CAbstractForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls() { return TRUE; }
	virtual BOOL BeforeHiding() { return TRUE; }
	virtual BOOL CanDragItem(CAttachedBlock *pAttached) { return FALSE; }
	virtual BOOL CanDropItem(CAttachedBlock *pAttached) { return FALSE; }

// Implementation
protected:
	virtual ~CAbstractForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Protected members
protected:
	CStationsView *m_pStationsView;
	CCuvsDoc *m_pDoc;

	//{{AFX_MSG(CAbstractForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __ABSTRACT_FORM_H_INCLUDED__
