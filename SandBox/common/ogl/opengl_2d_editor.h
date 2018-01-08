/* opengl_2d_editor.h

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

#ifndef __OPEN_GL_2D_EDITOR_H_INCLUDED__
#define __OPEN_GL_2D_EDITOR_H_INCLUDED__

#include "opengl_2d_widget.h"
#include "portable/rgba.h"
#include "geometry2d/geometry2d.h"
#include <vector>

namespace common {

namespace ogl {

using namespace portable;
using namespace geometry2d;

// Forward declarations
class cOpenGL_2D_Editor_Widget;


class CControlPoint2D
{
//Constuctors/destructors
public:
	CControlPoint2D();
	virtual ~CControlPoint2D();

// Public methods
public:
	double X() { return m_pControllablePoint->X(); }
	double Y() { return m_pControllablePoint->Y(); }
	bool IsOverMe(double x, double y, cOpenGL_2D_Editor_Widget *pEditor);
	void SetControllablePoint(SVector2D *pPoint) { m_pControllablePoint = pPoint; }
	void SetPosition(double x, double y);

// Public overridables
public:
	virtual void Render(cOpenGL_2D_Editor_Widget *pEditor) = 0;
	virtual Qt::CursorShape GetCursorShape() = 0;

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	SVector2D *m_pControllablePoint;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CWeakControlPoint2D : public CControlPoint2D
{
//Constuctors/destructors
public:
	CWeakControlPoint2D();
	virtual ~CWeakControlPoint2D();

// Public methods
public:

// Public overridables
public:
	virtual void Render(cOpenGL_2D_Editor_Widget *pEditor);
	virtual Qt::CursorShape GetCursorShape() { return Qt::SizeAllCursor; }

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CPositionControlPoint2D : public CControlPoint2D
{
//Constuctors/destructors
public:
	CPositionControlPoint2D();
	virtual ~CPositionControlPoint2D();

// Public methods
public:

// Public overridables
public:
	virtual void Render(cOpenGL_2D_Editor_Widget *pEditor);
	virtual Qt::CursorShape GetCursorShape() { return Qt::SizeAllCursor; }

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CEditorGeometry2D
{
	friend class cOpenGL_Editor2D;
//Constuctors/destructors
public:
	CEditorGeometry2D();
	virtual ~CEditorGeometry2D();

// Public methods
public:
	CGeometry2D *GetGeometry() { return m_pGeometry; }
	double CalcDistance(double x, double y) { return GetGeometry()->CalcDistance(x, y); }
	void SetEditor(cOpenGL_2D_Editor_Widget *pEditor) { m_pEditor = pEditor; }
	void RebuildDisplayList();
	void Render();
	void UpdateExtents(SExtents2D &extents) { m_pGeometry->UpdateExtents(extents); }
	void ConnectToRGBA(SRGBA *pRGBA);
	SRGBA *GetRGBA() { return m_pConnectedRGBA; }
	bool IsFilled() { return m_pGeometry->IsFilled(); }
	void SetFilled(bool bFilled = true) { m_pGeometry->SetFilled(bFilled); }
	void *GetUserData() { return m_pUserData; }
	void SetUserData(void *pUserData) { m_pUserData = pUserData; }
	void RenderControlPoints(cOpenGL_2D_Editor_Widget *pEditor);
	Qt::CursorShape GetCursorShape(double x, double y, cOpenGL_2D_Editor_Widget *pEditor);

// Public overridables
public:
	virtual void OnRealRendering() = 0;
	virtual int GetControlPointCount() { return 0; }
	virtual CControlPoint2D *GetControlPoint(int iPoint) { return NULL; }
	virtual void RecalcControlPoints(CControlPoint2D *pSourcePoint) { }

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	cOpenGL_2D_Editor_Widget *m_pEditor;
	CGeometry2D *m_pGeometry;
	SRGBA m_StubRGBA;
	SRGBA *m_pConnectedRGBA;
	GLuint m_DisplayList;
	void *m_pUserData;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};
typedef std::vector<CEditorGeometry2D *> cEditorGeometries;

class CEditorPoint2D : public CEditorGeometry2D
{
//Constuctors/destructors
public:
	CEditorPoint2D(SVector2D &Point);
	CEditorPoint2D(CPoint2D *pPoint);
	CEditorPoint2D(CPoint2D *pPoint, SRGBA *pRGBA);
	virtual ~CEditorPoint2D();

// Public methods
public:
	void ConnectToPoint(CPoint2D *pPoint);
	CPoint2D *GetConnectedPoint() { return m_pConnectedPoint; }
	double X() { return m_ControlPoint.X(); }
	double Y() { return m_ControlPoint.Y(); }

// Public overridables
public:
	virtual void OnRealRendering();
	virtual int GetControlPointCount() { return 1; }
	virtual CControlPoint2D *GetControlPoint(int iPoint) { return &m_ControlPoint; }

// Public members
public:

// Protected methods
protected:


// Protected overridables
protected:

// Protected members
protected:
	CPositionControlPoint2D m_ControlPoint;
	CPoint2D *m_pConnectedPoint;
	CPoint2D m_StubPoint;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CEditorLine2D : public CEditorGeometry2D
{
//Constuctors/destructors
public:
	CEditorLine2D(SVector2D &Point1, SVector2D &Point2);
	CEditorLine2D(CLine2D *pLine);
	CEditorLine2D(CLine2D *pLine, SRGBA *pRGBA);
	virtual ~CEditorLine2D();

// Public methods
public:
	void ConnectToLine(CLine2D *pLine);
	CLine2D *GetConnectedLine() { return m_pConnectedLine; }

// Public overridables
public:
	virtual void OnRealRendering();
	virtual int GetControlPointCount() { return 3; }
	virtual CControlPoint2D *GetControlPoint(int iPoint);
	virtual void RecalcControlPoints(CControlPoint2D *pSourcePoint);

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CWeakControlPoint2D m_Point1, m_Point2;
	CPositionControlPoint2D m_Center;
	CLine2D *m_pConnectedLine;
	CLine2D m_StubLine;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CEditorRectangle2D : public CEditorGeometry2D
{
//Constuctors/destructors
public:
	CEditorRectangle2D(SVector2D &NWPoint, SVector2D &SEPoint);
	CEditorRectangle2D(CRectangle2D *pRectangle);
	CEditorRectangle2D(CRectangle2D *pRectangle, SRGBA *pRGBA);
	virtual ~CEditorRectangle2D();

// Public methods
public:
	void ConnectToRectangle(CRectangle2D *pRectangle);
	CRectangle2D *GetConnectedRectangle() { return m_pConnectedRectangle; }

// Public overridables
public:
	virtual void OnRealRendering();
	virtual int GetControlPointCount() { return 5; }
	virtual CControlPoint2D *GetControlPoint(int iPoint);
	virtual void RecalcControlPoints(CControlPoint2D *pSourcePoint);

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CWeakControlPoint2D m_PointSW, m_PointSE, m_PointNW, m_PointNE;
	CPositionControlPoint2D m_Center;
	CRectangle2D *m_pConnectedRectangle;
	CRectangle2D m_StubRectangle;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};


typedef enum
{
	NONE_EDIT_MODE,
	POINT_EDIT_MODE,
	LINE_EDIT_MODE,
	RECTANGLE_EDIT_MODE,
	CIRCLE_3POINTS_EDIT_MODE,
	CIRCLE_DIAMETER_EDIT_MODE,
	CIRCLE_CENTER_RADIUS_EDIT_MODE
} EDIT_MODE_ENUM;

class cOpenGL_2D_Editor_Widget : public cOpenGL_2D_Widget
{
    Q_OBJECT    
	typedef cOpenGL_2D_Widget inherited;
    
// Construction/destruction
public:
	cOpenGL_2D_Editor_Widget(QWidget *pParent = NULL);
	virtual ~cOpenGL_2D_Editor_Widget();

// Public overridable methods
public:
	virtual void onSetDefaultOptions();

	// Cursor
	virtual Qt::CursorShape GetCursorShape();

// Protected overridable methods
protected:
	virtual void mousePressEvent(QMouseEvent *pEvent);
	virtual void mouseMoveEvent(QMouseEvent *pEvent);
	virtual void mouseReleaseEvent(QMouseEvent *pEvent);
	virtual void mouseDoubleClickEvent(QMouseEvent *pEvent);

	virtual void onRenderScene();
	virtual QColor getEditRubberColor() const { return Qt::gray; }
	virtual double GetGridSize() const { return GetWorldSizeX()/20.; }
	virtual QColor getGridColor() const { return Qt::lightGray; }

	// Geometry creation/changing
	// (OnXXXCreated methods return user data which will be stored with CEditorGeometry2D object)
	virtual void *OnPointCreated(CEditorPoint2D *pPoint) { return NULL; }
	virtual void *OnLineCreated(CEditorLine2D *pLine) { return NULL; }
	virtual void *OnRectangleCreated(CEditorRectangle2D *pRectangle) { return NULL; }
	virtual void OnGeometryChanged(CEditorGeometry2D *pGeometry, CControlPoint2D *pControlPoint) {}

	// Event handlers
	virtual void OnCleanup() {}
	virtual void OnEventSelectionChanged(CEditorGeometry2D *pSelectedGeometry, void *pWhere) {}

// Public methods
public:
	void Init();
	void Cleanup();

	CEditorPoint2D *CreatePoint(CPoint2D *pPoint);
	CEditorPoint2D *CreatePoint(CPoint2D *pPoint, SRGBA *pRGBA);
	CEditorPoint2D *CreatePoint(SVector2D &Point);

	CEditorLine2D *CreateLine(CLine2D *pLine);
	CEditorLine2D *CreateLine(CLine2D *pLine, SRGBA *pRGBA);
	CEditorLine2D *CreateLine(SVector2D &Point1, SVector2D &Point2);

	CEditorRectangle2D *CreateRectangle(CRectangle2D *pRectangle);
	CEditorRectangle2D *CreateRectangle(CRectangle2D *pRectangle, SRGBA *pRGBA);
	CEditorRectangle2D *CreateRectangle(SVector2D &NWPoint, SVector2D &SEPoint);

	void AddGeometry(CEditorGeometry2D *pGeometry);
	CEditorGeometry2D *GetSelection() { return m_pSelectedGeometry; }
	void SetSelection(CEditorGeometry2D *pGeometry, void *pWhoSelects);
	void SetSelection(CGeometry2D *pGeometry, void *pWhoSelects);
	void DeleteGeometry(CGeometry2D *pGeometryToBeDeleted);

	bool IsEditingEnabled() { return m_bEditingEnabled; }
	void EnableEditing(bool bEnable) { m_bEditingEnabled = bEnable; redraw(); }
	void ToggleEditingMode() { EnableEditing(!IsEditingEnabled()); }
	void SetEditMode(EDIT_MODE_ENUM mode) { m_CurrentEditMode = mode; m_nCurrentCreationControlPoint = 0; }
	EDIT_MODE_ENUM GetEditMode() { return m_CurrentEditMode; }
	bool IsGridEnabled() { return m_bGridEnabled; }
	void EnableGrid(bool bEnable) { m_bGridEnabled = bEnable; redraw(); }
	void ToggleGrid() { m_bGridEnabled = !m_bGridEnabled; redraw(); }

	void RebuildDisplayList(CEditorGeometry2D *pEditorGeometry);
	void RebuildDisplayList(CGeometry2D *pGeometry);
	void RebuildAllDisplayLists(bool bRedraw);
	void UpdateExtents();

// Protected methods
protected:
	CEditorGeometry2D *findGeometry(CGeometry2D *pGeometry);
	CEditorGeometry2D *getGeometryAt(double WorldX, double WorldY);
	CEditorGeometry2D *getGeometryAt(int ScreenX, int ScreenY);
	void ProbablyBreakCreationMode();
	bool CanCreateNewObject();
	void addPoint(CEditorPoint2D *pPoint);
	void addLine(CEditorLine2D *pLine);
	void addRectangle(CEditorRectangle2D *pRectangle);

// Private methods
private:

// Public members
public:

// Protected members
protected:
	CEditorGeometry2D *m_pSelectedGeometry;
	cEditorGeometries m_Geometries;

	SExtents2D m_Extents;
	bool m_bEditingEnabled;
	bool m_bGridEnabled;
	double m_fGridSize;
	CControlPoint2D *m_pMovingControlPoint;
	EDIT_MODE_ENUM m_CurrentEditMode;
	SVector2D m_CreationControlPoints[4];
	int m_nCurrentCreationControlPoint;

// Private members
private:
};


// Misc functionality

} // namespace ogl

} // namespace common


#endif //__OPEN_GL_2D_EDITOR_H_INCLUDED__

