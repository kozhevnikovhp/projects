/* OpenGLRenderer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	21 Jun 2007 - COpenGL_Chart2D class added

*/

#ifndef __OPEN_GL_RENDERER_H_INCLUDED__
#define __OPEN_GL_RENDERER_H_INCLUDED__

#include <gl\gl.h>
#include <gl\glaux.h>
#include "3DRenderer.h"

extern unsigned char ucWhiteColor[4];
extern unsigned char ucBlackColor[4];
extern unsigned char ucGrayColor[4];
extern unsigned char ucBrightRedColor[4];
extern unsigned char ucRedColor[4];
extern unsigned char ucBrightGreenColor[4];
extern unsigned char ucGreenColor[4];
extern unsigned char ucBrightBlueColor[4];
extern unsigned char ucBlueColor[4];
extern unsigned char ucYellowColor[4];

class COpenGL
{
// Constructors/destructors
public:
	COpenGL();
	virtual ~COpenGL();

// Public overridable methods
public:
	virtual void OnSetDefaultOptions() {}
	virtual void OnInitialized() {}

// Protected overridable methods
protected:
	virtual unsigned char *GetBackgroundColor() { return ucBlackColor; }

// Public methods
public:
	void StoreModelViewMatrix() { glMatrixMode(GL_MODELVIEW); glPushMatrix(); }
	void RestoreModelViewMatrix() { glMatrixMode(GL_MODELVIEW); glPopMatrix(); }
	void Draw2DText(const char *pszText);

// Protected methods
protected:
	LOGICAL InitGL(HDC hDC);
	void FinitGL();
	LOGICAL SetWindowPixelFormat();
	LOGICAL CreateViewGLContext();
	LOGICAL MakeGLContextCurrent();
	LOGICAL MakeGLContextNotCurrent();
	unsigned int Create2DRasterFont(const char *pszFontName, int height);
	unsigned int CreateVectorFont(const char *pszFontName, int height, GLYPHMETRICSFLOAT *pGlyphMetrics);

// Private methods
private:

// Protected members
protected:
	HDC m_hDC;
	HGLRC m_hGLContext;
// Private members
private:
};

typedef	enum 
{
	MouseClickDrag_Select,
	MouseClickDrag_ZoomRectangle,
	MouseClickDrag_DragScroll
} MouseDragTypeEnum;

class COpenGL_2D : public CRenderer, public COpenGL
{
// Constructors/destructors
public:
	COpenGL_2D();
	virtual ~COpenGL_2D();

// Public overridable methods
public:
	virtual LOGICAL Init(HDC hDC);
	virtual void Finit();
	virtual void SetDefaultView();
	virtual void Redraw();
	virtual void OnSize(int cx, int cy);
	virtual void OnLButtonDown(unsigned int uFlags, int x, int y);
	virtual void OnLButtonUp(unsigned int uFlags, int x, int y);
	virtual void OnRButtonDown(unsigned int uFlags, int x, int y);
	virtual void OnRButtonUp(unsigned int uFlags, int x, int y);
	virtual void OnMouseMove(unsigned int uFlags, int x, int y);
	virtual LOGICAL IsMouseMovePanXEnabled() { return LOGICAL_TRUE; }
	virtual LOGICAL IsMouseMovePanYEnabled() { return LOGICAL_TRUE; }

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return 2; }
	virtual double GetWorldSizeY() { return 2; }
	virtual double GetDefaultWorldCenterX() { return 0; }
	virtual double GetDefaultWorldCenterY() { return 0; }
	virtual double GetWorldScaleX() { return 1; }
	virtual double GetWorldScaleY() { return 1; }
	virtual unsigned char *GetZoomRectangleColor();
	virtual void OnRenderScene();
	virtual LOGICAL IsIsotropic() { return LOGICAL_TRUE; }
	// Event handlers
	virtual void OnEvent_SizeChanged(int cx, int cy) { }
	virtual void OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y) { }
	virtual void OnEvent_LeftMouseButtonReleased(unsigned int uFlags, int x, int y) { }
	virtual void OnEvent_RightMouseButtonPressed(unsigned int uFlags, int x, int y) { }
	virtual void OnEvent_RightMouseButtonReleased(unsigned int uFlags, int x, int y) { }
	virtual void OnEvent_MouseMoved(unsigned int uFlags, int x, int y) { }


// Public methods
public:
	void SetSelectMode() { m_MouseClickDragType = MouseClickDrag_Select; }
	LOGICAL IsInSelectMode() { return m_MouseClickDragType == MouseClickDrag_Select; }
	void SetDragScrollMode() { m_MouseClickDragType = MouseClickDrag_DragScroll; }
	LOGICAL IsInDragScrollMode() { return m_MouseClickDragType == MouseClickDrag_DragScroll; }
	void SetZoomRectangleMode() { m_MouseClickDragType = MouseClickDrag_ZoomRectangle; }
	LOGICAL IsInZoomRectangleMode(){ return m_MouseClickDragType == MouseClickDrag_ZoomRectangle; }

	void IncreaseZoomX(double fIncreaseBy, LOGICAL bRedraw = LOGICAL_TRUE);
	void IncreaseZoomY(double fIncreaseBy, LOGICAL bRedraw = LOGICAL_TRUE);
	void IncreaseZoom(double fIncreaseBy, LOGICAL bRedraw = LOGICAL_TRUE);
	double MostLeftVisibleX();
	double MostRightVisibleX();
	double MostTopVisibleY();
	double MostBottomVisibleY();
	double Get1PixelExtentsX() { return m_1Pixel2WorldX; }
	double Get1PixelExtentsY() { return m_1Pixel2WorldY; }

// Protected methods
protected:
	void SetupCamera();
	LOGICAL IsLDragging() { return m_bLDragging; }
	double GetFrustrumHeight() { return m_Frustrum.height; }
	double GetFrustrumWidth() { return m_Frustrum.width; }
	double GetFrustrumTop() { return m_Frustrum.top; }
	double GetFrustrumBottom() { return m_Frustrum.bottom; }
	double GetFrustrumRight() { return m_Frustrum.right; }
	double GetFrustrumLeft() { return m_Frustrum.left; }
	double ScreenToWorldX(int ScreenCoord);
	double ScreenToWorldY(int ScreenCoord);
	int WorldToScreenX(double WorldCoord);
	int WorldToScreenY(double WorldCoord);

// Private methods
private:
	void CalcWorld2ScreenTranslationX(double &a, double &b);
	void CalcWorld2ScreenTranslationY(double &a, double &b);

// Protected members
protected:
	int m_WindowWidth, m_WindowHeight;		// window sizes
	int m_MouseCurrentPosX, m_MouseCurrentPosY;
	int m_LButtonPressedAtX, m_LButtonPressedAtY;
	
	MouseDragTypeEnum m_MouseClickDragType;

// Private members
private:
	double m_fAspectRatio;
	double m_fWorldShiftDeltaX, m_fWorldShiftDeltaY;
	double m_fWorldZoomX, m_fWorldZoomY; // internal zooming, multiplied by custom scaling
	double m_fWorldShiftX, m_fWorldShiftY;
	LOGICAL m_bLDragging;
	struct 
	{
		double left, right, top, bottom;
		double height, width;
	} m_Frustrum;
	double m_1Pixel2WorldX, m_1Pixel2WorldY; // how wide and high 1 pixel on screen extents in the world ccords
};

class COpenGL_3D : public C3DRenderer, public COpenGL
{
// Constructors/destructors
public:
	COpenGL_3D();
	virtual ~COpenGL_3D();

// Public overridable methods
public:
	virtual LOGICAL Init(HDC hDC);
	virtual void Finit();
	virtual void SetDefaultView();
	virtual void Redraw();
	virtual void OnSetDefaultOptions();
	virtual void OnSize(int cx, int cy);
	virtual void OnLButtonDown(unsigned int nFlags, int x, int y);
	virtual void OnLButtonUp(unsigned int uFlags, int x, int y);
	virtual void OnRButtonDown(unsigned int nFlags, int x, int y);
	virtual void OnRButtonUp(unsigned int uFlags, int x, int y);
	virtual void OnMouseMove(unsigned int uFlags, int x, int y);

// Protected overridable methods
protected:
	virtual void OnCameraPosChanged();
	virtual void OnCameraDirChanged();
	virtual void OnRenderScene();
	// Event handlers
	virtual void OnEvent_SizeChanged(int cx, int cy) { }
	virtual void OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y) { }
	virtual void OnEvent_LeftMouseButtonReleased(unsigned int uFlags, int x, int y) { }
	virtual void OnEvent_MouseMoved(unsigned int uFlags, int x, int y) { }


// Public methods
public:

// Protected methods
protected:	void SetupCamera();

// Private methods
private:

// Protected members
protected:
	int m_WindowWidth, m_WindowHeight;		// window sizes

// Private members
private:
};

class COpenGL_Chart2D : public COpenGL_2D
{
	
// Construction/destruction
public:
	COpenGL_Chart2D();
	virtual ~COpenGL_Chart2D();

// Public overridable methods
public:
	virtual void OnRenderScene();
	virtual void OnInitialized();
	virtual void OnSetDefaultOptions();
	virtual LOGICAL IsIsotropic() { return LOGICAL_FALSE; }

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return (GetMaxXValue() - GetMinXValue()); }
	virtual double GetMinXValue() { return 0.; } // Should be overriden in derived classes according to available chart data
	virtual double GetMaxXValue() { return 1.; } // Should be overriden in derived classes according to available chart data
	virtual double GetWorldSizeY() { return (GetMaxYValue() - GetMinYValue()); }
	virtual double GetMinYValue() { return 0.; } // Should be overriden in derived classes according to available chart data
	virtual double GetMaxYValue() { return 1.; } // Should be overriden in derived classes according to available chart data
	virtual double GetDefaultWorldCenterX() { return (GetMaxXValue() + GetMinXValue())/2; }
	virtual double GetDefaultWorldCenterY() { return (GetMaxYValue() + GetMinYValue())/2; }
	virtual void RenderVerticalGrid();
	virtual void RenderHorizontalGrid();
	virtual void RenderCurves();
	virtual void RenderMarksForVertScale();
	virtual void RenderMarksForHorzScale();
	virtual void FormatXMark(char *pszMark, int MarkMaxLen, double value);
	virtual void FormatYMark(char *pszMark, int MarkMaxLen, double value);
	virtual int GetGridFontSize() { return 20; } // in pixels
	virtual int GetVertGridMinStep() { return 100; } // in pixels
	virtual unsigned char *GetGridColor();
	virtual unsigned char *GetSubgridColor();
	virtual unsigned char *GetGridMarksColor();
	// Event handlers

// Public methods
public:

// Protected methods
protected:

// Private methods
private:

// Public members
public:

// Protected members
protected:
	enum {
		MouseClickDrag_ZoomRectangle,
		MouseClickDrag_DragScroll
	} m_MouseClickDragType;

// Private members
private:
	double m_fVertGridStep, m_fHorzGridStep;
	unsigned int m_RasterFontListBaseID;
};

#endif //__OPEN_GL_RENDERER_H_INCLUDED__