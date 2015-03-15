#ifndef __EVENT_LOG_FORM_H_INCLUDED__
#define __EVENT_LOG_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EventLogForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventLogForm form view

#include "AbstractForm.h"

// Forward declarations
//class IControlTargetAPI;

class CEventLogForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CEventLogForm)

// Form Data
public:
	//{{AFX_DATA(CEventLogForm)
	enum { IDD = IDD_EVENT_LOG_FORM };
	CListCtrl	m_EventList;
	//}}AFX_DATA

// Attributes
public:
	CEventLogForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CEventLogForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL BeforeHiding();

	// Implementation
protected:
	virtual ~CEventLogForm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CControlObject *m_pObject;
//	IControlTargetAPI *m_pTarget;
//	CTargetInstance *m_pTargetInstance;

	//{{AFX_MSG(CEventLogForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EVENT_LOG_FORM_H_INCLUDED__
