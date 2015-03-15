// Cuvs.h : main header file for the CUVS application
//

#if !defined(AFX_CUVS_H__D80D2AC5_FE82_47C2_A3E3_871CEB27AD57__INCLUDED_)
#define AFX_CUVS_H__D80D2AC5_FE82_47C2_A3E3_871CEB27AD57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCuvsApp:
// See Cuvs.cpp for the implementation of this class
//

class CCuvsApp : public CWinApp
{
public:
	CCuvsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCuvsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCuvsApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUVS_H__D80D2AC5_FE82_47C2_A3E3_871CEB27AD57__INCLUDED_)
