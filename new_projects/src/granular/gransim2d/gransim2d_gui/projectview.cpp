#include "projectview.h"
#include <QVBoxLayout>
#include "ogl/opengl_2d_chart.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// cProjectView

cProjectView::cProjectView(QWidget *pParent) :  inherited(pParent)
{
	QBoxLayout *pLayout = new QVBoxLayout(this);

	QWidget *pChart = new common::ogl::cOpenGL_Chart2D(this);
	pLayout->addWidget(pChart);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// cProgectDock

cProjectDock::cProjectDock(const QString &title, QWidget *pParent) :  inherited(title, pParent)
{
	m_pView = new cProjectView(this);
	setWidget(m_pView);
}
