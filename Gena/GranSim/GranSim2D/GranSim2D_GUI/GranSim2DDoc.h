/* GranSim2DDoc.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	15 Sep 2008 - line form
	24 Sep 2008 - editor tool bar
	10 Oct 2008 - reflecting wall creation functionality
	11 Oct 2008 - linear/point particle source creation functionality
	13 Oct 2008 - status bar text from design; object selection
	16 Oct 2008 - splitted "object common form" and "geometry object common form"; Particle size and shape form
	13 Nov 2008 - material forms
	10 Dec 2008 - simulation stuff
	04 Jan 2009 - reflecting rectangle
	05 Jan 2009 - objects deletion
*/

#ifndef __GRANSIM2DDOC_H_INCLUDED__
#define __GRANSIM2DDOC_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GranSim2DDesign.h"

//Forward declarations
class CGranSim2DProjectView;
class CGranSim2DView;
class CAbstractForm;
class CGranSim2DRenderer;

class CGranSim2DDoc : public CDocument
{
protected: // create from serialization only
	CGranSim2DDoc();
	DECLARE_DYNCREATE(CGranSim2DDoc)

// Public methods
public:

// Public overrides
public:
	//{{AFX_VIRTUAL(CGranSim2DDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

	virtual ~CGranSim2DDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Public methods
public:
	void SetRenderer(CGranSim2DRenderer *pRenderer) { m_pRenderer = pRenderer; }
	CGranSim2DDesign *GetDesign() { return &m_Design; }

// Protected methods
protected:

// Protected members
protected:
	CGranSim2DDesign m_Design;
	CGranSim2DRenderer *m_pRenderer;

protected:
	//{{AFX_MSG(CGranSim2DDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CGranSim2DFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CGranSim2DFrame();
	DECLARE_DYNCREATE(CGranSim2DFrame)

// Public methods
public:
	BOOL Idle();
	void DesignCleaned();
	void ReflectingWallCreated(CReflectingWall *pWall, void *pWhere);
	void ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere);
	void DesignChanged();
	void MaterialCreated(CMaterial *pMaterial, void *pWhere);
	void ParticleTypeCreated(CParticleType *pType, void *pWhere);
	void ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere);
	void ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere);
	void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere);
	void ObjectDeleted(CGranSim2DObject *pObject, void *pWhere);
	void Refresh() { m_SplitterVert.RecalcLayout(), m_SplitterHor.RecalcLayout(); }

// Public overrides
public:
	//{{AFX_VIRTUAL(CGranSim2DFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

	virtual ~CGranSim2DFrame();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_ToolBar;
	CToolBar    m_Design2DViewToolBar;
	CToolBar m_EditorToolBar;
	CPoint m_EditorToolBarPosition;
	CSplitterWnd m_SplitterVert, m_SplitterHor;
	CGranSim2DProjectView *m_pProjectView;
	CGranSim2DView *m_p2DView;
	CGranSim2DDesign *m_pDesign;
	CGranSim2DRenderer *m_pRenderer;
	DWORD m_TimerID;

protected:
	afx_msg void OnUpdateObject(CCmdUI *pCmdUI);
	//{{AFX_MSG(CGranSim2DFrame)
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
	afx_msg void OnViewEditMode();
	afx_msg void OnUpdateViewEditMode(CCmdUI *pCmdUI);
	afx_msg void OnEditCreateReflectingWall();
	afx_msg void OnUpdateEditCreateReflectingWall(CCmdUI *pCmdUI);
	afx_msg void OnEditCreateParticleSourceLinear();
	afx_msg void OnUpdateEditCreateParticleSourceLinear(CCmdUI *pCmdUI);
	afx_msg void OnEditCreateParticleSourcePoint();
	afx_msg void OnUpdateEditCreateParticleSourcePoint(CCmdUI *pCmdUI);
	afx_msg void OnEditCreateNewParticleType();
	afx_msg void OnUpdateEditCreateNewParticleType(CCmdUI *pCmdUI);
	afx_msg void OnEditCreateNewMaterial();
	afx_msg void OnUpdateEditCreateNewMaterial(CCmdUI *pCmdUI);
	afx_msg void OnSimulationStart();
	afx_msg void OnUpdateSimulationStart(CCmdUI *pCmdUI);
	afx_msg void OnSimulationStop();
	afx_msg void OnSimulationPause();
	afx_msg void OnDestroy();
	afx_msg void OnEditCreateReflectingRectangle();
	afx_msg void OnUpdateEditCreateReflectingRectangle(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __GRANSIM2DDOC_H_INCLUDED__
