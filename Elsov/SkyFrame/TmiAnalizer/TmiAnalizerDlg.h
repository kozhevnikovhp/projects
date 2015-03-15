// TmiAnalizerDlg.h : header file
//

#if !defined(AFX_TMIANALIZERDLG_H__8666D83C_E4B8_11D5_91BE_0050BA5D75FE__INCLUDED_)
#define AFX_TMIANALIZERDLG_H__8666D83C_E4B8_11D5_91BE_0050BA5D75FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTmiAnalizerDlg dialog

class CTmiAnalizerDlg : public CDialog
{
// Construction
public:
	CTmiAnalizerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTmiAnalizerDlg)
	enum { IDD = IDD_TMIANALIZER_DIALOG };
	BOOL	m_bSplitByDates;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiAnalizerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTmiAnalizerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNewFileButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIANALIZERDLG_H__8666D83C_E4B8_11D5_91BE_0050BA5D75FE__INCLUDED_)
