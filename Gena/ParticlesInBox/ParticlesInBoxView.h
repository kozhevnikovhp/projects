// ParticlesInBoxView.h : interface of the CParticlesInBoxView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLESINBOXVIEW_H__4C7C35A3_227A_4939_9BBD_4F0F55B5AE06__INCLUDED_)
#define AFX_PARTICLESINBOXVIEW_H__4C7C35A3_227A_4939_9BBD_4F0F55B5AE06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"
#include "WorldRenderer.h"

// Forward declarations

class CParticlesInBoxView : public COpenGLView
{
protected: // create from serialization only
	CParticlesInBoxView();
	DECLARE_DYNCREATE(CParticlesInBoxView)

// Attributes
public:
	CParticlesInBoxDoc *GetDocument();

// Operations
public:

// Overrides
	virtual CRenderer *GetRenderer() { return m_pWorldRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

	//{{AFX_VIRTUAL(CParticlesInBoxView)
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
	virtual ~CParticlesInBoxView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

// Protected members
	CWorldRenderer *m_pWorldRenderer;

protected:
	//{{AFX_MSG(CParticlesInBoxView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ParticlesInBoxView.cpp
inline CParticlesInBoxDoc* CParticlesInBoxView::GetDocument()
   { return (CParticlesInBoxDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLESINBOXVIEW_H__4C7C35A3_227A_4939_9BBD_4F0F55B5AE06__INCLUDED_)
