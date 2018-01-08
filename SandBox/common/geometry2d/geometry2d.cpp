/* Geometry2D.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	16 Mar 2009 - initial creation (moved out from OpenGL_Editor.cpp)
	20 Mar 2009 - CGeometry2D and CEditorGeometry2D are splitted
*/

#include "geometry2d.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

namespace common {

namespace geometry2d {

///////////////////////////////////////////////////////////////////////////////
// struct SVector2D

SVector2D::SVector2D()
{
	Set(0, 0);
}

SVector2D::SVector2D(double x, double y)
{
	Set(x, y);
}

double SVector2D::Distance2(double x, double y)
{
	double dx = m_fX - x;
	double dy = m_fY - y;
	return (dx*dx+dy*dy);
}

double SVector2D::Distance(double x, double y)
{
	return sqrt(Distance2(x, y)); 
}

double SVector2D::GetAbs()
{
    return sqrt(GetAbsSq());
}

void SVector2D::SnapToGrid(double fGridSize)
{
    common::calc::SnapToGrid(m_fX, fGridSize);
    common::calc::SnapToGrid(m_fY, fGridSize);
}

SVector2D operator + (const SVector2D &v1, const SVector2D &v2)
{
    SVector2D ret;
    ret.m_fX = v1.m_fX + v2.m_fX;
    ret.m_fY = v1.m_fY + v2.m_fY;
    return ret;
}

SVector2D operator - (const SVector2D &v1, const SVector2D &v2)
{
	SVector2D ret;
	ret.m_fX = v1.m_fX - v2.m_fX;
	ret.m_fY = v1.m_fY - v2.m_fY;
	return ret;
}

SVector2D operator * (const double &c1, const SVector2D &v2)
{
	SVector2D ret;
	ret.m_fX = c1 * v2.m_fX;
	ret.m_fY = c1 * v2.m_fY;
	return ret;
}

SVector2D operator * (const SVector2D &v1, const double c2)
{
	SVector2D ret;
	ret.m_fX = c2 * v1.m_fX;
	ret.m_fY = c2 * v1.m_fY;
	return ret;
}

SVector2D operator * (const double c1, const SVector2D &v2)
{
	SVector2D ret;
	ret.m_fX = c1 * v2.m_fX;
	ret.m_fY = c1 * v2.m_fY;
	return ret;
}

SVector2D operator / (const SVector2D &v1, const double &c2)
{
	SVector2D ret;
	ret.m_fX = v1.m_fX/c2;
	ret.m_fY = v1.m_fY/c2;
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// struct SExtents2D

void SExtents2D::Init(SVector2D &point)
{
	m_fLeft = m_fRight = point.X();
	m_fBottom = m_fTop = point.Y();
}

void SExtents2D::Init(double fLeft, double fRight, double fBottom, double fTop)
{
	m_fLeft = fLeft;
	m_fRight = fRight;
	m_fBottom = fBottom;
	m_fTop = fTop;
}
void SExtents2D::Init()
{
        m_fLeft = +common::calc::VERY_BIG_DOUBLE;
        m_fRight = -common::calc::VERY_BIG_DOUBLE;
        m_fBottom = +common::calc::VERY_BIG_DOUBLE;
        m_fTop = -common::calc::VERY_BIG_DOUBLE;
}

void SExtents2D::Update(double x, double y)
{
	if (x < m_fLeft)
		m_fLeft = x;
	if (y < m_fBottom)
		m_fBottom = y;
	if (y > m_fTop)
		m_fTop = y;
	if (x > m_fRight)
		m_fRight = x;
}

void SExtents2D::Update(SExtents2D &Extents)
{
	if (Extents.m_fLeft < m_fLeft)
		m_fLeft = Extents.m_fLeft;
	if (Extents.m_fBottom < m_fBottom)
		m_fBottom = Extents.m_fBottom;
	if (Extents.m_fTop > m_fTop)
		m_fTop = Extents.m_fTop;
	if (Extents.m_fRight > m_fRight)
		m_fRight = Extents.m_fRight;
}


///////////////////////////////////////////////////////////////////////////////
// class CGeometry2D

CGeometry2D::CGeometry2D()
{
	m_Extents.Init(0,0,0,0);
	m_bFilled = false;
}

//virtual
CGeometry2D::~CGeometry2D()
{
}

///////////////////////////////////////////////////////////////////////////////
// class CPoint2D

CPoint2D::CPoint2D()
{
	m_Point.Set(0,0);
}

CPoint2D::CPoint2D(SVector2D &point)
{
	Set(point);
}

//virtual
CPoint2D::~CPoint2D()
{
}

void CPoint2D::Init(CPoint2D *pPoint)
{
	if (pPoint)
		Set(pPoint);
}

//virtual
void CPoint2D::RecalcExtents()
{
	m_Extents.m_fLeft = m_Extents.m_fRight = m_Point.X();
	m_Extents.m_fBottom = m_Extents.m_fTop = m_Point.Y();
}

//virtual
double CPoint2D::CalcDistance(double x, double y)
{
	return m_Point.Distance(x, y);
}

//virtual
void CPoint2D::SnapToGrid(double fGridSize)
{
	m_Point.SnapToGrid(fGridSize);
	RecalcExtents();
}

//virtual
void CPoint2D::Format(std::string &str)
{
	char szData[64];
	sprintf(szData, "%s { (%le, %le) }", GetGeomTypeStr(), m_Point.X(), m_Point.Y());
	str = &szData[0];
}

//virtual
void CPoint2D::Unformat(const char *pszGeometry)
{
	double x = m_Point.X(), y = m_Point.Y();
	char szGeomType[128];
	if (3 == sscanf(pszGeometry, "%s { (%lf, %lf) }", szGeomType, &x, &y))
		Set(x, y);
}


///////////////////////////////////////////////////////////////////////////////
// class CLine2D

CLine2D::CLine2D()
{
	m_Point1.Set(0,0);
	m_Point2.Set(0,0);
}

CLine2D::CLine2D(SVector2D *pPoint1, SVector2D *pPoint2)
{
	SetPoints(pPoint1, pPoint2);
}

CLine2D::CLine2D(SVector2D &point1, SVector2D &point2)
{
	SetPoints(point1, point2);
}

//virtual
CLine2D::~CLine2D()
{
}

void CLine2D::Init(CLine2D *pLine)
{
	if (pLine)
		SetPoints(pLine->m_Point1, pLine->m_Point2);
}

void CLine2D::SetPoint1(SVector2D &point)
{
	m_Point1.Set(point);
	Recalc();
}

void CLine2D::SetPoint1(double x, double y)
{
	m_Point1.Set(x, y);
	Recalc();
}

void CLine2D::SetPoint2(SVector2D &point)
{
	m_Point2.Set(point);
	Recalc();
}

void CLine2D::SetPoint2(double x, double y)
{
	m_Point2.Set(x, y);
	Recalc();
}

void CLine2D::SetPoints(SVector2D *pPoint1, SVector2D *pPoint2)
{
	m_Point1.Set(pPoint1);
	m_Point2.Set(pPoint2);
	Recalc();
}

void CLine2D::SetPoints(SVector2D &point1, SVector2D &point2)
{
	m_Point1.Set(point1);
	m_Point2.Set(point2);
	Recalc();
}

void CLine2D::SetPoints(double x1, double y1, double x2, double y2)
{
	m_Point1.Set(x1, y1);
	m_Point2.Set(x2, y2);
	Recalc();
}

void CLine2D::RecalcAux()
{
	m_Center.Set((m_Point1.X()+m_Point2.X())/2., (m_Point1.Y()+m_Point2.Y())/2.);
	m_fLength = m_Point1.Distance(m_Point2);
	m_fAngle = atan2(m_Point2.Y()-m_Point1.Y(), m_Point2.X()-m_Point1.X());
	if (m_fLength < 1.E-20)
	{
		m_Sine = 0;
		m_Cosine = 1;
	}
	else
	{
		m_Sine = (m_Point2.Y()-m_Point1.Y())/m_fLength;
		m_Cosine = (m_Point2.X()-m_Point1.X())/m_fLength;
	}
	m_U1 = XtoU(m_Point1.X(), m_Point1.Y());
	m_U2 = XtoU(m_Point2.X(), m_Point2.Y());
	m_V = YtoV(m_Point1.X(), m_Point1.Y());
}

void CLine2D::Move(double fShiftX, double fShiftY)
{
	m_Point1.Move(fShiftX, fShiftY);
	m_Point2.Move(fShiftX, fShiftY);
	Recalc();
}

void CLine2D::SetLength(double fLength)
{
	if (fLength <= 0)
		return;
	SetLengthAngleRadians(fLength, m_fAngle);
}

void CLine2D::SetAngleRadians(double fAngle)
{
	SetLengthAngleRadians(m_fLength, fAngle);
}

void CLine2D::SetLengthAngleRadians(double fLength, double fAngle)
{
	double dX = fLength/2*cos(fAngle);
	double dY = fLength/2*sin(fAngle);

	m_Point1.Set(m_Center.X() - dX, m_Center.Y() - dY);
	m_Point2.Set(m_Center.X() + dX, m_Center.Y() + dY);
	Recalc();
}

void CLine2D::GetPointProjection(double X, double Y, double &XProj, double &YProj, double &H)
{
	double U, V;
	XYtoUV(X, Y, U, V);
	H = m_V - V; // signed(!!!) height
	UVtoXY(U, m_V, XProj, YProj);
}

//virtual
void CLine2D::RecalcExtents()
{
	if (m_Point1.X() < m_Point2.X())
	{
		m_Extents.m_fLeft = m_Point1.X();
		m_Extents.m_fRight = m_Point2.X();
	}
	else
	{
		m_Extents.m_fLeft = m_Point2.X();
		m_Extents.m_fRight = m_Point1.X();
	}
	
	if (m_Point1.Y() < m_Point2.Y())
	{
		m_Extents.m_fBottom = m_Point1.Y();
		m_Extents.m_fTop = m_Point2.Y();
	}
	else
	{
		m_Extents.m_fBottom = m_Point2.Y();
		m_Extents.m_fTop = m_Point1.Y();
	}
}

//virtual
double CLine2D::CalcDistance(double x, double y)
{
	return DistanceToLine(m_Point1, m_Point2, x, y);
}

//virtual
void CLine2D::SnapToGrid(double fGridSize)
{
	m_Point1.SnapToGrid(fGridSize);
	m_Point2.SnapToGrid(fGridSize);
	RecalcExtents();
}

//virtual
void CLine2D::Format(std::string &str)
{
	char szData[128];
	sprintf(szData, "%s { (%e, %e), (%e, %e) }", GetGeomTypeStr(), m_Point1.X(), m_Point1.Y(), m_Point2.X(), m_Point2.Y());
	str = szData;
}

//virtual
void CLine2D::Unformat(const char *pszGeometry)
{
	double x1 = m_Point1.X(), y1 = m_Point1.Y();
	double x2 = m_Point2.X(), y2 = m_Point2.Y();
	char szGeomType[128];
	if (5 == sscanf(pszGeometry, "%s { (%lf, %lf), (%lf, %lf) }", szGeomType, &x1, &y1, &x2, &y2))
		SetPoints(x1, y1, x2, y2);
}

///////////////////////////////////////////////////////////////////////////////
// class CRectangle2D

CRectangle2D::CRectangle2D()
{
	Init(0, 0, 0, 0);
}

CRectangle2D::CRectangle2D(SVector2D &point1, SVector2D &point2)
{
	Init(point1, point2);
}

//virtual
CRectangle2D::~CRectangle2D()
{
}

void CRectangle2D::Init(SVector2D &point1, SVector2D &point2)
{
	double xc = (point1.X() + point2.X())/2;
	double yc = (point1.Y() + point2.Y())/2;
	double width = fabs(point1.X() - point2.X());
	double height = fabs(point1.Y() - point2.Y());
	Init(xc, yc, width, height);
}

void CRectangle2D::Init(CRectangle2D *pRectangle)
{
	if (pRectangle)
		Init(pRectangle->m_PointNW, pRectangle->m_PointSE);
}

void CRectangle2D::Init(double xc, double yc, double width, double height)
{
	m_Center.Set(xc, yc);
	m_Width = width;
	m_Height = height;
	m_PointNW.Set(xc - width/2, yc + height/2);
	m_PointNE.Set(xc + width/2, yc + height/2);
	m_PointSW.Set(xc - width/2, yc - height/2);
	m_PointSE.Set(xc + width/2, yc - height/2);
	Recalc();
}

void CRectangle2D::Move(double fShiftX, double fShiftY)
{
	m_PointNE.Move(fShiftX, fShiftY);
	m_PointNW.Move(fShiftX, fShiftY);
	m_PointSE.Move(fShiftX, fShiftY);
	m_PointSW.Move(fShiftX, fShiftY);
	Recalc();
}

bool CRectangle2D::IsPointInside(double x, double y)
{
	return ((x > m_PointNW.X()) && (x < m_PointSE.X()) &&
            (y > m_PointSE.Y()) && (y < m_PointNW.Y()));
}

void CRectangle2D::RecalcLines()
{
	m_LineN.SetPoints(m_PointNW, m_PointNE);
	m_LineE.SetPoints(m_PointNE, m_PointSE);
	m_LineS.SetPoints(m_PointSE, m_PointSW);
	m_LineW.SetPoints(m_PointSW, m_PointNW);
}

/*void CRectangle2D::SetAngleRadians(double fAngle)
{
	SetLengthAngleRadians(m_fLength, fAngle);
}

void CRectangle2D::SetLengthAngleRadians(double fLength, double fAngle)
{
	double dX = fLength/2*cos(fAngle);
	double dY = fLength/2*sin(fAngle);

	m_Point1.Init(m_Center.X() - dX, m_Center.Y() - dY);
	m_Point2.Init(m_Center.X() + dX, m_Center.Y() + dY);
	Recalc();
}*/

//virtual
void CRectangle2D::RecalcExtents()
{
	m_Extents.Init(m_Center);
	m_Extents.Update(m_PointNW);
	m_Extents.Update(m_PointNE);
	m_Extents.Update(m_PointSW);
	m_Extents.Update(m_PointSE);
}

//virtual
double CRectangle2D::CalcDistance(double x, double y)
{
	double min_distance = DistanceToLine(m_PointNW, m_PointNE, x, y);
	double distance = DistanceToLine(m_PointNE, m_PointSE, x, y);
	if (distance < min_distance)
		min_distance = distance;
	distance = DistanceToLine(m_PointSE, m_PointSW, x, y);
	if (distance < min_distance)
		min_distance = distance;
	distance = DistanceToLine(m_PointSW, m_PointNW, x, y);
	if (distance < min_distance)
		min_distance = distance;
	return min_distance;
}

//virtual
void CRectangle2D::SnapToGrid(double fGridSize)
{
	m_PointNE.SnapToGrid(fGridSize);
	m_PointNW.SnapToGrid(fGridSize);
	m_PointSE.SnapToGrid(fGridSize);
	m_PointSW.SnapToGrid(fGridSize);
	double x = (m_PointNE.X() + m_PointSW.X())/2;
	double w = fabs(m_PointNE.X() - m_PointSW.X());
	double y = (m_PointNE.Y() + m_PointSW.Y())/2;
	double h = fabs(m_PointNE.Y() - m_PointSW.Y());
	Init(x, y, w, h);
}

//virtual
void CRectangle2D::Format(std::string &str)
{
	char szData[128];
	sprintf(szData, "%s { (%e, %e), %e, %e }", GetGeomTypeStr(), m_Center.X(), m_Center.Y(), m_Width, m_Height);
	str = szData;
}

//virtual
void CRectangle2D::Unformat(const char *pszGeometry)
{
	double xc, yc, width, height;
	char szGeomType[128];
	if (5 == sscanf(pszGeometry, "%s { (%lf, %lf), %lf, %lf }", szGeomType, &xc, &yc, &width, &height))
		Init(xc, yc, width, height);
}


// Misc functionality

double DistanceToLine(SVector2D &Point1, SVector2D &Point2, double x, double y)
{
	double a2 = Point1.Distance2(x, y);
	double b2 = Point2.Distance2(x, y);
	double c2 = Point1.Distance2(Point2);

	// ordinary triangle
	double foo1 = b2-c2-a2;
	double foo2 = a2-c2-b2;

	if (foo1 > 0 || foo2 > 0)
	{
                return sqrt(std::min(a2, b2));
	}
	else
	{
		double d2=foo1*foo1/(4.*c2);
		return sqrt(a2-d2);
	}
}

int GetIntervalIntersections(double u0, double u1, double v0, double v1, double w[2])
{
	if (u1 < v0 || u0 > v1)
		return 0;
	if (u1 > u0)
	{
		if (u0 < v1)
		{
			if (u0 < v0)
				w[0] = v0;
			else
				w[0] = u0;
			
			if (u1 > v1)
				w[1] = v1;
			else
				w[1] = u1;
			if (fabs(w[0]-w[1]) < 1.e-06)
				return 1; // really equal vertices
			return 2;
		}
		else
		{ // u0 == v1
			w[0] = u0;
			return 1;
		}
	}
	else
	{ // u1 == v0
		w[0] = u1;
		return 1;
	}
}

int LinesIntersection(double x11, double y11, double x12, double y12, double x21, double y21, double x22, double y22, SVector2D *pIntersectionPoints)
{
	if (x11 < x21 && x11 < x22 && x12 < x21 && x12 < x22)
		return 0;
	if (x11 > x21 && x11 > x22 && x12 > x21 && x12 > x22)
		return 0;
	if (y11 < y21 && y11 < y22 && y12 < y21 && y12 < y22)
		return 0;
	if (y11 > y21 && y11 > y22 && y12 > y21 && y12 > y22)
		return 0;

	// D1 vector
	double D1x = x12-x11;
	double D1y = y12-y11;

	// D2 vector
	double D2x = x22-x21;
	double D2y = y22-y21;

	// E vector
	double Ex = x21-x11;
	double Ey = y21-y11;

	const double epsilon = 1.E-6; //relative value
	double eps2 = (D1x*D1x + D1y*D1y)/1.E+12;
	double cross = D1x*D2y - D1y*D2x;
	if (cross > eps2 || cross < -eps2)
	{ // ordinary intersection
		double s = (Ex*D2y-Ey*D2x)/cross;
		if (s >= -epsilon && s <= (1.+epsilon))
		{
			double t = (Ex*D1y-Ey*D1x)/cross;
			if (t >= -epsilon && t <= (1.+epsilon))
			{
				if (pIntersectionPoints)
					pIntersectionPoints[0].Set(x11 + s*D1x, y11 + s*D1y);
				return 1;
			}
			else
				return 0;
		}
		else
			return 0;
	}
	else
	{ // lines are parallel
		cross = Ex*D1y - Ey*D1x;
		if (cross > eps2 || cross < -eps2)
		{ // lines are different
			return 0;
		}
		else
		{ // lines are the same
			double w[2];
			double Length1_sq = (x12-x11)*(x12-x11) + (y12-y11)*(y12-y11);
			double s0 = (D1x*Ex + D1y*Ey)/Length1_sq;
			double s1 = s0 + (D1x*D2x + D1y*D2y)/Length1_sq;
                        double smin = std::min(s0, s1);
                        double smax = std::max(s0, s1);
			int nIntersections = GetIntervalIntersections(0, 1, smin, smax, w);
			if (pIntersectionPoints)
			{
				for (int i = 0; i < nIntersections; i++)
					pIntersectionPoints[i].Set(x11 + w[i]*D1x, y11 + w[i]*D1y);
			}
			return nIntersections;
		}
	}
}

void NormalizeAngleRadians(double &angle)
{
	if (angle > 0)
	{
                while (angle > common::calc::PI2)
                        angle -= common::calc::PI2;
	}
	else
	{
		while (angle < 0)
                        angle += common::calc::PI2;
	}
}

void NormalizeAngleDegrees(double &angle)
{
	if (angle > 0)
	{
		while (angle > 360)
			angle -= 360;
	}
	else
	{
		while (angle < 0)
			angle += 360;
	}
}

} // namespace geometry2d

} // namespace common

