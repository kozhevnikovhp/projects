/* NetworkFrameAPI.h : interface of the CNetworkFrameAPI class
  Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	07 Sep 2006 - initial creation
	20 Mar 2008 - moving host between groups event handling
*/

#ifndef __NETWORK_FRAME_API_H_INCLUDED__
#define __NETWORK_FRAME_API_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Network.h"

#define WM_HOST_CREATED (WM_USER + 21)
#define WM_HOST_CHANGED (WM_USER + 22)
#define WM_SUBNET_CREATED (WM_USER + 23)
#define WM_SUBNET_CHANGED (WM_USER + 24)
#define WM_HOST_ADDED_TO_SUBNET (WM_USER + 25)
#define WM_HOST_REMOVED_FROM_SUBNET (WM_USER + 26)

// Forward declaraions
class CNetworkDocAPI;
class CGroup;

class CNetworkSplitter : public CSplitterWnd
{
	DECLARE_DYNAMIC(CNetworkSplitter)

// Construction
public:
	CNetworkSplitter();

// Attributes
public:
// Operations
public:

// Overridables
protected:

public:

// Implementation
public:
	virtual ~CNetworkSplitter();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	//{{AFX_MSG(CNetworkSplitter)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNetworkFrameAPI : public CMDIChildWnd
{
	friend class CNetworkDocTemplate;	
protected: // create from serialization only
	CNetworkFrameAPI();
	DECLARE_DYNCREATE(CNetworkFrameAPI)

// Public methods
public:

// Public overridables
public:
	virtual void NewHostCreated(CHost *pHost) {}
	virtual void HostChanged(CHost *pHost) {}
	virtual void NewSubnetCreated(CSubnet *pSubnet) {}
	virtual void SubnetChanged(CSubnet *pSubnet) {}
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) {}
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) {}
	virtual void NewGroupCreated(CGroup *pGroup) {}
	virtual void GroupChanged(CGroup *pGroup) {}
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup) {}
	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged) {}
	virtual void NetObjectIgnored(CNetObject *pObject) {}
	virtual void NetObjectHidden(CNetObject *pObject) {}
	virtual void ShowHiddenObjects() {}
	virtual void ShowIgnoredObjects() {}
	
	virtual BOOL OnIdleMFC() { return FALSE; }
	virtual BOOL OnIdleNonMFCThread() { return FALSE; }

	virtual void Refresh() { RecalcLayout(); }

// Protected methods
protected:
	void SetDocument(CNetworkDocAPI *pDoc);

// Overrides
	//{{AFX_VIRTUAL(CNetworkFrameAPI)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkFrameAPI();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CNetworkDocAPI *m_pDoc;

protected:
	//{{AFX_MSG(CNetworkFrameAPI)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnHostCreated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHostChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSubnetCreated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSubnetChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHostAddedToSubnet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHostRemovedFromSubnet(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __NETWORK_FRAME_API_H_INCLUDED__
