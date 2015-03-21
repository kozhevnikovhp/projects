#ifndef __EXTENDED_TREE_VIEW_H_INCLUDED__
#define __EXTENDED_TREE_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ExTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExTreeView view

class CExTreeView : public CTreeView
{
protected:
	CExTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExTreeView)

// Attributes
public:

// Operations
public:
	int GetChildCount(HTREEITEM hItem);
	HTREEITEM GetTopParentItem(HTREEITEM hItem);
	void SetNewStyle(long lStyleMask, BOOL bSetBits);
	BOOL IsChildNodeOf(HTREEITEM hItemChild, HTREEITEM hItemSuspectedParent);
	BOOL FindItemUp(HTREEITEM hItemFrom, HTREEITEM hItemToFind);
	void DeleteAllChildren(HTREEITEM hItem);

// Overrides
	//{{AFX_VIRTUAL(CExTreeView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	HTREEITEM GetNextSearchItem(HTREEITEM hItem);
	HTREEITEM GetPrevSearchItem(HTREEITEM hItem);

protected:
	//{{AFX_MSG(CExTreeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXTENDED_TREE_VIEW_H_INCLUDED__
