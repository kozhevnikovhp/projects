#include "main_window.h"
#include "gransim2d_renderer.h"
#include "projectview.h"
#include "main_statusbar.h"
#include <QMenuBar>
#include <QFileDialog>

cMainWindow::cMainWindow(QWidget *pParent)	: QMainWindow(pParent)
{
    //m_pProjectDock = new cProjectDock("Project1", this);
    //addDockWidget(Qt::LeftDockWidgetArea, m_pProjectDock);

    //m_pProjectDock2 = new cProjectDock("Project2", this);
    //addDockWidget(Qt::RightDockWidgetArea, m_pProjectDock2);

	m_pRenderer = new cGranSim2DRenderer(this, &m_Design);
	m_pRenderer->EnableEditing(true);
	setCentralWidget(m_pRenderer);
 
	g_pGui = this;

	createActions();
	createMenu();
	createStatusBar();
}

cMainWindow::~cMainWindow()
{

}

//virtual
void cMainWindow::DesignCleaned()
{
	m_pRenderer->Cleanup();
}

//virtual
void cMainWindow::DesignChanged()
{
}

//virtual
void cMainWindow::ReflectingWallCreated(CReflectingWall *pWall, void *pWhere)
{
	m_pRenderer->ReflectingWallCreated(pWall, pWhere);
}

//virtual
void cMainWindow::ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere)
{
	m_pRenderer->ReflectingRectangleCreated(pRectangle, pWhere);
}

//virtual
void cMainWindow::MaterialCreated(CMaterial *pMaterial, void *pWhere)
{
}

//virtual
void cMainWindow::ParticleTypeCreated(CParticleType *pType, void *pWhere)
{
}

//virtual
void cMainWindow::ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere)
{
	m_pRenderer->ParticlePointSourceCreated(pSource, pWhere);
}

//virtual
void cMainWindow::ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere)
{
	m_pRenderer->ParticleLinearSourceCreated(pSource, pWhere);
}

//virtual
void cMainWindow::ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere)
{
}

//virtual
void cMainWindow::ObjectDeleted(CGranSim2DObject *pObject, void *pWhere)
{
}

//virtual
void cMainWindow::CannotSimulate(const char *pszWhy)
{
}

//virtual
void cMainWindow::showCursorPos(double x, double y)
{
	m_pStatusBar->showCursorPos(x, y);
}

void cMainWindow::showHint(char *pszHint)
{
	m_pStatusBar->showHint(pszHint);
}

void cMainWindow::createActions()
{
	m_pFileOpenAction = new QAction(tr("&Open..."), this);
	m_pFileOpenAction->setShortcut(tr("Ctrl+O"));
	m_pFileOpenAction->setToolTip(tr("Open"));
	connect(m_pFileOpenAction, SIGNAL(triggered()), this, SLOT(slot_file_open()));

	m_pFileSaveAction = new QAction(tr("&Save"), this);
	m_pFileSaveAction->setToolTip(tr("Save"));
	connect(m_pFileSaveAction, SIGNAL(triggered()), this, SLOT(slot_file_save()));

	m_pFileSaveAsAction = new QAction(tr("&Save As..."), this);
	m_pFileSaveAsAction->setToolTip(tr("Save As"));
	connect(m_pFileSaveAsAction, SIGNAL(triggered()), this, SLOT(slot_file_saveas()));

	m_pFileCloseAction = new QAction(tr("Close"), this);
	m_pFileCloseAction->setToolTip("Close");
	connect(m_pFileCloseAction, SIGNAL(triggered()), this, SLOT(slot_file_close()));

	m_pFileQuitAction = new QAction(tr("&Quit"), this);
	m_pFileQuitAction->setShortcut(tr("Ctrl+Q"));
	connect(m_pFileQuitAction, SIGNAL(triggered()), this, SLOT(slot_file_quit()));
}

void cMainWindow::createMenu()
{
	QMenuBar *pMenuBar = menuBar();

	QMenu *pMenu = pMenuBar->addMenu(tr("&File"));
	pMenu->addAction(m_pFileOpenAction);
	pMenu->addAction(m_pFileSaveAction);
	pMenu->addAction(m_pFileSaveAsAction);
	pMenu->addAction(m_pFileCloseAction);
	pMenu->addSeparator();
	pMenu->addAction(m_pFileQuitAction);

	pMenu = pMenuBar->addMenu(tr("&Edit"));
	//pMenu->addAction(edit_options_action_);

	pMenu = pMenuBar->addMenu(tr("&Help"));
}

void cMainWindow::createStatusBar()
{
	m_pStatusBar = new cMainStatusBar(this);
	setStatusBar(m_pStatusBar);
}

void cMainWindow::slot_file_open()
{
	QFileDialog  dlg(this,
					"Open",
					QDir::currentPath(),
					"GranSim2D files (*.grn);;All files (*)");
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	if (dlg.exec() == QDialog::Accepted)
	{
		const QStringList filenames(dlg.selectedFiles());
		if (!filenames.isEmpty())
		{
			const QString fn(filenames[0]);
			m_Design.LoadFromFile(fn.toStdString());
		}
	}
	m_pRenderer->setDefaultView();
	m_pRenderer->redraw();
}

void cMainWindow::slot_file_save()
{
}

void cMainWindow::slot_file_saveas()
{
}

void cMainWindow::slot_file_close()
{
}

void cMainWindow::slot_file_quit()
{
	close();
}

