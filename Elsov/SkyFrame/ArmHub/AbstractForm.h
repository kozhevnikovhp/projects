#if !defined(AFX_ABSTRACTFORM_H__1C2CEF08_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
#define AFX_ABSTRACTFORM_H__1C2CEF08_300A_11D4_BBDF_E46DF892F25A__INCLUDED_

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

class CAttachedBlock;
class CStation;
class CStationsView;

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
	CStation *m_pStation;

// Attributes
public:
	CStationsView *m_pStationsView;

// Operations
public:
	void Timer();
// Overrides
	//{{AFX_VIRTUAL(CAbstractForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls() { return TRUE; }
	virtual BOOL BeforeHiding() { return TRUE; }
	virtual BOOL DoTimer() { return TRUE; }
	virtual int DeltaTimerTime() { return 1; }
	virtual BOOL IsAlarm(CStation *pStation) { return FALSE; }

// Implementation
protected:
	virtual ~CAbstractForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected members
protected:

	//{{AFX_MSG(CAbstractForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CTime m_LastTimerTime;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSTRACTFORM_H__1C2CEF08_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
