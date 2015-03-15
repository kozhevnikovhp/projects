/* NetworkAnalyser.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	01 Sep 2006 - idle thread instead of OnIdle() method
	01 Nov 2006 - IDatabaseAPI interface
	15 Jun 2007 - Host traffic display
*/


#ifndef __NETWORK_ANALIZER_H_INCLUDED__
#define __NETWORK_ANALIZER_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "GuiAPI.h"
#include "Portable.h"
#include "ThreadObject.h"

// Forward declarations
class CDocVisitorAPI;
class CNetwork;
class CNetworkAnalyzerApp;
class COdbcDatabase;

class CIdleThread : public CThreadObject
{
//Attributes
public:
	CIdleThread(CNetworkAnalyzerApp *pApp);
	virtual ~CIdleThread();

// Public methods
public:

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

	// CThreadObject interface
	virtual void OnThreadStarted();
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();

// Protected members
protected:
	CNetworkAnalyzerApp *m_pApp;
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
// CNetworkAnalyzerApp:
// See NetworkAnalyzer.cpp for the implementation of this class
//

class CNetworkAnalyzerApp : public CWinApp, public IGuiAPI
{
	friend class CIdleThread;
public:
	CNetworkAnalyzerApp();

// Overrides
	// Database
	virtual IDatabaseAPI *OpenDatabase();
	
	// Events
	virtual void NewHostCreated(CHost *pHost);
	virtual void HostChanged(CHost *pHost);
	virtual void NewSubnetCreated(CSubnet *pSubnet);
	virtual void SubnetChanged(CSubnet *pSubnet);
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);

	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);

	// NetObject configuration and context menu
	virtual LOGICAL HostContextMenu(int ScreenX, int ScreenY, CHost *pHost);
	virtual LOGICAL SubnetContextMenu(int ScreenX, int ScreenY, CSubnet *pSubnet);

	// Colors
	virtual unsigned char *PlaneView_HostColor() { return m_ucHostColor; }
	virtual unsigned char *PlaneView_SelectedHostColor()  { return m_ucSelectedHostColor; }
	virtual unsigned char *PlaneView_HostTextColor()  { return m_ucHostTextColor; }
	virtual unsigned char *PlaneView_WarningHostColor()  { return m_ucWarningHostColor; }
	virtual unsigned char *PlaneView_AlarmHostColor()  { return m_ucAlarmHostColor; }
	virtual unsigned char *PlaneView_SubnetColor() { return m_ucSubnetColor; }
	virtual unsigned char *PlaneView_SelectedSubnetColor() { return m_ucSelectedSubnetColor; }

	//{{AFX_VIRTUAL(CNetworkAnalyzerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CNetworkAnalyzerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHostDelete();
	afx_msg void OnUpdateHostDelete(CCmdUI *pCmdUI);
	afx_msg void OnHostConfigure();
	afx_msg void OnUpdateHostConfigure(CCmdUI *pCmdUI);
	afx_msg void OnSubnetConfigure();
	afx_msg void OnUpdateSubnetConfigure(CCmdUI* pCmdUI);
	afx_msg void OnSubnetDelete();
	afx_msg void OnUpdateSubnetDelete(CCmdUI* pCmdUI);
	afx_msg void OnHostLookTraffic();
	//}}AFX_MSG

// Protected methods
protected:
	BOOL IdleProcess();
	unsigned long VisitAllDocs(CDocVisitorAPI *pVisitor, BOOL bBlock);
	void UpdateWholeGUI();

	DECLARE_MESSAGE_MAP()
// Protected members
protected:
	COdbcDatabase *m_pDatabase;
	LOGICAL m_bContinueWithoutDB;
	CNetwork *m_pNetwork;
	CIdleThread *m_pIdleThread;
	CPortableCriticalSection m_CriticalSection;
	CHost *m_pHostContextMenu; // host for which context menu is invoked
	CSubnet *m_pSubnetContextMenu; // subnet for which context menu is invoked

	unsigned char m_ucHostColor[4];
	unsigned char m_ucSelectedHostColor[4];
	unsigned char m_ucHostTextColor[4];
	unsigned char m_ucWarningHostColor[4];
	unsigned char m_ucAlarmHostColor[4];
	unsigned char m_ucSubnetColor[4];
	unsigned char m_ucSelectedSubnetColor[4];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__NETWORK_ANALIZER_H_INCLUDED__
