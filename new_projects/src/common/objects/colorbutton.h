/* ColorButton.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	08 Jul 2008 - initial creation

*/

#ifndef __COLOR_BUTTON_H_INCLUDED__
#define __COLOR_BUTTON_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Portable.h"

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
// Construction/destruction
public:
	CColorButton();
	virtual ~CColorButton();

// Public methods
public:
	void SetColor(COLORREF color);
	void ChangeColor(SRGBA *pRGBA);

// Overrides
	//{{AFX_VIRTUAL(CColorButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Protected methods
protected:
	void FreeBrush();
	
// Protected members
protected:
	HBRUSH m_hBrush;
	COLORREF m_Color;

protected:
	//{{AFX_MSG(CColorButton)
	afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __COLOR_BUTTON_H_INCLUDED__
