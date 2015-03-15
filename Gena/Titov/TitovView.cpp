// TitovView.cpp : implementation of the CTitovView class
//

#include "stdafx.h"
#include "Titov.h"

#include "TitovDoc.h"
#include "TitovView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitovView

IMPLEMENT_DYNCREATE(CTitovView, CView)

BEGIN_MESSAGE_MAP(CTitovView, CView)
	//{{AFX_MSG_MAP(CTitovView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitovView construction/destruction

CTitovView::CTitovView()
{
}

CTitovView::~CTitovView()
{
}

BOOL CTitovView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTitovView drawing

void CTitovView::OnDraw(CDC* pDC)
{
	CRect rect(0,0,0,0);
	GetClientRect(rect);
	CTitovApp *pApp = (CTitovApp *)AfxGetApp();
	pApp->Draw(pDC, rect);
}

/////////////////////////////////////////////////////////////////////////////
// CTitovView printing

BOOL CTitovView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTitovView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTitovView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTitovView diagnostics

#ifdef _DEBUG
void CTitovView::AssertValid() const
{
	CView::AssertValid();
}

void CTitovView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTitovDoc* CTitovView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTitovDoc)));
	return (CTitovDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTitovView message handlers

