// BmpView.cpp : implementation file
//

#include "stdafx.h"
#include "BmpView.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapView

IMPLEMENT_DYNCREATE(CBitmapView, CScrollView)

CBitmapView::CBitmapView()
{
	m_ZoomPercent=100;
	m_Origin=CPoint(0, 0);
	FileName="Нет_изображения";
}

CBitmapView::~CBitmapView()
{
}

BEGIN_MESSAGE_MAP(CBitmapView, CScrollView)
	//{{AFX_MSG_MAP(CBitmapView)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBitmapView::InitDIB()
{
	m_Bmp.InitDIB();
	UpdateScrollSizes();
	DoRealizePalette();
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapView drawing

void CBitmapView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	m_TotalSize.cx = m_TotalSize.cy = 10;
	SetScrollSizes(MM_TEXT, m_TotalSize);
}

void CBitmapView::OnDraw(CDC* pDC)
{
//	pDC->SetMapMode(MM_HIMETRIC);
	if (m_Bmp.IsImage())
	{
		int cxDIB = (int)GetImageWidth();         // Size of DIB - x
		int cyDIB = (int)GetImageHeight();        // Size of DIB - y
	/*	CRect ClientRect;
		GetClientRect(&ClientRect);*/
		CRect rcDIB(0,0,cxDIB, cyDIB);//=ClientToBitmap(ClientRect);
	/*	rcDIB.top = 0;
		rcDIB.left = 0;
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;*/
		CRect rcDest;//= ClientRect+m_Origin+GetScrollPosition();//rcDIB;
		rcDest.top=rcDest.left=0;
		rcDest.right=cxDIB*m_ZoomPercent/100;
		rcDest.bottom = cyDIB*m_ZoomPercent/100;
		rcDest+=m_Origin;
//		PaintBits();		
		m_Bmp.PaintDIB(pDC, &rcDest, &rcDIB);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapView diagnostics

#ifdef _DEBUG
void CBitmapView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBitmapView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBitmapView message handlers

void CBitmapView::OnDestroy() 
{
	Free();
	CScrollView::OnDestroy();
}

BOOL CBitmapView::LoadFromFile(CString FileFullPath)
{
	if (!m_Bmp.LoadFromFile(FileFullPath)) return FALSE;
	UpdateScrollSizes();
	Invalidate();
	return TRUE;
}

void CBitmapView::DoRealizePalette()
{
	CPalette* pPal = m_Bmp.GetPalette();//m_palDIB;
	if (pPal != NULL)
	{
		CFrameWnd *pAppFrame = (CFrameWnd *) AfxGetApp()->m_pMainWnd;
		//ASSERT_KINDOF(CMainFrame, pAppFrame);

		CClientDC appDC(pAppFrame);
		// All views but one should be a background palette.
		// wParam contains a handle to the active view, so the SelectPalette
		// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
		CPalette* oldPalette = appDC.SelectPalette(pPal, TRUE);
		if (oldPalette != NULL)
		{
			UINT nColorsChanged = appDC.RealizePalette();
			if (nColorsChanged > 0)
				GetDocument()->UpdateAllViews(NULL);
			appDC.SelectPalette(oldPalette, TRUE);
		}
		else
		{
			TRACE0("\tSelectPalette failed in CBitmapView::OnPaletteChanged\n");
		}
	}
}

BOOL CBitmapView::Free()
{
	m_Bmp.Free();
	m_TotalSize=CSize(1,1);
	SetScrollSizes(MM_TEXT, m_TotalSize);
	FileName="Нет_изображения";
	Invalidate();
	UpdateWindow();
	return TRUE;
}

BOOL CBitmapView::SetZoom(int ZoomPercent, BOOL bRedraw)
{
	m_ZoomPercent=ZoomPercent;
	if (m_ZoomPercent<=0) m_ZoomPercent=100;
	m_TotalSize = CSize(GetImageWidth()*m_ZoomPercent/100,
		GetImageHeight()*m_ZoomPercent/100)+(CSize)m_Origin;
	SetScrollSizes(MM_TEXT, m_TotalSize);
	if (bRedraw) {	Invalidate();	UpdateWindow(); }
	return TRUE;
}

BOOL CBitmapView::SetOrigin(CPoint Origin, BOOL bRedraw)
{
	m_Origin=Origin;
	if (bRedraw) {	Invalidate();	UpdateWindow(); }
	return TRUE;
}

CPoint CBitmapView::GetMousePlacement()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	return ClientToBitmap(point);
}

CPoint CBitmapView::ClientToBitmap(CPoint ClientPoint)
{
	CPoint ScrollPoint=GetScrollPosition();
	CPoint RetPoint=ClientPoint;
	RetPoint.x=(int)((RetPoint.x+ScrollPoint.x)*100./m_ZoomPercent)-m_Origin.x;
	RetPoint.y=(int)((RetPoint.y+ScrollPoint.y)*100./m_ZoomPercent)-m_Origin.y;
	return RetPoint;
}

CRect CBitmapView::ClientToBitmap(CRect ClientRect)
{
	CPoint ScrollPoint=GetScrollPosition();
	CRect RetRect=ClientRect;
	RetRect.top=(int)((RetRect.top+ScrollPoint.y)*100./m_ZoomPercent)-m_Origin.y;
	RetRect.bottom=(int)((RetRect.bottom+ScrollPoint.y)*100./m_ZoomPercent)-m_Origin.y;
	RetRect.left=(int)((RetRect.left+ScrollPoint.x)*100./m_ZoomPercent)-m_Origin.x;
	RetRect.right=(int)((RetRect.right+ScrollPoint.x)*100./m_ZoomPercent)-m_Origin.x;
	return RetRect;
}

BOOL CBitmapView::SetImage(HBITMAP hBmp)
{
	m_Bmp.SetImage(hBmp);
	UpdateScrollSizes();
	DoRealizePalette();
	return TRUE;
}

BOOL CBitmapView::SetImage(CBmp &Bmp)
{ 
	BOOL ret=m_Bmp.SetImage(Bmp);
	Invalidate();
	UpdateScrollSizes();
	DoRealizePalette();
	return ret;
}

void CBitmapView::UpdateScrollSizes()
{
	m_TotalSize = CSize(GetImageWidth()*m_ZoomPercent/100+10, 
		GetImageHeight()*m_ZoomPercent/100+10)+(CSize)m_Origin;
	SetScrollSizes(MM_TEXT, m_TotalSize);
}

