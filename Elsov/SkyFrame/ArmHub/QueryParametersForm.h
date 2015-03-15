#if !defined(AFX_QUERY_PARAMETERS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
#define AFX_QUERY_PARAMETERS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// QueryParametersForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQueryParametersForm form view

#include "AbstractForm.h"

class CQueryParametersForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CQueryParametersForm)

// Form Data
public:
	//{{AFX_DATA(CQueryParametersForm)
	enum { IDD = IDD_PARAMETERS_FORM };
	CSpinButtonCtrl	m_RequeryPeriodSpin;
	CSpinButtonCtrl	m_QueryPeriodSpin;
	BOOL	m_bQueryEnabled;
	UINT	m_QueryPeriod;
	CString	m_NextQueryTime;
	CString	m_LastReplyTime;
	CString	m_TimeElapsedSinceLastReply;
	CString	m_TimeTillNextQuery;
	UINT	m_RequeryPeriod;
	//}}AFX_DATA

// Attributes
public:
	CQueryParametersForm();

// Operations
public:

	// Overrides
	//{{AFX_VIRTUAL(CParametersForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:

	virtual void OnInitialUpdate();
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
//	virtual int DeltaTimerTime() { return 5; }
	virtual BOOL IsAlarm(CStation *pStation);

	// Implementation
protected:
	virtual ~CQueryParametersForm();
	void UpdateTimes();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CQueryParametersForm)
	afx_msg void OnApplyButton();
	afx_msg void OnQueryEnabledCheck();
	afx_msg void OnRightNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERY_PARAMETERS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
