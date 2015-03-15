// MibViewer.h : main header file for the MIBVIEWER application
//

#if !defined(AFX_MIBVIEWER_H__DF9991CA_ABBE_4BE1_A0BF_06C8EC702FB2__INCLUDED_)
#define AFX_MIBVIEWER_H__DF9991CA_ABBE_4BE1_A0BF_06C8EC702FB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMibViewerApp:
// See MibViewer.cpp for the implementation of this class
//

class CMibViewerApp : public CWinApp
{
public:
	CMibViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMibViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMibViewerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIBVIEWER_H__DF9991CA_ABBE_4BE1_A0BF_06C8EC702FB2__INCLUDED_)
