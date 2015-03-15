// StationsView.h : interface of the CStationsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATIONSVIEW_H__850AA257_2201_11D5_9670_008048B59EAA__INCLUDED_)
#define AFX_STATIONSVIEW_H__850AA257_2201_11D5_9670_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArmHubDoc;

class CStationsView : public CTreeView
{
protected: // create from serialization only
	CStationsView();
	DECLARE_DYNCREATE(CStationsView)

// Attributes
public:
	CArmHubDoc *GetDocument();

// Operations
public:
	void AddStation(CStation *pStation);
	CStation *GetSelectedStation();
	void DeleteSelectedStation();
	void UpdateSelectedStationName();
	void Clear();
	void MarkAlarm(HTREEITEM Item);
	void MarkAlarm(HTREEITEM Item, BOOL bAlarm);

// Overrides
	// ClassWizard generated virtual function overrides
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

protected:
	void TurnToAbstractPane(CAbstractForm *pOld);

protected:
	HTREEITEM m_PrevSelection;
	CImageList m_ImageList;
	CTreeCtrl *m_pTreeCtrl;
	CAbstractForm *m_pAbstractPane;
	CAbstractForm *m_pCurrentlyVisibleForm;
	UINT m_TimerID;

protected:
	//{{AFX_MSG(CStationsView)
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in StationsView.cpp
inline CArmHubDoc* CStationsView::GetDocument()
   { return (CArmHubDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSVIEW_H__850AA257_2201_11D5_9670_008048B59EAA__INCLUDED_)
