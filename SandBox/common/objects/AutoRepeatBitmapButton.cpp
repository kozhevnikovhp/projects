// AutoRepeatBitmapButton.cpp : implementation file
//

#include "stdafx.h"
#include "AutoRepeatBitmapButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoRepeatBitmapButton

CAutoRepeatBitmapButton::CAutoRepeatBitmapButton()
{
	m_bRepeatEnabled=TRUE;
	m_RepeatDelay=100;
	m_pfnAutoRepeatMethod=NULL;
}

CAutoRepeatBitmapButton::~CAutoRepeatBitmapButton()
{
}

BEGIN_MESSAGE_MAP(CAutoRepeatBitmapButton, CBitmapButton)
	//{{AFX_MSG_MAP(CAutoRepeatBitmapButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoRepeatBitmapButton message handlers

void CAutoRepeatBitmapButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CBitmapButton::OnLButtonDown(nFlags, point);
	if (!m_pfnAutoRepeatMethod) return;
	if (!m_bRepeatEnabled) return;
	m_TimerID = SetTimer(1, m_RepeatDelay, NULL);
}

void CAutoRepeatBitmapButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	KillTimer(m_TimerID);	
	CBitmapButton::OnLButtonUp(nFlags, point);
}

void CAutoRepeatBitmapButton::OnTimer(UINT nIDEvent) 
{
	(*m_pfnAutoRepeatMethod)(GetParent());
	CBitmapButton::OnTimer(nIDEvent);
}
