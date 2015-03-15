// OpenGLChartCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "networkanalyzer.h"
#include "OpenGLChartCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenGLStaticCtrl

COpenGLStaticCtrl::COpenGLStaticCtrl()
{
	m_pDC  = NULL;
}

COpenGLStaticCtrl::~COpenGLStaticCtrl()
{
}


//virtual
void COpenGLStaticCtrl::CreateRenderer()
{
	m_pDefaultRenderer = new COpenGL_2D(m_hWnd);
}

//virtual
void COpenGLStaticCtrl::DestroyRenderer()
{
	if (m_pDefaultRenderer)
		delete m_pDefaultRenderer;
	m_pDefaultRenderer = NULL;
}

BEGIN_MESSAGE_MAP(COpenGLStaticCtrl, CStatic)
	//{{AFX_MSG_MAP(COpenGLStaticCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLChartCtrl message handlers

BOOL COpenGLStaticCtrl::PreCreateWindow(CREATESTRUCT &cs) 
{
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CStatic::PreCreateWindow(cs);
}

int COpenGLStaticCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void COpenGLStaticCtrl::OnDestroy() 
{
	delete m_pDC;
	DestroyRenderer();
	CStatic::OnDestroy();
}

BOOL COpenGLStaticCtrl::OnEraseBkgnd(CDC *pDC) 
{
	return TRUE;
}

BOOL COpenGLStaticCtrl::Setup()
{
	if (m_pDC)
		return TRUE;
	m_pDC = new CClientDC(this);
	HDC hDC = m_pDC->GetSafeHdc();
	CreateRenderer();
	GetRenderer()->Init(hDC);
	CRect rect; GetClientRect(&rect);
	GetRenderer()->OnSize(rect.Width(), rect.Height());
	return TRUE;
}

void COpenGLStaticCtrl::OnPaint() 
{
//	CPaintDC dc(this); // device context for painting
	
	if (GetRenderer())
		GetRenderer()->Redraw();	
}

void COpenGLStaticCtrl::OnSize(UINT nType, int cx, int cy) 
{
	if (GetRenderer())
		GetRenderer()->OnSize(cx,cy);
	CStatic::OnSize(nType, cx, cy);
}

void COpenGLStaticCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
		GetRenderer()->OnLButtonDown(nFlags, point.x, point.y);
	SetCapture();
	CStatic::OnLButtonDown(nFlags, point);
}

void COpenGLStaticCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
		GetRenderer()->OnLButtonUp(nFlags, point.x, point.y);
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

void COpenGLStaticCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetRenderer())
		GetRenderer()->OnMouseMove(nFlags, point.x, point.y);
	CStatic::OnMouseMove(nFlags, point);
}
