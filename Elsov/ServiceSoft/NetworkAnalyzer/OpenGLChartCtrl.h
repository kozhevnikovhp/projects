#if !defined(AFX_OPENGLCHARTCTRL_H__62F8F313_2589_434E_BC82_7D65EDEBB9BB__INCLUDED_)
#define AFX_OPENGLCHARTCTRL_H__62F8F313_2589_434E_BC82_7D65EDEBB9BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLChartCtrl.h : header file
//
#include <gl\gl.h>
#include <gl\glaux.h>

#include "OpenGLRenderer.h"

/////////////////////////////////////////////////////////////////////////////
// COpenGLStaticCtrl window

class COpenGLStaticCtrl : public CStatic
{
// Construction
public:
	COpenGLStaticCtrl();

// Attributes
public:

// Operations
public:
	BOOL Setup();

// Overrides
	virtual CRenderer *GetRenderer() { return m_pDefaultRenderer; };
	virtual void CreateRenderer();
	virtual void DestroyRenderer();
	virtual void SetDefaultView() { GetRenderer()->SetDefaultView(); }
	//{{AFX_VIRTUAL(COpenGLStaticCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenGLStaticCtrl();

protected:
	//{{AFX_MSG(COpenGLStaticCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CClientDC *m_pDC;
	CRenderer *m_pDefaultRenderer;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLCHARTCTRL_H__62F8F313_2589_434E_BC82_7D65EDEBB9BB__INCLUDED_)
