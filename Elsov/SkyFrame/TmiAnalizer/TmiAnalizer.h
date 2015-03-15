// TmiAnalizer.h : main header file for the TMIANALIZER application
//

#if !defined(AFX_TMIANALIZER_H__8666D83A_E4B8_11D5_91BE_0050BA5D75FE__INCLUDED_)
#define AFX_TMIANALIZER_H__8666D83A_E4B8_11D5_91BE_0050BA5D75FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTmiAnalizerApp:
// See TmiAnalizer.cpp for the implementation of this class
//

class CTmiAnalizerApp : public CWinApp
{
public:
	CTmiAnalizerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiAnalizerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTmiAnalizerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIANALIZER_H__8666D83A_E4B8_11D5_91BE_0050BA5D75FE__INCLUDED_)
