// ExTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "ExTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExTreeView

IMPLEMENT_DYNCREATE(CExTreeView, CTreeView)

CExTreeView::CExTreeView()
{
}

CExTreeView::~CExTreeView()
{
}


BEGIN_MESSAGE_MAP(CExTreeView, CTreeView)
	//{{AFX_MSG_MAP(CExTreeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExTreeView drawing

void CExTreeView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CExTreeView diagnostics

#ifdef _DEBUG
void CExTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CExTreeView::Dump(CDumpContext &dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

void CExTreeView::SetNewStyle(long lStyleMask, BOOL bSetBits)
{
	long		lStyleOld;

	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld &= ~lStyleMask;
	if (bSetBits)
		lStyleOld |= lStyleMask;

	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

BOOL CExTreeView::IsChildNodeOf(HTREEITEM hItemChild, HTREEITEM hItemSuspectedParent)
{
	do
	{
		if (hItemChild == hItemSuspectedParent)
			break;
	}
	while ((hItemChild = GetTreeCtrl().GetParentItem(hItemChild)) != NULL);

	return (hItemChild != NULL);
}

HTREEITEM CExTreeView::GetTopParentItem(HTREEITEM hItem)
{
	HTREEITEM ParentItem = hItem;
	while (hItem)
	{
		ParentItem = GetTreeCtrl().GetParentItem(ParentItem);
		if (!ParentItem) break;
		hItem = ParentItem;
	}
	return hItem;
}

int CExTreeView::GetChildCount(HTREEITEM hItem)
{
	if (hItem == NULL) return 0;
	int count = 0;
	HTREEITEM ChildItem = GetTreeCtrl().GetChildItem(hItem);
	while (ChildItem)
	{
		count++;
		ChildItem = GetTreeCtrl().GetNextItem(ChildItem, TVGN_NEXT);
	}
	return count;
}

HTREEITEM CExTreeView::GetNextSearchItem(HTREEITEM hItem)
{
	HTREEITEM NextItem = GetTreeCtrl().GetChildItem(hItem);
	if (!NextItem)
		NextItem = GetTreeCtrl().GetNextSiblingItem(hItem);
	if (NextItem)
		return NextItem;
	HTREEITEM ParentItem = hItem;
	while (1)
	{
		ParentItem = GetTreeCtrl().GetParentItem(ParentItem);
		if (!ParentItem)
			return NULL;
		NextItem = GetTreeCtrl().GetNextSiblingItem(ParentItem);
		if (!NextItem) continue;
		if (!GetTreeCtrl().GetParentItem(NextItem)) return NULL;
		if (NextItem) return NextItem;
	}
}

HTREEITEM CExTreeView::GetPrevSearchItem(HTREEITEM hItem)
{
	HTREEITEM PrevItem = GetTreeCtrl().GetPrevSiblingItem(hItem);
	if (PrevItem) return PrevItem;
	return GetTreeCtrl().GetParentItem(hItem);
}

// Определяем, не встречается ли hItemToFind в дереве, если подниматься в нем от hItemFrom
BOOL CExTreeView::FindItemUp(HTREEITEM hItemFrom, HTREEITEM hItemToFind)
{
	HTREEITEM FoundItem = hItemFrom;
	while (FoundItem)
	{
		if (FoundItem == hItemToFind)
			return TRUE;
		FoundItem = GetPrevSearchItem(FoundItem);
	}
	return (FoundItem != NULL);
}

void CExTreeView::DeleteAllChildren(HTREEITEM hItem)
{
	HTREEITEM ChildItem = GetTreeCtrl().GetChildItem(hItem);
	while (ChildItem)
	{
		GetTreeCtrl().DeleteItem(ChildItem);
		ChildItem = GetTreeCtrl().GetChildItem(hItem);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CExTreeView message handlers
