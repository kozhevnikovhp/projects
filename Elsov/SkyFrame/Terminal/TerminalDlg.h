// TerminalDlg.h : header file
//

#if !defined(AFX_TERMINALDLG_H__0F2B3B06_2CD3_11D4_BBDF_E687FCE07158__INCLUDED_)
#define AFX_TERMINALDLG_H__0F2B3B06_2CD3_11D4_BBDF_E687FCE07158__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"

/////////////////////////////////////////////////////////////////////////////
// CTerminalDlg dialog

class CTerminalDlg : public CDialog
{
// Construction
public:
	CSerialPort m_Port;
	CTerminalDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTerminalDlg)
	enum { IDD = IDD_TERMINAL_DIALOG };
	CString	m_Command;
	CString	m_Reply;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTerminalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnSend();
	afx_msg void OnSendCRLFButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALDLG_H__0F2B3B06_2CD3_11D4_BBDF_E687FCE07158__INCLUDED_)
