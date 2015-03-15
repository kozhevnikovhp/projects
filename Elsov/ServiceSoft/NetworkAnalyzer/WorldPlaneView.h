/* WorldPlaneView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	16 May 2007 - context menu
*/

#if !defined(AFX_WORLDPLANEVIEW_H__506BBDE1_0F6F_4022_A213_9C56430D5C26__INCLUDED_)
#define AFX_WORLDPLANEVIEW_H__506BBDE1_0F6F_4022_A213_9C56430D5C26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"
#include "WorldPlaneRenderer.h"

// Forward declarations

/////////////////////////////////////////////////////////////////////////////
// CWorldPlaneView view

class CWorldPlaneView : public COpenGLView
{
protected:
	CWorldPlaneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWorldPlaneView)

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
	void GuiCreated(IGuiAPI *pGui);
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	
	BOOL OnIdleProcess();

	void ZoomIn();
	void ZoomOut();
	void InitialView();
	void SetSelectMode();
	void OnUpdateSelectMode(CCmdUI *pCmdUI);
	void SetDragScrollMode();
	void OnUpdateDragScrollMode(CCmdUI *pCmdUI);
	void SetZoomRectangleMode();
	void OnUpdateZoomRectangleMode(CCmdUI *pCmdUI);
	CNetObject *GetObjectUnderMouse();

// Overrides
	virtual CRenderer *GetRenderer() { return m_pWorldPlaneRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

	//{{AFX_VIRTUAL(CWorldPlaneView)
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWorldPlaneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CWorldPlaneRenderer *m_pWorldPlaneRenderer;

protected:
	//{{AFX_MSG(CWorldPlaneView)
	afx_msg void OnTestAntialiasing();
	afx_msg void OnUpdateTestAntialiasing(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDPLANEVIEW_H__506BBDE1_0F6F_4022_A213_9C56430D5C26__INCLUDED_)
