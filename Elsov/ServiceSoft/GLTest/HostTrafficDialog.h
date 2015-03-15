#if !defined(AFX_HOSTTRAFFICDIALOG_H__E43ACCA3_FA36_4747_8AB2_FE098249F960__INCLUDED_)
#define AFX_HOSTTRAFFICDIALOG_H__E43ACCA3_FA36_4747_8AB2_FE098249F960__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HostTrafficDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHostTrafficDialog dialog

class CHostTrafficDialog : public CDialog
{
// Construction
public:
	CHostTrafficDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHostTrafficDialog)
	enum { IDD = IDD_HOST_TRAFFIC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHostTrafficDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHostTrafficDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOSTTRAFFICDIALOG_H__E43ACCA3_FA36_4747_8AB2_FE098249F960__INCLUDED_)
