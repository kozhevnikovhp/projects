//{{AFX_INCLUDES()
#include "calendar.h"
//}}AFX_INCLUDES
#if !defined(AFX_WORKINGHOURSDIALOG_H__8F42CD73_A36D_4F11_9A6D_86025083571D__INCLUDED_)
#define AFX_WORKINGHOURSDIALOG_H__8F42CD73_A36D_4F11_9A6D_86025083571D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkingHoursDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkingHoursDialog dialog

class CWorkingHoursDialog : public CDialog
{
// Construction
public:
	CWorkingHoursDialog(CWnd *pParent = NULL);   // standard constructor
	BOOL HoursNeeded() { return m_bHours; }

// Dialog Data
	//{{AFX_DATA(CWorkingHoursDialog)
	enum { IDD = IDD_WORKING_HOURS_QUERY_DIALOG };
	CCalendar	m_BeginDate;
	CCalendar	m_EndDate;
	//}}AFX_DATA
	CTime m_BeginTime, m_EndTime;
	BOOL m_bHours;

// Overrides
	//{{AFX_VIRTUAL(CWorkingHoursDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CWorkingHoursDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKINGHOURSDIALOG_H__8F42CD73_A36D_4F11_9A6D_86025083571D__INCLUDED_)
