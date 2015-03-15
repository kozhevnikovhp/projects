// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1C2CEEFA_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
#define AFX_STDAFX_H__1C2CEEFA_300A_11D4_BBDF_E46DF892F25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <mmsystem.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "HardwareTest.h"
#include "AbstractForm.h"

extern const char *pszFtpUsername;
extern const char *pszFtpPassword;
extern const char *pszError;
extern const char *pszWarning;
extern const char *pszInfo;

#define pAPP ((CHardwareTestApp *)AfxGetApp())


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1C2CEEFA_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
