#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QDockWidget>


class cProjectView : public QWidget
{
	Q_OBJECT
	typedef QWidget inherited;

public:
	explicit cProjectView(QWidget *pParent = 0);

signals:

public slots:


};

class cProjectDock : public QDockWidget
{
    Q_OBJECT
	typedef QDockWidget inherited;

public:
	explicit cProjectDock(const QString &title, QWidget *pParent = 0);

signals:

public slots:

protected:
	cProjectView *m_pView;
};

#endif // PROJECTVIEW_H
