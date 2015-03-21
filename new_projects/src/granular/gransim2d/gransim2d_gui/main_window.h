#ifndef cMainWindow_H
#define cMainWindow_H

#include <QtGui/QMainWindow>
#include "../engine/gui_api.h"
#include "../engine/gransim2d_design.h"

// Forward declarations
class cMainStatusBar;
class cGranSim2DRenderer;
class cProjectDock;

class cMainWindow : public QMainWindow, public IGuiAPI
{
    Q_OBJECT

public:
	cMainWindow(QWidget *parent = 0);
	~cMainWindow();

	void showCursorPos(double x, double y);
	void showHint(char *pszHint);

	// Events
	virtual void DesignCleaned();
	virtual void DesignChanged();
	virtual void ReflectingWallCreated(CReflectingWall *pWall, void *pWhere);
	virtual void ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere);
	virtual void MaterialCreated(CMaterial *pMaterial, void *pWhere);
	virtual void ParticleTypeCreated(CParticleType *pType, void *pWhere);
	virtual void ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere);
	virtual void ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere);
	virtual void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere);
	virtual void ObjectDeleted(CGranSim2DObject *pObject, void *pWhere);
	virtual void CannotSimulate(const char *pszWhy);

protected:
	void createActions();
	void createMenu();
	void createStatusBar();

protected Q_SLOTS:
	void slot_file_open();
	void slot_file_save();
	void slot_file_saveas();
	void slot_file_close();
	void slot_file_quit();

protected:
	cGranSim2DDesign m_Design;

	cGranSim2DRenderer *m_pRenderer;
	cProjectDock *m_pProjectDock;
	cProjectDock *m_pProjectDock2;
	cMainStatusBar *m_pStatusBar;

	QAction *m_pFileOpenAction;
	QAction *m_pFileSaveAction;
	QAction *m_pFileSaveAsAction;
	QAction *m_pFileCloseAction;
	QAction *m_pFileQuitAction;
};

#endif // cMainWindow_H
