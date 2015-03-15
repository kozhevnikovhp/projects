// LithoView.cpp : implementation of the CLithoView class
//

#include "stdafx.h"
#include "Litho.h"

#include "LithoDoc.h"
#include "LithoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLithoView

IMPLEMENT_DYNCREATE(CLithoView, COpenGLView)

BEGIN_MESSAGE_MAP(CLithoView, COpenGLView)
	//{{AFX_MSG_MAP(CLithoView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLithoView construction/destruction

CLithoView::CLithoView()
{
	// TODO: add construction code here

}

CLithoView::~CLithoView()
{
}

BOOL CLithoView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COpenGLView::PreCreateWindow(cs);
}

//virtual
void CLithoView::CreateRenderer()
{
	m_pRenderer = new CLithoRenderer();
}

//virtual
void CLithoView::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CLithoView drawing

void CLithoView::OnDraw(CDC *pDC)
{
	CLithoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CLithoView printing

BOOL CLithoView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLithoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLithoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CLithoView diagnostics

#ifdef _DEBUG
void CLithoView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CLithoView::Dump(CDumpContext &dc) const
{
	COpenGLView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLithoView message handlers

char *CLithoView::GetDisplayedText()
{ 
	CPoint ScreenPoint;
	GetCursorPos(&ScreenPoint);
	ScreenToClient(&ScreenPoint);

	return m_pRenderer->GetDisplayedText(ScreenPoint.x, ScreenPoint.y);
}

