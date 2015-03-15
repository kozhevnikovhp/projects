// ArmHub.h : main header file for the ARMHUB application
//

#if !defined(AFX_ARMHUB_H__850AA24F_2201_11D5_9670_008048B59EAA__INCLUDED_)
#define AFX_ARMHUB_H__850AA24F_2201_11D5_9670_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CArmHubApp:
// See ArmHub.cpp for the implementation of this class
//

class CArmHubApp : public CWinApp
{
public:
	CArmHubApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArmHubApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CArmHubApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARMHUB_H__850AA24F_2201_11D5_9670_008048B59EAA__INCLUDED_)
