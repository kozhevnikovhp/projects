// MibViewerDocView.h : interface of the CMibViewerFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIBVIEWERDOCVIEW_H__FA5D6D32_BF6E_4B9E_BEA0_161C8E5A793C__INCLUDED_)
#define AFX_MIBVIEWERDOCVIEW_H__FA5D6D32_BF6E_4B9E_BEA0_161C8E5A793C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declarations
class CMibViewerDoc;
class CMibTree;
class CNodeForm;

class CMibViewerView : public CTreeView
{
protected: // create from serialization only
	CMibViewerView();
	DECLARE_DYNCREATE(CMibViewerView)

// Attributes
public:
	CMibViewerDoc *GetDocument();

// Public methods
public:
	void LoadMibTree(CMibTree &MibTree);
	void SetNodeForm(CNodeForm *pForm) { m_pNodeForm = pForm; }

// Overrides
	//{{AFX_VIRTUAL(CMibViewerView)
	public:
	virtual void OnDraw(CDC *pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMibViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CNodeForm *m_pNodeForm;

// Generated message map functions
protected:
	//{{AFX_MSG(CMibViewerView)
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MibViewerView.cpp
inline CMibViewerDoc* CMibViewerView::GetDocument()
   { return (CMibViewerDoc *)m_pDocument; }
#endif

class CMibViewerFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMibViewerFrame();
	DECLARE_DYNCREATE(CMibViewerFrame)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CMibViewerFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMibViewerFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CSplitterWnd m_SplitterVert, m_SplitterHor;

protected:
	//{{AFX_MSG(CMibViewerFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIBVIEWERDOCVIEW_H__FA5D6D32_BF6E_4B9E_BEA0_161C8E5A793C__INCLUDED_)
