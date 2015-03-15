/* GranSim2DProjectView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	15 Sep 2008 - tree selection change
	23 Sep 2008 - project tree items appearance and updating
	13 Oct 2008 - object selection changed
	16 Oct 2008 - splitted "object common form" and "geometry object common form"; Particle size and shape form
	17 Oct 2008 - particle material form
	13 Nov 2008 - material constants form
	15 Nov 2008 - particle type material form, reflecting object material form, particle types for sources implemented
	05 Jan 2009 - objects deletion
	25 Aug 2009 - Simulation settings form
*/

#ifndef __GRANSIM2D_PROJECT_VIEW_H_INCLUDED__
#define __GRANSIM2D_PROJECT_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Portable.h"
#include "..\GranSim2DDesign.h"

// Forward declarations
class CGranSim2DDoc;
class CGranSim2DFrame;
class CAbstractForm;

class CProjectTreeItemVisitor
{
// Public overridables
public:
	virtual LOGICAL Visit(HTREEITEM hItem) = 0;

// Protected overridables
protected:
};

class CSelectionChangedItemVisitor : public CProjectTreeItemVisitor
{
// Construction/destruction
public:
	CSelectionChangedItemVisitor(CTreeCtrl *pTreeCtrl, CGranSim2DObject *pObject);
	virtual ~CSelectionChangedItemVisitor();

// Public overridables
public:
	virtual LOGICAL Visit(HTREEITEM hItem);

// Protected overridables
protected:
	CTreeCtrl *m_pTreeCtrl;
	CGranSim2DObject *m_pObject;
};


class CObjectDeletedItemVisitor : public CProjectTreeItemVisitor
{
// Construction/destruction
public:
	CObjectDeletedItemVisitor(CTreeCtrl *pTreeCtrl, CGranSim2DObject *pObject);
	virtual ~CObjectDeletedItemVisitor();

// Public overridables
public:
	virtual LOGICAL Visit(HTREEITEM hItem);

// Protected overridables
protected:
	CTreeCtrl *m_pTreeCtrl;
	CGranSim2DObject *m_pObject;
};


class CGranSim2DProjectView : public CTreeView
{
// Constructors/destructors
public:
	CGranSim2DProjectView();
	virtual ~CGranSim2DProjectView();
	DECLARE_DYNCREATE(CGranSim2DProjectView)

// Public methods
public:
	CGranSim2DDoc *GetDocument();
	void SetDesign(CGranSim2DDesign *pDesign) { m_pDesign = pDesign; }
	void SetRenderer(CGranSim2DRenderer *pRenderer) { m_pRenderer = pRenderer; }
	void ClearAll();
	void CreateForms(CSplitterWnd &Splitter);
	void DesignChanged();
	void ReflectingWallCreated(CReflectingWall *pWall);
	void ReflectingRectangleCreated(CReflectingRectangle *pRectangle);
	void MaterialCreated(CMaterial *pMaterial);
	void ParticleTypeCreated(CParticleType *pType);
	void ParticlePointSourceCreated(CParticlePointSource *pSource);
	void ParticleLinearSourceCreated(CParticleLinearSource *pSource);
	void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere);
	void ObjectDeleted(CGranSim2DObject *pObject);
	void UpdateSelectedItem();
	CAbstractForm *GetVisibleForm() { return m_pCurrentlyVisibleForm; }

// Public overridables
	//{{AFX_VIRTUAL(CGranSim2DProjectView)
	public:
	virtual void OnDraw(CDC *pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	HTREEITEM ObjectCreated(CGranSim2DObject *pObject, HTREEITEM hParentItem);
	HTREEITEM NewItem(char *pszText, int nImage, long FormIndex, CGranSim2DObject *pObject, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	void SwitchToDefaultForm(CAbstractForm *pOldForm);
	void Unselect();
	CAbstractForm *GetForm(int index) { return m_pForms[index]; }
	void AddForm(int IndexInViewArray, CAbstractForm *pForm, CWnd *pParent);
	void CreateForm(int IndexInViewArray, CView *pViewToAdd, CWnd *pParent);
	void VisitAllItems(CProjectTreeItemVisitor *pVisitor);
	LOGICAL VisitChildItems(CProjectTreeItemVisitor *pVisitor, HTREEITEM hItem);
	LOGICAL VisitItem(CProjectTreeItemVisitor *pVisitor, HTREEITEM hItem);

// Protected members
protected:
	CGranSim2DFrame *m_pFrame;
	CGranSim2DDesign *m_pDesign;
	CGranSim2DRenderer *m_pRenderer;

	HTREEITEM m_hProjectItem;
	HTREEITEM m_hSimulationItem;
	HTREEITEM m_hMaterialsItem;
	HTREEITEM m_hParticleTypesItem;
	HTREEITEM m_hParticleSourcesItem;
	HTREEITEM m_hReflectingObjectsItem;

	CImageList m_ImageList;
	HTREEITEM m_hPrevSelection;
	CAbstractForm *m_pCurrentlyVisibleForm;
	CAbstractForm *m_pForms[STOP_FORM_INDEX];

protected:
	//{{AFX_MSG(CGranSim2DProjectView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GranSim2DView.cpp
inline CGranSim2DDoc *CGranSim2DProjectView::GetDocument()
   { return (CGranSim2DDoc *)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__GRANSIM2D_PROJECT_VIEW_H_INCLUDED__
