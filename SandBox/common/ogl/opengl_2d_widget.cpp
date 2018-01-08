/* opengl_2d_widget.cpp

Revision history:
*/

#include "opengl_2d_widget.h"
#include <stdio.h>
#include <algorithm>
#include <cmath>

namespace common {

namespace ogl {

/////////////////////////////////////////////////////////////////
// cOpenGL_2D_Widget

cOpenGL_2D_Widget::cOpenGL_2D_Widget(QWidget *parent) : inherited(parent)
{
	m_fWorldShiftX = m_fWorldShiftY = 0.;
	m_fWorldShiftDeltaX = m_fWorldShiftDeltaY = 0.;
	m_fWorldZoomX = m_fWorldZoomY = 1;
	m_bZoomingRectangle = false;
	setDefaultView();
}

//virtual
double cOpenGL_2D_Widget::getAspectRatio() const
{
	if (isIsotropic())
		return (m_WindowHeight == 0) ? m_WindowHeight : (double)m_WindowWidth/(double)m_WindowHeight;
    return 1.;
}

//virtual
void cOpenGL_2D_Widget::paintGL()
{
	if (m_WindowHeight <= 0)
		return; // not initialized yet

	QColor color = getBackgroundColor();
	glClearColor(color.redF(), color.greenF(), color.blueF(), 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDrawBuffer(GL_BACK);
	SetupCamera();
	storeModelViewMatrix();  // store MODELVIEW matrix (user can forget to restore it inside OnRenderScene)
	onRenderScene();
	restoreModelViewMatrix();
	if (isZoomingRectangle() && isLDragging())
	{
		glPushAttrib(GL_ENABLE_BIT); // we will disable depth test therefore it must be restored
		setColor3f(getZoomRectangleColor());
		glDisable(GL_DEPTH_TEST);
		double x1 = ScreenToWorldX(m_LMouseButtonDrag.whereButtonPressedX());
		double x2 = ScreenToWorldX(m_MouseCurrentPosX);
		double y1 = ScreenToWorldY(m_LMouseButtonDrag.whereButtonPressedY());
		double y2 = ScreenToWorldY(m_MouseCurrentPosY);
		glBegin(GL_LINE_LOOP);
			glVertex2d(x1, y1);
			glVertex2d(x2, y1);
			glVertex2d(x2, y2);
			glVertex2d(x1, y2);
		glEnd();
		glPopAttrib();
	}

	glFinish();
}

//virtual
void cOpenGL_2D_Widget::setDefaultView()
{
	m_fWorldShiftX = -GetDefaultWorldCenterX();
	m_fWorldShiftY = -GetDefaultWorldCenterY();
	m_fWorldZoomX = m_fWorldZoomY =	1;
}

//virtual
void cOpenGL_2D_Widget::mousePressEvent(QMouseEvent *pEvent)
{
	const int buttons(pEvent->buttons());
	if (buttons & Qt::LeftButton)
	{
		m_fWorldShiftDeltaX = m_fWorldShiftDeltaY = 0.;
	}

	inherited::mousePressEvent(pEvent);
}

//virtual
void cOpenGL_2D_Widget::mouseMoveEvent(QMouseEvent *pEvent)
{
	//const int buttons(pEvent->buttons());
	const Qt::KeyboardModifiers mods(pEvent->modifiers());
	m_MouseCurrentPosX = pEvent->x();
	m_MouseCurrentPosY = pEvent->y();
	if (isLDragging())
	{
		if (mods & Qt::ShiftModifier)
		{
			if (IsMouseMovePanXEnabled())
				m_fWorldShiftDeltaX = ScreenToWorldX(m_MouseCurrentPosX)-ScreenToWorldX(m_LMouseButtonDrag.whereButtonPressedX());
			if (IsMouseMovePanYEnabled())
				m_fWorldShiftDeltaY = ScreenToWorldY(m_MouseCurrentPosY)-ScreenToWorldY(m_LMouseButtonDrag.whereButtonPressedY());
			redraw();
		}
		else if (mods & Qt::ControlModifier)
		{
			m_bZoomingRectangle = true;
			redraw();
		}
	}
	inherited::mouseMoveEvent(pEvent);
}

//virtual
void cOpenGL_2D_Widget::mouseReleaseEvent(QMouseEvent *pEvent)
{
	const Qt::KeyboardModifiers mods(pEvent->modifiers());
	if (isLDragging())
	{
		if (mods & Qt::ControlModifier)
		{
			int DeltaX = std::abs(m_LMouseButtonDrag.whereButtonPressedX()-pEvent->x());
			int DeltaY = std::abs(m_LMouseButtonDrag.whereButtonPressedY()-pEvent->y());
			if (DeltaX > 10 && DeltaY > 10)
			{
				m_fWorldShiftX = -ScreenToWorldX((m_LMouseButtonDrag.whereButtonPressedX()+pEvent->x())/2);
				m_fWorldShiftY = -ScreenToWorldY((m_LMouseButtonDrag.whereButtonPressedX()+pEvent->y())/2);
				m_fWorldZoomX *= (double)m_WindowWidth/DeltaX;
				m_fWorldZoomY *= (double)m_WindowHeight/DeltaY;
				if (isIsotropic())
				{
					if (m_fWorldZoomX > m_fWorldZoomY)
						m_fWorldZoomX = m_fWorldZoomY;
					else
						m_fWorldZoomY = m_fWorldZoomX;
				}
				redraw();
			}
		}

		m_fWorldShiftX += m_fWorldShiftDeltaX;
		m_fWorldShiftY += m_fWorldShiftDeltaY;
		m_fWorldShiftDeltaX = m_fWorldShiftDeltaY = 0.;
	}
	m_bZoomingRectangle = false;

	inherited::mouseReleaseEvent(pEvent);
}

//virtual
void cOpenGL_2D_Widget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
	inherited::mouseDoubleClickEvent(pEvent);
}

//virtual
void cOpenGL_2D_Widget::wheelEvent(QWheelEvent *pEvent)
{
	IncreaseZoom(std::exp(pEvent->delta() / 1000.0));
	inherited::wheelEvent(pEvent);
}

void cOpenGL_2D_Widget::onRenderScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINES );
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.1f, 0.1f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.9f, 0.1f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.5f, 0.9f);
	glEnd();
}

void cOpenGL_2D_Widget::SetupCamera()
{
	// Set OpenGL perspective, viewport and mode
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	m_Frustrum.left = ScreenToWorldX(0);
	m_Frustrum.right = ScreenToWorldX(m_WindowWidth);
	m_Frustrum.top = ScreenToWorldY(0);
	m_Frustrum.bottom = ScreenToWorldY(m_WindowHeight);
	m_Frustrum.height = m_Frustrum.top-m_Frustrum.bottom;
	m_Frustrum.width = m_Frustrum.right - m_Frustrum.left;
	m_1Pixel2WorldX = (m_Frustrum.right - m_Frustrum.left)/m_WindowWidth;
	m_1Pixel2WorldY = (m_Frustrum.top - m_Frustrum.bottom)/m_WindowHeight;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(m_Frustrum.left, m_Frustrum.right, m_Frustrum.bottom, m_Frustrum.top, -1, 1);
}

void cOpenGL_2D_Widget::IncreaseZoomX(double fIncreaseBy, bool bRedraw)
{
	m_fWorldZoomX *= fIncreaseBy;
	if (bRedraw)
		redraw();
}

void cOpenGL_2D_Widget::IncreaseZoomY(double fIncreaseBy, bool bRedraw)
{
	m_fWorldZoomY *= fIncreaseBy;
	if (bRedraw)
		redraw();
}

void cOpenGL_2D_Widget::IncreaseZoom(double fIncreaseBy, bool bRedraw)
{
	IncreaseZoomX(fIncreaseBy, false);
	IncreaseZoomY(fIncreaseBy, bRedraw);
}

double cOpenGL_2D_Widget::MostLeftVisibleX()
{
	return ScreenToWorldX(0);
}

double cOpenGL_2D_Widget::MostRightVisibleX()
{
	return ScreenToWorldX(m_WindowWidth);
}

double cOpenGL_2D_Widget::MostTopVisibleY()
{
	return ScreenToWorldY(m_WindowHeight);
}

double cOpenGL_2D_Widget::MostBottomVisibleY()
{
	return ScreenToWorldY(0);
}

void cOpenGL_2D_Widget::CalcWorld2ScreenTranslationX(double &a, double &b)
{
	double scale = GetWorldScaleX()*m_fWorldZoomX;
	a = m_WindowWidth*scale/GetWorldSizeX();
	b = m_WindowWidth*(0.5+(m_fWorldShiftX+m_fWorldShiftDeltaX)*scale/GetWorldSizeX());
}

void cOpenGL_2D_Widget::CalcWorld2ScreenTranslationY(double &a, double &b)
{
	double scale = GetWorldScaleY()*m_fWorldZoomY;
    double fAspectRatio = getAspectRatio();
	a = m_WindowHeight*scale/GetWorldSizeY()*fAspectRatio;
	b = m_WindowHeight*(0.5+(m_fWorldShiftY+m_fWorldShiftDeltaY)*scale/GetWorldSizeY()*fAspectRatio);
}

int cOpenGL_2D_Widget::WorldToScreenX(double WorldCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationX(a, b);
	return ((int)(a*WorldCoord + b + 0.49999));
}

int cOpenGL_2D_Widget::WorldToScreenY(double WorldCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationY(a, b);
	return (int)(a*WorldCoord + b + 0.49999);
}

double cOpenGL_2D_Widget::ScreenToWorldX(int ScreenCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationX(a, b);
	return (ScreenCoord-b)/a;
}

double cOpenGL_2D_Widget::ScreenToWorldY(int ScreenCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationY(a, b);
	return (m_WindowHeight-ScreenCoord-b)/a;
}

} // namespace ogl

} // namespace common


