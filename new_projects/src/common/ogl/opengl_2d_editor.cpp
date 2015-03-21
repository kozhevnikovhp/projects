/* opengl_2d_editor.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Sep 2008 - initial creation
	25 Sep 2008 - control points introduced
	09 Oct 2008 - dragging control point changes the geometry
	10 Oct 2008 - new object functionality; background and editing rubber colors
	11 Oct 2008 - 2D-points functionality
	04 Jan 2009 - 2D-rectangles functionality (base)
	05 Jan 2009 - geometry deletion
	10 Mar 2009 - grid, snapping to grid
	20 Mar 2009 - CGeometry2D and CEditorGeometry2D are splitted
*/

#include "opengl_2d_editor.h"
#include "calc/calc.h"
#include <math.h>

namespace common {

namespace ogl {

void RenderExtents(SExtents2D &extents)
{
	if (extents.m_fLeft > extents.m_fRight)
		return; //not initialized, do nothing
	glBegin(GL_LINE_LOOP);
		glVertex2d(extents.m_fLeft, extents.m_fBottom);
		glVertex2d(extents.m_fLeft, extents.m_fTop);
		glVertex2d(extents.m_fRight, extents.m_fTop);
		glVertex2d(extents.m_fRight, extents.m_fBottom);
		glVertex2d(extents.m_fLeft, extents.m_fBottom);
	glEnd();
}


///////////////////////////////////////////////////////////////////////////////
// class CControlPoint2D

const int CONTROL_POINT_SIZE = 8; // (in pixels)

CControlPoint2D::CControlPoint2D()
{
	m_pControllablePoint = NULL;
}

//virtual
CControlPoint2D::~CControlPoint2D()
{
}

bool CControlPoint2D::IsOverMe(double x, double y, cOpenGL_2D_Editor_Widget *pEditor)
{
	return ((fabs(x-X()) < CONTROL_POINT_SIZE*pEditor->Get1PixelExtentsX()) && (fabs(y-Y()) < CONTROL_POINT_SIZE*pEditor->Get1PixelExtentsX()));
}

void CControlPoint2D::SetPosition(double x, double y)
{
	m_pControllablePoint->Set(x, y);
}

///////////////////////////////////////////////////////////////////////////////
// class CWeakControlPoint2D

CWeakControlPoint2D::CWeakControlPoint2D()
{
}

//virtual
CWeakControlPoint2D::~CWeakControlPoint2D()
{
}

//virtual
void CWeakControlPoint2D::Render(cOpenGL_2D_Editor_Widget *pEditor)
{
	double sizeX = CONTROL_POINT_SIZE*pEditor->Get1PixelExtentsX()/2.;
	double sizeY = CONTROL_POINT_SIZE*pEditor->Get1PixelExtentsY()/2.;
	glBegin(GL_QUADS);
		glVertex2d(X()-sizeX, Y()-sizeY);
		glVertex2d(X()-sizeX, Y()+sizeY);
		glVertex2d(X()+sizeX, Y()+sizeY);
		glVertex2d(X()+sizeX, Y()-sizeY);
	glEnd();
}

///////////////////////////////////////////////////////////////////////////////
// class CPositionControlPoint2D

CPositionControlPoint2D::CPositionControlPoint2D()
{
}

//virtual
CPositionControlPoint2D::~CPositionControlPoint2D()
{
}

//virtual
void CPositionControlPoint2D::Render(cOpenGL_2D_Editor_Widget *pEditor)
{
	double sizeX = CONTROL_POINT_SIZE*pEditor->Get1PixelExtentsX()/2;
	double sizeY = CONTROL_POINT_SIZE*pEditor->Get1PixelExtentsY()/2;
	glBegin(GL_QUADS);
		glVertex2d(X(), Y()-sizeY);
		glVertex2d(X()-sizeX, Y());
		glVertex2d(X(), Y()+sizeY);
		glVertex2d(X()+sizeX, Y());
	glEnd();
}


///////////////////////////////////////////////////////////////////////////////
// class CEditorGeometry2D

CEditorGeometry2D::CEditorGeometry2D()
{
	m_pGeometry = NULL;
	m_DisplayList = 0;
	ConnectToRGBA(&m_StubRGBA);
	m_StubRGBA.Init(0xFF, 0x00, 0x00);
}

//virtual
CEditorGeometry2D::~CEditorGeometry2D()
{
	glDeleteLists(m_DisplayList, 1);
}

void CEditorGeometry2D::RebuildDisplayList()
{
	if (!m_DisplayList) // does not exist yet
		m_DisplayList = glGenLists(1);
	glNewList(m_DisplayList, GL_COMPILE);
		glColor4ubv(GetRGBA()->GetRGBAptr());
		OnRealRendering();
	glEndList();
}

void CEditorGeometry2D::Render()
{
	if (!m_DisplayList) // does not exist yet
		RebuildDisplayList();
	glCallList(m_DisplayList);
}

void CEditorGeometry2D::ConnectToRGBA(SRGBA *pRGBA)
{
	m_pConnectedRGBA = pRGBA;
}

void CEditorGeometry2D::RenderControlPoints(cOpenGL_2D_Editor_Widget *pEditor)
{
	for (int iPoint = 0; iPoint < GetControlPointCount(); iPoint++)
	{
		CControlPoint2D *pPoint = GetControlPoint(iPoint);
		pPoint->Render(pEditor);
	}
}

Qt::CursorShape CEditorGeometry2D::GetCursorShape(double x, double y, cOpenGL_2D_Editor_Widget *pEditor)
{
	for (int iPoint = 0; iPoint < GetControlPointCount(); iPoint++)
	{
		CControlPoint2D *pPoint = GetControlPoint(iPoint);
		if (pPoint->IsOverMe(x, y, pEditor))
			return pPoint->GetCursorShape();
	}
	return Qt::ArrowCursor;
}


///////////////////////////////////////////////////////////////////////////////
// class CEditorPoint2D

CEditorPoint2D::CEditorPoint2D(SVector2D &Point)
{
	ConnectToPoint(&m_StubPoint);
	m_StubPoint.Set(Point);
}

CEditorPoint2D::CEditorPoint2D(CPoint2D *pPoint)
{
	ConnectToPoint(pPoint);
}

CEditorPoint2D::CEditorPoint2D(CPoint2D *pPoint, SRGBA *pRGBA)
{
	ConnectToPoint(pPoint);
	ConnectToRGBA(pRGBA);
}

//virtual
CEditorPoint2D::~CEditorPoint2D()
{
}

void CEditorPoint2D::ConnectToPoint(CPoint2D *pPoint)
{
	m_pConnectedPoint = pPoint;
	m_ControlPoint.SetControllablePoint(pPoint->GetPoint());
	m_pGeometry = pPoint;
}

//virtual
void CEditorPoint2D::OnRealRendering()
{
	glPointSize(3);
	glBegin(GL_POINTS);
		glVertex2d(m_ControlPoint.X(), m_ControlPoint.Y());
	glEnd();
}

///////////////////////////////////////////////////////////////////////////////
// class CEditorLine2D

CEditorLine2D::CEditorLine2D(SVector2D &Point1, SVector2D &Point2)
{
	ConnectToLine(&m_StubLine);
	m_StubLine.GetPoint1()->Set(Point1);
	m_StubLine.GetPoint2()->Set(Point2);
}

CEditorLine2D::CEditorLine2D(CLine2D *pLine)
{
	ConnectToLine(pLine);
}

CEditorLine2D::CEditorLine2D(CLine2D *pLine, SRGBA *pRGBA)
{
	ConnectToLine(pLine);
	ConnectToRGBA(pRGBA);
}

//virtual
CEditorLine2D::~CEditorLine2D()
{
}

void CEditorLine2D::ConnectToLine(CLine2D *pLine)
{
	m_pConnectedLine = pLine;
	m_Point1.SetControllablePoint(pLine->GetPoint1());
	m_Point2.SetControllablePoint(pLine->GetPoint2());
	m_Center.SetControllablePoint(pLine->GetCenterPoint());
	m_pGeometry = pLine;
}


//virtual
void CEditorLine2D::OnRealRendering()
{
	glBegin(GL_LINES);
		glVertex2d(m_Point1.X(), m_Point1.Y());
		glVertex2d(m_Point2.X(), m_Point2.Y());
	glEnd();
}

//virtual
CControlPoint2D *CEditorLine2D::GetControlPoint(int iPoint)
{
	if (iPoint == 0)
		return &m_Point1;
	else if (iPoint == 1)
		return &m_Point2;
	else if (iPoint == 2)
		return &m_Center;

	return NULL;
}

//virtual
void CEditorLine2D::RecalcControlPoints(CControlPoint2D *pSourcePoint)
{
	if (pSourcePoint == &m_Center)
	{
		double ShiftX = m_pConnectedLine->XCenter() - (m_pConnectedLine->X1() + m_pConnectedLine->X2())/2;
		double ShiftY = m_pConnectedLine->YCenter() - (m_pConnectedLine->Y1() + m_pConnectedLine->Y2())/2;
		m_pConnectedLine->Move(ShiftX, ShiftY);
	}
	else
	{
		m_pConnectedLine->RecalcAux();
	}
}


///////////////////////////////////////////////////////////////////////////////
// class CEditorRectangle2D

CEditorRectangle2D::CEditorRectangle2D(SVector2D &NWPoint, SVector2D &SEPoint)
{
	ConnectToRectangle(&m_StubRectangle);
	double x = (NWPoint.X() + SEPoint.X())/2;
	double w = fabs(NWPoint.X() - SEPoint.X());
	double y = (NWPoint.Y() + SEPoint.Y())/2;
	double h = fabs(NWPoint.Y() - SEPoint.Y());
	m_StubRectangle.Init(x, y, w, h);
}

CEditorRectangle2D::CEditorRectangle2D(CRectangle2D *pRectangle)
{
	ConnectToRectangle(pRectangle);
}

CEditorRectangle2D::CEditorRectangle2D(CRectangle2D *pRectangle, SRGBA *pRGBA)
{
	ConnectToRectangle(pRectangle);
	ConnectToRGBA(pRGBA);
}

//virtual
CEditorRectangle2D::~CEditorRectangle2D()
{
}

void CEditorRectangle2D::ConnectToRectangle(CRectangle2D *pRectangle)
{
	m_pConnectedRectangle = pRectangle;
	m_PointNW.SetControllablePoint(pRectangle->GetPointNW());
	m_PointNE.SetControllablePoint(pRectangle->GetPointNE());
	m_PointSW.SetControllablePoint(pRectangle->GetPointSW());
	m_PointSE.SetControllablePoint(pRectangle->GetPointSE());
	m_Center.SetControllablePoint(pRectangle->GetCenterPoint());
	m_pGeometry = pRectangle;
}

//virtual
void CEditorRectangle2D::OnRealRendering()
{
	GLenum mode = GL_LINE_LOOP;
	if (IsFilled() && !m_pEditor->IsEditingEnabled())
		mode = GL_QUADS;
	glBegin(mode);
		glVertex2d(m_PointNW.X(), m_PointNW.Y());
		glVertex2d(m_PointNE.X(), m_PointNE.Y());
		glVertex2d(m_PointSE.X(), m_PointSE.Y());
		glVertex2d(m_PointSW.X(), m_PointSW.Y());
	glEnd();
}

//virtual
CControlPoint2D *CEditorRectangle2D::GetControlPoint(int iPoint)
{
	if (iPoint == 0)
		return &m_PointNW;
	else if (iPoint == 1)
		return &m_PointNE;
	else if (iPoint == 2)
		return &m_PointSE;
	else if (iPoint == 3)
		return &m_PointSW;
	else if (iPoint == 4)
		return &m_Center;

	return NULL;
}

//virtual
void CEditorRectangle2D::RecalcControlPoints(CControlPoint2D *pSourcePoint)
{
	double x, y, w, h;
	if (pSourcePoint == &m_Center)
	{
		double ShiftX = m_pConnectedRectangle->XCenter() - (m_pConnectedRectangle->XNE() + m_pConnectedRectangle->XSW())/2;
		double ShiftY = m_pConnectedRectangle->YCenter() - (m_pConnectedRectangle->YNE() + m_pConnectedRectangle->YSW())/2;
		m_pConnectedRectangle->Move(ShiftX, ShiftY);
	}
	else if (pSourcePoint == &m_PointNW || pSourcePoint == &m_PointSE)
	{
		x = (m_pConnectedRectangle->XNW() + m_pConnectedRectangle->XSE())/2;
		w = fabs(m_pConnectedRectangle->XNW() - m_pConnectedRectangle->XSE());
		y = (m_pConnectedRectangle->YNW() + m_pConnectedRectangle->YSE())/2;
		h = fabs(m_pConnectedRectangle->YNW() - m_pConnectedRectangle->YSE());
		m_pConnectedRectangle->Init(x, y, w, h);
	}
	else if (pSourcePoint == &m_PointNE || pSourcePoint == &m_PointSW)
	{
		x = (m_pConnectedRectangle->XNE() + m_pConnectedRectangle->XSW())/2;
		w = fabs(m_pConnectedRectangle->XNE() - m_pConnectedRectangle->XSW());
		y = (m_pConnectedRectangle->YNE() + m_pConnectedRectangle->YSW())/2;
		h = fabs(m_pConnectedRectangle->YNE() - m_pConnectedRectangle->YSW());
		m_pConnectedRectangle->Init(x, y, w, h);
	}
}


///////////////////////////////////////////////////////////////////////////
// cOpenGL_2D_Editor_Widget

cOpenGL_2D_Editor_Widget::cOpenGL_2D_Editor_Widget(QWidget *pParent) : inherited(pParent)
{
	m_bEditingEnabled = false;
	SetEditMode(NONE_EDIT_MODE);
    setBackgroundColor(Qt::blue);
	Init();
}

//virtual
cOpenGL_2D_Editor_Widget::~cOpenGL_2D_Editor_Widget()
{
	Cleanup();
}

//virtual
void cOpenGL_2D_Editor_Widget::onSetDefaultOptions()
{
	glEnable(GL_POINT_SMOOTH);
}

//virtual
Qt::CursorShape cOpenGL_2D_Editor_Widget::GetCursorShape()
{
	if (!IsEditingEnabled() || m_CurrentEditMode != NONE_EDIT_MODE)
		return Qt::ArrowCursor;
	if (!m_pSelectedGeometry)
		return Qt::ArrowCursor;
	double x = ScreenToWorldX(m_MouseCurrentPosX);
	double y = ScreenToWorldY(m_MouseCurrentPosY);
	return m_pSelectedGeometry->GetCursorShape(x, y, this);
}

//virtual
void cOpenGL_2D_Editor_Widget::onRenderScene()
{
	glColor3d(0.5, 0.5, 0.5);
	//RenderExtents(m_Extents);

	// Render all the geometries
	for (cEditorGeometries::const_iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		pGeometry->Render();
	}
	if (m_pSelectedGeometry)
	{
		m_pSelectedGeometry->Render(); // once again to be on top
		if (IsEditingEnabled() && GetEditMode() == NONE_EDIT_MODE)
		{
			m_pSelectedGeometry->RenderControlPoints(this);
		}
	}

	// Render object having been created at this moment by its creation control points
	if (IsEditingEnabled() && m_nCurrentCreationControlPoint != 0)
	{
		setColor3f(getEditRubberColor());
		if (m_CurrentEditMode == LINE_EDIT_MODE)
		{
			glBegin(GL_LINES);
				glVertex2d(m_CreationControlPoints[0].X(), m_CreationControlPoints[0].Y());
				glVertex2d(m_CreationControlPoints[1].X(), m_CreationControlPoints[1].Y());
			glEnd();
		}
		else if (m_CurrentEditMode == RECTANGLE_EDIT_MODE)
		{
			glBegin(GL_LINE_LOOP);
				glVertex2d(m_CreationControlPoints[0].X(), m_CreationControlPoints[0].Y());
				glVertex2d(m_CreationControlPoints[1].X(), m_CreationControlPoints[0].Y());
				glVertex2d(m_CreationControlPoints[1].X(), m_CreationControlPoints[1].Y());
				glVertex2d(m_CreationControlPoints[0].X(), m_CreationControlPoints[1].Y());
			glEnd();
		}
	}

	if (IsGridEnabled())
	{
		double fGridSize = GetGridSize();
		double fLeft = GetFrustrumLeft();
		common::calc::SnapToGrid(fLeft, fGridSize);
		double fRight = GetFrustrumRight();
		common::calc::SnapToGrid(fRight, fGridSize);
		double fTop = GetFrustrumTop();
		common::calc::SnapToGrid(fTop, fGridSize);
		double fBottom = GetFrustrumBottom();
		common::calc::SnapToGrid(fBottom, fGridSize);
		setColor3f(getGridColor());
		glPointSize(1);
		glBegin(GL_POINTS);
		double x = fLeft;
		while (x <= fRight)
		{
			double y = fBottom;
			while (y <= fTop)
			{
				glVertex2d(x, y);
				y += fGridSize;
			}
			x += fGridSize;
		}
		glEnd();
	}
}

//virtual
void cOpenGL_2D_Editor_Widget::mousePressEvent(QMouseEvent *pEvent)
{ // We will try to move control points here if we are not in one of new object creation modes
	m_pMovingControlPoint = NULL;
	if (!CanCreateNewObject())
		return;

	double WorldX = ScreenToWorldX(pEvent->x());
	double WorldY = ScreenToWorldY(pEvent->y());

	if (m_CurrentEditMode == NONE_EDIT_MODE)
	{
		bool bFound = false;
		if (m_pSelectedGeometry)
		{
			for (int iPoint = 0; iPoint < m_pSelectedGeometry->GetControlPointCount(); iPoint++)
			{
				CControlPoint2D *pPoint = m_pSelectedGeometry->GetControlPoint(iPoint);
				if (pPoint->IsOverMe(WorldX, WorldY, this))
				{
					m_pMovingControlPoint = pPoint;
					bFound = true;
					break;
				}
			}
		}
		if (!bFound)
		{
			CEditorGeometry2D *pGeometry = getGeometryAt(WorldX, WorldY);
			if (m_pSelectedGeometry != pGeometry)
			{
				if (!pGeometry && m_pMovingControlPoint)
				{
				}
				else
				{
					SetSelection(pGeometry, this);
				}
			}
			m_pMovingControlPoint = NULL;
		}
	}
	else
	{
		if (IsGridEnabled())
		{
			common::calc::SnapToGrid(WorldX, GetGridSize());
			common::calc::SnapToGrid(WorldY, GetGridSize());
		}
		m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
		++m_nCurrentCreationControlPoint;
		// Duplicate to the next point
		m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
	}
	ProbablyBreakCreationMode();
	inherited::mousePressEvent(pEvent);
}

//virtual
void cOpenGL_2D_Editor_Widget::mouseReleaseEvent(QMouseEvent *pEvent)
{
	m_pMovingControlPoint = NULL;
	if (!CanCreateNewObject())
		return;
	int x = pEvent->x();
	int y = pEvent->y();
	if (m_CurrentEditMode != NONE_EDIT_MODE)
	{
		if (abs(x - m_LMouseButtonDrag.whereButtonPressedX()) < 5 && abs(y - m_LMouseButtonDrag.whereButtonPressedY()) < 5)
		{ // button released too close to the location where it was pressed so no new control point is created
		}
		else
		{
			double WorldX = ScreenToWorldX(x);
			double WorldY = ScreenToWorldY(y);
			if (IsGridEnabled())
			{
				common::calc::SnapToGrid(WorldX, GetGridSize());
				common::calc::SnapToGrid(WorldY, GetGridSize());
			}
			m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
			++m_nCurrentCreationControlPoint;
			ProbablyBreakCreationMode();
		}
	}
	inherited::mouseReleaseEvent(pEvent);
}

//virtual
void cOpenGL_2D_Editor_Widget::mouseMoveEvent(QMouseEvent *pEvent)
{
	int x = pEvent->x();
	int y = pEvent->y();
	if (IsEditingEnabled())
	{
		double WorldX = ScreenToWorldX(x);
		double WorldY = ScreenToWorldY(y);
		setCursor(GetCursorShape());
		if (IsGridEnabled())
		{
			common::calc::SnapToGrid(WorldX, GetGridSize());
			common::calc::SnapToGrid(WorldY, GetGridSize());
		}

		if (m_pMovingControlPoint && m_pSelectedGeometry)
		{
			m_pMovingControlPoint->SetPosition(WorldX, WorldY);
			m_pSelectedGeometry->RecalcControlPoints(m_pMovingControlPoint);
			m_pSelectedGeometry->GetGeometry()->RecalcExtents();
			RebuildDisplayList(m_pSelectedGeometry);
			OnGeometryChanged(m_pSelectedGeometry, m_pMovingControlPoint);
			redraw();
		}
		else if (m_CurrentEditMode != NONE_EDIT_MODE)
		{
			if (abs(x - m_LMouseButtonDrag.whereButtonPressedX()) < 5 && abs(y - m_LMouseButtonDrag.whereButtonPressedY()) < 5)
			{ // button released too close to the location where it was pressed so no new control point is created
			}
			else
			{
				m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
				redraw();
			}
		}
	}
	inherited::mouseMoveEvent(pEvent);
}

//virtual
void cOpenGL_2D_Editor_Widget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
	inherited::mouseDoubleClickEvent(pEvent);
}

CEditorGeometry2D *cOpenGL_2D_Editor_Widget::findGeometry(CGeometry2D *pGeometry)
{
	for (cEditorGeometries::const_iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pEditorGeometry = *p;
		if (pEditorGeometry->GetGeometry() == pGeometry)
			return pEditorGeometry; // found
	}
	return NULL; // not found
}

void cOpenGL_2D_Editor_Widget::DeleteGeometry(CGeometry2D *pGeometryToBeDeleted)
{
	if (!pGeometryToBeDeleted)
		return;
	if (m_pSelectedGeometry->GetGeometry() == pGeometryToBeDeleted)
		m_pSelectedGeometry = NULL;

	for (cEditorGeometries::iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		if (pGeometry->GetGeometry() == pGeometryToBeDeleted)
		{
			delete pGeometry;
			m_Geometries.erase(p);
			break;
		}
	}
	redraw();
}

CEditorGeometry2D *cOpenGL_2D_Editor_Widget::getGeometryAt(double WorldX, double WorldY)
{
	double tolerance = 5*Get1PixelExtentsX();
	double MinDistance = common::calc::VERY_BIG_DOUBLE;
	CEditorGeometry2D *pNearestGeometry = NULL;
	for (cEditorGeometries::const_iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		double distance = pGeometry->CalcDistance(WorldX, WorldY);
		if (distance < tolerance && distance < MinDistance)
		{
			MinDistance = distance;
			pNearestGeometry = pGeometry;
		}
	}
	return pNearestGeometry;
}

CEditorGeometry2D *cOpenGL_2D_Editor_Widget::getGeometryAt(int ScreenX, int ScreenY)
{
	double WorldX = ScreenToWorldX(ScreenX);
	double WorldY = ScreenToWorldY(ScreenY);
	return getGeometryAt(WorldX, WorldY);
}

void cOpenGL_2D_Editor_Widget::ProbablyBreakCreationMode()
{
	if (m_CurrentEditMode == POINT_EDIT_MODE)
	{
		if (m_nCurrentCreationControlPoint == 1)
		{
		//	SetEditMode(NONE_EDIT_MODE);
			m_nCurrentCreationControlPoint = 0;
			CreatePoint(m_CreationControlPoints[0]);
		}
	}
	else if (m_CurrentEditMode == LINE_EDIT_MODE)
	{
		if (m_nCurrentCreationControlPoint == 2)
		{
		//	SetEditMode(NONE_EDIT_MODE);
			m_nCurrentCreationControlPoint = 0;
			CreateLine(m_CreationControlPoints[0], m_CreationControlPoints[1]);
		}
	}
	if (m_CurrentEditMode == RECTANGLE_EDIT_MODE)
	{
		if (m_nCurrentCreationControlPoint == 2)
		{
		//	SetEditMode(NONE_EDIT_MODE);
			m_nCurrentCreationControlPoint = 0;
			CreateRectangle(m_CreationControlPoints[0], m_CreationControlPoints[1]);
		}
	}
	else if (m_CurrentEditMode == CIRCLE_3POINTS_EDIT_MODE)
	{
		if (m_nCurrentCreationControlPoint == 3)
		{
		//	SetEditMode(NONE_EDIT_MODE);
			m_nCurrentCreationControlPoint = 0;
		}
	}
}

bool cOpenGL_2D_Editor_Widget::CanCreateNewObject()
{
	return (IsEditingEnabled());
}

void cOpenGL_2D_Editor_Widget::Init()
{
	Cleanup();
	m_Extents.Init(-1,1,-1,1);
	m_pMovingControlPoint = NULL;
	m_bGridEnabled = false;
}

void cOpenGL_2D_Editor_Widget::Cleanup()
{
	m_pSelectedGeometry = NULL;
	OnCleanup();
	for (cEditorGeometries::const_iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		delete pGeometry;
	}
	m_Geometries.resize(0);
}

CEditorPoint2D *cOpenGL_2D_Editor_Widget::CreatePoint(CPoint2D *pPoint)
{
	CEditorPoint2D *pNewPoint = new CEditorPoint2D(pPoint);
	addPoint(pNewPoint);
	return pNewPoint;
}

CEditorPoint2D *cOpenGL_2D_Editor_Widget::CreatePoint(CPoint2D *pPoint, SRGBA *pRGBA)
{
	CEditorPoint2D *pNewPoint = new CEditorPoint2D(pPoint, pRGBA);
	addPoint(pNewPoint);
	return pNewPoint;
}

CEditorPoint2D *cOpenGL_2D_Editor_Widget::CreatePoint(SVector2D &Point)
{
	CEditorPoint2D *pNewPoint = new CEditorPoint2D(Point);
	addPoint(pNewPoint);
	return pNewPoint;
}

void cOpenGL_2D_Editor_Widget::addPoint(CEditorPoint2D *pPoint)
{
	AddGeometry(pPoint);
	void *pUserData = OnPointCreated(pPoint);
	pPoint->SetUserData(pUserData);
}

CEditorLine2D *cOpenGL_2D_Editor_Widget::CreateLine(CLine2D *pLine)
{
	CEditorLine2D *pNewLine = new CEditorLine2D(pLine);
	addLine(pNewLine);
	return pNewLine;
}

CEditorLine2D *cOpenGL_2D_Editor_Widget::CreateLine(CLine2D *pLine, SRGBA *pRGBA)
{
	CEditorLine2D *pNewLine = new CEditorLine2D(pLine, pRGBA);
	addLine(pNewLine);
	return pNewLine;
}

CEditorLine2D *cOpenGL_2D_Editor_Widget::CreateLine(SVector2D &Point1, SVector2D &Point2)
{
	CEditorLine2D *pNewLine = new CEditorLine2D(Point1, Point2);
	addLine(pNewLine);
	return pNewLine;
}

void cOpenGL_2D_Editor_Widget::addLine(CEditorLine2D *pLine)
{
	AddGeometry(pLine);
	void *pUserData = OnLineCreated(pLine);
	pLine->SetUserData(pUserData);
}

CEditorRectangle2D *cOpenGL_2D_Editor_Widget::CreateRectangle(CRectangle2D *pRectangle)
{
	CEditorRectangle2D *pNewRectangle = new CEditorRectangle2D(pRectangle);
	addRectangle(pNewRectangle);
	return pNewRectangle;
}

CEditorRectangle2D *cOpenGL_2D_Editor_Widget::CreateRectangle(CRectangle2D *pRectangle, SRGBA *pRGBA)
{
	CEditorRectangle2D *pNewRectangle = new CEditorRectangle2D(pRectangle, pRGBA);
	addRectangle(pNewRectangle);
	return pNewRectangle;
}

CEditorRectangle2D *cOpenGL_2D_Editor_Widget::CreateRectangle(SVector2D &NWPoint, SVector2D &SEPoint)
{
	CEditorRectangle2D *pNewRectangle = new CEditorRectangle2D(NWPoint, SEPoint);
	addRectangle(pNewRectangle);
	return pNewRectangle;
}

void cOpenGL_2D_Editor_Widget::addRectangle(CEditorRectangle2D *pRectangle)
{
	AddGeometry(pRectangle);
	void *pUserData = OnRectangleCreated(pRectangle);
	pRectangle->SetUserData(pUserData);
}

void cOpenGL_2D_Editor_Widget::AddGeometry(CEditorGeometry2D *pGeometry)
{
	pGeometry->SetEditor(this);
	m_Geometries.push_back(pGeometry);
	pGeometry->GetGeometry()->RecalcExtents();
	pGeometry->GetGeometry()->UpdateExtents(m_Extents);
	redraw();
}

void cOpenGL_2D_Editor_Widget::SetSelection(CEditorGeometry2D *pGeometry, void *pWhoSelects)
{
	if (m_pSelectedGeometry == pGeometry)
		return; // do nothing, selected already
	m_pSelectedGeometry = pGeometry;
	OnEventSelectionChanged(pGeometry, pWhoSelects);
	redraw();
}

void cOpenGL_2D_Editor_Widget::SetSelection(CGeometry2D *pGeometry, void *pWhoSelects)
{
	if (m_pSelectedGeometry && m_pSelectedGeometry->GetGeometry() == pGeometry)
		return; // do nothing, selected already
	CEditorGeometry2D *pEditorGeometry = findGeometry(pGeometry);
	if (pEditorGeometry)
	{
		SetSelection(pEditorGeometry, pWhoSelects);
	}
	OnEventSelectionChanged(pEditorGeometry, pWhoSelects);
	redraw();
}

void cOpenGL_2D_Editor_Widget::UpdateExtents()
{
	m_Extents.Init();
	for (cEditorGeometries::const_iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pEditorGeometry = *p;
		pEditorGeometry->GetGeometry()->RecalcExtents();
		pEditorGeometry->GetGeometry()->UpdateExtents(m_Extents);
	}
}

void cOpenGL_2D_Editor_Widget::RebuildDisplayList(CEditorGeometry2D *pEditorGeometry)
{
	makeCurrent();
	pEditorGeometry->RebuildDisplayList();
}

void cOpenGL_2D_Editor_Widget::RebuildDisplayList(CGeometry2D *pGeometry)
{
	CEditorGeometry2D *pEditorGeometry = findGeometry(pGeometry);
	if (pEditorGeometry)
		RebuildDisplayList(pEditorGeometry);
}

void cOpenGL_2D_Editor_Widget::RebuildAllDisplayLists(bool bRedraw)
{
	makeCurrent();
	for (cEditorGeometries::const_iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		pGeometry->RebuildDisplayList();
	}
	if (bRedraw)
		redraw();
}


} // namespace ogl

} // namespace common

