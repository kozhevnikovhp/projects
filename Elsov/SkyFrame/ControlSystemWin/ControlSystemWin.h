// ControlSystemWin.h : main header file for the CONTROLSYSTEMWIN application
//

#if !defined(AFX_CONTROLSYSTEMWIN_H__2DF4EE7F_56DE_47F3_B820_822D87CD95D8__INCLUDED_)
#define AFX_CONTROLSYSTEMWIN_H__2DF4EE7F_56DE_47F3_B820_822D87CD95D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CControlSystemApp:
// See ControlSystemWin.cpp for the implementation of this class
//

class CControlSystemApp : public CWinApp
{
public:
	CControlSystemApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlSystemApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CControlSystemApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSYSTEMWIN_H__2DF4EE7F_56DE_47F3_B820_822D87CD95D8__INCLUDED_)
