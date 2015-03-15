// ComPortHelperView.h : interface of the CComPortHelperView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPORTHELPERVIEW_H__D8454873_523F_4CAB_94FB_2E710678CDDC__INCLUDED_)
#define AFX_COMPORTHELPERVIEW_H__D8454873_523F_4CAB_94FB_2E710678CDDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"

// Forward declarations
class CComPortHelperDoc;

class CTargetPort : public CSerialPort
{
public:
	virtual char *GetControlString();
	virtual unsigned int GetReadTimeout() { return 1; }
	virtual unsigned int GetWriteTimeout() { return 1; }
};


const unsigned int BUFFER_SIZE = 0xFFFF;

class CComPortHelperView : public CFormView
{
protected: // create from serialization only
	CComPortHelperView();
	DECLARE_DYNCREATE(CComPortHelperView)

public:
	//{{AFX_DATA(CComPortHelperView)
	enum { IDD = IDD_COMPORTHELPER_FORM };
	CButton	m_SkipInputButton;
	CEdit	m_ToPortCtrl;
	CButton	m_FromScrollToBottom;
	CEdit	m_FromPortCtrl;
	CComboBox	m_PortCombo;
	CString	m_FromPortString;
	CString	m_ToPortString;
	CString	m_ModeString;
	CString	m_PortStatus;
	//}}AFX_DATA

// Attributes
public:
	CComPortHelperDoc *GetDocument();
	CSerialPort *GetPort() { return &m_Port; }
	int OnIdle();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CComPortHelperView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComPortHelperView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTargetPort m_Port;
	int m_nByte;
	CFont m_Font;
	BOOL m_bFromScrollToBottom;
	BOOL m_bSkipInput;
	unsigned char m_szBuffer[BUFFER_SIZE];
	int m_nBytesToSend;
	BOOL m_bHalfByte;
	unsigned char m_cHalfOctet;

protected:
	void UpdateToString();
	void DigitPressed(unsigned char half_octet);
	//{{AFX_MSG(CComPortHelperView)
	afx_msg void OnOpenButton();
	afx_msg void OnDestroy();
	afx_msg void OnClearFromButton();
	afx_msg void OnFromScrollToBottomCheck();
	afx_msg void On0Button();
	afx_msg void On1Button();
	afx_msg void On2Button();
	afx_msg void On3Button();
	afx_msg void On4Button();
	afx_msg void On5Button();
	afx_msg void On6Button();
	afx_msg void On7Button();
	afx_msg void On8Button();
	afx_msg void On9Button();
	afx_msg void OnAButton();
	afx_msg void OnBButton();
	afx_msg void OnCButton();
	afx_msg void OnDButton();
	afx_msg void OnEButton();
	afx_msg void OnFButton();
	afx_msg void OnBackspaceButton();
	afx_msg void OnToClearButton();
	afx_msg void OnConfigureButton();
	afx_msg void OnSendButton();
	afx_msg void OnSkipInputCheck();
	afx_msg void OnCloseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ComPortHelperView.cpp
inline CComPortHelperDoc* CComPortHelperView::GetDocument()
   { return (CComPortHelperDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPORTHELPERVIEW_H__D8454873_523F_4CAB_94FB_2E710678CDDC__INCLUDED_)
