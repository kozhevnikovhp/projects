/* GranSim2DView.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	10 Oct 2008 - reflecting wall creation functionality
	11 Oct 2008 - linear/point particle source creation functionality
	13 Oct 2008 - object selection changed
*/

#include "stdafx.h"
#include "gransim2d.h"
#include "GranSim2DView.h"
#include "GranSim2DDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DView

IMPLEMENT_DYNCREATE(CGranSim2DView, COpenGLView)

CGranSim2DView::CGranSim2DView()
{
}

CGranSim2DView::~CGranSim2DView()
{
}

char *CGranSim2DView::GetDisplayedText()
{
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CPoint ScreenPoint;
	GetCursorPos(&ScreenPoint);
	ScreenToClient(&ScreenPoint);

	return m_pRenderer->GetDisplayedText(ScreenPoint.x, ClientRect.Height()-ScreenPoint.y);
}

//virtual
void CGranSim2DView::CreateRenderer()
{
	CGranSim2DDoc *pDoc = (CGranSim2DDoc *)GetDocument();
	m_pRenderer = new CGranSim2DRenderer();
	pDoc->SetRenderer(m_pRenderer);
	m_pRenderer->SetDesign(pDoc->GetDesign());
}

//virtual
void CGranSim2DView::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}


BEGIN_MESSAGE_MAP(CGranSim2DView, COpenGLView)
	//{{AFX_MSG_MAP(CGranSim2DView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DView drawing

void CGranSim2DView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DView diagnostics

#ifdef _DEBUG
void CGranSim2DView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CGranSim2DView::Dump(CDumpContext &dc) const
{
	COpenGLView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DView message handlers
