// MeshView.h : interface of the CMeshView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHVIEW_H__DA9C25FE_7792_4C6C_BA3C_A80C971EA40A__INCLUDED_)
#define AFX_MESHVIEW_H__DA9C25FE_7792_4C6C_BA3C_A80C971EA40A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Poly2rt\poly2rt.hpp"
#include "..\Poly2rt\mesh_poly.hpp"

typedef enum
{
	REGIME_NONE,
	REGIME_AREA,
	REGIME_CUTOUT
} RegimeEnum;

class CMeshView : public CView
{
protected: // create from serialization only
	CMeshView();
	DECLARE_DYNCREATE(CMeshView)

// Attributes
public:
	CMeshDoc* GetDocument();

// Operations
public:
	void Redraw() { Invalidate(); UpdateWindow(); }
	void DrawArea(CDC *pDC, int height, AreaList *pArea);
	void Draw34(CDC *pDC, int height, S34 *pFigure);

	void Dispose(VerList *pVertex);

	void Clean(AreaList *&pArea);
	void Clean(S34 *&p34);
	void Close(AreaList *pArea);

	void NewVertex(AreaList *pArea, float x, float y);
	AreaList *NewVoid(AreaList *pArea);


// Overrides
	//{{AFX_VIRTUAL(CMeshView)
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
	virtual ~CMeshView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	AreaList *m_pArea;
	AreaList *m_pCurrentOutline;
	VerList *m_pLastVertex;

	S34 *m_pMesh;

	RegimeEnum m_Regime;
	CClientDC *m_pDC;
	CPoint StartPointL, EndPointL;
	BOOL m_bStartPointSpecified;
	BOOL m_bDrawMesh;
	BOOL m_bAreaClosed;
	CPen m_BluePen, m_RedPen, m_GreenPen;
	CBrush m_YellowBrush;

// Generated message map functions
protected:
	//{{AFX_MSG(CMeshView)
	afx_msg void OnAreaNewOutline();
	afx_msg void OnAreaGenerateMesh();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAreaVoid();
	afx_msg void OnUpdateAreaVoid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAreaOutline(CCmdUI *pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewMesh();
	afx_msg void OnUpdateViewMesh(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MeshView.cpp
inline CMeshDoc* CMeshView::GetDocument()
   { return (CMeshDoc *)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHVIEW_H__DA9C25FE_7792_4C6C_BA3C_A80C971EA40A__INCLUDED_)
