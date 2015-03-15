// GLTestView.cpp : implementation of the CGLTestView class
//

#include "stdafx.h"
#include "GLTest.h"

#include "GLTestDoc.h"
#include "GLTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGLTestRenderer::CGLTestRenderer()
{
	SetZoomRectangleMode();
}

//virtual
CGLTestRenderer::~CGLTestRenderer()
{
}

//virtual
void CGLTestRenderer::OnRenderScene()
{
	TRACE("Calling list %d\n", m_WorkingDisplayList);
	glCallList(m_WorkingDisplayList);
}

void CGLTestRenderer::RebuildDisplayList()
{
	MakeGLContextCurrent();
	TRACE("Creation list %d\n", m_SpareDisplayList);
	glNewList(m_SpareDisplayList, GL_COMPILE);
		COpenGL_2D::OnRenderScene();
	glEndList();
	// Swap working and spare lists
	int dummy = m_SpareDisplayList;
	m_SpareDisplayList = m_WorkingDisplayList;
	m_WorkingDisplayList = dummy;
	MakeGLContextNotCurrent();
}

//virtual
void CGLTestRenderer::OnInitialized()
{
	COpenGL_2D::OnInitialized();
//	m_VectorFontListBaseID = CreateVectorFont("Arial Cyr", -28, m_GlyphMetrics);

	m_SpareDisplayList = glGenLists(1);
	m_WorkingDisplayList = glGenLists(1);
	RebuildDisplayList();
}


/////////////////////////////////////////////////////////////////////////////
// CGLTestStatic

CGLTestStatic::CGLTestStatic()
{
	int a = 1;
}

CGLTestStatic::~CGLTestStatic()
{
}

//virtual
void CGLTestStatic::CreateRenderer()
{
	m_pRenderer = new CGLTestRenderer();
}

//virtual
void CGLTestStatic::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}


BEGIN_MESSAGE_MAP(CGLTestStatic, COpenGLStatic)
	//{{AFX_MSG_MAP(CGLTestStatic)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLTestStatic message handlers


/////////////////////////////////////////////////////////////////////////////
// CGLTestView

IMPLEMENT_DYNCREATE(CGLTestView, COpenGLView)

BEGIN_MESSAGE_MAP(CGLTestView, COpenGLView)
	//{{AFX_MSG_MAP(CGLTestView)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, COpenGLView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLTestView construction/destruction

CGLTestView::CGLTestView()
{
	m_pRenderer = NULL;
}

CGLTestView::~CGLTestView()
{
}

//virtual
void CGLTestView::CreateRenderer()
{
	m_pRenderer = new CGLTestRenderer();
}

//virtual
void CGLTestView::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}

BOOL CGLTestView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COpenGLView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGLTestView drawing

void CGLTestView::OnDraw(CDC *pDC)
{
	CGLTestDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CGLTestView printing

BOOL CGLTestView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGLTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGLTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGLTestView diagnostics

#ifdef _DEBUG
void CGLTestView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CGLTestView::Dump(CDumpContext& dc) const
{
	COpenGLView::Dump(dc);
}

CGLTestDoc* CGLTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLTestDoc)));
	return (CGLTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGLTestView message handlers

int CGLTestView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COpenGLView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetTimer(1, 5000, NULL);	
	return 0;
}

void CGLTestView::OnTimer(UINT nIDEvent) 
{
	m_pRenderer->RebuildDisplayList();
	COpenGLView::OnTimer(nIDEvent);
	SendMessage(WM_PAINT, 0, 0);
}

