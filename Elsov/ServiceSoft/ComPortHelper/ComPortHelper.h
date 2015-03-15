// ComPortHelper.h : main header file for the COMPORTHELPER application
//

#if !defined(AFX_COMPORTHELPER_H__696F95A9_CDE2_4045_9773_D9638AC194C5__INCLUDED_)
#define AFX_COMPORTHELPER_H__696F95A9_CDE2_4045_9773_D9638AC194C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperApp:
// See ComPortHelper.cpp for the implementation of this class
//

// Forward declarations
class CComPortHelperView;

class CComPortHelperApp : public CWinApp
{
	friend class CComPortHelperView;
public:
	CComPortHelperApp();

// Overrides
	//{{AFX_VIRTUAL(CComPortHelperApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

protected:
	CComPortHelperView *m_pView;

// Implementation
	//{{AFX_MSG(CComPortHelperApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPORTHELPER_H__696F95A9_CDE2_4045_9773_D9638AC194C5__INCLUDED_)
