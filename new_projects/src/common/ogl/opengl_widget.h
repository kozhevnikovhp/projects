/* opengl_widget.h

Revision history:
*/

#pragma once

#include <QGLWidget>
#include <QMouseEvent>

namespace common {

namespace ogl {

class cMouseDrag
{
public:
    cMouseDrag();

    void buttonPressed(QMouseEvent *pEvent);
    void buttonReleased();
    void processed(QMouseEvent *pEvent);

    int whereButtonPressedX() const { return m_ButtonPressedAtX; }
    int whereButtonPressedY() const { return m_ButtonPressedAtY; }
    int lastEventProcessedX() const { return m_LastProcessedX; }
    int lastEventProcessedY() const { return m_LastProcessedY; }
    bool isDragging() const { return m_bDragging; }
    
protected:
	int m_ButtonPressedAtX, m_ButtonPressedAtY;
    int m_LastProcessedX, m_LastProcessedY;
    bool m_bDragging;
};
    
class cOpenGL_Widget : public QGLWidget
{
    Q_OBJECT
	typedef QGLWidget inherited;

public:
	explicit cOpenGL_Widget(QWidget *pParent = NULL);

	void redraw() { updateGL(); }
	void storeModelViewMatrix() { glMatrixMode(GL_MODELVIEW); glPushMatrix(); }
	void restoreModelViewMatrix() { glMatrixMode(GL_MODELVIEW); glPopMatrix(); }
	void draw2DText(const char *pszText);

    void setBackgroundColor(const QColor &color) { m_BackgroundColor = color; }
    QColor getBackgroundColor() const { return m_BackgroundColor; }

	bool isLDragging() const { return m_LMouseButtonDrag.isDragging(); }
	bool isRDragging() const { return m_RMouseButtonDrag.isDragging(); }

signals:

public slots:

protected:
    // QT OpenGL events
	virtual	void initializeGL();
	virtual void resizeGL(int width, int height);

    // QT mouse events
	virtual void mousePressEvent(QMouseEvent *pEvent);
	virtual void mouseReleaseEvent(QMouseEvent *pEvent);

	virtual void onInitialized() {}
	virtual void setDefaultView() {}
	virtual bool isMouseTrackingNeeded() { return true; }

    virtual double getAspectRatio() const;

    void setColor3f(QColor color) { glColor3f(color.redF(), color.greenF(), color.blueF()); }
    void setColor4f(QColor color) { glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF()); }

//	unsigned int create2DRasterFont(const char *pszFontName, int height);
//	unsigned int createVectorFont(const char *pszFontName, int height, GLYPHMETRICSFLOAT *pGlyphMetrics);

protected:
   	int m_WindowWidth, m_WindowHeight;		// window sizes
	int m_MouseCurrentPosX, m_MouseCurrentPosY;
    cMouseDrag m_LMouseButtonDrag;
    cMouseDrag m_RMouseButtonDrag;

    QColor m_BackgroundColor;
};

} // namespace ogl

} // namespace common

