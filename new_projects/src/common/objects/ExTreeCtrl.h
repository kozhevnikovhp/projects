#if !defined(AFX_EXTREECTRL_H__4A28BA61_DDE8_11D2_ABE8_008048EEE0E8__INCLUDED_)
#define AFX_EXTREECTRL_H__4A28BA61_DDE8_11D2_ABE8_008048EEE0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExTreeCtrl.h : header file
//

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl window
//

class CDirTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDirTreeCtrl();

// Attributes
public:

// Operations
public:
    void Initialize (bool clear=true);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	bool SetPath (CString path);
	CString GetFullPath (HTREEITEM item);
	
	virtual ~CDirTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDirTreeCtrl)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HTREEITEM Dir2Item(HTREEITEM current, char *dir);
	virtual bool MatchExtension(CString file);
	void RefreshTreeImages(HTREEITEM item, int img);
	CString GetPathFromHere (CString s, HTREEITEM itm);
	HTREEITEM AddItem_Core (const char *name, HTREEITEM parent, bool isDir, int icon, int iconOpen);
	void ExpandBranch (HTREEITEM parent);
	void DeleteAllChild (HTREEITEM itm,  bool isEmpty=true);
	HTREEITEM AddItem(const char * path, HTREEITEM parent, bool isDir=FALSE, int iconEx=-1, int iconEx2=-1);
	
	HTREEITEM m_desktop_root;
	CString   m_desktop_path;

	HTREEITEM m_drives_root;
	char      m_drives_name[500];

	CImageList m_img;
};

/////////////////////////////////////////////////////////////////////////////
// CExTreeCtrl window

class CExTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CExTreeCtrl();

// Attributes
public:

// Operations
public:
	int GetChildCount(HTREEITEM Item);
	HTREEITEM GetTopParentItem(HTREEITEM Item);
	HTREEITEM GetNextSearchItem(HTREEITEM Item);
	HTREEITEM GetPrevSearchItem(HTREEITEM Item);
	void SetNewStyle(long lStyleMask, BOOL bSetBits);
	BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);

// Overrides
	//{{AFX_VIRTUAL(CExTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExTreeCtrl)
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	BOOL FindItemUp(HTREEITEM ItemFrom, HTREEITEM ItemToFind);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMSelTreeCtrl window

class CMSelTreeCtrl : public CExTreeCtrl
{
	int CompareItems(HTREEITEM Item1, HTREEITEM Item2);
	BOOL AddNewSelection(HTREEITEM Item);
// Construction
public:
	CMSelTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CMSelTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectAll();
	BOOL DeleteItem(HTREEITEM hItem); // overloaded function
	BOOL DeleteAllItems(); // overloaded function
	HTREEITEM GetFirstSelectedItem();
	HTREEITEM GetLastSelectedItem();
	HTREEITEM GetNextSelectedItem(HTREEITEM hItem);
	HTREEITEM GetPrevSelectedItem(HTREEITEM hItem);
	BOOL SelectItem(HTREEITEM hItem); // overloaded function
	inline void EnableMSelection(BOOL bEnable=TRUE) { m_bMSelectionEnabled=bEnable; }
	inline BOOL IsMSelectionEnabled() { return m_bMSelectionEnabled; }
	void ClearSelection();
	void SetSelection(HTREEITEM Item, BOOL bSelected=TRUE);
	BOOL IsSelected(HTREEITEM Item);
	virtual ~CMSelTreeCtrl();

	// Generated message map functions
protected:
	void RefreshSelection();
	void RemoveItemFromSelectionList(HTREEITEM hItem);
	BOOL SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo);
	//{{AFX_MSG(CMSelTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	CList<HTREEITEM, HTREEITEM> m_SelectionList;
private:
	BOOL m_bMSelectionEnabled;
	HTREEITEM m_hItemFirstSel;
	DECLARE_MESSAGE_MAP()
};

enum TypeOfDragging
{
	RightDragging=-1,
	NoDragging=0,
	LeftDragging=1
};

class CDragTreeCtrl : public CMSelTreeCtrl
{
// Construction
public:
	CDragTreeCtrl();

// Attributes
public:
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDragTreeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	inline void EnableDragging(BOOL bEnable=TRUE) { m_bDraggingEnabled=bEnable; }
	inline BOOL IsDraggingEnabled() { return m_bDraggingEnabled; }
	HTREEITEM MoveItem(HTREEITEM hitem, HTREEITEM hNewParent, TypeOfDragging DraggingType);
	inline BOOL IsDraggingNow() {return m_DraggingType!=NoDragging;}
	virtual ~CDragTreeCtrl();
	void	OnButtonUp(void);

	// Generated message map functions
protected:
	inline TypeOfDragging GetTypeOfDragging() {return m_DraggingType; }
	virtual void OnItemMoved(HTREEITEM hItemDrag, HTREEITEM hItemDrop, TypeOfDragging DraggingType) {}
	virtual BOOL CanDragItem(HTREEITEM hItem, TypeOfDragging DraggingType) { return TRUE; }
	virtual BOOL CanDropItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop, TypeOfDragging DraggingType) { return TRUE; }
	virtual void BeforeMouseReleased() {}
	virtual void AfterMouseReleased() {}
	virtual void BeforeMoving(HTREEITEM DragItem) {}
	virtual void AfterMoving(HTREEITEM TransferredItem) {}

	//{{AFX_MSG(CDragTreeCtrl)
	afx_msg void OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pLResult);
	afx_msg void OnBeginRDrag(LPNMHDR pnmhdr, LRESULT *pLResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	void BeginDrag();
	HTREEITEM TransferItem(HTREEITEM hitem, HTREEITEM hNewParent, TypeOfDragging DraggingType);

	DECLARE_MESSAGE_MAP()
private:
	CList<HTREEITEM, HTREEITEM> m_ItemsToMoveList;
	TypeOfDragging m_DraggingType;
	BOOL m_bDraggingEnabled;
	HTREEITEM m_hItemDrag;
	HTREEITEM m_hItemDrop;
	HTREEITEM m_LastItem;
	CImageList *m_pImageList;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTREECTRL_H__4A28BA61_DDE8_11D2_ABE8_008048EEE0E8__INCLUDED_)
