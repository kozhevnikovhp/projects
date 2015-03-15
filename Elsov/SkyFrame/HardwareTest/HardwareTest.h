// HardwareTest.h : main header file for the HARDWARETEST application
//

#ifndef __HARDWARETEST_H_INCLUDED__
#define __HARDWARETEST_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

////////////////////////////////////////////////////////////////////////////
// CHardwareTestApp:
// See HardwareTest.cpp for the implementation of this class
//

class CHardwareTestApp : public CWinApp
{
public:
	CHardwareTestApp();

// Overrides
	//{{AFX_VIRTUAL(CHardwareTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Public members
public:
// Protected members
protected:

	// Implementation
	//{{AFX_MSG(CHardwareTestApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __HARDWARETEST_H_INCLUDED__
