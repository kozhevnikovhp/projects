// GLTest.h : main header file for the GLTEST application
//

#if !defined(AFX_GLTEST_H__B5CFCF0E_12AE_4A26_9F76_67EA059E09B8__INCLUDED_)
#define AFX_GLTEST_H__B5CFCF0E_12AE_4A26_9F76_67EA059E09B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGLTestApp:
// See GLTest.cpp for the implementation of this class
//

class CGLTestApp : public CWinApp
{
public:
	CGLTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGLTestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLTEST_H__B5CFCF0E_12AE_4A26_9F76_67EA059E09B8__INCLUDED_)
