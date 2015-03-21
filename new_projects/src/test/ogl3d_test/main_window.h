#ifndef cMainWindow_H
#define cMainWindow_H

#include <QtGui/QMainWindow>
#include "ogl/opengl_3d_widget.h"

// Forward declarations
class cMainStatusBar;

using namespace common;
using namespace ogl;

class cCentralWidget : public cOpenGL_3D_Widget
{
    //virtual double getWorldScaleZ() const { return 2; }
};
    
class cMainWindow : public QMainWindow
{
    Q_OBJECT

public:
	cMainWindow(QWidget *parent = 0);
	~cMainWindow();

	void showCursorPos(double x, double y);
	void showHint(char *pszHint);

	// Events

protected:
	void createActions();
	void createMenu();
    void createStatusBar();

protected Q_SLOTS:
	void slot_file_quit();

protected:
	QAction *m_pFileQuitAction;
    cMainStatusBar *m_pStatusBar;
    cCentralWidget *m_pCentralWidget;
};

#endif // cMainWindow_H
