// ControlSystemView.h : interface of the CControlSystemView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLSYSTEMVIEW_H__DBB225BD_CC33_4317_A8D3_511269A3C42F__INCLUDED_)
#define AFX_CONTROLSYSTEMVIEW_H__DBB225BD_CC33_4317_A8D3_511269A3C42F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DragTreeView.h"

// Forward declarations
class CControlSystemDoc;
class CTargetInstance;
class CControlObject;
class CAbstractForm;

class CControlSystemView : public CDragTreeView
{
protected: // create from serialization only
	CControlSystemView();
	DECLARE_DYNCREATE(CControlSystemView)

// Attributes
public:

// Operations
public:
	void AddNewTarget(CTargetInstance *pTargetInstance);
	void AddNewObject();
	CTargetInstance *DeleteSelectedTarget();
	BOOL CanBeDeletedSelectedItem();
	BOOL CanAddChildToSelectedItem();
	void UpdateSelectedText(CString &Text);
	void UpdateSelectedImage(CControlObject *pObject);
	void RefreshSelectedTarget(CTargetInstance *pTargetInstance);

// Overrides
	//{{AFX_VIRTUAL(CControlSystemView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlSystemView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	CControlSystemDoc *GetDocument();
	CAbstractForm *GetCurrentForm() { return m_pCurrentlyVisibleForm; }
	void TurnToAbstractPane(CAbstractForm *pOld);
	void LoadFromDB(HTREEITEM hItem, CTargetInstance *pTargetInstance);
	void RequeryAllObjects(HTREEITEM hItem, CTargetInstance *pTargetInstance, int ParentObjectID = -1, CControlObject *pParent = NULL);
	HTREEITEM InsertObject(HTREEITEM hItem, CControlObject *pObject, CTargetInstance *pTargetInstance);

// protected overridables
protected:
	//Drag-drop
	virtual BOOL CanDragItem(HTREEITEM hDragItem);
	virtual BOOL CanDropItem(HTREEITEM hDragItem, HTREEITEM hDropItem);
	virtual void DraggingStarted(HTREEITEM hDraggedItem);
	virtual void DraggingCompleted(HTREEITEM hDroppedItem);

	// Protected members
protected:
	HTREEITEM m_PrevSelection;
	CImageList m_ImageList;
	CTreeCtrl *m_pTreeCtrl;
	CAbstractForm *m_pAbstractPane;
	CAbstractForm *m_pCurrentlyVisibleForm;
	UINT m_TimerID;


protected:
	//{{AFX_MSG(CControlSystemView)
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ControlSystemView.cpp
inline CControlSystemDoc* CControlSystemView::GetDocument()
   { return (CControlSystemDoc *)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSYSTEMVIEW_H__DBB225BD_CC33_4317_A8D3_511269A3C42F__INCLUDED_)
