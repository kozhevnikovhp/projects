#ifndef __DRAG_TREE_VIEW_H_INCLUDED__
#define __DRAG_TREE_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DragTreeView.h : header file
//

#include "ExTreeView.h"

/////////////////////////////////////////////////////////////////////////////
// CDragTreeView view

class CDragTreeView : public CExTreeView
{
protected:
	CDragTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDragTreeView)

// Attributes
public:

// Operations
public:

// protected overridables
protected:
	virtual BOOL IsDraggingEnabled() { return TRUE; }
	virtual BOOL CanDragItem(HTREEITEM hDragItem) { return TRUE; }
	virtual BOOL CanDropItem(HTREEITEM hDragItem, HTREEITEM hDropItem) { return TRUE; }
	virtual void DraggingStarted(HTREEITEM hDraggedItem) {}
	virtual void DraggingCompleted(HTREEITEM hDroppedItem) {}
	virtual void DraggingBeforeMoving(HTREEITEM hDragItem) {}
	virtual void DraggingAfterMoving(HTREEITEM hTransferredItem) {}
	virtual void OnDraggingItemMoved(HTREEITEM hItemDrag, HTREEITEM hItemDrop) {}
	virtual void OnCannotMoveAtTheSamePlace(HTREEITEM hItemDrag, HTREEITEM hItemDrop) { MessageBeep(0); }
	virtual void OnCannotMoveParentToChild(HTREEITEM hItemDrag, HTREEITEM hItemDrop) { MessageBeep(0); }
	virtual void OnCannotDrop(HTREEITEM hItemDrag, HTREEITEM hItemDrop) { MessageBeep(0); }

// protected members
protected:
	HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hNewParent);
	HTREEITEM TransferItem(HTREEITEM hItem, HTREEITEM hNewParent);
	BOOL IsDraggingNow() { return m_bDragging; }

// Overrides
	//{{AFX_VIRTUAL(CDragTreeView)
	public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDragTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// private members
private:
	HTREEITEM m_hItemDrag;
	HTREEITEM m_hItemDrop;
	HTREEITEM m_hLastItem;
	CImageList *m_pImageList;
	BOOL m_bDragging;

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragTreeView)
	afx_msg void OnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DRAG_TREE_VIEW_H_INCLUDED__
