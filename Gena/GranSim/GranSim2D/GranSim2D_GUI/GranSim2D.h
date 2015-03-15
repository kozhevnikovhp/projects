/* GranSim2D.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	13 Oct 2008 - object selection changed
	16 Oct 2008 - particle types introduced
	13 Nov 2008 - materials
	10 Dec 2008 - "cannot simulate" diagnostics
	05 Jan 2009 - objects deletion
*/

#ifndef __GRANSIM2D_H_INCLUDED__
#define __GRANSIM2D_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "..\GuiAPI.H"

// Forward declarations
class CGranSim2DFrame;

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DApp:
// See GranSim2D.cpp for the implementation of this class
//

class CGranSim2DApp : public CWinApp, public IGuiAPI
{
// Constructors/destructors
public:
	CGranSim2DApp();

// Public overridables
public:
	virtual void DesignCleaned();
	virtual void DesignChanged();
	virtual void ReflectingWallCreated(CReflectingWall *pWall, void *pWhere);
	virtual void ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere);
	virtual void MaterialCreated(CMaterial *pMaterial, void *pWhere);
	virtual void ParticleTypeCreated(CParticleType *pType, void *pWhere);
	virtual void ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere);
	virtual void ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere);
	virtual void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere);
	virtual void ObjectDeleted(CGranSim2DObject *pObject, void *pWhere);
	virtual void CannotSimulate(const char *pszWhy);

	//{{AFX_VIRTUAL(CGranSim2DApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGranSim2DApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
protected:
	CGranSim2DFrame *m_pAppFrameWnd;
	CSingleDocTemplate *m_pDocTemplate;
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__GRANSIM2D_H_INCLUDED__
