#ifndef MAIN_STATUS_BAR_H
#define MAIN_STATUS_BAR_H

#include <QStatusBar>
#include <QLabel>

class cMainStatusBar : public QStatusBar
{
	Q_OBJECT

	typedef QStatusBar inherited;
public:
    explicit cMainStatusBar(QWidget *parent = 0);

	void showCursorPos(double x, double y);
	void showHint(char *pszHint);

signals:

public slots:

protected:
	QLabel *m_pCursorPosLabel;
	QLabel *m_pHintLabel;
};

#endif // MAIN_STATUS_BAR_H
