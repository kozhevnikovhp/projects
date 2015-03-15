// GranSim2D_TestView.h : interface of the CGranSim2D_TestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRANSIM2D_TESTVIEW_H__73D22455_6175_4F83_82DC_7987FE375B49__INCLUDED_)
#define AFX_GRANSIM2D_TESTVIEW_H__73D22455_6175_4F83_82DC_7987FE375B49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLView.h"
#include "..\GranSim2DDesign.h"

class CGranSim2D_TestDoc;

class CTestRenderer : public COpenGL_2D
{
// Construction/destruction
public:
	CTestRenderer();
	virtual ~CTestRenderer();

// Public overridable methods
public:

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return 10; }
	virtual double GetWorldSizeY() { return GetWorldSizeX(); }
	virtual double GetDefaultWorldCenterX() { return 0; }
	virtual double GetDefaultWorldCenterY() { return 0; }
//	virtual double GetWorldScaleX() { return 1; }
//	virtual double GetWorldScaleY() { return 1; }
	virtual void OnRenderScene();
	// Event handlers

// Public methods
public:
	void SetParticle1Pos();
	void SetParticle2Pos();
	void Check();
	char *GetDisplayedText(int x, int y);

// Protected methods
protected:
	void SetParticlePos(CParticle2D *pParticle);

// Private methods
private:

// Public members
public:

// Protected members
protected:
	CParticleType m_ParticleType1;
	CParticle2D m_Particle1;

	CParticleType m_ParticleType2;
	CParticle2D m_Particle2;
	
	LOGICAL m_bContactPointFound;
	SContact m_ParticleContact;

	CLine2D m_Line;
	LOGICAL m_bLineContactPointFound;
	SContact m_LineContact;
	CReflectingWall *m_pWall;

	char m_szText[1024];
	
// Private members
private:
};

class CGranSim2D_TestView : public COpenGLView
{
protected: // create from serialization only
	CGranSim2D_TestView();
	DECLARE_DYNCREATE(CGranSim2D_TestView)

// Attributes
public:
	CGranSim2D_TestDoc *GetDocument();
	virtual CRenderer *GetRenderer() { return m_pRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

// Operations
public:
	char *GetDisplayedText();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGranSim2D_TestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGranSim2D_TestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTestRenderer *m_pRenderer;

// Generated message map functions
protected:
	//{{AFX_MSG(CGranSim2D_TestView)
	afx_msg void OnParticle1pos();
	afx_msg void OnParticle2Pos();
	afx_msg void OnCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GranSim2D_TestView.cpp
inline CGranSim2D_TestDoc* CGranSim2D_TestView::GetDocument()
   { return (CGranSim2D_TestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRANSIM2D_TESTVIEW_H__73D22455_6175_4F83_82DC_7987FE375B49__INCLUDED_)
