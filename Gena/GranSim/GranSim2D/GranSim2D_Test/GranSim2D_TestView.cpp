// GranSim2D_TestView.cpp : implementation of the CGranSim2D_TestView class
//

#include "stdafx.h"
#include "GranSim2D_Test.h"

#include "GranSim2D_TestDoc.h"
#include "GranSim2D_TestView.h"
#include "ParticlePosDialog.h"
#include "..\GranSim2D_GUI\GranSim2DRenderer.h" // some rendering stuff

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestRenderer::CTestRenderer()
{
	m_ParticleType1.SetShapeAspectRatio(1);
	m_ParticleType1.SetShapeSquareness(2);

	m_Particle1.m_pParticleType = &m_ParticleType1;
	m_Particle1.m_Size = 0.06;
	m_Particle1.SetFi(CURRENT_STATE, 0.34697787);
	m_Particle1.SetPosition(CURRENT_STATE, 0.425044, 0.1473119);
	m_Particle1.SetVelocity(CURRENT_STATE, 0, 0);
	m_bContactPointFound = LOGICAL_FALSE;

	m_ParticleType2.SetShapeAspectRatio(4);
	m_ParticleType2.SetShapeSquareness(3);

	m_Particle2.m_pParticleType = &m_ParticleType2;
	m_Particle2.m_Size = 2;
	m_Particle2.SetFi(CURRENT_STATE, 0);
	m_Particle2.SetPosition(CURRENT_STATE, 0.0, 0.0);
	m_Particle2.SetVelocity(CURRENT_STATE, 0, 0);

	m_Line.SetPoint1(0.45469, 0.5242716);
	m_Line.SetPoint2(0.45469, -0.7313391);
	m_pWall = new CReflectingWall(&m_Line);
	m_bLineContactPointFound = LOGICAL_FALSE;
}

//virtual
CTestRenderer::~CTestRenderer()
{
	delete m_pWall;
}

//virtual
void CTestRenderer::OnRenderScene()
{
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
		glVertex2d(m_Line.X1(), m_Line.Y1());
		glVertex2d(m_Line.X2(), m_Line.Y2());
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslated(m_Particle1.m_State[CURRENT_STATE].m_Position.m_fX, m_Particle1.m_State[CURRENT_STATE].m_Position.m_fY, 0);
		glScaled(m_Particle1.m_Size, m_Particle1.m_Size, 1);
		glRotated(RADIANS_2_DEGREES(m_Particle1.GetFi(CURRENT_STATE)), 0, 0, 1);
		FineRenderParticleType(m_Particle1.m_pParticleType);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslated(m_Particle2.m_State[CURRENT_STATE].m_Position.m_fX, m_Particle2.m_State[CURRENT_STATE].m_Position.m_fY, 0);
		glScaled(m_Particle2.m_Size, m_Particle2.m_Size, 1);
		glRotated(RADIANS_2_DEGREES(m_Particle2.GetFi(CURRENT_STATE)), 0, 0, 1);
		FineRenderParticleType(m_Particle2.m_pParticleType);
	glPopMatrix();

	glPointSize(3);

	if (m_bContactPointFound)
	{
		glColor3d(0,0,1);
		glBegin(GL_POINTS);
			glVertex2d(m_ParticleContact.Point1.X(), m_ParticleContact.Point1.Y());
		glEnd();
		glBegin(GL_LINES);
			glVertex2d(m_ParticleContact.ContactPoint.X(), m_ParticleContact.ContactPoint.Y());
			glVertex2d(m_ParticleContact.ContactPoint.X()+m_ParticleContact.NormalForceDirection.X(), m_ParticleContact.ContactPoint.Y()+m_ParticleContact.NormalForceDirection.Y());
			glVertex2d(m_ParticleContact.ContactPoint.X(), m_ParticleContact.ContactPoint.Y());
			glVertex2d(m_ParticleContact.ContactPoint.X()+m_ParticleContact.TangentForceDirection.X(), m_ParticleContact.ContactPoint.Y()+m_ParticleContact.TangentForceDirection.Y());
		glEnd();
		
		glColor3d(1,0,1);
		glBegin(GL_POINTS);
			glVertex2d(m_ParticleContact.Point2.X(), m_ParticleContact.Point2.Y());
		glEnd();
		glBegin(GL_LINES);
			glVertex2d(m_ParticleContact.ContactPoint.X(), m_ParticleContact.ContactPoint.Y());
			glVertex2d(m_ParticleContact.ContactPoint.X()-m_ParticleContact.NormalForceDirection.X(), m_ParticleContact.ContactPoint.Y()-m_ParticleContact.NormalForceDirection.Y());
			glVertex2d(m_ParticleContact.ContactPoint.X(), m_ParticleContact.ContactPoint.Y());
			glVertex2d(m_ParticleContact.ContactPoint.X()-m_ParticleContact.TangentForceDirection.X(), m_ParticleContact.ContactPoint.Y()-m_ParticleContact.TangentForceDirection.Y());
		glEnd();
	}

	if (m_bLineContactPointFound)
	{
		glColor3d(0,1,1);
		glBegin(GL_POINTS);
			glVertex2d(m_LineContact.Point1.X(), m_LineContact.Point1.Y());
			glVertex2d(m_LineContact.Point2.X(), m_LineContact.Point2.Y());
		glEnd();
	}
}

void CTestRenderer::SetParticle1Pos()
{
	SetParticlePos(&m_Particle1);
}

void CTestRenderer::SetParticle2Pos()
{
	SetParticlePos(&m_Particle2);
}

void CTestRenderer::SetParticlePos(CParticle2D *pParticle)
{
	CParticlePosDialog dlg;
	dlg.m_X = pParticle->m_State[CURRENT_STATE].m_Position.m_fX;
	dlg.m_Y = pParticle->m_State[CURRENT_STATE].m_Position.m_fY;
	dlg.m_Fi = RADIANS_2_DEGREES(pParticle->GetFi(CURRENT_STATE));
	if (dlg.DoModal() != IDOK)
		return;
	pParticle->m_State[CURRENT_STATE].m_Position.m_fX = dlg.m_X;
	pParticle->m_State[CURRENT_STATE].m_Position.m_fY = dlg.m_Y;
	pParticle->SetFi(CURRENT_STATE, DEGREES_2_RADIANS(dlg.m_Fi));
	Redraw();
}

void CTestRenderer::Check()
{
	DWORD s = GetTickCount();
	for (int i = 0; i < 1; i++)
	{
		//m_bLineContactPointFound = m_Particle1.IsOverlapped(CURRENT_STATE, &m_Line, &m_LineContact);
		m_bContactPointFound = m_Particle1.IsOverlapped(CURRENT_STATE, &m_Particle2, &m_ParticleContact);
	}
	DWORD e = GetTickCount();
	CString str; str.Format("%f sec", (e-s)/1000.);
	::MessageBox(NULL, str, "Report", MB_ICONINFORMATION);
/*	double x1, y1, fi1, fi2;
	for (x1 = -2; x1 <= 2; x1 += 0.1)
	{
		m_pParticle1->m_X = x1;
		for (y1 = -2; y1 <= 2; y1 += 0.1)
		{
			m_pParticle1->m_Y = y1;
			for (fi1 = 0; fi1 <= 360; fi1 += 1)
			{
				m_pParticle1->SetFi(DEGREES_2_RADIANS(fi1));
				for (fi2 = 0; fi2 <= 360; fi2 += 1)
				{
					m_pParticle2->SetFi(DEGREES_2_RADIANS(fi2));
					m_bContactPointFound = m_pParticle1->IsOverlapped(m_pParticle2, &m_ParticleContact);
					Redraw();
					Sleep(0);
				}
			}
		}
	}*/
}

char *CTestRenderer::GetDisplayedText(int x, int y)
{
	sprintf(m_szText, "%f %f", ScreenToWorldX(x), ScreenToWorldY(y));
	return m_szText;
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestView

IMPLEMENT_DYNCREATE(CGranSim2D_TestView, COpenGLView)

BEGIN_MESSAGE_MAP(CGranSim2D_TestView, COpenGLView)
	//{{AFX_MSG_MAP(CGranSim2D_TestView)
	ON_COMMAND(ID_PARTICLE1POS, OnParticle1pos)
	ON_COMMAND(ID_PARTICLE2POS, OnParticle2Pos)
	ON_COMMAND(ID_CHECK, OnCheck)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, COpenGLView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestView construction/destruction

CGranSim2D_TestView::CGranSim2D_TestView()
{
	m_pRenderer = NULL;
}

CGranSim2D_TestView::~CGranSim2D_TestView()
{
}

BOOL CGranSim2D_TestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COpenGLView::PreCreateWindow(cs);
}

//virtual
void CGranSim2D_TestView::CreateRenderer()
{
	m_pRenderer = new CTestRenderer();
}

//virtual
void CGranSim2D_TestView::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
	m_pRenderer = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestView drawing

void CGranSim2D_TestView::OnDraw(CDC* pDC)
{
	CGranSim2D_TestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestView printing

BOOL CGranSim2D_TestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGranSim2D_TestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGranSim2D_TestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestView diagnostics

#ifdef _DEBUG
void CGranSim2D_TestView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CGranSim2D_TestView::Dump(CDumpContext& dc) const
{
	COpenGLView::Dump(dc);
}

CGranSim2D_TestDoc* CGranSim2D_TestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGranSim2D_TestDoc)));
	return (CGranSim2D_TestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestView message handlers

void CGranSim2D_TestView::OnParticle1pos() 
{
	m_pRenderer->SetParticle1Pos();
}

void CGranSim2D_TestView::OnParticle2Pos() 
{
	m_pRenderer->SetParticle2Pos();
}

void CGranSim2D_TestView::OnCheck() 
{
	m_pRenderer->Check();
	m_pRenderer->Redraw();
}

char *CGranSim2D_TestView::GetDisplayedText()
{
	if (!m_pRenderer)
		return "";

	CPoint ScreenPoint;
	GetCursorPos(&ScreenPoint);
	ScreenToClient(&ScreenPoint);

	return m_pRenderer->GetDisplayedText(ScreenPoint.x, ScreenPoint.y);
}

