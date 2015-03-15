/* OpenGLEditor.cpp

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

#include "stdafx.h"
#include <math.h>
#include "OpenGLEditor.h"

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

LOGICAL CControlPoint2D::IsOverMe(double x, double y, COpenGL_Editor2D *pEditor)
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
void CWeakControlPoint2D::Render(COpenGL_Editor2D *pEditor)
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
void CPositionControlPoint2D::Render(COpenGL_Editor2D *pEditor)
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

void CEditorGeometry2D::RenderControlPoints(COpenGL_Editor2D *pEditor)
{
	for (int iPoint = 0; iPoint < GetControlPointCount(); iPoint++)
	{
		CControlPoint2D *pPoint = GetControlPoint(iPoint);
		pPoint->Render(pEditor);
	}
}

CursorTypeEnum CEditorGeometry2D::GetCursor(double x, double y, COpenGL_Editor2D *pEditor)
{
	for (int iPoint = 0; iPoint < GetControlPointCount(); iPoint++)
	{
		CControlPoint2D *pPoint = GetControlPoint(iPoint);
		if (pPoint->IsOverMe(x, y, pEditor))
			return pPoint->GetCursor();
	}
	return CURSOR_ARROW;
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


///////////////////////////////////////////////////////////////////////////////
// class CListOfGeometries2D

CListOfGeometries2D::CListOfGeometries2D()
{
	m_pSelectedGeometry = NULL;
}

//virtual
CListOfGeometries2D::~CListOfGeometries2D()
{
	DeleteAll();
}

void CListOfGeometries2D::DeleteAll()
{
	for (iterator p = begin(); p != end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		delete pGeometry;
	}
	resize(0);
}

void CListOfGeometries2D::Render(COpenGL_Editor2D *pEditor)
{
	for (iterator p = begin(); p != end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		pGeometry->Render();
	}
	if (m_pSelectedGeometry)
	{
		m_pSelectedGeometry->Render(); // once again to be on top
		if (pEditor->IsEditingEnabled() && pEditor->GetEditMode() == NONE_EDIT_MODE)
		{
			m_pSelectedGeometry->RenderControlPoints(pEditor);
		}
	}
}

void CListOfGeometries2D::DeleteGeometry(CGeometry2D *pGeometryToBeDeleted)
{
	if (!pGeometryToBeDeleted)
		return;
	if (m_pSelectedGeometry->GetGeometry() == pGeometryToBeDeleted)
		SetSelection(NULL);
	for (iterator p = begin(); p != end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		if (pGeometry->GetGeometry() == pGeometryToBeDeleted)
		{
			delete pGeometry;
			erase(p);
			break;
		}
	}
}

CEditorGeometry2D *CListOfGeometries2D::GetGeometryAt(double WorldX, double WorldY, double tolerance)
{
	double MinDistance = VERY_BIG_DOUBLE;
	CEditorGeometry2D *pNearestGeometry = NULL;
	for (iterator p = begin(); p != end(); p++)
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

void CListOfGeometries2D::RebuildAllDisplayLists()
{
	for (iterator p = begin(); p != end(); p++)
	{
		CEditorGeometry2D *pGeometry = *p;
		pGeometry->RebuildDisplayList();
	}
}

///////////////////////////////////////////////////////////////////////////
// COpenGL_Editor2D

COpenGL_Editor2D::COpenGL_Editor2D() : COpenGL_2D()
{
	m_bEditingEnabled = LOGICAL_FALSE;
	SetEditMode(NONE_EDIT_MODE);
	Init();
}

//virtual
COpenGL_Editor2D::~COpenGL_Editor2D()
{
	Cleanup();
}

//virtual
void COpenGL_Editor2D::OnSetDefaultOptions()
{
	glEnable(GL_POINT_SMOOTH);
}

//virtual
CursorTypeEnum COpenGL_Editor2D::GetCursor()
{
	if (!IsEditingEnabled() || m_CurrentEditMode != NONE_EDIT_MODE)
		return CURSOR_ARROW;
	CEditorGeometry2D *pSelectedGeometry = m_Geometries.GetSelection();
	if (!pSelectedGeometry)
		return CURSOR_ARROW;
	double x = ScreenToWorldX(m_MouseCurrentPosX);
	double y = ScreenToWorldY(m_MouseCurrentPosY);
	return pSelectedGeometry->GetCursor(x, y, this);
}

//virtual
void COpenGL_Editor2D::OnRenderScene()
{
	glColor3d(0.5, 0.5, 0.5);
	//RenderExtents(m_Extents);

	m_Geometries.Render(this);

	// Render object having been created at this moment by its creation control points
	if (IsEditingEnabled() && m_nCurrentCreationControlPoint != 0)
	{
		glColor4ubv(GetEditRubberColor());
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
		::SnapToGrid(fLeft, fGridSize);
		double fRight = GetFrustrumRight();
		::SnapToGrid(fRight, fGridSize);
		double fTop = GetFrustrumTop();
		::SnapToGrid(fTop, fGridSize);
		double fBottom = GetFrustrumBottom();
		::SnapToGrid(fBottom, fGridSize);
		glColor4ubv(GetGridColor());
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
void COpenGL_Editor2D::OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y)
{ // We will try to move control points here if we are not in one of new object creation modes
	m_pMovingControlPoint = NULL;
	if (!CanCreateNewObject())
		return;
	
	double WorldX = ScreenToWorldX(x);
	double WorldY = ScreenToWorldY(y);

	if (m_CurrentEditMode == NONE_EDIT_MODE && IsInSelectMode())
	{
		CEditorGeometry2D *pSelectedGeometry = m_Geometries.GetSelection();
		if (pSelectedGeometry)
		{
			for (int iPoint = 0; iPoint < pSelectedGeometry->GetControlPointCount(); iPoint++)
			{
				CControlPoint2D *pPoint = pSelectedGeometry->GetControlPoint(iPoint);
				if (pPoint->IsOverMe(WorldX, WorldY, this))
				{
					m_pMovingControlPoint = pPoint;
					break;
				}
			}
		}
		CEditorGeometry2D *pGeometry = GetGeometryAt(WorldX, WorldY);
		if (pSelectedGeometry != pGeometry)
		{
			if (!pGeometry && m_pMovingControlPoint)
			{
			}
			else
			{
				SetSelection(pGeometry, this);
			}
		}
	}
	else
	{
		if (IsGridEnabled())
		{
			::SnapToGrid(WorldX, GetGridSize());
			::SnapToGrid(WorldY, GetGridSize());
		}
		m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
		++m_nCurrentCreationControlPoint;
		// Duplicate to the next point
		m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
	}
	ProbablyBreakCreationMode();
}

//virtual
void COpenGL_Editor2D::OnEvent_LeftMouseButtonReleased(unsigned int uFlags, int x, int y)
{
	m_pMovingControlPoint = NULL;
	if (!CanCreateNewObject())
		return;
	if (m_CurrentEditMode != NONE_EDIT_MODE)
	{
		if (abs(x - m_LButtonPressedAtX) < 5 && abs(y - m_LButtonPressedAtY) < 5)
		{ // button released too close to the location where it was pressed so no new control point is created
		}
		else
		{
			double WorldX = ScreenToWorldX(x);
			double WorldY = ScreenToWorldY(y);
			if (IsGridEnabled())
			{
				::SnapToGrid(WorldX, GetGridSize());
				::SnapToGrid(WorldY, GetGridSize());
			}
			m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
			++m_nCurrentCreationControlPoint;
			ProbablyBreakCreationMode();
		}
	}
}

//virtual
void COpenGL_Editor2D::OnEvent_RightMouseButtonPressed(unsigned int uFlags, int x, int y)
{
	if (m_CurrentEditMode != NONE_EDIT_MODE)
		m_nCurrentCreationControlPoint = 0;
}

//virtual
void COpenGL_Editor2D::OnEvent_MouseMoved(unsigned int uFlags, int x, int y)
{
	if (IsEditingEnabled())
	{
		double WorldX = ScreenToWorldX(x);
		double WorldY = ScreenToWorldY(y);
		if (IsGridEnabled())
		{
			::SnapToGrid(WorldX, GetGridSize());
			::SnapToGrid(WorldY, GetGridSize());
		}
		
		if (m_pMovingControlPoint)
		{
			m_pMovingControlPoint->SetPosition(WorldX, WorldY);
			CEditorGeometry2D *pSelectedGeometry = m_Geometries.GetSelection();
			if (pSelectedGeometry)
			{
				pSelectedGeometry->RecalcControlPoints(m_pMovingControlPoint);
				pSelectedGeometry->GetGeometry()->RecalcExtents();
				RebuildDisplayList(pSelectedGeometry);
				OnGeometryChanged(pSelectedGeometry, m_pMovingControlPoint);
			}
			Redraw();
		}
		else if (m_CurrentEditMode != NONE_EDIT_MODE)
		{
			if (abs(x - m_LButtonPressedAtX) < 5 && abs(y - m_LButtonPressedAtY) < 5)
			{ // button released too close to the location where it was pressed so no new control point is created
			}
			else
			{
				m_CreationControlPoints[m_nCurrentCreationControlPoint].Set(WorldX, WorldY);
				Redraw();
			}
		}
	}
}

CEditorGeometry2D *COpenGL_Editor2D::GetGeometryAt(double WorldX, double WorldY)
{
	return m_Geometries.GetGeometryAt(WorldX, WorldY, 5*Get1PixelExtentsX());
}

CEditorGeometry2D *COpenGL_Editor2D::GetGeometryAt(int ScreenX, int ScreenY)
{
	double WorldX = ScreenToWorldX(ScreenX);
	double WorldY = ScreenToWorldY(ScreenY);
	return GetGeometryAt(WorldX, WorldY);
}

void COpenGL_Editor2D::ProbablyBreakCreationMode()
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

LOGICAL COpenGL_Editor2D::CanCreateNewObject()
{
	return (IsEditingEnabled() && !IsInDragScrollMode() && !IsInZoomRectangleMode());
}

void COpenGL_Editor2D::Init()
{
	Cleanup();
	m_Extents.Init(-1,1,-1,1);
	m_pMovingControlPoint = NULL;
	m_bGridEnabled = LOGICAL_FALSE;
}

void COpenGL_Editor2D::Cleanup()
{
	m_Geometries.SetSelection(NULL);
	OnCleanup();
	m_Geometries.DeleteAll();
}

CEditorPoint2D *COpenGL_Editor2D::CreatePoint(CPoint2D *pPoint)
{
	CEditorPoint2D *pNewPoint = new CEditorPoint2D(pPoint);
	AddPoint(pNewPoint);
	return pNewPoint;
}

CEditorPoint2D *COpenGL_Editor2D::CreatePoint(CPoint2D *pPoint, SRGBA *pRGBA)
{
	CEditorPoint2D *pNewPoint = new CEditorPoint2D(pPoint, pRGBA);
	AddPoint(pNewPoint);
	return pNewPoint;
}

CEditorPoint2D *COpenGL_Editor2D::CreatePoint(SVector2D &Point)
{
	CEditorPoint2D *pNewPoint = new CEditorPoint2D(Point);
	AddPoint(pNewPoint);
	return pNewPoint;
}

void COpenGL_Editor2D::AddPoint(CEditorPoint2D *pPoint)
{
	AddGeometry(pPoint);
	void *pUserData = OnPointCreated(pPoint);
	pPoint->SetUserData(pUserData);
}

CEditorLine2D *COpenGL_Editor2D::CreateLine(CLine2D *pLine)
{
	CEditorLine2D *pNewLine = new CEditorLine2D(pLine);
	AddLine(pNewLine);
	return pNewLine;
}

CEditorLine2D *COpenGL_Editor2D::CreateLine(CLine2D *pLine, SRGBA *pRGBA)
{
	CEditorLine2D *pNewLine = new CEditorLine2D(pLine, pRGBA);
	AddLine(pNewLine);
	return pNewLine;
}

CEditorLine2D *COpenGL_Editor2D::CreateLine(SVector2D &Point1, SVector2D &Point2)
{
	CEditorLine2D *pNewLine = new CEditorLine2D(Point1, Point2);
	AddLine(pNewLine);
	return pNewLine;
}

void COpenGL_Editor2D::AddLine(CEditorLine2D *pLine)
{
	AddGeometry(pLine);
	void *pUserData = OnLineCreated(pLine);
	pLine->SetUserData(pUserData);
}

CEditorRectangle2D *COpenGL_Editor2D::CreateRectangle(CRectangle2D *pRectangle)
{
	CEditorRectangle2D *pNewRectangle = new CEditorRectangle2D(pRectangle);
	AddRectangle(pNewRectangle);
	return pNewRectangle;
}

CEditorRectangle2D *COpenGL_Editor2D::CreateRectangle(CRectangle2D *pRectangle, SRGBA *pRGBA)
{
	CEditorRectangle2D *pNewRectangle = new CEditorRectangle2D(pRectangle, pRGBA);
	AddRectangle(pNewRectangle);
	return pNewRectangle;
}

CEditorRectangle2D *COpenGL_Editor2D::CreateRectangle(SVector2D &NWPoint, SVector2D &SEPoint)
{
	CEditorRectangle2D *pNewRectangle = new CEditorRectangle2D(NWPoint, SEPoint);
	AddRectangle(pNewRectangle);
	return pNewRectangle;
}

void COpenGL_Editor2D::AddRectangle(CEditorRectangle2D *pRectangle)
{
	AddGeometry(pRectangle);
	void *pUserData = OnRectangleCreated(pRectangle);
	pRectangle->SetUserData(pUserData);
}

void COpenGL_Editor2D::AddGeometry(CEditorGeometry2D *pGeometry)
{
	pGeometry->SetEditor(this);
	m_Geometries.push_back(pGeometry);
	pGeometry->GetGeometry()->RecalcExtents();
	pGeometry->GetGeometry()->UpdateExtents(m_Extents);
}

void COpenGL_Editor2D::SetSelection(CEditorGeometry2D *pGeometry, void *pWhoSelects)
{
	CEditorGeometry2D *pCurrentlySelected = m_Geometries.GetSelection();
	if (pCurrentlySelected == pGeometry)
		return; // do nothing, selected already
	m_Geometries.SetSelection(pGeometry);
	OnEventSelectionChanged(pGeometry, pWhoSelects);
	Redraw();
}

void COpenGL_Editor2D::SetSelection(CGeometry2D *pGeometry, void *pWhoSelects)
{
	CEditorGeometry2D *pCurrentlySelected = m_Geometries.GetSelection();
	if (pCurrentlySelected && pCurrentlySelected->GetGeometry() == pGeometry)
		return; // do nothing, selected already
	CEditorGeometry2D *pEditorGeometry = NULL;
	for (CListOfGeometries2D::iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		pEditorGeometry = *p;
		if (pEditorGeometry->GetGeometry() == pGeometry)
		{
			SetSelection(pEditorGeometry, pWhoSelects);
			break;
		}
	}
	OnEventSelectionChanged(pEditorGeometry, pWhoSelects);
	Redraw();
}

void COpenGL_Editor2D::DeleteGeometry(CGeometry2D *pGeometryToBeDeleted)
{
	m_Geometries.DeleteGeometry(pGeometryToBeDeleted);
	Redraw();
}

void COpenGL_Editor2D::UpdateExtents()
{
	m_Extents.Init();
	for (CListOfGeometries2D::iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pEditorGeometry = *p;
		pEditorGeometry->GetGeometry()->RecalcExtents();
		pEditorGeometry->GetGeometry()->UpdateExtents(m_Extents);
	}
}

void COpenGL_Editor2D::RebuildDisplayList(CEditorGeometry2D *pEditorGeometry)
{
	MakeGLContextCurrent();
	pEditorGeometry->RebuildDisplayList();
	MakeGLContextNotCurrent();
}

void COpenGL_Editor2D::RebuildDisplayList(CGeometry2D *pGeometry)
{
	for (CListOfGeometries2D::iterator p = m_Geometries.begin(); p != m_Geometries.end(); p++)
	{
		CEditorGeometry2D *pEditorGeometry = *p;
		if (pEditorGeometry->GetGeometry() == pGeometry)
			RebuildDisplayList(pEditorGeometry);
	}
}

void COpenGL_Editor2D::RebuildAllDisplayLists(LOGICAL bRedraw)
{
	MakeGLContextCurrent();
	m_Geometries.RebuildAllDisplayLists();
	MakeGLContextNotCurrent();
	if (bRedraw)
		Redraw();
}

