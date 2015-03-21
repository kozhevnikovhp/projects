/* opengl_3d_widget.cpp

Revision history:
*/

#include "opengl_3d_widget.h"
#include <stdio.h>
#include <cmath>

namespace common {

namespace ogl {

double degreesToRadians(double degrees)
{
    return degrees*3.1415926/180.;
}

double radiansToDegrees(double radians)
{
    return radians/3.1415926*180.;
}

/////////////////////////////////////////////////////////////////
// cOpenGL_3D_Widget

cOpenGL_3D_Widget::cOpenGL_3D_Widget(QWidget *parent) : inherited(parent)
{
    initCamera();
    initWorld();
	m_WorldDeltaRx = m_WorldDeltaRy = m_WorldDeltaRz = 0;
    m_IsotropicScale = 1;
}

//virtual
void cOpenGL_3D_Widget::paintGL()
{
	if (m_WindowHeight <= 0)
		return; // not initialized yet

	QColor color = getBackgroundColor();
	glClearColor(color.redF(), color.greenF(), color.blueF(), 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupCamera();
	storeModelViewMatrix();  // store MODELVIEW matrix (user can forget to restore it inside OnRenderScene)
    
	//glTranslated(m_WorldX, m_WorldY, m_WorldZ);
	glRotated(m_WorldRx+m_WorldDeltaRx, 1.0, 0.0, 0.0);
	glRotated(m_WorldRy+m_WorldDeltaRy, 0.0, 1.0, 0.0);
	glRotated(m_WorldRz+m_WorldDeltaRz, 0.0, 0.0, 1.0);
	glScaled(m_IsotropicScale*getWorldScaleX(), m_IsotropicScale*getWorldScaleY(), m_IsotropicScale*getWorldScaleZ());
    
	onRenderScene();
    
	restoreModelViewMatrix();

	glFinish();
}

//virtual
void cOpenGL_3D_Widget::setDefaultView()
{
	setDefaultWorldOri();
	setDefaultCameraPos();
}

//virtual
void cOpenGL_3D_Widget::setDefaultWorldOri()
{
    initWorld();
}

//virtual
void cOpenGL_3D_Widget::setDefaultCameraPos()
{
    m_CameraX = 15*getWorldSize();
    m_CameraY = m_CameraZ = 0; // camera look to the origin from positive OX direction
}

//virtual
void cOpenGL_3D_Widget::mousePressEvent(QMouseEvent *pEvent)
{
	m_WorldDeltaRx = m_WorldDeltaRy = m_WorldDeltaRz = 0;

	inherited::mousePressEvent(pEvent);
}

//virtual
void cOpenGL_3D_Widget::mouseMoveEvent(QMouseEvent *pEvent)
{
	m_MouseCurrentPosX = pEvent->x();
	m_MouseCurrentPosY = pEvent->y();
    cMouseDrag *pRotation = getMouseRotation(pEvent);
	if (pRotation)
    {
        double dRz = (pEvent->x()-pRotation->whereButtonPressedX())*getTrackballSensitivity()/m_WindowWidth;
        double dRy = (pEvent->y()-pRotation->whereButtonPressedY())*getTrackballSensitivity()/m_WindowHeight;
        
        m_WorldDeltaRy = radiansToDegrees(dRy);
        m_WorldDeltaRz = radiansToDegrees(dRz);
        
        redraw();
    }

    cMouseDrag *pPan = getMousePanning(pEvent);
    if (pPan)
    {
        double D = getCameraToWorldDistance();
        double dx = m_CameraX - m_WorldDx;
        double dy = m_CameraY - m_WorldDy;
        double Dxy = std::sqrt(dx*dx + dy*dy);
        double cosTheta = Dxy/D;
        double sinTheta = (m_CameraY - m_WorldDy)/D;
        double cosPhi = dx/Dxy;
        double sinPhi = dy/Dxy;
        double onePixelInWorld = 2*D*tan(degreesToRadians(getCameraViewAngle()/2))/m_WindowHeight;
        double horzDrag = onePixelInWorld*(pEvent->x()-pPan->lastEventProcessedX());
        double dPhi = horzDrag/D;
        double vertDrag = onePixelInWorld*(pEvent->y()-pPan->lastEventProcessedY());
        double dTheta = vertDrag/D;
        m_WorldDx += +D*(cosTheta*sinPhi*dPhi + sinTheta*cosPhi*dTheta);
        m_WorldDy += -D*(cosTheta*cosPhi*dPhi + sinTheta*sinPhi*dTheta);
        m_WorldDz += +D*cosTheta*dTheta;

        pPan->processed(pEvent);
        redraw();
    }
    
	inherited::mouseMoveEvent(pEvent);
}

//virtual
void cOpenGL_3D_Widget::mouseReleaseEvent(QMouseEvent *pEvent)
{
	if (getMouseRotation(pEvent))
	{
        m_WorldRx += m_WorldDeltaRx;
        m_WorldRy += m_WorldDeltaRy;
        m_WorldRz += m_WorldDeltaRz;
        m_WorldDeltaRx = m_WorldDeltaRy = m_WorldDeltaRz = 0;
    }
	inherited::mouseReleaseEvent(pEvent);
}

//virtual
void cOpenGL_3D_Widget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
	inherited::mouseDoubleClickEvent(pEvent);
}

//virtual
void cOpenGL_3D_Widget::wheelEvent(QWheelEvent *pEvent)
{
    m_IsotropicScale *= std::exp(pEvent->delta() / 1000.0);
    redraw();
	inherited::wheelEvent(pEvent);
}

//virtual
cMouseDrag *cOpenGL_3D_Widget::getMouseRotation(QMouseEvent *pEvent)
{
    if (isLDragging())
    {
        const Qt::KeyboardModifiers mods(pEvent->modifiers());
        if (mods == 0)
            return &m_LMouseButtonDrag; // rotation if there is no any modifiers only (default behaviour)
    }
    return NULL;
}

//virtual
cMouseDrag *cOpenGL_3D_Widget::getMousePanning(QMouseEvent *pEvent)
{
    if (isLDragging())
    {
        const Qt::KeyboardModifiers mods(pEvent->modifiers());
        if (mods & Qt::ShiftModifier)
            return &m_LMouseButtonDrag; // panning if left mouse and SHIFT buttons are pressed (default behaviour)
    }
    return NULL;
}

void cube(double size)
{
    double fAlpha = 0.5;
    glBegin(GL_QUAD_STRIP);
        glColor4d(1.0, 0.0, 1.0, fAlpha);
        glVertex3d(-size, size, size);
        glColor4d(1.0, 0.0, 0.0, fAlpha);
        glVertex3d(-size, -size, size);
        glColor4d(1.0, 1.0, 1.0, fAlpha);
        glVertex3d(size, size, size);
        glColor4d(1.0, 1.0, 0.0, fAlpha);
        glVertex3d(size, -size, size);
        glColor4d(0.0, 1.0, 1.0, fAlpha);
        glVertex3d(size, size, -size);
        glColor4d(0.0, 1.0, 0.0, fAlpha);
        glVertex3d(size, -size, -size);
        glColor4d(0.0, 0.0, 1.0, fAlpha);
        glVertex3d(-size, size, -size);
        glColor4d(0.5, 0.5, 0.5, fAlpha);
        glVertex3d(-size, -size,  -size);
        glColor4d(1.0, 0.0, 1.0, fAlpha);
        glVertex3d(-size, size, size);
        glColor4d(1.0, 0.0, 0.0, fAlpha);
        glVertex3d(-size, -size, size);
    glEnd();
    glBegin(GL_QUADS);
        glColor4d(1.0, 0.0, 1.0, fAlpha);
        glVertex3d(-size, size, size);
        glColor4d(1.0, 1.0, 1.0, fAlpha);
        glVertex3d(size, size, size);
        glColor4d(0.0, 1.0, 1.0, fAlpha);
        glVertex3d(size, size, -size);
        glColor4d(0.0, 0.0, 1.0, fAlpha);
        glVertex3d(-size, size, -size);
        glColor4d(1.0, 0.0, 0.0, fAlpha);
        glVertex3d(-size, -size, size);
        glColor4d(1.0, 1.0, 0.0, fAlpha);
        glVertex3d(size, -size, size);
        glColor4d(0.0, 1.0, 0.0, fAlpha);
        glVertex3d(size, -size, -size);
        glColor4d(0.0, 0.0, 0.0, fAlpha);
        glVertex3d(-size, -size, -size);
    glEnd();
}

//virtual
void cOpenGL_3D_Widget::onRenderScene()
{    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);

	glColor3d(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(1.5, 0.0, 0.0);
	glEnd();
    renderText(1.52, 0, 0, "X");
    
	glColor3d(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 1.5, 0.0);
	glEnd();
    renderText(0, 1.52, 0, "Y");

	glColor3d(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 1.5);
    glEnd();
    renderText(0, 0, 1.52, "Z");

	storeModelViewMatrix();
    cube(0.5);
    glTranslated(0.55, 0.35, 0.85);
    glRotated(45, 1, 0, 0);
    glRotated(45, 0, 1, 0);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    cube(0.3);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
	restoreModelViewMatrix();
}

void cOpenGL_3D_Widget::setupCamera()
{
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(getCameraViewAngle(), getAspectRatio(), 0.01*getWorldSize(), 100*getWorldSize());

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_CameraX, m_CameraY, m_CameraZ,
              m_WorldDx, m_WorldDy, m_WorldDz,
              0, 0, 1);
}

void cOpenGL_3D_Widget::lookFromTop(bool bRedraw)
{
    m_WorldRx = 0;
    m_WorldRy = 90;
    m_WorldRz = 90;
    if (bRedraw)
        redraw();
}

void cOpenGL_3D_Widget::lookFromBottom(bool bRedraw)
{
    resetPan();
    m_WorldRx = 0;
    m_WorldRy = -90;
    m_WorldRz = 90;
    if (bRedraw)
        redraw();
}
    
void cOpenGL_3D_Widget::lookFromLeft(bool bRedraw)
{
    resetPan();
    m_WorldRx = 0;
    m_WorldRy = 0;
    m_WorldRz = -90;
    if (bRedraw)
        redraw();
}

void cOpenGL_3D_Widget::lookFromRight(bool bRedraw)
{
    resetPan();
    m_WorldRx = 0;
    m_WorldRy = 0;
    m_WorldRz = 90;
    if (bRedraw)
        redraw();
}

void cOpenGL_3D_Widget::lookFromFront(bool bRedraw)
{
    resetPan();
    m_WorldRx = 0;
    m_WorldRy = 0;
    m_WorldRz = 0;
    if (bRedraw)
        redraw();
}

void cOpenGL_3D_Widget::lookFromBack(bool bRedraw)
{
    resetPan();
    m_WorldRx = 0;
    m_WorldRy = 0;
    m_WorldRz = 180;
    if (bRedraw)
        redraw();
}

double cOpenGL_3D_Widget::getCameraToWorldDistance() const
{
    return std::sqrt(m_CameraX*m_CameraX + m_CameraY*m_CameraY + m_CameraZ*m_CameraZ);
}

void cOpenGL_3D_Widget::initCamera()
{
    m_CameraX = 1;
    m_CameraY = m_CameraZ = 0; // camera look to the origin from positive OX direction
}

void cOpenGL_3D_Widget::initWorld()
{
    resetPan();
    m_WorldRx = 0;
    m_WorldRy = 20;
    m_WorldRz = -20;
}


} // namespace ogl

} // namespace common




