//{{AFX_INCLUDES()
#include "calendar.h"
//}}AFX_INCLUDES
#if !defined(AFX_DELETETMIDIALOG_H__FA377010_F2BD_4259_941E_F92B903CF014__INCLUDED_)
#define AFX_DELETETMIDIALOG_H__FA377010_F2BD_4259_941E_F92B903CF014__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteTMIDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteTMIDialog dialog

class CDeleteTMIDialog : public CDialog
{
// Construction
public:
	CDeleteTMIDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteTMIDialog)
	enum { IDD = IDD_DELETE_TMI_FROM_TIME_TO_TIME_DIALOG };
	CCalendar	m_BeginDate;
	CCalendar	m_EndDate;
	//}}AFX_DATA
	CTime m_BeginTime, m_EndTime;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteTMIDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteTMIDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETETMIDIALOG_H__FA377010_F2BD_4259_941E_F92B903CF014__INCLUDED_)
