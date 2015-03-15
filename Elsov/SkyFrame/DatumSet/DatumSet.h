// DatumSet.h : main header file for the DATUMSET application
//

#if !defined(AFX_DATUMSET_H__2F0246A1_17CB_472A_A055_7253B9472905__INCLUDED_)
#define AFX_DATUMSET_H__2F0246A1_17CB_472A_A055_7253B9472905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDatumSetApp:
// See DatumSet.cpp for the implementation of this class
//

class CDatumSetApp : public CWinApp
{
public:
	CDatumSetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumSetApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDatumSetApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMSET_H__2F0246A1_17CB_472A_A055_7253B9472905__INCLUDED_)
