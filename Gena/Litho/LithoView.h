// LithoView.h : interface of the CLithoView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __LITHO_VIEW_H_INCLUDED__
#define __LITHO_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"
#include "LithoRenderer.h"

// Forward declarations
class CLithoDoc;

class CLithoView : public COpenGLView
{
protected: // create from serialization only
	CLithoView();
	DECLARE_DYNCREATE(CLithoView)

// Attributes
public:
	CLithoDoc *GetDocument() { return (CLithoDoc *)m_pDocument; }
	char *GetDisplayedText();

	// Public overrides
public:
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

	//{{AFX_VIRTUAL(CLithoView)
	public:
	virtual void OnDraw(CDC *pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLithoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CLithoRenderer *m_pRenderer;

// Generated message map functions
protected:
	//{{AFX_MSG(CLithoView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __LITHO_VIEW_H_INCLUDED__
