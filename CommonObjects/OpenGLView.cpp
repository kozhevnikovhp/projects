/* OpenGLView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	05 Nov 2008 - COpenGLStatic class

*/

#include "stdafx.h"

#include "OpenGLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenGLStatic

COpenGLStatic::COpenGLStatic()
{
	m_pDefaultRenderer = NULL;
	m_pDC = NULL;
}

COpenGLStatic::~COpenGLStatic()
{
}

void COpenGLStatic::Setup()
{
	if (m_pDC)
		return; // initialized already
	CreateRenderer();
	m_pDC = new CClientDC(this);
	HDC hDC = m_pDC->GetSafeHdc();
	GetRenderer()->Init(hDC);
	CRect rect; GetClientRect(&rect);
	GetRenderer()->OnSize(rect.Width(), rect.Height());
}

BEGIN_MESSAGE_MAP(COpenGLStatic, C3DStatic)
	//{{AFX_MSG_MAP(COpenGLStatic)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
void COpenGLStatic::CreateRenderer()
{
	m_pDefaultRenderer = new COpenGL_3D();
}

//virtual
void COpenGLStatic::DestroyRenderer()
{
	if (m_pDefaultRenderer)
		delete m_pDefaultRenderer;
	m_pDefaultRenderer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLStatic message handlers


BOOL COpenGLStatic::PreCreateWindow(CREATESTRUCT &cs) 
{
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return C3DStatic::PreCreateWindow(cs);
}

int COpenGLStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (C3DStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void COpenGLStatic::OnDestroy() 
{
	delete m_pDC;
	C3DStatic::OnDestroy();
}

BOOL COpenGLStatic::OnEraseBkgnd(CDC *pDC) 
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, C3DView)

BEGIN_MESSAGE_MAP(COpenGLView, C3DView)
	//{{AFX_MSG_MAP(COpenGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, C3DView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, C3DView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, C3DView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLView construction/destruction

COpenGLView::COpenGLView()
{
	m_pDC = NULL;
	m_pDefaultRenderer = NULL;
}

COpenGLView::~COpenGLView()
{
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return C3DView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView drawing

void COpenGLView::OnDraw(CDC *pDC)
{
}

//virtual
void COpenGLView::CreateRenderer()
{
	m_pDefaultRenderer = new COpenGL_3D();
}

//virtual
void COpenGLView::DestroyRenderer()
{
	if (m_pDefaultRenderer)
		delete m_pDefaultRenderer;
	m_pDefaultRenderer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView printing

BOOL COpenGLView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void COpenGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void COpenGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView diagnostics

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	C3DView::AssertValid();
}

void COpenGLView::Dump(CDumpContext &dc) const
{
	C3DView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLView message handlers

int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (C3DView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pDC = new CClientDC(this);
	HDC hDC = m_pDC->GetSafeHdc();
	if (GetRenderer()->Init(hDC) != LOGICAL_TRUE)
		return -1;

	return 0;
}

void COpenGLView::OnDestroy() 
{
	delete m_pDC;
	C3DView::OnDestroy();
}

BOOL COpenGLView::OnEraseBkgnd(CDC *pDC) 
{
	return TRUE;
}



