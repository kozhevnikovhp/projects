// QuerySDMS.h : main header file for the QUERYSDMS application
//

#if !defined(AFX_QUERYSDMS_H__BF9CF1FC_56DC_4B13_B302_758094D64D3C__INCLUDED_)
#define AFX_QUERYSDMS_H__BF9CF1FC_56DC_4B13_B302_758094D64D3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Portable.h"

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSApp:
// See QuerySDMS.cpp for the implementation of this class
//

class CQuerySDMSFrame;

class CQuerySDMSApp : public CWinApp
{
public:
	CQuerySDMSApp();
	void SetFrame(CQuerySDMSFrame *pFrame) { m_pFrame = pFrame; }
	IPADDRESS_TYPE GetIpAddress() { return m_IpAddress; }
	BOOL IsWriteEnabled() { return m_bWriteEnabled; }
	char *GetName() { return m_pszName; }

// Overrides
	//{{AFX_VIRTUAL(CQuerySDMSApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CQuerySDMSApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG

	CQuerySDMSFrame *m_pFrame;
	BOOL m_bWriteEnabled;
	char *m_pszName;
	IPADDRESS_TYPE m_IpAddress;

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYSDMS_H__BF9CF1FC_56DC_4B13_B302_758094D64D3C__INCLUDED_)
