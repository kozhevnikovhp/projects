// ParticlesInBox.h : main header file for the PARTICLESINBOX application
//

#if !defined(AFX_PARTICLESINBOX_H__B5E75B73_9641_4267_8197_E2C2798F39FD__INCLUDED_)
#define AFX_PARTICLESINBOX_H__B5E75B73_9641_4267_8197_E2C2798F39FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// Forward declarations
class CParticlesInBoxDoc;
class CWorld;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxApp:
// See ParticlesInBox.cpp for the implementation of this class
//

class CParticlesInBoxApp : public CWinApp
{
public:
	CParticlesInBoxApp();
	CWorld *GetWorld();

// Overrides
	//{{AFX_VIRTUAL(CParticlesInBoxApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL
	CParticlesInBoxDoc *m_pDoc;

// Implementation
	//{{AFX_MSG(CParticlesInBoxApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLESINBOX_H__B5E75B73_9641_4267_8197_E2C2798F39FD__INCLUDED_)
