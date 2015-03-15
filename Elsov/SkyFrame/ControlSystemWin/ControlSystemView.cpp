// ControlSystemView.cpp : implementation of the CControlSystemView class
//

#include "stdafx.h"
#include "ControlSystemWin.h"

#include "ControlSystemAPI.h"
#include "ControlSystemDoc.h"
#include "ControlSystemFrame.h"
#include "ControlSystemView.h"
#include "AbstractForm.h"
#include "Attached.h"
#include "TargetInstance.h"
#include "ControlTargetAPI.h"
#include "ControlObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlSystemView

IMPLEMENT_DYNCREATE(CControlSystemView, CDragTreeView)

BEGIN_MESSAGE_MAP(CControlSystemView, CDragTreeView)
	//{{AFX_MSG_MAP(CControlSystemView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CDragTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CDragTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CDragTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlSystemView construction/destruction

CControlSystemView::CControlSystemView()
{
	m_PrevSelection = NULL;
	m_pTreeCtrl = NULL;
	m_pCurrentlyVisibleForm = NULL;
}

CControlSystemView::~CControlSystemView()
{
}

BOOL CControlSystemView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CDragTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CControlSystemView drawing

void CControlSystemView::OnDraw(CDC *pDC)
{
	CControlSystemDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CControlSystemView::OnInitialUpdate()
{
	CDragTreeView::OnInitialUpdate();
 
	CControlSystemFrame *pFrame = (CControlSystemFrame *)GetParentFrame();
	m_pAbstractPane = (CAbstractForm *)pFrame->GetTopAbstractPane();
//	SetFont(&font);
	m_ImageList.Create(IDB_BITMAP_TREE, 16, 5, RGB(192,192,192));
	m_pTreeCtrl->SetImageList(&m_ImageList, TVSIL_NORMAL);

	long lStyleOld;
	lStyleOld = GetWindowLong(m_pTreeCtrl->m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
}

/////////////////////////////////////////////////////////////////////////////
// CControlSystemView printing

BOOL CControlSystemView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CControlSystemView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	// TODO: add extra initialization before printing
}

void CControlSystemView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CControlSystemView diagnostics

#ifdef _DEBUG
void CControlSystemView::AssertValid() const
{
	CDragTreeView::AssertValid();
}

void CControlSystemView::Dump(CDumpContext &dc) const
{
	CDragTreeView::Dump(dc);
}

CControlSystemDoc* CControlSystemView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CControlSystemDoc)));
	return (CControlSystemDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlSystemView message handlers

void CControlSystemView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CWaitCursor cursor;

	CControlSystemFrame *pFrame = (CControlSystemFrame *)GetParentFrame();
	CAbstractForm *pOld = (CAbstractForm *)(pFrame->GetTopAbstractPane());
	m_pCurrentlyVisibleForm = NULL;
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAttachedBlock *pOldPtr=NULL, *pNewPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	if (m_PrevSelection)
		pOldPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(m_PrevSelection);

	if (!pNewPtr)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAbstractForm *pNew = GetDocument()->m_pForms[pNewPtr->m_ViewIndex];

	UINT OldIDD = pOld->GetDlgCtrlID();
	UINT NewIDD = pNew->GetDlgCtrlID();
	if (OldIDD == NewIDD && Item == m_PrevSelection) return;
	if (pOldPtr)
	{
		if (!pOld->BeforeHiding())
		{
			m_pTreeCtrl->SelectItem(m_PrevSelection);
			return;
		}
	}

	pNew->m_pAttached = pNewPtr;
	if (pNew->SetupControls())
	{
		m_PrevSelection = Item;
		m_pCurrentlyVisibleForm = pNew;
		if (OldIDD != NewIDD)
		{
			pOld->ShowWindow(SW_HIDE);
			pNew->ShowWindow(SW_SHOW);
			pOld->SetDlgCtrlID(NewIDD);
			pNew->SetDlgCtrlID(OldIDD);
			pFrame->Refresh();
		}
	}
	else
	{
		MessageBeep(0);
		m_pTreeCtrl->SelectItem(m_PrevSelection);
	}
	*pResult = 0;
}

void CControlSystemView::TurnToAbstractPane(CAbstractForm *pOld)
{
	CControlSystemFrame *pFrame = (CControlSystemFrame *)GetParentFrame();

	UINT OldIDD = pOld->GetDlgCtrlID();
	UINT NewIDD = m_pAbstractPane->GetDlgCtrlID();
	pOld->ShowWindow(SW_HIDE);
	m_pAbstractPane->ShowWindow(SW_SHOW);
	pOld->SetDlgCtrlID(NewIDD);
	m_pAbstractPane->SetDlgCtrlID(OldIDD);
	if (m_PrevSelection)
		pOld->BeforeHiding();
	pFrame->Refresh();
}

void CControlSystemView::AddNewTarget(CTargetInstance *pTargetInstance)
{
	if (!pTargetInstance)
		return;
	IControlTargetAPI *pTarget = pTargetInstance->GetTarget();
	if (!pTarget)
		return;
	CString ItemName;
	pTargetInstance->GetFullName(ItemName);
	int PictureID = pTarget->GetTargetPictureID();
	HTREEITEM Item = m_pTreeCtrl->InsertItem(ItemName, PictureID, PictureID);
	CAttachedBlock *pAttached = new CAttachedBlock(TARGET_FORM_INDEX, pTargetInstance);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	LoadFromDB(Item, pTargetInstance);
}

void CControlSystemView::AddNewObject()
{
	HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
	if (!hItem)
		return;

	if (!GetCurrentForm()->CanAddChild())
		return;

	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hItem);
	if (!pAttached)
		return;
	CTargetInstance *pTargetInstance = pAttached->GetTargetInstance();

	CControlObject *pNewObject = GetCurrentForm()->CreateNewObject(pTargetInstance);
	ASSERT(pNewObject);
	if (!pNewObject)
		return;

	HTREEITEM hNewItem = InsertObject(hItem, pNewObject, pTargetInstance);
	m_pTreeCtrl->EnsureVisible(hNewItem);
}

int CControlSystemView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDragTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TimerID = SetTimer(1,1000, NULL);	
	m_pTreeCtrl = &GetTreeCtrl();
	
	return 0;
}

void CControlSystemView::OnDestroy() 
{
	KillTimer(m_TimerID);
	CDragTreeView::OnDestroy();	
}

void CControlSystemView::OnTimer(UINT nIDEvent) 
{
	if (m_pCurrentlyVisibleForm)
		m_pCurrentlyVisibleForm->Timer();
	CDragTreeView::OnTimer(nIDEvent);
}

void CControlSystemView::OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult) 
{
	if (!IsDraggingNow())
	{
		NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
		CAttachedBlock *pAttached = (CAttachedBlock *)pNMTreeView->itemOld.lParam;
		delete pAttached;
	}
	*pResult = 0;
}

CTargetInstance *CControlSystemView::DeleteSelectedTarget()
{
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return NULL;

	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	CAbstractForm *pCurrentForm = GetDocument()->m_pForms[pAttached->m_ViewIndex];
	if (!pCurrentForm->CanBeDeleted())
		return NULL;
	
	m_pTreeCtrl->SelectItem(NULL);
	m_PrevSelection = NULL;
	m_pCurrentlyVisibleForm = NULL;

	CTargetInstance *pInstance = pCurrentForm->DeleteSelectedObject(pAttached);
	m_pTreeCtrl->DeleteItem(Item);
	OnSelChanged(NULL, NULL);

	return pInstance;
}

BOOL CControlSystemView::CanBeDeletedSelectedItem()
{
	CAbstractForm *pCurrentForm = GetCurrentForm();
	if (!pCurrentForm)
		return FALSE;
	return pCurrentForm->CanBeDeleted();
}

BOOL CControlSystemView::CanAddChildToSelectedItem()
{
	CAbstractForm *pCurrentForm = GetCurrentForm();
	if (!pCurrentForm)
		return FALSE;
	return pCurrentForm->CanAddChild();
}

//virtual
BOOL CControlSystemView::CanDragItem(HTREEITEM hDragItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hDragItem);
	if (!pAttached)
		return FALSE;
	return pAttached->CanDragItem();
}

//virtual
BOOL CControlSystemView::CanDropItem(HTREEITEM hDragItem, HTREEITEM hDropItem)
{
	CAttachedBlock *pTarget = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hDropItem);
	if (!pTarget)
		return FALSE;
	CAttachedBlock *pDragged = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hDragItem);
	if (!pDragged)
		return FALSE;
	return pTarget->CanDropItem(pDragged);
}

//virtual
void CControlSystemView::DraggingStarted(HTREEITEM hDraggedItem)
{
	m_PrevSelection = NULL;
}

//virtual
void CControlSystemView::DraggingCompleted(HTREEITEM hDroppedItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hDroppedItem);
	ASSERT(pAttached);
	if (!pAttached)
		return;
	HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hDroppedItem);
	ASSERT(hParentItem);
	if (!hParentItem)
		return;
	CAttachedBlock *pParentAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hParentItem);
	ASSERT(pParentAttached);
	if (!pParentAttached)
		return;
	pParentAttached->OnAfterDrop(pAttached);
}

void CControlSystemView::UpdateSelectedText(CString &Text)
{
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return;
	m_pTreeCtrl->SetItemText(Item, Text);
}

void CControlSystemView::UpdateSelectedImage(CControlObject *pObject)
{
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return;
	int iImage = pObject->GetPictureID();
	m_pTreeCtrl->SetItemImage(Item, iImage, iImage);
}

void CControlSystemView::LoadFromDB(HTREEITEM Item, CTargetInstance *pTargetInstance)
{
	m_pTreeCtrl->SetRedraw(FALSE);
//	CString ItemName;
//	ItemName.LoadString(IDS_TREEVIEW_EVENTLOG);
//	HTREEITEM EventLogItem = m_pTreeCtrl->InsertItem(ItemName, PICTURE_ID_BOOK_AND_HAND, PICTURE_ID_BOOK_AND_HAND, Item);

//	ItemName.LoadString(IDS_TREEVIEW_OBJECTS);
//	HTREEITEM ObjectsItem = m_pTreeCtrl->InsertItem(ItemName, PICTURE_ID_GEAR, PICTURE_ID_GEAR, Item);
	HTREEITEM ObjectsItem = Item; // skip "Control objects item

	TRY
	{
		RequeryAllObjects(ObjectsItem, pTargetInstance);
	}
	CATCH(CDBException, e)
		MessageBox(e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
	END_CATCH
	m_pTreeCtrl->SetRedraw(TRUE);
}

void CControlSystemView::RequeryAllObjects(HTREEITEM hItem, CTargetInstance *pTargetInstance, int ParentObjectID /*=-1*/, CControlObject *pParent /*=NULL*/)
{
	CControlSystemDatabase *pDB = pTargetInstance->GetDatabase();
	if (!pDB->IsOpen())
		return;
	IControlTargetAPI *pTarget = pTargetInstance->GetTarget();

	CDatabaseObjectDesc *pObjectDescs;
	int nDescs;
	if (pDB->QueryDbChildObjects(ParentObjectID, pObjectDescs, nDescs))
	{
		for (int iDesc = 0; iDesc < nDescs; iDesc++)
		{
			CDatabaseObjectDesc *pDesc = pObjectDescs + iDesc;
			CControlObject *pObject = pTargetInstance->CreateControlObject(pParent, pDesc->m_ID, pDesc->m_TYPE, pDesc->m_NAME, pDesc->m_bDeleted);
			HTREEITEM hNewItem = InsertObject(hItem, pObject, pTargetInstance);
			RequeryAllObjects(hNewItem, pTargetInstance, pDesc->m_ID, pObject);
		}
		if (pObjectDescs)
		{
			delete[] pObjectDescs;
		}
	}
}

HTREEITEM CControlSystemView::InsertObject(HTREEITEM hItem, CControlObject *pObject, CTargetInstance *pTargetInstance)
{
	CString ItemName = pObject->GetFullName();
	int PictureID;
	if (pObject->IsDeleted())
		PictureID = PICTURE_ID_DELETED;
	else
		PictureID = pTargetInstance->GetTarget()->GetObjectPictureID(pObject->GetType());
	HTREEITEM hNewItem = m_pTreeCtrl->InsertItem(ItemName, PictureID, PictureID, hItem);
	CAttachedBlock *pAttached = new CAttachedBlockForObject(OBJECT_FORM_INDEX, pTargetInstance, pObject);
	m_pTreeCtrl->SetItemData(hNewItem, (DWORD)pAttached);

	// Event log
//	ItemName.LoadString(IDS_TREEVIEW_EVENTLOG);
//	HTREEITEM hEventLogItem = m_pTreeCtrl->InsertItem(ItemName, PICTURE_ID_BOOK_AND_HAND, PICTURE_ID_BOOK_AND_HAND, hNewItem);

	// TMI
//	ItemName.LoadString(IDS_TREEVIEW_TMI);
//	HTREEITEM hTmiItem = m_pTreeCtrl->InsertItem(ItemName, PICTURE_ID_TOOLS, PICTURE_ID_TOOLS, hNewItem);

	return hNewItem;
}

void CControlSystemView::RefreshSelectedTarget(CTargetInstance *pTargetInstance)
{
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return;
	HTREEITEM TargetItem = GetTopParentItem(Item);
	if (!TargetItem)
		return;

	DeleteAllChildren(TargetItem);
	LoadFromDB(TargetItem, pTargetInstance);
}


