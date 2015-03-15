/* GranSim2DView.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	10 Oct 2008 - reflecting wall creation functionality
	11 Oct 2008 - linear/point particle source creation functionality
	13 Oct 2008 - object selection changed
*/

#ifndef __GRANSIM2DVIEW_H_INCLUDED__
#define __GRANSIM2DVIEW_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"
///#include "GranSim2DDesign.h" //????
#include "GranSim2DRenderer.h"

// Forward declarations
//class CGranSim2DRenderer;

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DView view

class CGranSim2DView : public COpenGLView
{
protected:
	CGranSim2DView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGranSim2DView)

// Operations
public:
	char *GetDisplayedText();
	CGranSim2DRenderer *GetDesignRenderer() { return m_pRenderer; }
	
	// Public overrides
public:
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

	//{{AFX_VIRTUAL(CGranSim2DView)
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGranSim2DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CGranSim2DRenderer *m_pRenderer;

protected:
	//{{AFX_MSG(CGranSim2DView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __GRANSIM2DVIEW_H_INCLUDED__
