// IpHelperTest.h : main header file for the IPHELPERTEST application
//

#if !defined(AFX_IPHELPERTEST_H__426A839E_F406_4BCA_BE3C_A7AA9B1FD122__INCLUDED_)
#define AFX_IPHELPERTEST_H__426A839E_F406_4BCA_BE3C_A7AA9B1FD122__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIpHelperTestApp:
// See IpHelperTest.cpp for the implementation of this class
//

class CIpHelperTestApp : public CWinApp
{
public:
	CIpHelperTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIpHelperTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIpHelperTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPHELPERTEST_H__426A839E_F406_4BCA_BE3C_A7AA9B1FD122__INCLUDED_)
