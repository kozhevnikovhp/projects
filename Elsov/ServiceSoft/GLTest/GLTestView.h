// GLTestView.h : interface of the CGLTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLTESTVIEW_H__DBDA39C6_9489_4B6D_83EF_CF36F86ED6D7__INCLUDED_)
#define AFX_GLTESTVIEW_H__DBDA39C6_9489_4B6D_83EF_CF36F86ED6D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"

class CGLTestRenderer : public COpenGL_2D
{
// Construction/destruction
public:
	CGLTestRenderer();
	virtual ~CGLTestRenderer();
	void RebuildDisplayList();

// Public overridable methods
public:
	virtual void OnRenderScene();
	virtual void OnInitialized();

// Protected overridable methods
protected:

// Event handlers

// Public methods
public:

	// Protected methods
protected:

// Private methods
private:

// Protected members
protected:
//	unsigned int m_VectorFontListBaseID;
//	GLYPHMETRICSFLOAT m_GlyphMetrics[256];
	unsigned int m_WorkingDisplayList, m_SpareDisplayList;

// Private members
private:
};

/////////////////////////////////////////////////////////////////////////////
// CGLTestStatic window

class CGLTestStatic : public COpenGLStatic
{
// Construction
public:
	CGLTestStatic();

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CGLTestStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGLTestStatic();
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

protected:
	CGLTestRenderer *m_pRenderer;

// Generated message map functions
protected:
	//{{AFX_MSG(CGLTestStatic)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CGLTestView : public COpenGLView
{
protected: // create from serialization only
	CGLTestView();
	DECLARE_DYNCREATE(CGLTestView)

// Attributes
public:
	CGLTestDoc* GetDocument();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CGLTestView)
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
	virtual ~CGLTestView();
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CGLTestRenderer *m_pRenderer;

// Generated message map functions
protected:
	//{{AFX_MSG(CGLTestView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GLTestView.cpp
inline CGLTestDoc* CGLTestView::GetDocument()
   { return (CGLTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLTESTVIEW_H__DBDA39C6_9489_4B6D_83EF_CF36F86ED6D7__INCLUDED_)
