/* opengl_2d_widget.h

Revision history:
*/

#ifndef __OPENGL_2D_WIDGET_H_INCLUDED__
#define __OPENGL_2D_WIDGET_H_INCLUDED__

#include "opengl_widget.h"

namespace common {

namespace ogl {

class cOpenGL_2D_Widget : public cOpenGL_Widget
{
	Q_OBJECT
	typedef cOpenGL_Widget inherited;

public:
	explicit cOpenGL_2D_Widget(QWidget *pParent = NULL);

signals:

public slots:

protected:
	virtual	void paintGL();

// Public overridable methods
public:
	virtual void setDefaultView();

// Protected overridable methods
protected:
	virtual void mousePressEvent(QMouseEvent *pEvent);
	virtual void mouseMoveEvent(QMouseEvent *pEvent);
	virtual void mouseReleaseEvent(QMouseEvent *pEvent);
	virtual void mouseDoubleClickEvent(QMouseEvent *pEvent);
	virtual void wheelEvent(QWheelEvent *pEvent);

    virtual double getAspectRatio() const;
	virtual double GetWorldSizeX() const { return 2; }
	virtual double GetWorldSizeY() const { return 2; }
	virtual double GetDefaultWorldCenterX() const { return 0; }
	virtual double GetDefaultWorldCenterY() const { return 0; }
	virtual double GetWorldScaleX() const { return 1; }
	virtual double GetWorldScaleY() const { return 1; }
	virtual QColor getZoomRectangleColor() const { return Qt::gray; }

	virtual bool IsMouseMovePanXEnabled() const { return true; }
	virtual bool IsMouseMovePanYEnabled() const { return true; }

	virtual void onRenderScene();
	virtual bool isIsotropic() const { return true; }


// Public methods
public:
	void IncreaseZoomX(double fIncreaseBy, bool bRedraw = true);
	void IncreaseZoomY(double fIncreaseBy, bool bRedraw = true);
	void IncreaseZoom(double fIncreaseBy, bool bRedraw = true);
	double MostLeftVisibleX();
	double MostRightVisibleX();
	double MostTopVisibleY();
	double MostBottomVisibleY();
	double Get1PixelExtentsX() { return m_1Pixel2WorldX; }
	double Get1PixelExtentsY() { return m_1Pixel2WorldY; }

// Protected methods
protected:
	void SetupCamera();
	bool isZoomingRectangle() { return m_bZoomingRectangle; }
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

// Private members
private:
	double m_fWorldShiftDeltaX, m_fWorldShiftDeltaY;
	double m_fWorldShiftX, m_fWorldShiftY;
	double m_fWorldZoomX, m_fWorldZoomY; // internal zooming, multiplied by custom scaling
	bool m_bZoomingRectangle;
	struct
	{
		double left, right, top, bottom;
		double height, width;
	} m_Frustrum;
	double m_1Pixel2WorldX, m_1Pixel2WorldY; // how wide and high 1 pixel on screen extents in the world ccords
};


} // namespace ogl

} // namespace common


#endif // __OPENGL_2D_WIDGET_H_INCLUDED__
