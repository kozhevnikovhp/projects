// DragTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "DragTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragTreeView

IMPLEMENT_DYNCREATE(CDragTreeView, CExTreeView)

CDragTreeView::CDragTreeView()
{
	m_hItemDrag = NULL;
	m_hItemDrop = NULL;
	m_hLastItem = NULL;
	m_pImageList = NULL;
	m_bDragging = FALSE;
}

CDragTreeView::~CDragTreeView()
{
}

BEGIN_MESSAGE_MAP(CDragTreeView, CExTreeView)
	//{{AFX_MSG_MAP(CDragTreeView)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragTreeView drawing

void CDragTreeView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDragTreeView diagnostics

#ifdef _DEBUG
void CDragTreeView::AssertValid() const
{
	CExTreeView::AssertValid();
}

void CDragTreeView::Dump(CDumpContext &dc) const
{
	CExTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDragTreeView message handlers

void CDragTreeView::OnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if (!IsDraggingEnabled()) return;
	CPoint ptAction = pNMTreeView->ptDrag;
	UINT nFlags;

	m_hItemDrag = GetTreeCtrl().HitTest(ptAction, &nFlags);
	DraggingStarted(m_hItemDrag);
	m_hItemDrop = NULL;
	if (!CanDragItem(m_hItemDrag))
	{
		m_bDragging = FALSE;
		return;
	}

	ASSERT(m_pImageList == NULL);
	m_pImageList = GetTreeCtrl().CreateDragImage(m_hItemDrag);  // get the image list for dragging
	if (m_pImageList)
	{
		m_pImageList->DragShowNolock(TRUE);
		m_pImageList->SetDragCursorImage(0, CPoint(0, 0));
		m_pImageList->BeginDrag(0, CPoint(0,0));
		m_pImageList->DragMove(ptAction);
		m_pImageList->DragEnter(this, ptAction);
	}
	m_bDragging = TRUE;
	SetCapture();
	
	*pResult = 0;
}

void CDragTreeView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (IsDraggingEnabled() && IsDraggingNow())
	{
		HTREEITEM hNewItem = m_hItemDrag;
		if (m_pImageList)
		{
			m_pImageList->DragLeave(this);
			m_pImageList->EndDrag();
			delete m_pImageList;
			m_pImageList = NULL;
		}

		GetTreeCtrl().SetRedraw(FALSE);
		
		if (m_hItemDrag != NULL && m_hItemDrop != NULL)
		{
			if (GetTreeCtrl().GetParentItem(m_hItemDrag) == m_hItemDrop)
				OnCannotMoveAtTheSamePlace(m_hItemDrag, m_hItemDrop);
			else if (IsChildNodeOf(m_hItemDrop, m_hItemDrag))
				OnCannotMoveParentToChild(m_hItemDrag, m_hItemDrop);
			else if (!CanDropItem(m_hItemDrag, m_hItemDrop))
				OnCannotDrop(m_hItemDrag, m_hItemDrop);
			else
				hNewItem = MoveItem(m_hItemDrag, m_hItemDrop);
		}

		GetTreeCtrl().SetRedraw(TRUE);
		ReleaseCapture();
		m_bDragging = FALSE;
		GetTreeCtrl().SelectDropTarget(NULL);
		if (hNewItem != m_hItemDrag)
		{
			DraggingCompleted(hNewItem);
			GetTreeCtrl().SelectItem(hNewItem);
		}
	}
	
	CExTreeView::OnLButtonUp(nFlags, point);
}

void CDragTreeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT flags;

	if (IsDraggingEnabled() && IsDraggingNow())
	{
		if (m_pImageList)
			m_pImageList->DragMove(point);
		HTREEITEM hItem = GetTreeCtrl().HitTest(point, &flags);
		HTREEITEM hNewItem;
		if (flags & TVHT_ABOVE)
		{
			hNewItem = GetTreeCtrl().GetPrevSiblingItem(m_hLastItem);
			if (!hNewItem)
				hNewItem = GetTreeCtrl().GetParentItem(m_hLastItem);
			GetTreeCtrl().EnsureVisible(hNewItem);
			m_hLastItem = hNewItem;
		}
		if (flags & TVHT_BELOW)
		{
			hNewItem=GetTreeCtrl().GetNextSiblingItem(m_hLastItem);
			if (!hNewItem)
			{
				hNewItem=GetTreeCtrl().GetParentItem(m_hLastItem);
				hNewItem=GetTreeCtrl().GetNextSiblingItem(hNewItem);
			}
			GetTreeCtrl().EnsureVisible(hNewItem);
			m_hLastItem = hNewItem;;
		}
		if (hItem != NULL)
		{
			m_pImageList->DragLeave(this);
			GetTreeCtrl().SelectDropTarget(hItem);
			m_hItemDrop = hItem;
			m_pImageList->DragEnter(this, point);
			m_hLastItem = hItem;
		}
	}

	CExTreeView::OnMouseMove(nFlags, point);
}

BOOL CDragTreeView::PreTranslateMessage(MSG *pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && IsDraggingNow())
	{
		m_bDragging = FALSE;
		if (m_pImageList)
		{
			m_pImageList->DragLeave(NULL);
			m_pImageList->EndDrag();
			ReleaseCapture();
			GetTreeCtrl().SelectDropTarget(NULL);
			delete m_pImageList;
		}
		m_pImageList = NULL;
		SetRedraw(TRUE);
		return TRUE;		// DO NOT process further
	}
	return CExTreeView::PreTranslateMessage(pMsg);
}

HTREEITEM CDragTreeView::MoveItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop)
{
	GetTreeCtrl().SetRedraw(FALSE);
	DraggingBeforeMoving(hItemDrag);
	HTREEITEM TransferredItem = TransferItem(hItemDrag, hItemDrop);
	OnDraggingItemMoved(hItemDrag, hItemDrop);
	GetTreeCtrl().DeleteItem(hItemDrag);
	DraggingAfterMoving(TransferredItem);
	GetTreeCtrl().SetRedraw(TRUE);
	
	return TransferredItem;
}

HTREEITEM CDragTreeView::TransferItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop)
{
	TV_INSERTSTRUCT		tvstruct;
	TCHAR				sztBuffer[50];
	HTREEITEM			hNewItem, hFirstChild;

	tvstruct.item.hItem = hItemDrag;
	tvstruct.item.cchTextMax = 49;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvstruct.item.stateMask = TVIS_OVERLAYMASK | TVIS_STATEIMAGEMASK;
	GetTreeCtrl().GetItem(&tvstruct.item);  // get information of the dragged element
	tvstruct.hParent = hItemDrop;
	tvstruct.hInsertAfter = TVI_LAST;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT | TVIF_PARAM;
	hNewItem = GetTreeCtrl().InsertItem(&tvstruct);
	GetTreeCtrl().Expand(tvstruct.hParent, TVE_EXPAND);

	while ((hFirstChild = GetTreeCtrl().GetChildItem(hItemDrag)) != NULL)
	{
		TransferItem(hFirstChild, hNewItem);  // recursively transfer all the items
		GetTreeCtrl().DeleteItem(hFirstChild);		// delete the first child and all its children
	}
	GetTreeCtrl().Expand(hNewItem, TVE_COLLAPSE);
	return hNewItem;
}

