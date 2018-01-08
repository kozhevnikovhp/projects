/* Geometry2D.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	16 Mar 2009 - initial creation (moved out from OpenGL_Editor.h)
	20 Mar 2009 - CGeometry2D and CEditorGeometry2D are splitted
*/

#ifndef __GEOMETRY2D_H_INCLUDED__
#define __GEOMETRY2D_H_INCLUDED__

#include "portable/portable.h"
#include "calc/calc.h"
#include <string>

namespace common {

namespace geometry2d {

// Forward declarations


typedef struct SVector2D
{
public:
	SVector2D();
	SVector2D(double x, double y);
	void Set(const double x, const double y) { m_fX = x; m_fY = y; }
	void Set(const SVector2D *pPoint) { Set(pPoint->m_fX, pPoint->m_fY); }
	void Set(const SVector2D &point) { Set(point.m_fX, point.m_fY); }
	void Zero() { Set(0, 0); }
	double GetAbsSq() { return (m_fX*m_fX+m_fY*m_fY); }
	double GetAbs();
	void Move(double dx, double dy) { m_fX += dx; m_fY += dy; }
	double X() { return m_fX; }
	double Y() { return m_fY; }
	double Distance2(double x, double y);
	double Distance2(SVector2D &point) { return Distance2(point.X(), point.Y()); }
	double Distance(double x, double y);
	double Distance(SVector2D &point) { return Distance(point.X(), point.Y()); }
	void SnapToGrid(double fGridSize);

    SVector2D& operator = (const SVector2D &v) { Set(v); return *this; };
	friend SVector2D operator + (const SVector2D &v1, const SVector2D &v2);
	friend SVector2D operator - (const SVector2D &v1, const SVector2D &v2);
	friend SVector2D operator * (const SVector2D &v1, const double c2);
	friend SVector2D operator * (const double c1, const SVector2D &v2);
	friend SVector2D operator / (const SVector2D &v1, const double &v2);
	void operator += (const SVector2D &v) { m_fX += v.m_fX;	m_fY += v.m_fY; }
	void operator -= (const SVector2D &v) { m_fX -= v.m_fX;	m_fY -= v.m_fY; }
	void operator *= (const double c) { m_fX *= c;	m_fY *= c; }
	void operator /= (const double c) { m_fX /= c;	m_fY /= c; }

// Public members
public:
	double m_fX, m_fY;

// Protected members
protected:
} SVector2D;


typedef struct SExtents2D
{
public:
	void Init(SVector2D &point);
	void Init(double fLeft, double fRight, double fBottom, double fTop);
	void Init();
	bool IsInitialized() const { return (m_fLeft < m_fRight) && (m_fBottom < m_fTop); }
	void Update(double x, double y);
	void Update(SVector2D &point) { Update(point.X(), point.Y()); }
	void Update(SExtents2D &Extents);
	double GetSizeX() { return (m_fRight - m_fLeft); }
	double GetCenterX() { return (m_fRight + m_fLeft)/2; }
	double GetSizeY() { return (m_fTop - m_fBottom); }
	double GetCenterY() { return (m_fTop + m_fBottom)/2; }
	void Expand(double fExpand) { m_fLeft -= fExpand; m_fRight += fExpand; m_fBottom -= fExpand; m_fTop += fExpand; }
	bool IsInside(SVector2D &position) { return IsInside(position.X(), position.Y()); }
	bool IsInside(double x, double y) { return x >= m_fLeft && x <= m_fRight && y >= m_fBottom && y <= m_fTop; }

	double m_fLeft, m_fRight, m_fTop, m_fBottom;

// Protected members
protected:
	
} SExtents2D;

class CGeometry2D
{
//Constuctors/destructors
public:
	CGeometry2D();
	virtual ~CGeometry2D();

// Public methods
public:
	void UpdateExtents(SExtents2D &extents) { extents.Update(m_Extents); }
	bool IsFilled() { return m_bFilled; }
	void SetFilled(bool bFilled = true) { m_bFilled = bFilled; }
	void Unformat(std::string &str) { Unformat(str.c_str()); }

// Public overridables
public:
	virtual const char *GetGeomTypeStr() = 0;
	virtual void RecalcExtents() = 0;
	virtual double CalcDistance(double x, double y) = 0;
	virtual void SnapToGrid(double fGridSize) = 0;
	virtual void Format(std::string &str) = 0;
	virtual void Unformat(const char *pszGeometry) = 0;

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	SExtents2D m_Extents;
	bool m_bFilled;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CPoint2D : public CGeometry2D
{
//Constuctors/destructors
public:
	CPoint2D();
	CPoint2D(SVector2D &point);
	virtual ~CPoint2D();

// Public methods
public:
	void Init(CPoint2D *pPoint);
	void Set(SVector2D &point) { Set(point.X(), point.Y()); }
	void Set(double x, double y) { m_Point.Set(x, y); }
	void Set(CPoint2D *pPoint) { m_Point.Set(pPoint->X(), pPoint->Y()); }
	SVector2D *GetPoint() { return &m_Point; }
	double X() { return m_Point.X(); }
	double Y() { return m_Point.Y(); }

// Public overridables
public:
	virtual const char *GetGeomTypeStr() { return "POINT_2D"; }
	virtual void RecalcExtents();
	virtual double CalcDistance(double x, double y);
	virtual void SnapToGrid(double fGridSize);
	virtual void Format(std::string &str);
	virtual void Unformat(const char *pszGeometry);

// Public members
public:

// Protected methods
protected:


// Protected overridables
protected:

// Protected members
protected:
	SVector2D m_Point;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CLine2D : public CGeometry2D
{
//Constuctors/destructors
public:
	CLine2D();
	CLine2D(SVector2D *pPoint1, SVector2D *pPoint2);
	CLine2D(SVector2D &point1, SVector2D &point2);
	virtual ~CLine2D();

// Public methods
public:
	void Init(CLine2D *pLine);
	void SetPoint1(SVector2D &point);
	void SetPoint1(double x, double y);
	void SetPoint2(SVector2D &point);
	void SetPoint2(double x, double y);
	void SetPoints(SVector2D *pPoint1, SVector2D *pPoint2);
	void SetPoints(SVector2D &point1, SVector2D &point2);
	void SetPoints(double x1, double y1, double x2, double y2);
	double X1() { return m_Point1.X(); }
	double Y1() { return m_Point1.Y(); }
	double X2() { return m_Point2.X(); }
	double Y2() { return m_Point2.Y(); }
	double XCenter() { return m_Center.X(); }
	double YCenter() { return m_Center.Y(); }
	double GetLength() { return m_fLength; }
	SVector2D *GetPoint1() { return &m_Point1; }
	SVector2D *GetPoint2() { return &m_Point2; }
	SVector2D *GetCenterPoint() { return &m_Center; }
	double GetAngleRadians() { return m_fAngle; } // in radians
    double GetAngleDegrees() { return common::calc::RADIANS_2_DEGREES(m_fAngle); } // in degrees
	void Move(double fShiftX, double fShiftY);
	void SetLength(double fLength);
	void SetAngleRadians(double fAngle);
    void SetAngleDegrees(double fAngle) { SetAngleRadians(common::calc::DEGREES_2_RADIANS(fAngle)); }
	void SetLengthAngleRadians(double fLength, double fAngle);
    void SetLengthAngleDegrees(double fLength, double fAngle) { SetLengthAngleRadians(fLength, common::calc::DEGREES_2_RADIANS(fAngle)); }
	void Recalc() { RecalcAux(); RecalcExtents(); }
	void RecalcAux();
	void GetPointProjection(double X, double Y, double &XProj, double &YProj, double &H);
	double GetPointHeightAbove(double X, double Y) { return (YtoV(X, Y) - m_V); } // signed(!!!) height

// Public overridables
public:
    virtual const char *GetGeomTypeStr() { return "LINE_2D"; }
	virtual void RecalcExtents();
	virtual double CalcDistance(double x, double y);
	virtual void SnapToGrid(double fGridSize);
	virtual void Format(std::string &str);
	virtual void Unformat(const char *pszGeometry);

// Public members
public:

// Protected methods
protected:
	double XtoU(const double X, const double Y) { return +X*m_Cosine + Y*m_Sine; }
	double YtoV(const double X, const double Y) { return -X*m_Sine + Y*m_Cosine; }
	void XYtoUV(const double X, const double Y, double &U, double &V) { U = XtoU(X, Y); V = YtoV(X, Y); }
	void UtoX(const double U, const double V, double &X) { X = +U*m_Cosine - V*m_Sine; }
	void VtoY(const double U, const double V, double &Y) { Y = +U*m_Sine + V*m_Cosine; }
	void UVtoXY(const double U, const double V, double &X, double &Y) { UtoX(U, V, X); VtoY(U, V, Y); }

// Protected overridables
protected:

// Protected members
protected:
	SVector2D m_Point1, m_Point2;
	SVector2D m_Center;
	double m_fLength;
	double m_fAngle;
	double m_Sine, m_Cosine;
	double m_U1, m_U2, m_V;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CRectangle2D : public CGeometry2D
{
//Constuctors/destructors
public:
	CRectangle2D();
	CRectangle2D(SVector2D &point1, SVector2D &point2);
	virtual ~CRectangle2D();

// Public methods
public:
	void Init(SVector2D &point1, SVector2D &point2);
	void Init(CRectangle2D *pRectangle);
	void Init(double xc, double yc, double width, double height);
	double XCenter() { return m_Center.X(); }
	double YCenter() { return m_Center.Y(); }
	double XNE() { return m_PointNE.X(); }
	double YNE() { return m_PointNE.Y(); }
	double XNW() { return m_PointNW.X(); }
	double YNW() { return m_PointNW.Y(); }
	double XSE() { return m_PointSE.X(); }
	double YSE() { return m_PointSE.Y(); }
	double XSW() { return m_PointSW.X(); }
	double YSW() { return m_PointSW.Y(); }
	double Width() { return m_Width; }
	double Height() { return m_Height; }
	SVector2D *GetPointNW() { return &m_PointNW; }
	SVector2D *GetPointNE() { return &m_PointNE; }
	SVector2D *GetPointSW() { return &m_PointSW; }
	SVector2D *GetPointSE() { return &m_PointSE; }
	SVector2D *GetCenterPoint() { return &m_Center; }
	CLine2D *GetSideN() { return &m_LineN; }
	CLine2D *GetSideE() { return &m_LineE; }
	CLine2D *GetSideS() { return &m_LineS; }
	CLine2D *GetSideW() { return &m_LineW; }
//	double GetAngleRadians() { return m_fAngle; } // in radians
//	double GetAngleDegrees() { return RADIANS_2_DEGREES(m_fAngle); } // in degrees
	void Move(double fShiftX, double fShiftY);
//	void SetAngleRadians(double fAngle);
//	void SetAngleDegrees(double fAngle) { SetAngleRadians(DEGREES_2_RADIANS(fAngle)); }
	void Recalc() { RecalcExtents(); RecalcLines(); }
	bool IsPointInside(double x, double y);

// Public overridables
public:
	virtual const char *GetGeomTypeStr() { return "RECTANGLE_2D"; }
	virtual void RecalcExtents();
	virtual double CalcDistance(double x, double y);
	virtual void SnapToGrid(double fGridSize);
	virtual void Format(std::string &str);
	virtual void Unformat(const char *pszGeometry);

// Public members
public:

// Protected methods
protected:
	void RecalcLines();

// Protected overridables
protected:

// Protected members
protected:
	SVector2D m_PointSW, m_PointSE, m_PointNW, m_PointNE;
	SVector2D m_Center;
	CLine2D m_LineW, m_LineS, m_LineE, m_LineN;
	double m_Width, m_Height;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};


// Misc

double DistanceToLine(SVector2D &Point1, SVector2D &Point2, double x, double y);
inline double DistanceToLine(SVector2D &Point1, SVector2D &Point2, SVector2D &Point)
{
	return DistanceToLine(Point1, Point2, Point.X(), Point.Y());
}

// Lines intersection
int LinesIntersection(double x11, double y11, double x12, double y12, double x21, double y21, double x22, double y22, SVector2D *pIntersectionPoints);

void NormalizeAngleRadians(double &angle);
void NormalizeAngleDegrees(double &angle);

} // namespace geometry2d

} // namespace common

#endif //__GEOMETRY2D_H_INCLUDED__
