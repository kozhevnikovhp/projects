/* opengl_3d_widget.h

Revision history:
*/

#pragma once

#include "opengl_widget.h"

namespace common {

namespace ogl {

class cOpenGL_3D_Widget : public cOpenGL_Widget
{
	Q_OBJECT
	typedef cOpenGL_Widget inherited;

public:
	explicit cOpenGL_3D_Widget(QWidget *pParent = NULL);

signals:

public slots:

protected:
	virtual	void paintGL();

// Public overridable methods
public:
	virtual void setDefaultView();
	virtual void setDefaultWorldOri();
	virtual void setDefaultCameraPos();

// Protected overridable methods
protected:
    // QT events
	virtual void mousePressEvent(QMouseEvent *pEvent);
	virtual void mouseMoveEvent(QMouseEvent *pEvent);
	virtual void mouseReleaseEvent(QMouseEvent *pEvent);
	virtual void mouseDoubleClickEvent(QMouseEvent *pEvent);
	virtual void wheelEvent(QWheelEvent *pEvent);

    virtual double getWorldSize() const { return 1; } // typical size
    virtual double getWorldScaleX() const { return 1; }
    virtual double getWorldScaleY() const { return 1; }
    virtual double getWorldScaleZ() const { return 1; }

    virtual void onRenderScene();
    
    virtual double getCameraViewAngle() const { return 10; }
    virtual double getTrackballSensitivity() const { return 1; }

// Public methods
public:
    void lookFromTop(bool bRedraw = true);
    void lookFromBottom(bool bRedraw = true);
    void lookFromLeft(bool bRedraw = true);
    void lookFromRight(bool bRedraw = true);
    void lookFromFront(bool bRedraw = true);
    void lookFromBack(bool bRedraw = true);

    double getCameraToWorldDistance() const;

// Protected methods
protected:
    virtual cMouseDrag *getMouseRotation(QMouseEvent *pEvent);
    virtual cMouseDrag *getMousePanning(QMouseEvent *pEvent);
    
	// Camera operations
    void setupCamera();
	void getWorldOri(double &WorldRx, double &WorldRy, double &WorldRz) const { WorldRx = m_WorldRx; WorldRy = m_WorldRy; WorldRz = m_WorldRz; }
	void setWorldOri(double WorldRx, double WorldRy, double WorldRz, bool bRedraw = true);
    void resetPan() { m_WorldDx = m_WorldDy = m_WorldDz = 0; }

// Protected members
protected:
    // world shift (panning)
    double m_WorldDx, m_WorldDy, m_WorldDz;
    
    // world rotation
	double m_WorldRx, m_WorldDeltaRx;
	double m_WorldRy, m_WorldDeltaRy;
	double m_WorldRz, m_WorldDeltaRz;

    // camera position
	double m_CameraX, m_CameraY, m_CameraZ;
    
    double m_IsotropicScale;
    
// Private methods
private:
    void initCamera();
    void initWorld();

// Private members
private:
};


} // namespace ogl

} // namespace common

