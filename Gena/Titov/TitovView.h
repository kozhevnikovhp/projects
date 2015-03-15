// TitovView.h : interface of the CTitovView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITOVVIEW_H__CCE4D493_5E67_43D1_AD10_4E722C0F2C86__INCLUDED_)
#define AFX_TITOVVIEW_H__CCE4D493_5E67_43D1_AD10_4E722C0F2C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTitovView : public CView
{
protected: // create from serialization only
	CTitovView();
	DECLARE_DYNCREATE(CTitovView)

// Attributes
public:
	CTitovDoc *GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitovView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitovView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
///	CClientDC *m_pDC;

// Generated message map functions
protected:
	//{{AFX_MSG(CTitovView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TitovView.cpp
inline CTitovDoc* CTitovView::GetDocument()
   { return (CTitovDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITOVVIEW_H__CCE4D493_5E67_43D1_AD10_4E722C0F2C86__INCLUDED_)
