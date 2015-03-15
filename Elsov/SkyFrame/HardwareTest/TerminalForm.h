#if !defined(AFX_TERMINALFORM_H__90D3D520_3612_11D4_BBDF_80D7975C816D__INCLUDED_)
#define AFX_TERMINALFORM_H__90D3D520_3612_11D4_BBDF_80D7975C816D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerminalForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerminalForm form view

class CDevice;

class CTerminalForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTerminalForm)

// Form Data
public:
	//{{AFX_DATA(CTerminalForm)
	enum { IDD = IDD_TERMINAL_FORM };
	CEdit	m_ReplyCtrl;
	CString	m_Reply;
	CString	m_Command;
	UINT	m_RepeatInterval;
	BOOL	m_bRepeat;
	//}}AFX_DATA

// Attributes
public:
	CTerminalForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTerminalForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual void OnMcSymbolReceived(char cSymbol);

// Implementation
protected:
	virtual ~CTerminalForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:
	CDevice *m_pDevice;
	DWORD m_LastCommandTime;

	// Generated message map functions
	//{{AFX_MSG(CTerminalForm)
	afx_msg void OnSendButton();
	afx_msg void OnSendCRLFButton();
	afx_msg void OnSendEscButton();
	afx_msg void OnSendFileButton();
	afx_msg void OnRepeatCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALFORM_H__90D3D520_3612_11D4_BBDF_80D7975C816D__INCLUDED_)
