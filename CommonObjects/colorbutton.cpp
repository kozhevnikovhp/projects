/* ColorButton.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	08 Jul 2008 - initial creation

*/

#include "stdafx.h"
#include "colorbutton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
	m_hBrush = NULL;
}

CColorButton::~CColorButton()
{
	FreeBrush();
}

void CColorButton::SetColor(COLORREF color)
{
	FreeBrush();
	m_Color = color;
	m_hBrush = CreateSolidBrush(color);
	Invalidate();
	UpdateWindow();
}

void CColorButton::ChangeColor(SRGBA *pRGBA)
{
	CColorDialog dlg(RGBAtoCOLORREF(pRGBA), CC_RGBINIT, this);
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		pRGBA->Init(GetRValue(color), GetGValue(color), GetBValue(color));
		SetColor(color);
	}
}

void CColorButton::FreeBrush()
{
	if (m_hBrush)
		DeleteObject(m_hBrush);
	m_hBrush = NULL;
}

BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

HBRUSH CColorButton::CtlColor(CDC *pDC, UINT nCtlColor) 
{
	//pDC->SetTextColor(RGB(0,200,0));
//	pDC->SetBkColor(m_Color);    // text bkgnd
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return m_hBrush;
}

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
}
