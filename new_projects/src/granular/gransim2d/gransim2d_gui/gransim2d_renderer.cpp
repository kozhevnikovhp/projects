/* gransim2d_renderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Mar 2009 - initial creation (from GranSim2DDesign.cpp)
	28 Mar 2009 - split design and editor/renderer stuff
*/

#include "gransim2d_renderer.h"
#include "main_window.h"
#include "../engine/gui_api.h"

///////////////////////////////////////////////////////////////////////////
// cGranSim2DRenderer

cGranSim2DRenderer::cGranSim2DRenderer(cMainWindow *pMainWindow, cGranSim2DDesign *pDesign) : inherited(pMainWindow)
{
    printf("cGranSim2DRenderer::cGranSim2DRenderer\n");
	m_pMainWindow = pMainWindow;
	m_pDesign = pDesign;
	m_pSelectedObject = NULL;
	m_pProjectView = NULL;
	m_bCreationFromFile = false;
}

//virtual
cGranSim2DRenderer::~cGranSim2DRenderer()
{
}

//virtual
void cGranSim2DRenderer::mouseMoveEvent(QMouseEvent *pEvent)
{
	double xWorld = ScreenToWorldX(pEvent->x());
	double yWorld = ScreenToWorldY(pEvent->y());
	m_pMainWindow->showCursorPos(xWorld, yWorld);

	CGranSim2DObject *pObject = m_pDesign->GetObjectAt(xWorld, yWorld, 5*Get1PixelExtentsX());
	if (pObject)
		m_pMainWindow->showHint(pObject->GetFullName());
	else
		m_pMainWindow->showHint("");

	inherited::mouseMoveEvent(pEvent);
}

//virtual
void cGranSim2DRenderer::onRenderScene()
{
	// Bounding box
	const SExtents2D *pBox = m_pDesign->GetBoundingBox();
	if (pBox->IsInitialized())
	{
		glColor3d(0.2, 0.2, 0.2);
		glBegin(GL_QUADS);
			glVertex2d(pBox->m_fLeft, pBox->m_fTop);
			glVertex2d(pBox->m_fRight, pBox->m_fTop);
			glVertex2d(pBox->m_fRight, pBox->m_fBottom);
			glVertex2d(pBox->m_fLeft, pBox->m_fBottom);
		glEnd();
	}

	// Sim cells
	const CSimCells2D &SimCells = m_pDesign->GetSimCells();
	glColor3d(1, 1, 0);
	for (int i = 0; i < SimCells.GetCols(); i++)
	{
		for (int j = 0; j < SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = SimCells(i,j);
			SExtents2D *pBox = pCell->GetBox();
			glBegin(GL_LINE_LOOP);
				glVertex2d(pBox->m_fLeft, pBox->m_fTop);
				glVertex2d(pBox->m_fRight, pBox->m_fTop);
				glVertex2d(pBox->m_fRight, pBox->m_fBottom);
				glVertex2d(pBox->m_fLeft, pBox->m_fBottom);
			glEnd();
		}
	}
	// Objects
	inherited::onRenderScene();

	// Particles
	RenderParticles();
}

void cGranSim2DRenderer::RenderParticles()
{
	glColor3d(1,0,0);
	int i, j;
	const CSimCells2D &SimCells = m_pDesign->GetSimCells();
	for (i = 0; i < SimCells.GetCols(); i++)
	{
		for (j = 0; j < SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = SimCells(i,j);
			for (int k = 0; k < pCell->GetParticleCount(); k++)
			{
				CParticle2D *pParticle = pCell->GetParticle(k);
		/*glBegin(GL_POINTS);
			glVertex2d(pParticle->m_State[CURRENT_STATE].m_Position.m_fX, pParticle->m_State[CURRENT_STATE].m_Position.m_fY);
		glEnd();*/

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				
				glTranslated(pParticle->m_State[CURRENT_STATE].m_Position.m_fX, pParticle->m_State[CURRENT_STATE].m_Position.m_fY, 0);
				glScaled(pParticle->m_fSize, pParticle->m_fSize, 1);
				glRotated(common::calc::RADIANS_2_DEGREES(pParticle->GetFi(CURRENT_STATE)), 0, 0, 1);
				FineRenderParticleType(pParticle->m_pParticleType);


				glPopMatrix();
			}
		}
	}
}

//virtual
void cGranSim2DRenderer::OnEventSelectionChanged(CEditorGeometry2D *pSelectedGeometry, void *pWhere)
{
	CGranSim2DObject *pObject = NULL;
	if (pSelectedGeometry)
		pObject = (CGranSim2DObject *)pSelectedGeometry->GetUserData();
	Select(pObject);
	if (g_pGui)
		g_pGui->ObjectSelectionChanged(pObject, pWhere);
}

//virtual
void *cGranSim2DRenderer::OnPointCreated(CEditorPoint2D *pEditorPoint)
{
	if (m_bCreationFromFile)
		return m_pDesign->GetCurrentObject();
	CGeometry2DObject *pObject = m_pDesign->OnPointCreatedExternally(pEditorPoint->GetConnectedPoint(), this);
	// Reconnect editor object to the geometry object from design, not to stub one
	pEditorPoint->ConnectToPoint((CPoint2D *)pObject->GetGeometry());
	pEditorPoint->ConnectToRGBA(pObject->GetRGBA());

	redraw();
	return pObject;
}

//virtual
void *cGranSim2DRenderer::OnLineCreated(CEditorLine2D *pEditorLine)
{
	if (m_bCreationFromFile)
		return m_pDesign->GetCurrentObject();
	CGeometry2DObject *pObject = m_pDesign->OnLineCreatedExternally(pEditorLine->GetConnectedLine(), this);
	// Reconnect editor object to the geometry object from design, not to stub one
	pEditorLine->ConnectToLine((CLine2D *)pObject->GetGeometry());
	pEditorLine->ConnectToRGBA(pObject->GetRGBA());

	redraw();
	return pObject;
}

//virtual
void *cGranSim2DRenderer::OnRectangleCreated(CEditorRectangle2D *pEditorRectangle)
{
	if (m_bCreationFromFile)
		return m_pDesign->GetCurrentObject();
	CGeometry2DObject *pObject = m_pDesign->OnRectangleCreatedExternally(pEditorRectangle->GetConnectedRectangle(), this);
	// Reconnect editor object to the geometry object from design, not to stub one
	pEditorRectangle->ConnectToRectangle((CRectangle2D *)pObject->GetGeometry());
	pEditorRectangle->ConnectToRGBA(pObject->GetRGBA());

	redraw();
	return pObject;
}

//virtual
void cGranSim2DRenderer::OnGeometryChanged(CEditorGeometry2D *pGeometry, CControlPoint2D *pControlPoint)
{
	g_pGui->DesignChanged();
}

void cGranSim2DRenderer::ZoomIn()
{
	IncreaseZoom(2);
}

void cGranSim2DRenderer::ZoomOut()
{
	IncreaseZoom(0.5);
}

void cGranSim2DRenderer::ToggleReflectingWallCreation()
{
	if (!m_pDesign->IsInCreationReflectingWall())
	{
		m_pDesign->SetReflectingWallCreationMode();
		SetEditMode(LINE_EDIT_MODE); // editor's line mode
	}
	else
	{
		m_pDesign->SetNoneCreationMode();
		SetEditMode(NONE_EDIT_MODE); // editor's line mode
	}
	redraw();
}

void cGranSim2DRenderer::ToggleReflectingRectangleCreation()
{
	if (!m_pDesign->IsInCreationReflectingRectangle())
	{
		m_pDesign->SetReflectingRectangleCreationMode();
		SetEditMode(RECTANGLE_EDIT_MODE); // editor's line mode
	}
	else
	{
		m_pDesign->SetNoneCreationMode();
		SetEditMode(NONE_EDIT_MODE); // editor's line mode
	}
	redraw();
}

void cGranSim2DRenderer::ToggleParticleLinearSourceCreation()
{
	if (!m_pDesign->IsInCreationParticleLinearSource())
	{
		m_pDesign->SetParticleLinearSourceCreationMode();
		SetEditMode(LINE_EDIT_MODE); // editor's line mode
	}
	else
	{
		m_pDesign->SetNoneCreationMode();
		SetEditMode(NONE_EDIT_MODE);
	}
	redraw();
}

void cGranSim2DRenderer::ToggleParticlePointSourceCreation()
{
	if (!m_pDesign->IsInCreationParticlePointSource())
	{
		m_pDesign->SetParticlePointSourceCreationMode();
		SetEditMode(POINT_EDIT_MODE); // editor's point mode
	}
	else
	{
		m_pDesign->SetNoneCreationMode();
		SetEditMode(NONE_EDIT_MODE);
	}
	redraw();
}

void cGranSim2DRenderer::Select(CGranSim2DObject *pObject)
{
	if (m_pSelectedObject)
		m_pSelectedObject->Unselect();
	if (pObject)
		pObject->Select();
	m_pSelectedObject = pObject;
}

void cGranSim2DRenderer::ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere)
{
	if (pWhere == this)
		return;
	CGeometry2D *pGeometry = NULL;
	if (pObject)
		pGeometry = pObject->GetGeometry();
	CGeometry2D *pSelectedGeometry = NULL;
	CEditorGeometry2D *pSelectedEditorGeometry = GetSelection();
	if (pSelectedEditorGeometry)
		pSelectedGeometry =pSelectedEditorGeometry->GetGeometry();
	if (pSelectedGeometry == pGeometry)
		return;
	SetSelection(pGeometry, pWhere);
}

void cGranSim2DRenderer::ReflectingWallCreated(CReflectingWall *pWall, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
	m_bCreationFromFile = true;
	CEditorLine2D *pEditorLine = CreateLine(pWall->GetLine(), pWall->GetRGBA());
	m_bCreationFromFile = false;
}

void cGranSim2DRenderer::ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
	m_bCreationFromFile = true;
	CEditorRectangle2D *pEditorRectangle = CreateRectangle(pRectangle->GetRectangle(), pRectangle->GetRGBA());
	m_bCreationFromFile = false;
}

void cGranSim2DRenderer::MaterialCreated(CMaterial *pMaterial, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
}

void cGranSim2DRenderer::ParticleTypeCreated(CParticleType *pType, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
}

void cGranSim2DRenderer::ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
	m_bCreationFromFile = true;
	CEditorPoint2D *pEditorPoint = CreatePoint(pSource->GetPoint(), pSource->GetRGBA());
	m_bCreationFromFile = false;
}

void cGranSim2DRenderer::ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
	m_bCreationFromFile = true;
	CEditorLine2D *pEditorLine = CreateLine(pSource->GetLine(), pSource->GetRGBA());
	m_bCreationFromFile = false;
}

void cGranSim2DRenderer::ObjectDeleted(CGranSim2DObject *pObject, void *pWhere)
{
	if (pWhere == this)
		return; // do nothing
}

void cGranSim2DRenderer::DeleteSelectedObject()
{
	if (!m_pSelectedObject)
		return; // do nothing
	g_pGui->ObjectDeleted(m_pSelectedObject, this);
	g_pGui->DesignChanged();
	m_pSelectedObject->DeleteFromDesign();
	DeleteGeometry(m_pSelectedObject->GetGeometry());
	delete m_pSelectedObject;
	m_pSelectedObject = NULL;
}

char *cGranSim2DRenderer::GetDisplayedText(int x, int y)
{
	if (m_pDesign->IsSimulating())
	{
		sprintf(m_szText, "Time = %g s, dT = %g s", m_pDesign->GetSimulationTime(), m_pDesign->GetSimulationTimeStep());
	}
	else
	{
		double WorldX = ScreenToWorldX(x);
		double WorldY = ScreenToWorldX(y);
		CGranSim2DObject *pObject = m_pDesign->GetObjectAt(WorldX, WorldY, 5*Get1PixelExtentsX());
		if (pObject)
			strcpy(m_szText, pObject->GetFullName());
		else
			sprintf(m_szText, "%f %f", WorldX, WorldY);
	}
	return m_szText;
}


// Rendering particle type, fine quality and no any optimization
void FineRenderParticleType(CParticleType *pType)
{
	glColor3f(1, 0, 0);
	glPointSize(1);
	//glBegin(GL_POINTS);
	glBegin(GL_LINE_LOOP);
		for (int i = 0; i < N_PARTICLE_SHAPE_VERTICES; i+=1)
			glVertex2d(pType->m_Vertices[i].x, pType->m_Vertices[i].y);
	glEnd();
/*	glColor4ubv(ucYellowColor);
	glBegin(GL_LINE_LOOP);
		glVertex2d(-0.5, -0.5/pType->GetShapeAspectRatio());
		glVertex2d(+0.5, -0.5/pType->GetShapeAspectRatio());
		glVertex2d(+0.5, +0.5/pType->GetShapeAspectRatio());
		glVertex2d(-0.5, +0.5/pType->GetShapeAspectRatio());
	glEnd();*/

	
/*	glColor4ubv(ucYellowColor);
	glPointSize(1);
	glBegin(GL_POINTS);
		for (i = 0; i < N_PARTICLE_SHAPE_VERTICES; i+=1)
			glVertex2d(pType->m_Vertices[i].Xk, pType->m_Vertices[i].Yk);
	glEnd();*/
}

