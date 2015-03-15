// Titov.h : main header file for the TITOV application
//

#if !defined(AFX_TITOV_H__F6B315F1_6AD2_4EE0_A15F_8CC5040C9F5C__INCLUDED_)
#define AFX_TITOV_H__F6B315F1_6AD2_4EE0_A15F_8CC5040C9F5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Hyperlink.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_EMailHyperlink;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTitovApp:
// See Titov.cpp for the implementation of this class
//

class CTitovApp : public CWinApp
{
public:
	CTitovApp();
	void Draw(CDC *pDC, CRect DrawRect);
	void DrawParticles(CDC *pDC);
	char *GetTimeString();
	char *GetNLeftString();
	char *GetNRightString();

// Overrides
	//{{AFX_VIRTUAL(CTitovApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
// Protected methods
// Public members
public:
	CDocument *m_pDoc;
	HPEN m_hLeftPen, m_hRightPen;
	CRect m_DrawRect;

// Implementation
	//{{AFX_MSG(CTitovApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITOV_H__F6B315F1_6AD2_4EE0_A15F_8CC5040C9F5C__INCLUDED_)
