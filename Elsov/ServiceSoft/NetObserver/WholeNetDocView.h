/* WholeNetDocView.h - header file for all MFC doc-view architecture WholeNet functionality.

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	16 May 2007 - context menu
	20 Oct 2007 - OpenGL tree view instead of MFC tree view
	15 Nov 2007 - global IGuiAPI pointer instead of member variables
	15 Jan 2008 - splitting MFC- and NonMFC-visitors to avoid multithread problems with MFC-controls
	19 Jan 2008 - merged view-doc-frame files into this one
*/

#ifndef __WHOLE_NET_DOCVIEW_H_INCLUDED__
#define __WHOLE_NET_DOCVIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"
#include "NetworkDocAPI.h"
#include "NetworkFrameAPI.h"
#include "WholeNetRenderer.h"

// Forward declarations

class CWholeNetDoc : public CNetworkDocAPI
{
protected: // create from serialization only
	CWholeNetDoc();
	DECLARE_DYNCREATE(CWholeNetDoc)

// Public methods
public:

// Public overridabled
public:
	virtual LOGICAL IsMine(CNetObject *pObject) { return LOGICAL_TRUE; } // all objects are mine
	virtual void UpdateWindowCaption() { SetTitle("Whole Network"); }

	//{{AFX_VIRTUAL(CWholeNetDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWholeNetDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWholeNetDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWholeNetView view

class CWholeNetView : public COpenGLView
{
protected:
	CWholeNetView();           // protected constructor used by dynamic creation
	virtual ~CWholeNetView(); 

	DECLARE_DYNCREATE(CWholeNetView)

// Public methods
public:
	CNetObject *GetObjectUnderMouse();
	void RefreshView(LOGICAL bRefreshAnyway);
	void AppearanceGroupChanged() { GetWorldRenderer()->MakeDirty(); }

// Protected overridables
protected:
	virtual CWholeNetRenderer *GetWorldRenderer() { ASSERT(0); return NULL; }

protected:
	//{{AFX_MSG(CWholeNetView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWholeNetTreeView view

class CWholeNetTreeView : public CWholeNetView
{
protected:
	CWholeNetTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWholeNetTreeView)

// Attributes
public:

// Operations
public:
	void NewHostCreated(CHost *pHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	void NetObjectIgnored(CNetObject *pObject);
	void NetObjectHidden(CNetObject *pObject);
	void ShowHiddenObjects();
	void ShowIgnoredObjects();
	
	BOOL OnIdleNonMFCThread();

	void InitialView();
	void SetSelectMode();
	void OnUpdateSelectMode(CCmdUI *pCmdUI);
	void SetDragScrollMode();
	void OnUpdateDragScrollMode(CCmdUI *pCmdUI);
	void SetZoomRectangleMode();
	void OnUpdateZoomRectangleMode(CCmdUI *pCmdUI);

// Overrides
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();
	virtual CWholeNetRenderer *GetWorldRenderer() { return m_pRenderer; }

	//{{AFX_VIRTUAL(CWholeNetTreeView)
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWholeNetTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CWholeNetTreeRenderer *m_pRenderer;

protected:
	//{{AFX_MSG(CWholeNetTreeView)
	afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
	afx_msg void OnViewSortItemsByIpAddress();
	afx_msg void OnUpdateViewSortItemsByIpAddress(CCmdUI *pCmdUI);
	afx_msg void OnViewSortItemsByName();
	afx_msg void OnUpdateViewSortItemsByName(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView view

class CWholeNetPlaneView : public CWholeNetView
{
protected:
	CWholeNetPlaneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWholeNetPlaneView)

// Attributes
public:

// Operations
public:
	void NewHostCreated(CHost *pHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	void NetObjectIgnored(CNetObject *pObject);
	void NetObjectHidden(CNetObject *pObject);
	void ShowHiddenObjects();
	void ShowIgnoredObjects();
	
	BOOL OnIdleNonMFCThread();

	void ZoomIn();
	void ZoomOut();
	void InitialView();
	void SetSelectMode();
	void OnUpdateSelectMode(CCmdUI *pCmdUI);
	void SetDragScrollMode();
	void OnUpdateDragScrollMode(CCmdUI *pCmdUI);
	void SetZoomRectangleMode();
	void OnUpdateZoomRectangleMode(CCmdUI *pCmdUI);

// Overrides
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();
	virtual CWholeNetRenderer *GetWorldRenderer() { return m_pRenderer; }

	//{{AFX_VIRTUAL(CWorldPlaneView)
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWholeNetPlaneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CWholeNetPlaneRenderer *m_pRenderer;

protected:
	//{{AFX_MSG(CWorldPlaneView)
	afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CWholeNetFrame : public CNetworkFrameAPI
{
	DECLARE_DYNCREATE(CWholeNetFrame)
public:
	CWholeNetFrame();

// Public methods
public:
	
// Public overridables
public:
	virtual void NewHostCreated(CHost *pHost);
	virtual void HostChanged(CHost *pHost);
	virtual void NewSubnetCreated(CSubnet *pSubnet);
	virtual void SubnetChanged(CSubnet *pSubnet);
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	virtual void GroupChanged(CGroup *pGroup);
	virtual void NetObjectIgnored(CNetObject *pObject);
	virtual void NetObjectHidden(CNetObject *pObject);
	virtual void ShowHiddenObjects();
	virtual void ShowIgnoredObjects();
	
	virtual BOOL OnIdleNonMFCThread();

// Overrides
	//{{AFX_VIRTUAL(CWholeNetFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWholeNetFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CStatusBar  m_wndStatusBar;
	CToolBar    m_TreeViewToolBar;
	CToolBar    m_PlaneViewToolBar;
	CNetworkSplitter m_SplitterWnd;
	CWholeNetTreeView *m_pTreeView;
	CWholeNetPlaneView *m_pPlaneView;

// Generated message map functions
protected:
	//{{AFX_MSG(CWholeNetFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewDragScroll();
	afx_msg void OnUpdateViewDragScroll(CCmdUI *pCmdUI);
	afx_msg void OnViewInitialView();
	afx_msg void OnViewSelect();
	afx_msg void OnUpdateViewSelect(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomRectangle();
	afx_msg void OnUpdateViewZoomRectangle(CCmdUI *pCmdUI);
	afx_msg void OnViewTreeViewSelect();
	afx_msg void OnUpdateViewTreeViewSelect(CCmdUI *pCmdUI);
	afx_msg void OnViewTreeViewDragScroll();
	afx_msg void OnUpdateViewTreeViewDragScroll(CCmdUI *pCmdUI);
	afx_msg void OnTreeViewInitialView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateObject(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__WHOLE_NET_DOCVIEW_H_INCLUDED__
