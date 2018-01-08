/* 3DView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	03 0ct 2008 - cursors
	05 Nov 2008 - C3DStatic class

*/


#include "stdafx.h"
#include "3DView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static SCursorDesc g_CursorDescs[] = {
	{ IDC_ARROW, NULL },
	{ IDC_IBEAM, NULL },
	{ IDC_WAIT, NULL },
	{ IDC_CROSS, NULL },
	{ IDC_UPARROW, NULL },
	{ IDC_SIZEALL, NULL },
	{ IDC_SIZENWSE, NULL },
	{ IDC_SIZENESW, NULL },
	{ IDC_SIZEWE, NULL },
	{ IDC_SIZENS, NULL }
};

/////////////////////////////////////////////////////////////////////////////
// C3DStatic

C3DStatic::C3DStatic()
{
}

C3DStatic::~C3DStatic()
{
}

void C3DStatic::ChangeCursor()
{
	CursorTypeEnum NewCursor = GetRenderer()->GetCursor();
	//if (NewCursor == m_OldCursor)
	//	return; // do nothing
	if (!g_CursorDescs[NewCursor].hCursor)
	{
		g_CursorDescs[NewCursor].hCursor = AfxGetApp()->LoadStandardCursor(g_CursorDescs[NewCursor].pszCursorName);
	}
	SetCursor(g_CursorDescs[NewCursor].hCursor);
	m_OldCursor = NewCursor;
}


BEGIN_MESSAGE_MAP(C3DStatic, CStatic)
	//{{AFX_MSG_MAP(C3DStatic)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DStatic message handlers

int C3DStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CreateRenderer();
	m_OldCursor = GetRenderer()->GetCursor();
	
	return 0;
}

void C3DStatic::OnDestroy() 
{
	DestroyRenderer();	
	CStatic::OnDestroy();
}

void C3DStatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (GetRenderer())
		GetRenderer()->OnKeyDown(nChar, nRepCnt, nFlags); 
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}

void C3DStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnLButtonDown(nFlags, point.x, point.y);
		ChangeCursor();
	}
	SetCapture();
	CStatic::OnLButtonDown(nFlags, point);
}

void C3DStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnLButtonUp(nFlags, point.x, point.y);
		ChangeCursor();
	}
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

void C3DStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnMouseMove(nFlags, point.x, point.y);
		ChangeCursor();
	}
	CStatic::OnMouseMove(nFlags, point);
}

void C3DStatic::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnRButtonDown(nFlags, point.x, point.y);
		ChangeCursor();
	}
	SetCapture();
	CStatic::OnRButtonDown(nFlags, point);
}

void C3DStatic::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnRButtonUp(nFlags, point.x, point.y);
		ChangeCursor();
	}
	ReleaseCapture();
	CStatic::OnRButtonUp(nFlags, point);
}

void C3DStatic::OnSize(UINT nType, int cx, int cy) 
{
	if (GetRenderer())
		GetRenderer()->OnSize(cx,cy);
	CStatic::OnSize(nType, cx, cy);
}

void C3DStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (GetRenderer())
		GetRenderer()->Redraw();	
}

/////////////////////////////////////////////////////////////////////////////
// C3DView

IMPLEMENT_DYNCREATE(C3DView, CView)

C3DView::C3DView()
{
}

C3DView::~C3DView()
{
}

BEGIN_MESSAGE_MAP(C3DView, CView)
	//{{AFX_MSG_MAP(C3DView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DView drawing

void C3DView::OnDraw(CDC *pDC)
{
}


/////////////////////////////////////////////////////////////////////////////
// C3DView diagnostics

#ifdef _DEBUG
void C3DView::AssertValid() const
{
	CView::AssertValid();
}

void C3DView::Dump(CDumpContext &dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

void C3DView::ChangeCursor()
{
	CursorTypeEnum NewCursor = GetRenderer()->GetCursor();
	//if (NewCursor == m_OldCursor)
	//	return; // do nothing
	if (!g_CursorDescs[NewCursor].hCursor)
	{
		g_CursorDescs[NewCursor].hCursor = AfxGetApp()->LoadStandardCursor(g_CursorDescs[NewCursor].pszCursorName);
	}
	SetCursor(g_CursorDescs[NewCursor].hCursor);
	m_OldCursor = NewCursor;
}

/////////////////////////////////////////////////////////////////////////////
// C3DView message handlers

int C3DView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CreateRenderer();
	m_OldCursor = GetRenderer()->GetCursor();
	
	return 0;
}

void C3DView::OnDestroy() 
{
	DestroyRenderer();	
	CView::OnDestroy();
}

void C3DView::OnSize(UINT nType, int cx, int cy) 
{
	if (GetRenderer())
		GetRenderer()->OnSize(cx,cy);
	CView::OnSize(nType, cx, cy);
}

void C3DView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnLButtonDown(nFlags, point.x, point.y);
		ChangeCursor();
	}
	SetCapture();
	CView::OnLButtonDown(nFlags, point);
}

void C3DView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnMouseMove(nFlags, point.x, point.y);
		ChangeCursor();
	}
	CView::OnMouseMove(nFlags, point);
}

void C3DView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnLButtonUp(nFlags, point.x, point.y);
		ChangeCursor();
	}
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

void C3DView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnRButtonDown(nFlags, point.x, point.y);
		ChangeCursor();
	}
	SetCapture();
	CView::OnRButtonDown(nFlags, point);
}

void C3DView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
	{
		GetRenderer()->OnRButtonUp(nFlags, point.x, point.y);
		ChangeCursor();
	}
	ReleaseCapture();
	CView::OnRButtonUp(nFlags, point);
}

void C3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (GetRenderer())
		GetRenderer()->OnKeyDown(nChar, nRepCnt, nFlags); 
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void C3DView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (GetRenderer())
		GetRenderer()->Redraw();	
}


