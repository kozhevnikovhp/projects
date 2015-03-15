#if !defined(AFX_TMIOPTIONSDIALOG_H__E8E3C9E1_DCED_11D5_91BE_0050BA5D75FE__INCLUDED_)
#define AFX_TMIOPTIONSDIALOG_H__E8E3C9E1_DCED_11D5_91BE_0050BA5D75FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiOptionsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiOptionsDialog dialog

class CTmiOptionsDialog : public CDialog
{
// Construction
public:
	CTmiOptionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTmiOptionsDialog)
	enum { IDD = IDD_TMI_OPTIONS_DIALOG };
	short	m_IpPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiOptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTmiOptionsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIOPTIONSDIALOG_H__E8E3C9E1_DCED_11D5_91BE_0050BA5D75FE__INCLUDED_)
