/* NetworkAnalyzer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	15 May 2007 - object name in status bar

*/

#ifndef __NETWORK_ANALYSER_FRAME_H_INCLUDED__
#define __NETWORK_ANALYSER_FRAME_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkFrameAPI.h"

// Forward declarations
class CNetworkAnalyzerView;
class CWorldPlaneView;

class CNetworkAnalyzerFrame : public CNetworkFrameAPI
{
	
protected: // create from serialization only
	CNetworkAnalyzerFrame();
	DECLARE_DYNCREATE(CNetworkAnalyzerFrame)

// Attributes
public:

// Operations
public:
	virtual void NewHostCreated(CHost *pHost);
	virtual void HostChanged(CHost *pHost);
	virtual void NewSubnetCreated(CSubnet *pSubnet);
	virtual void SubnetChanged(CSubnet *pSubnet);
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void GuiCreated(IGuiAPI *pGui);
	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	
	virtual BOOL OnIdleProcess();

// Overrides
	//{{AFX_VIRTUAL(CNetworkAnalyzerFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkAnalyzerFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_WorldPlaneViewToolBar;
	CNetworkSplitter m_SplitterWnd;

	CNetworkAnalyzerView *m_pTreeView;
	CWorldPlaneView *m_pWorldPlaneView;

protected:
	//{{AFX_MSG(CNetworkAnalyzerFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewSelect();
	afx_msg void OnUpdateViewSelect(CCmdUI *pCmdUI);
	afx_msg void OnViewInitialview();
	afx_msg void OnViewDragScroll();
	afx_msg void OnUpdateViewDragScroll(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomRectangle();
	afx_msg void OnUpdateViewZoomRectangle(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateObject(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __NETWORK_ANALYSER_FRAME_H_INCLUDED__
