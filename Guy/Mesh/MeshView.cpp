// MeshView.cpp : implementation of the CMeshView class
//

#include "stdafx.h"
#include "Mesh.h"

#include "MeshDoc.h"
#include "MeshView.h"
#include <math.h>
#include "..\Poly2rt\common_def.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ID_ALLAREAS *id_AreaList = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMeshView

IMPLEMENT_DYNCREATE(CMeshView, CView)

BEGIN_MESSAGE_MAP(CMeshView, CView)
	//{{AFX_MSG_MAP(CMeshView)
	ON_COMMAND(ID_AREA_NEWOUTLINE, OnAreaNewOutline)
	ON_COMMAND(ID_AREA_GENERATAMESH, OnAreaGenerateMesh)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_AREA_VOID, OnAreaVoid)
	ON_UPDATE_COMMAND_UI(ID_AREA_VOID, OnUpdateAreaVoid)
	ON_UPDATE_COMMAND_UI(ID_AREA_NEWOUTLINE, OnUpdateAreaOutline)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_MESH, OnViewMesh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MESH, OnUpdateViewMesh)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshView construction/destruction

CMeshView::CMeshView()
{
	m_Regime = REGIME_NONE;
	m_pCurrentOutline = NULL;
	m_pLastVertex = NULL;
	m_pArea = NULL;
	m_pMesh = NULL;
	m_pDC = NULL;
	m_bStartPointSpecified = FALSE;
	m_bDrawMesh = TRUE;
	m_bAreaClosed = TRUE;
}

CMeshView::~CMeshView()
{
	Clean(m_pArea);

	// Clean mesh (figure list)
	Clean(m_pMesh);
	CleanSections(&g_Sections);
}

BOOL CMeshView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMeshView drawing

void CMeshView::OnDraw(CDC *pDC)
{
	CRect rect;
	GetClientRect(&rect);
	if (m_bDrawMesh)
	{
		pDC->SelectObject(m_GreenPen);
		S34 *p34 = m_pMesh;
		while (p34)
		{
			Draw34(pDC, rect.Height(), p34);
			p34 = p34->pNext;
		}

		if (g_Sections.pSections)
		{
			for (int i = 0; i < g_Sections.nSections; i++)
			{
				SSection *pSection = g_Sections.pSections+i;
				if (pSection->bAlive)
					pDC->SelectObject(m_RedPen);
				else
					pDC->SelectObject(m_BluePen);

				{
					pDC->MoveTo(pSection->x1, rect.Height()-pSection->y1);
					pDC->LineTo(pSection->x2, rect.Height()-pSection->y2);
				}
			}
		}
	}

	pDC->SelectStockObject(BLACK_PEN);
	DrawArea(pDC, rect.Height(), m_pArea);
}

/////////////////////////////////////////////////////////////////////////////
// CMeshView printing

BOOL CMeshView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMeshView::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMeshView::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMeshView diagnostics

#ifdef _DEBUG
void CMeshView::AssertValid() const
{
	CView::AssertValid();
}

void CMeshView::Dump(CDumpContext &dc) const
{
	CView::Dump(dc);
}

CMeshDoc* CMeshView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMeshDoc)));
	return (CMeshDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMeshView message handlers

void CMeshView::OnAreaNewOutline() 
{
	Clean(m_pArea);
	CleanSections(&g_Sections);
	Clean(m_pMesh);
	m_pArea = (AreaList *)malloc(sizeof(AreaList));
	memset(m_pArea, 0, sizeof(AreaList));
	m_pArea->iType = RS_SOLID;
	
	m_Regime = REGIME_AREA;
	m_bAreaClosed = FALSE;
	m_pCurrentOutline = m_pArea;
	Redraw();
}

void CMeshView::DrawArea(CDC *pDC, int height, AreaList *pArea)
{
	if (!pArea)
		return;
	int exclude = 0;
	if (!m_bAreaClosed && m_pCurrentOutline == pArea)
		exclude = 1;
	if (pArea->iNumVer && pArea->segver)
	{
		VerList *pVertex = pArea->segver;
		for (int i = 0; i < pArea->iNumVer-exclude; i++)
		{
			if (!pVertex)
				break;
			pDC->MoveTo(pVertex->fx1, height - pVertex->fy1);
			pDC->LineTo(pVertex->fx2, height - pVertex->fy2);
			pVertex = pVertex->next;
		}
	}
	if (!pArea->voids)
		return;
	VoidList *pVoid = pArea->voids;
	while (pVoid)
	{
		DrawArea(pDC, height, pVoid->voidarea);
		pVoid = pVoid->next;
	}
}

void CMeshView::Draw34(CDC *pDC, int height, S34 *p34)
{
	pDC->SelectObject(m_YellowBrush);
	POINT pts[4];
	switch (p34->type)
	{
	case TYPE_RECTANGLE:
		pDC->Rectangle(p34->left, height-p34->top, p34->right, height-p34->bottom);
		pDC->MoveTo(p34->left, height-p34->top);
		pDC->LineTo(p34->right, height-p34->top);
		pDC->LineTo(p34->right, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->top);
		break;
	case TYPE_NW:
		pts[0].x = p34->left; pts[0].y = height-p34->top;
		pts[1].x = p34->right; pts[1].y = height-p34->top;
		pts[2].x = p34->left; pts[2].y = height-p34->bottom;
		pDC->Polygon(pts, 3);
		pDC->MoveTo(p34->left, height-p34->top);
		pDC->LineTo(p34->right, height-p34->top);
		pDC->LineTo(p34->left, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->top);
		break;
	case TYPE_NE:
		pts[0].x = p34->right; pts[0].y = height-p34->top;
		pts[1].x = p34->right; pts[1].y = height-p34->bottom;
		pts[2].x = p34->left; pts[2].y = height-p34->top;
		pDC->Polygon(pts, 3);
		pDC->MoveTo(p34->right, height-p34->top);
		pDC->LineTo(p34->right, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->top);
		pDC->LineTo(p34->right, height-p34->top);
		break;
	case TYPE_SE:
		pts[0].x = p34->right; pts[0].y = height-p34->bottom;
		pts[1].x = p34->left; pts[1].y = height-p34->bottom;
		pts[2].x = p34->right; pts[2].y = height-p34->top;
		pDC->Polygon(pts, 3);
		pDC->MoveTo(p34->right, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->bottom);
		pDC->LineTo(p34->right, height-p34->top);
		pDC->LineTo(p34->right, height-p34->bottom);
		break;
	case TYPE_SW:
		pts[0].x = p34->left; pts[0].y = height-p34->bottom;
		pts[1].x = p34->left; pts[1].y = height-p34->top;
		pts[2].x = p34->right; pts[2].y = height-p34->bottom;
		pDC->Polygon(pts, 3);
		pDC->MoveTo(p34->left, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->top);
		pDC->LineTo(p34->right, height-p34->bottom);
		pDC->LineTo(p34->left, height-p34->bottom);
		break;
	}
}

void CMeshView::Dispose(VerList *pVertex)
{
	if (!pVertex)
		return;
	do
	{
		VerList *pNextVertex = pVertex->next;
		SI_FREE(pVertex, VerList*);
		pVertex = pNextVertex;
	} while (pVertex);
}

void CMeshView::Clean(AreaList *&pArea)
{
	if (!pArea)
		return;
	if (pArea->iNumVer && pArea->segver)
		Dispose(pArea->segver);

	if (pArea->voids)
	{
		VoidList *pVoid = pArea->voids;
		while (pVoid)
		{
			VoidList *pNextVoid = pVoid->next;
			Clean(pVoid->voidarea);
			SI_FREE(pVoid, VoidList*);
			pVoid = pNextVoid;
		}
	}

	SI_FREE(pArea, AreaList*);
	pArea = NULL;
}

void CMeshView::Clean(S34 *&pMesh)
{
	S34 *p34 = pMesh;
	while (p34)
	{
		S34 *pNext34 = p34->pNext;
		SI_FREE(p34, S34*);
		p34 = pNext34;
	}
	pMesh = NULL;
}

void CMeshView::Close(AreaList *pArea)
{
	if (pArea->iNumVer && pArea->segver && m_pLastVertex)
	{
		//NewVertex(pArea, pArea->segver->fx1, pArea->segver->fy1);
		m_pLastVertex->fx2 = pArea->segver->fx1;
		m_pLastVertex->fy2 = pArea->segver->fy1;
	}
	m_pLastVertex = NULL;
	m_bAreaClosed = TRUE;
}

void CMeshView::NewVertex(AreaList *pArea, float x, float y)
{
	++pArea->iNumVer;
	VerList *pNewVertex = (VerList*)malloc(sizeof(VerList));
	memset(pNewVertex, 0, sizeof(VerList));
	pNewVertex->fx1 = x;
	pNewVertex->fy1 = y;
	if (!pArea->segver)
		pArea->segver = pNewVertex;
	
	if (m_pLastVertex)
	{
		m_pLastVertex->fx2 = x;
		m_pLastVertex->fy2 = y;
		m_pLastVertex->next = pNewVertex;
	}
	m_pLastVertex = pNewVertex;
}

AreaList *CMeshView::NewVoid(AreaList *pArea)
{
	m_bAreaClosed = FALSE;
	AreaList *pNewArea = (AreaList *)malloc(sizeof(AreaList));
	memset(pNewArea, 0, sizeof(AreaList));
	pNewArea->iType = RS_VOID;

	m_pCurrentOutline = pNewArea;
	VoidList *pVoid = (VoidList *)malloc(sizeof(VoidList));
	memset(pVoid, 0, sizeof(VoidList));
	pVoid->voidarea = pNewArea;
	pVoid->next = pArea->voids;
	pArea->voids = pVoid;
	return pNewArea;
}

void CMeshView::OnAreaGenerateMesh() 
{
	Clean(m_pMesh);
	CleanSections(&g_Sections);
	m_pMesh = MeshArea(m_pArea, 50);
	Redraw();
}

void CMeshView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	Close(m_pCurrentOutline);
	Redraw();
	m_Regime = REGIME_NONE;
	m_bStartPointSpecified = FALSE;
	CView::OnRButtonDown(nFlags, point);
}

void CMeshView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pCurrentOutline)
	{
		CRect rect;
		GetClientRect(&rect);
		/*if (m_bSnapToGrid)
		{
			point.x = (point.x+GRID_SIZE/2)/GRID_SIZE*GRID_SIZE;
			point.y = (point.y+GRID_SIZE/2)/GRID_SIZE*GRID_SIZE;
		}*/
		float y = rect.Height()-(float)point.y;
		m_bStartPointSpecified = TRUE;
		StartPointL = point;
		NewVertex(m_pCurrentOutline, (float)point.x, y);
		Redraw();
	}
	CView::OnLButtonDown(nFlags, point);
}

void CMeshView::OnAreaVoid() 
{
	m_Regime = REGIME_CUTOUT;
	m_pCurrentOutline = NewVoid(m_pArea);
}

void CMeshView::OnUpdateAreaVoid(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_Regime == REGIME_CUTOUT);
}

void CMeshView::OnUpdateAreaOutline(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_Regime == REGIME_AREA);
}

int CMeshView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_pDC)
		m_pDC = new CClientDC(this);
	m_pDC->SetROP2(R2_NOTXORPEN);
	m_BluePen.CreatePen(PS_SOLID, 1, RGB(0,0,255));
	m_RedPen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
	m_GreenPen.CreatePen(PS_SOLID, 2, RGB(0,255,0));
	m_YellowBrush.CreateSolidBrush(RGB(255,255,0));
	
	return 0;
}

void CMeshView::OnDestroy()
{
	delete m_pDC;
	DeleteObject(m_RedPen);
	DeleteObject(m_BluePen);
	DeleteObject(m_GreenPen);
	DeleteObject(m_YellowBrush);
	CView::OnDestroy();
}

void CMeshView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bStartPointSpecified)
	{
		m_pDC->MoveTo(StartPointL.x, StartPointL.y);
		m_pDC->LineTo(EndPointL.x, EndPointL.y);
		m_pDC->MoveTo(StartPointL.x, StartPointL.y);
		m_pDC->LineTo(point.x, point.y);
	}
	EndPointL = point;
	CView::OnMouseMove(nFlags, point);
}

void CMeshView::OnViewMesh() 
{
	m_bDrawMesh = !m_bDrawMesh;	
	Redraw();
}

void CMeshView::OnUpdateViewMesh(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_bDrawMesh);	
}

void CMeshView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
}
