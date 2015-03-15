//{{AFX_INCLUDES()
#include "calendar.h"
//}}AFX_INCLUDES
#if !defined(AFX_REQULATIONSQUERYPARAMSDIALOG_H__539EB0F7_5BD3_4404_8E09_A0F507D7DC57__INCLUDED_)
#define AFX_REQULATIONSQUERYPARAMSDIALOG_H__539EB0F7_5BD3_4404_8E09_A0F507D7DC57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// RequlationsQueryParamsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRequlationsQueryParamsDialog dialog

class CRegulationsQueryParamsDialog : public CDialog
{
// Construction
public:
	CRegulationsQueryParamsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegulationsQueryParamsDialog)
	enum { IDD = IDD_REGULATIONS_QUERY_DIALOG };
	CListBox	m_StationsListBox;
	CCalendar	m_BeginDate;
	CCalendar	m_EndDate;
	//}}AFX_DATA
	CTime m_BeginTime, m_EndTime;

// Overrides
	//{{AFX_VIRTUAL(CRegulationsQueryParamsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CRegulationsQueryParamsDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REQULATIONSQUERYPARAMSDIALOG_H__539EB0F7_5BD3_4404_8E09_A0F507D7DC57__INCLUDED_)
