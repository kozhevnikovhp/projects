/* GranSim2DProjectView.cpp

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

#include "stdafx.h"
#include "GranSim2D.h"

#include "GranSim2DDoc.h"
#include "GranSim2DProjectView.h"
#include "GranSim2DRenderer.h"
#include "ControlForms.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// class CSelectionChangedItemVisitor

CSelectionChangedItemVisitor::CSelectionChangedItemVisitor(CTreeCtrl *pTreeCtrl, CGranSim2DObject *pObject)
{
	m_pTreeCtrl = pTreeCtrl;
	m_pObject = pObject;
}

//virtual
CSelectionChangedItemVisitor::~CSelectionChangedItemVisitor()
{
}

//virtual
LOGICAL CSelectionChangedItemVisitor::Visit(HTREEITEM hItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hItem);
	if (!pAttached)
		return LOGICAL_FALSE; // not processed, further processing required
	CGranSim2DObject *pObject = (CGranSim2DObject *)pAttached->m_pObject;
	if (pObject == m_pObject)
	{
		m_pTreeCtrl->SelectItem(hItem);
		if (pAttached->m_ViewIndex == LINE_GEOMETRY_FORM_INDEX ||
			pAttached->m_ViewIndex == POINT_GEOMETRY_FORM_INDEX ||
			pAttached->m_ViewIndex == RECTANGLE_GEOMETRY_FORM_INDEX)
			return LOGICAL_TRUE; // processed, no further processing required
	}
	return LOGICAL_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// class CSelectionChangedItemVisitor

CObjectDeletedItemVisitor::CObjectDeletedItemVisitor(CTreeCtrl *pTreeCtrl, CGranSim2DObject *pObject)
{
	m_pTreeCtrl = pTreeCtrl;
	m_pObject = pObject;
}

//virtual
CObjectDeletedItemVisitor::~CObjectDeletedItemVisitor()
{
}

//virtual
LOGICAL CObjectDeletedItemVisitor::Visit(HTREEITEM hItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hItem);
	if (!pAttached)
		return LOGICAL_FALSE; // not processed, further processing required
	CGranSim2DObject *pObject = (CGranSim2DObject *)pAttached->m_pObject;
	if (pObject == m_pObject)
	{
		m_pTreeCtrl->DeleteItem(hItem);
		return LOGICAL_TRUE; // processed, no further processing required
	}
	return LOGICAL_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DProjectView

IMPLEMENT_DYNCREATE(CGranSim2DProjectView, CTreeView)

BEGIN_MESSAGE_MAP(CGranSim2DProjectView, CTreeView)
	//{{AFX_MSG_MAP(CGranSim2DProjectView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DProjectView construction/destruction

CGranSim2DProjectView::CGranSim2DProjectView()
{
	m_hProjectItem = NULL;
	m_hPrevSelection = NULL;
	m_pCurrentlyVisibleForm = NULL;
	m_pFrame = NULL;
	m_pDesign = NULL;
}

CGranSim2DProjectView::~CGranSim2DProjectView()
{
}

BOOL CGranSim2DProjectView::PreCreateWindow(CREATESTRUCT &cs)
{

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DProjectView drawing

void CGranSim2DProjectView::OnDraw(CDC *pDC)
{
	CGranSim2DDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DProjectView printing

BOOL CGranSim2DProjectView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGranSim2DProjectView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	// TODO: add extra initialization before printing
}

void CGranSim2DProjectView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	// TODO: add cleanup after printing
}

HTREEITEM CGranSim2DProjectView::NewItem(char *pszText, int nImage, long nFormIndex, CGranSim2DObject *pObject, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = GetTreeCtrl().InsertItem(pszText, nImage, nImage, hParent, hInsertAfter);
	CAttachedBlock *pAttached = new CAttachedBlock(nFormIndex, m_pDesign, pObject);
	GetTreeCtrl().SetItemData(hItem, (DWORD)pAttached);
	return hItem;
}

void CGranSim2DProjectView::ClearAll()
{
	m_hPrevSelection = NULL;
	GetTreeCtrl().SelectItem(NULL);
	GetTreeCtrl().DeleteAllItems();
	m_hProjectItem = NewItem("Project Settings", 4, DEFAULT_FORM_INDEX, NULL);
	m_hSimulationItem = NewItem("Simulation", 4, SIM_SETTINGS_FORM_INDEX, NULL, m_hProjectItem);
	m_hMaterialsItem = NewItem("Materials", 4, DEFAULT_FORM_INDEX, NULL, NULL);
	m_hParticleTypesItem = NewItem("Particle Types", 4, DEFAULT_FORM_INDEX, NULL, NULL);
	m_hParticleSourcesItem = NewItem("Particle Sources", 4, DEFAULT_FORM_INDEX, NULL, NULL);
	m_hReflectingObjectsItem = NewItem("Reflecting objects", 4, DEFAULT_FORM_INDEX, NULL, NULL);
}

HTREEITEM CGranSim2DProjectView::ObjectCreated(CGranSim2DObject *pObject, HTREEITEM hParentItem)
{
	GetTreeCtrl().SetRedraw(FALSE);
	SFormDesc *pFormDesc = pObject->GetFormDesc(0);
	HTREEITEM hCommonItem = NewItem(pObject->GetFullName(), 4+pFormDesc->ImageIndex, pFormDesc->FormIndex, pObject, hParentItem);
	for (int iFormNumber = 1; iFormNumber < pObject->GetFormCount(); iFormNumber++)
	{
		pFormDesc = pObject->GetFormDesc(iFormNumber);
		HTREEITEM hItem = NewItem(pFormDesc->pszTextDesc, 4+pFormDesc->ImageIndex, pFormDesc->FormIndex, pObject, hCommonItem);
	}
	GetTreeCtrl().SetRedraw(TRUE);
	return hCommonItem;
}

void CGranSim2DProjectView::DesignChanged()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (!hItem)
		return; // do nothing
	CAttachedBlock *pAttached = (CAttachedBlock *)GetTreeCtrl().GetItemData(hItem);
	int nViewIndex = pAttached->m_ViewIndex;
	CAbstractForm *pCurrentForm = GetForm(nViewIndex);
	pCurrentForm->OnDesignChanged();
}

void CGranSim2DProjectView::ReflectingWallCreated(CReflectingWall *pWall)
{
	ObjectCreated(pWall, m_hReflectingObjectsItem);
}

void CGranSim2DProjectView::ReflectingRectangleCreated(CReflectingRectangle *pRectangle)
{
	ObjectCreated(pRectangle, m_hReflectingObjectsItem);
}

void CGranSim2DProjectView::MaterialCreated(CMaterial *pMaterial)
{
	HTREEITEM hItem = ObjectCreated(pMaterial, m_hMaterialsItem);
	if (m_pDesign->GetCurrentRegime() == STANDBY_REGIME)
		GetTreeCtrl().SelectItem(hItem);
}

void CGranSim2DProjectView::ParticleTypeCreated(CParticleType *pType)
{
	HTREEITEM hItem = ObjectCreated(pType, m_hParticleTypesItem);
	if (m_pDesign->GetCurrentRegime() == STANDBY_REGIME)
		GetTreeCtrl().SelectItem(hItem);
}

void CGranSim2DProjectView::ParticlePointSourceCreated(CParticlePointSource *pSource)
{
	ObjectCreated(pSource, m_hParticleSourcesItem);
}

void CGranSim2DProjectView::ParticleLinearSourceCreated(CParticleLinearSource *pSource)
{
	ObjectCreated(pSource, m_hParticleSourcesItem);
}

void CGranSim2DProjectView::ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere)
{
	if (pWhere == this)
		return;
	CSelectionChangedItemVisitor visitor(&GetTreeCtrl(), pObject);
	VisitAllItems(&visitor);
}

void CGranSim2DProjectView::ObjectDeleted(CGranSim2DObject *pObject)
{
	Unselect();
	CObjectDeletedItemVisitor visitor(&GetTreeCtrl(), pObject);
	VisitAllItems(&visitor);
}

void CGranSim2DProjectView::VisitAllItems(CProjectTreeItemVisitor *pVisitor)
{
	VisitChildItems(pVisitor, TVI_ROOT);
}

LOGICAL CGranSim2DProjectView::VisitChildItems(CProjectTreeItemVisitor *pVisitor, HTREEITEM hItem)
{
	if (VisitItem(pVisitor, hItem))
		return LOGICAL_TRUE; // processed, no further processing required
	HTREEITEM hNextItem, hChildItem = GetTreeCtrl().GetChildItem(hItem);

	while (hChildItem != NULL)
	{
		hNextItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		if (VisitChildItems(pVisitor, hChildItem))
			return LOGICAL_TRUE; // processed, no further processing required
		hChildItem = hNextItem;
	}
	return LOGICAL_FALSE; // not processed, further processing required
}

LOGICAL CGranSim2DProjectView::VisitItem(CProjectTreeItemVisitor *pVisitor, HTREEITEM hItem)
{
	if (hItem == TVI_ROOT)
		return LOGICAL_FALSE;
	return pVisitor->Visit(hItem);
}

void CGranSim2DProjectView::UpdateSelectedItem()
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (!hItem)
		return; // do nothing
	CAttachedBlock *pAttached = (CAttachedBlock *)GetTreeCtrl().GetItemData(hItem);
	if (!pAttached)
		return; // do nothing
	CGranSim2DObject *pObject = (CGranSim2DObject *)pAttached->m_pObject;
	if (!pObject)
		return; // do nothing
	char *pszText = pObject->GetFullName();
	GetTreeCtrl().SetItemText(hItem, pszText);
}

void CGranSim2DProjectView::SwitchToDefaultForm(CAbstractForm *pOldForm)
{
	UINT OldIDD = pOldForm->GetDlgCtrlID();
	m_pCurrentlyVisibleForm = GetForm(DEFAULT_FORM_INDEX);
	UINT NewIDD = m_pCurrentlyVisibleForm->GetDlgCtrlID();
	pOldForm->ShowWindow(SW_HIDE);
	m_pCurrentlyVisibleForm->ShowWindow(SW_SHOW);
	pOldForm->SetDlgCtrlID(NewIDD);
	m_pCurrentlyVisibleForm->SetDlgCtrlID(OldIDD);
	if (m_hPrevSelection)
		pOldForm->Leaving();
	m_pFrame->Refresh();
}

void CGranSim2DProjectView::Unselect()
{
	GetTreeCtrl().SelectItem(NULL);
	m_hPrevSelection = NULL;
}

void CGranSim2DProjectView::CreateForms(CSplitterWnd &Splitter)
{
	m_pForms[DEFAULT_FORM_INDEX] = (CAbstractForm *)Splitter.GetPane(1, 0);
	m_pForms[DEFAULT_FORM_INDEX]->SetProjectView(this);
	m_pForms[DEFAULT_FORM_INDEX]->SetDesign(m_pDesign);
	m_pForms[DEFAULT_FORM_INDEX]->SetRenderer(m_pRenderer);
	AddForm(SIM_SETTINGS_FORM_INDEX, new CSimSettingsForm(), &Splitter);
	AddForm(OBJECT_COMMON_FORM_INDEX, new CObjectCommonForm(), &Splitter);
	AddForm(MATERIAL_CONSTANTS_INDEX, new CMaterialConstantsForm(), &Splitter);
	AddForm(PARTICLE_TYPE_SIZE_INDEX, new CParticleSizeForm(), &Splitter);
	AddForm(PARTICLE_TYPE_SHAPE_INDEX, new CParticleShapeForm(), &Splitter);
	AddForm(PARTICLE_TYPE_MATERIAL_INDEX, new CParticleMaterialForm(), &Splitter);
	AddForm(GEOMETRY_OBJECT_COMMON_FORM_INDEX, new CGeometryObjectCommonForm(), &Splitter);
	AddForm(LINE_GEOMETRY_FORM_INDEX, new CLineGeometryForm(), &Splitter);
	AddForm(RECTANGLE_GEOMETRY_FORM_INDEX, new CRectangleGeometryForm(), &Splitter);
	AddForm(POINT_GEOMETRY_FORM_INDEX, new CPointGeometryForm(), &Splitter);
	AddForm(PARTICLE_SOURCE_TYPE_FORM_INDEX, new CParticleSourceTypeForm(), &Splitter);
	AddForm(PARTICLE_SOURCE_INTENSITY_FORM_INDEX, new CParticleSourceIntensityForm(), &Splitter);
	AddForm(REFLECTING_OBJECT_MATERIAL_FORM_INDEX, new CReflectingObjectMaterialForm(), &Splitter);
}

void CGranSim2DProjectView::AddForm(int IndexInViewArray, CAbstractForm *pForm, CWnd *pParent)
{
	CreateForm(IndexInViewArray, pForm, pParent);
	pForm->SetProjectView(this);
	pForm->SetDesign(m_pDesign);
	pForm->SetRenderer(m_pRenderer);
	m_pForms[IndexInViewArray] = pForm;
}

void CGranSim2DProjectView::CreateForm(int IndexInViewArray, CView *pViewToAdd, CWnd *pParent)
{
	pViewToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, pParent,
		AFX_IDW_PANE_FIRST+IndexInViewArray+16, NULL);
	GetDocument()->AddView(pViewToAdd);
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DProjectView diagnostics

#ifdef _DEBUG
void CGranSim2DProjectView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CGranSim2DProjectView::Dump(CDumpContext &dc) const
{
	CTreeView::Dump(dc);
}

CGranSim2DDoc *CGranSim2DProjectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGranSim2DDoc)));
	return (CGranSim2DDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DProjectView message handlers

int CGranSim2DProjectView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ImageList.Create(IDB_BITMAP_TREE, 16, 5, RGB(192,192,192));
	GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);
	long lStyleOld;
	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
	
	return 0;
}

void CGranSim2DProjectView::OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;

	CAttachedBlock *pAttached = (CAttachedBlock *)pNMTreeView->itemOld.lParam;
	delete pAttached;
	pNMTreeView->itemOld.lParam = NULL;
	
	*pResult = 0;
}

void CGranSim2DProjectView::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult) 
{
	CWaitCursor cursor;

	CAbstractForm *pOldForm = m_pCurrentlyVisibleForm;
	if (m_pFrame && pOldForm)
	{
		m_pCurrentlyVisibleForm = NULL;
		HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
		if (!hItem)
		{
			SwitchToDefaultForm(pOldForm);
			return;
		}
		CAttachedBlock *pAttached = (CAttachedBlock *)GetTreeCtrl().GetItemData(hItem);
		int nNewViewIndex = pAttached->m_ViewIndex;

		CAbstractForm *pNewForm = GetForm(nNewViewIndex);

		UINT OldIDD = pOldForm->GetDlgCtrlID();
		UINT NewIDD = 0;
		if (pNewForm)
			NewIDD = pNewForm->GetDlgCtrlID();
		if (OldIDD == NewIDD && hItem == m_hPrevSelection)
			return;
		if (m_hPrevSelection != NULL)
		{
			LOGICAL bGoForward = pOldForm->CanLeave();
			if (bGoForward)
			{
				if (pOldForm->IsModified())
					bGoForward = pOldForm->Leaving();
			}
			
			if (bGoForward)
			{
				m_pRenderer->Select(NULL);
			}
			else
			{
				GetTreeCtrl().SelectItem(m_hPrevSelection);
				m_pCurrentlyVisibleForm = pOldForm;
				return;
			}
		}

		if (pNewForm)
		{
			pNewForm->SetAttachedBlock(pAttached);
			if (pNewForm->Entering())
			{
				m_pRenderer->Select(pAttached->m_pObject);
				g_pGui->ObjectSelectionChanged(pAttached->m_pObject, this);
				pNewForm->ClearModifiedFlag();
				m_hPrevSelection = hItem;
				m_pCurrentlyVisibleForm = pNewForm;
				if (OldIDD != NewIDD)
				{
					pOldForm->ShowWindow(SW_HIDE);
					pNewForm->ShowWindow(SW_SHOW);
					pOldForm->SetDlgCtrlID(NewIDD);
					pNewForm->SetDlgCtrlID(OldIDD);
					m_pFrame->Refresh();
				}
			}
			else
			{
				MessageBeep(0);
				GetTreeCtrl().SelectItem(m_hPrevSelection);
			}
		}
		else
			SwitchToDefaultForm(pOldForm);

	}
	
	*pResult = 0;
}

void CGranSim2DProjectView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	m_pFrame = (CGranSim2DFrame *)GetParentFrame();
	m_pCurrentlyVisibleForm = GetForm(DEFAULT_FORM_INDEX);
}
