//{{AFX_INCLUDES()
#include "calendar.h"
//}}AFX_INCLUDES
#if !defined(AFX_TMIQUERYPARAMSDIALOG_H__B8F7C204_E3AF_4A28_9F8E_5CBC63C04795__INCLUDED_)
#define AFX_TMIQUERYPARAMSDIALOG_H__B8F7C204_E3AF_4A28_9F8E_5CBC63C04795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiQueryParamsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParamsDialog dialog

class CTmiQueryParamsDialog : public CDialog
{
// Construction
public:
	CTmiQueryParamsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTmiQueryParamsDialog)
	enum { IDD = IDD_TMI_QUERY_DIALOG };
	CCalendar	m_BeginDate;
	CCalendar	m_EndDate;
	//}}AFX_DATA
	static CTime m_BeginTime, m_EndTime;
	static BOOL m_bCalendarsInitialized;
	BOOL m_bAZS;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiQueryParamsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTmiQueryParamsDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIQUERYPARAMSDIALOG_H__B8F7C204_E3AF_4A28_9F8E_5CBC63C04795__INCLUDED_)
