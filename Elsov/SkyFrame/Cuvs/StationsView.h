// StationsView.h : interface of the CStationsView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __STATIONS_VIEW_H_INCLUDED__
#define __STATIONS_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DragTreeView.h"

// Forward declarations
class CAbstractForm;

class CStationsView : public CDragTreeView
{
	friend class CStationsFrame;
protected: // create from serialization only
	CStationsView();
	DECLARE_DYNCREATE(CStationsView)

// Attributes
public:
	HTREEITEM InsertAllNetItem();
	HTREEITEM InsertStation(CString &StationName, long StationID, HTREEITEM ParentItem);
	void SetSelectedItemLabel(CString &NewLabel);

// protected overridables
protected:
	virtual BOOL CanDragItem(HTREEITEM DragItem);
	virtual BOOL CanDropItem(HTREEITEM DragItem, HTREEITEM DropItem);
	virtual void DraggingStarted(HTREEITEM DraggedItem);
	virtual void DraggingCompleted(HTREEITEM DroppedItem);
	
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CStationsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStationsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Protected methods
protected:
	void TurnToAbstractPane(CAbstractForm *pOld);

// Protected members
protected:
	CImageList m_ImageList;
	CTreeCtrl *m_pTreeCtrl;
	CCuvsDoc *m_pDoc;
	CAbstractForm *m_pCurrentlyVisibleForm;
	HTREEITEM m_PrevSelection;

// Generated message map functions
protected:
	//{{AFX_MSG(CStationsView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __STATIONS_VIEW_H_INCLUDED__
