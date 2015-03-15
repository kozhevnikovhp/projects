// ParticlesInBoxView.cpp : implementation of the CParticlesInBoxView class
//

#include "stdafx.h"
#include "ParticlesInBox.h"

#include "ParticlesInBoxDoc.h"
#include "ParticlesInBoxView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxView

IMPLEMENT_DYNCREATE(CParticlesInBoxView, COpenGLView)

BEGIN_MESSAGE_MAP(CParticlesInBoxView, COpenGLView)
	//{{AFX_MSG_MAP(CParticlesInBoxView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, COpenGLView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxView construction/destruction

CParticlesInBoxView::CParticlesInBoxView()
{
	// TODO: add construction code here

}

CParticlesInBoxView::~CParticlesInBoxView()
{
}

BOOL CParticlesInBoxView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COpenGLView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxView drawing

void CParticlesInBoxView::OnDraw(CDC *pDC)
{
	CParticlesInBoxDoc* pDoc = GetDocument();
}

//virtual
void CParticlesInBoxView::CreateRenderer()
{
	CWorld *pWorld = GetDocument()->GetWorld();
	m_pWorldRenderer = new CWorldRenderer();
	m_pWorldRenderer->SetWorld(pWorld);
	pWorld->SetRenderer(m_pWorldRenderer);
}

//virtual
void CParticlesInBoxView::DestroyRenderer()
{
	if (m_pWorldRenderer)
		delete m_pWorldRenderer;
	m_pWorldRenderer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxView printing

BOOL CParticlesInBoxView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CParticlesInBoxView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CParticlesInBoxView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxView diagnostics

#ifdef _DEBUG
void CParticlesInBoxView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CParticlesInBoxView::Dump(CDumpContext& dc) const
{
	COpenGLView::Dump(dc);
}

CParticlesInBoxDoc* CParticlesInBoxView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CParticlesInBoxDoc)));
	return (CParticlesInBoxDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxView message handlers
