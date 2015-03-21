#include "main_window.h"
#include "main_statusbar.h"
#include <QMenuBar>
#include <QFileDialog>

cMainWindow::cMainWindow(QWidget *pParent)	: QMainWindow(pParent)
{
    m_pCentralWidget = new cCentralWidget;
	setCentralWidget(m_pCentralWidget);
 
	createActions();
	createMenu();
	createStatusBar();
}

cMainWindow::~cMainWindow()
{

}

void cMainWindow::createActions()
{
	m_pFileQuitAction = new QAction(tr("&Quit"), this);
	m_pFileQuitAction->setShortcut(tr("Ctrl+Q"));
	connect(m_pFileQuitAction, SIGNAL(triggered()), this, SLOT(slot_file_quit()));
}

void cMainWindow::createMenu()
{
	QMenuBar *pMenuBar = menuBar();

	QMenu *pMenu = pMenuBar->addMenu(tr("&File"));
	pMenu->addAction(m_pFileQuitAction);

	pMenu = pMenuBar->addMenu(tr("&Help"));
}

void cMainWindow::createStatusBar()
{
	m_pStatusBar = new cMainStatusBar(this);
	setStatusBar(m_pStatusBar);
}

void cMainWindow::slot_file_quit()
{
	close();
}

