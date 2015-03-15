// TmiDatabase.h : main header file for the TMIDATABASE application
//

#if !defined(AFX_TMIDATABASE_H__E5D0DD78_E633_4288_98F8_99C630AD23F2__INCLUDED_)
#define AFX_TMIDATABASE_H__E5D0DD78_E633_4288_98F8_99C630AD23F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTmiDatabaseApp:
// See TmiDatabase.cpp for the implementation of this class
//

class CTmiDatabaseApp : public CWinApp
{
public:
	CTmiDatabaseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiDatabaseApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTmiDatabaseApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIDATABASE_H__E5D0DD78_E633_4288_98F8_99C630AD23F2__INCLUDED_)
