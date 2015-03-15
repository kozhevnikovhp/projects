// NetworkAnalyzerView.h : interface of the CNetworkAnalyzerView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_ANALYZER_VIEW_H_INCLUDED__
#define __NETWORK_ANALYZER_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declarations
class CNetworkAnalyzerDoc;
class CNetObject;

// Visitors

class CItemVisitorAPI
{
// Public overridables
public:
	virtual unsigned long Visit(CTreeCtrl &TreeCtrl, HTREEITEM hItem) = 0;

// Protected overridables
protected:

};

class CIdleVisitor : public CItemVisitorAPI
{
//Constuctors/destructors
public:
	CIdleVisitor() {}
	virtual ~CIdleVisitor() {}

// Public overridables
public:
	virtual unsigned long Visit(CTreeCtrl &TreeCtrl, HTREEITEM hItem);

// Protected members
protected:
};



class CNetworkAnalyzerView : public CTreeView
{
protected: // create from serialization only
	CNetworkAnalyzerView();
	DECLARE_DYNCREATE(CNetworkAnalyzerView)

// Public interface
public:
	void NewHostCreated(CHost *pHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	void GuiCreated(IGuiAPI *pGui) { m_pGui = pGui; }
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	BOOL OnIdleProcess();
// Attributes
public:
	CNetworkAnalyzerDoc *GetDocument();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CNetworkAnalyzerView)
	public:
	virtual void OnDraw(CDC *pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkAnalyzerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	HTREEITEM FindItemInsertAfter(HTREEITEM hRootItem, CNetObject *pObject, HTREEITEM &hNextItem);
	HTREEITEM Find(CNetObject *pObject);
	HTREEITEM FindIn(CNetObject *pObject, HTREEITEM hItem);
	HTREEITEM InsertHost(HTREEITEM hSubnetItem, CHost *pHost);
	HTREEITEM InsertNetObject(HTREEITEM hRootItem, CNetObject *pObject, int nImageIndex);
	CNetObject *GetNetObject(HTREEITEM hItem);
	void VisitAllItems(CItemVisitorAPI *pVisitor, HTREEITEM hParentItem);

// Protected members
protected:
	CImageList m_ImageList;
	unsigned long m_uLastOnIdleProcessedTime;
	IGuiAPI *m_pGui;

protected:
	//{{AFX_MSG(CNetworkAnalyzerView)
	afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NetworkAnalyzerView.cpp
inline CNetworkAnalyzerDoc *CNetworkAnalyzerView::GetDocument()
   { return (CNetworkAnalyzerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__NETWORK_ANALYZER_VIEW_H_INCLUDED__
