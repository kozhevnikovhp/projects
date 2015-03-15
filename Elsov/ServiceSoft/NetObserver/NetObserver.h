/* NetObserver.h : main header file for the NETOBSERVER application
	
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	01 Sep 2006 - idle thread instead of OnIdle() method
	01 Nov 2006 - IDatabaseAPI interface
	15 Jun 2007 - Host traffic display
	14 Nov 2007 - CNetworkDocTemplate, CHostDocTemplate, CSubnetDocTemplate
	15 Nov 2007 - global IGuiAPI pointer instead of member variables
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables
	10 Jan 2008 - host group created and changed functionality
	15 Jan 2008 - splitting MFC- and NonMFC-visitors to avoid multithread problems with MFC-controls 
	20 Mar 2008 - moving host between groups event handling
*/


#ifndef __NET_OBSERVER_H_INCLUDED__
#define __NET_OBSERVER_H_INCLUDED__

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
#include "OdbcDatabase.h"

// Forward declarations
class CDocVisitorAPI;
class CNetwork;
class CNetObserverApp;
class CNetworkDocAPI;
class CNetworkDocTemplate;
class CHostDocTemplate;
class CHostDocTemplate;
class CSubnetDocTemplate;
class CGroupEditorDocTemplate;


class CIdleNonMFCThread : public CThreadObject
{
//Attributes
public:
	CIdleNonMFCThread(CNetObserverApp *pApp);
	virtual ~CIdleNonMFCThread();

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
	CNetObserverApp *m_pApp;
};

class CNetObserverApp : public CWinApp, public IGuiAPI
{
	friend class CIdleNonMFCThread;
	friend class CNetObserverFrame;
public:
	CNetObserverApp();

// Public overridables
public:
	// Database
	virtual IDatabaseAPI *OpenDatabase();
	
	// Events
	virtual void NewHostCreated(CHost *pHost);
	virtual void HostChanged(CHost *pHost);
	virtual void NewSubnetCreated(CSubnet *pSubnet);
	virtual void SubnetChanged(CSubnet *pSubnet);
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostIgnored(CHost *pHost);
	virtual void HostHidden(CHost *pHost);
	virtual void IgnoredObjectsVisibilityChanged();
	virtual void HiddenObjectsVisibilityChanged();
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);

	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);

	// NetObject configuration and context menu
	virtual LOGICAL HostContextMenu(int ScreenX, int ScreenY, CHost *pHost);
	virtual LOGICAL SubnetContextMenu(int ScreenX, int ScreenY, CSubnet *pSubnet);

	// Parameters
	virtual time_t GetTrafficStoreInterval() { return 300; }
	virtual unsigned char GetMaxDistanceInHops()  { return 5; }
	virtual LOGICAL AreHiddenObjectsVisible() { return m_bHiddenObjectsVisible; }
	virtual LOGICAL AreIgnoredObjectsVisible() { return m_bIgnoredObjectsVisible; }

	// Operations
	
// Overrides
	//{{AFX_VIRTUAL(CNetObserverApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CNetObserverApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHostConfigure();
	afx_msg void OnHostLookTraffic();
	afx_msg void OnSubnetConfigure();
	afx_msg void OnHostDelete();
	afx_msg void OnUpdateHostDelete(CCmdUI *pCmdUI);
	afx_msg void OnHostHide();
	afx_msg void OnUpdateHostHide(CCmdUI *pCmdUI);
	afx_msg void OnViewShowIgnoredObjects();
	afx_msg void OnUpdateViewShowIgnoredObjects(CCmdUI *pCmdUI);
	afx_msg void OnViewShowHiddenObjects();
	afx_msg void OnUpdateViewShowHiddenObjects(CCmdUI *pCmdUI);
	afx_msg void OnToolsGroupEditor();
	//}}AFX_MSG

// Protected methods
protected:
	BOOL OnIdleNonMFCThread();
	unsigned long VisitAllDocs(CDocVisitorAPI *pVisitor);
	void UpdateWholeGUI();
	void CreateAllThreads();
	void KillAllThreads();
	DECLARE_MESSAGE_MAP()

// Protected members
protected:
	CIdleNonMFCThread *m_pIdleThread;
	CHost *m_pHostContextMenu; // host for which context menu is invoked
	CSubnet *m_pSubnetContextMenu; // subnet for which context menu is invoked
	CNetworkDocTemplate *m_pWholeNetDocTemplate;
	CHostDocTemplate *m_pDisplayHostTrafficDocTemplate;
	CHostDocTemplate *m_pHostConfigDocTemplate;
	CSubnetDocTemplate *m_pSubnetConfigDocTemplate;
	CGroupEditorDocTemplate *m_pGroupEditorDocTemplate;

	LOGICAL m_bHiddenObjectsVisible, m_bIgnoredObjectsVisible;
};

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

#endif // __NET_OBSERVER_H_INCLUDED__
