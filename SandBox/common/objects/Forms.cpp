// Forms.cpp : implementation file
//

#include "stdafx.h"
#include "Forms.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoodFormView

IMPLEMENT_DYNCREATE(CGoodFormView, CFormView)

CGoodFormView::CGoodFormView(UINT ID) : CFormView(ID)
{
	//{{AFX_DATA_INIT(CGoodFormView)
	//}}AFX_DATA_INIT
}

CGoodFormView::~CGoodFormView()
{}

void CGoodFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoodFormView)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGoodFormView, CFormView)
	//{{AFX_MSG_MAP(CGoodFormView)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, NeedToolTipsHandler)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoodFormView diagnostics

#ifdef _DEBUG
void CGoodFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGoodFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGoodFormView message handlers

void CGoodFormView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar)
	{
		int MinPos, MaxPos;
		pScrollBar->GetScrollRange(&MinPos, &MaxPos);
		int i=pScrollBar->GetScrollPos();

		switch ( nSBCode )
		{
			case SB_PAGEDOWN : i+=10; break;
			case SB_LINEDOWN : i++; break;
			case SB_PAGEUP :  i-=10; break;
			case SB_LINEUP : i--; break;
			case SB_BOTTOM : i=MaxPos; break;
			case SB_TOP : i=MinPos; break;
			case SB_THUMBPOSITION : 
			case SB_THUMBTRACK : i=nPos;
		}
		pScrollBar->SetScrollPos(i);
		UpdateTexts();
	}
	else
		CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CGoodFormView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar)
	{
		int MinPos, MaxPos;
		pScrollBar->GetScrollRange(&MinPos, &MaxPos);
		int i=pScrollBar->GetScrollPos();

		switch ( nSBCode )
		{
			case SB_PAGEDOWN : i+=10; break;
			case SB_LINEDOWN : i++; break;
			case SB_PAGEUP :  i-=10; break;
			case SB_LINEUP : i--; break;
			case SB_BOTTOM : i=MaxPos; break;
			case SB_TOP : i=MinPos; break;
			case SB_THUMBPOSITION : 
			case SB_THUMBTRACK : i=nPos;
		}
		pScrollBar->SetScrollPos(i);
		UpdateTexts();
	}
	else
		CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CGoodFormView::NeedToolTipsHandler(UINT id, NMHDR *pTTTStruct, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID =pTTTStruct->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		nID=::GetDlgCtrlID((HWND)nID);
		pTTT->lpszText=MAKEINTRESOURCE(nID);
		pTTT->hinst = AfxGetResourceHandle();
		return(TRUE);
	}
	return(FALSE);
}

void CGoodFormView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	EnableToolTips();	
}
