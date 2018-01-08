/* OpenGLView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	05 Nov 2008 - COpenGLStatic class

*/

#ifndef __OPENGL_VIEW_H_INCLUDED__
#define __OPENGL_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <gl\gl.h>
#include <gl\glaux.h>

#include "3DView.h"
#include "OpenGLRenderer.h"

/////////////////////////////////////////////////////////////////////////////
// COpenGLStatic

class COpenGLStatic : public C3DStatic
{
// Construction
public:
	COpenGLStatic();

// Public methods
public:
	void Setup();

// Overrides
	//{{AFX_VIRTUAL(COpenGLStatic)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenGLStatic();
	virtual CRenderer *GetRenderer() { return m_pDefaultRenderer; };
	virtual void CreateRenderer();
	virtual void DestroyRenderer();
	virtual void SetDefaultView() { GetRenderer()->SetDefaultView(); }

protected:
	//{{AFX_MSG(COpenGLStatic)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	CClientDC *m_pDC;
	CRenderer *m_pDefaultRenderer;
};

class COpenGLView : public C3DView
{
protected: // create from serialization only
	COpenGLView();

	DECLARE_DYNCREATE(COpenGLView)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(COpenGLView)
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
	virtual ~COpenGLView();
	virtual CRenderer *GetRenderer() { return m_pDefaultRenderer; };
	virtual void CreateRenderer();
	virtual void DestroyRenderer();
	virtual void SetDefaultView() { GetRenderer()->SetDefaultView(); }
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COpenGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CClientDC *m_pDC;
	CRenderer *m_pDefaultRenderer;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __OPENGL_VIEW_H_INCLUDED__
