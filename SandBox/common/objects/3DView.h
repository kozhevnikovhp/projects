/* 3DView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	03 0ct 2008 - cursors
	05 Nov 2008 - C3DStatic class

*/

#ifndef __3D_VIEW_H_INCLUDED__
#define __3D_VIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DView.h : header file
//

#include "3DRenderer.h"

typedef struct
{
	char *pszCursorName;
	HCURSOR hCursor;
} SCursorDesc;

/////////////////////////////////////////////////////////////////////////////
// C3DStatic

class C3DStatic : public CStatic
{
// Construction
public:
	C3DStatic();

// Attributes
public:

// Operations
public:
	void Redraw() { InvalidateRect(NULL); UpdateWindow(); }

// Overrides
	//{{AFX_VIRTUAL(C3DStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~C3DStatic();
	virtual CRenderer *GetRenderer() { ASSERT(FALSE); return NULL; } // must be overriden
	virtual void CreateRenderer() { ASSERT(FALSE); } // must be overriden
	virtual void DestroyRenderer() { ASSERT(FALSE); } // must be overriden

// Protected methods
protected:
	void ChangeCursor();

// Generated message map functions
protected:
	//{{AFX_MSG(C3DStatic)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Protected members
protected:
	CursorTypeEnum m_OldCursor;
};

/////////////////////////////////////////////////////////////////////////////
// C3DView view

class C3DView : public CView
{
protected:
	C3DView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C3DView)

// Attributes
public:

// Operations
public:
	void Redraw() { InvalidateRect(NULL); UpdateWindow(); }

// Overrides
	//{{AFX_VIRTUAL(C3DView)
	protected:
	virtual void OnDraw(CDC *pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~C3DView();
	virtual CRenderer *GetRenderer() { ASSERT(FALSE); return NULL; } // must be overriden
	virtual void CreateRenderer() { ASSERT(FALSE); } // must be overriden
	virtual void DestroyRenderer() { ASSERT(FALSE); } // must be overriden
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void ChangeCursor();

	//{{AFX_MSG(C3DView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
protected:
	CursorTypeEnum m_OldCursor;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __3D_VIEW_H_INCLUDED__
