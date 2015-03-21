#include "main_statusbar.h"
#include <stdio.h>

cMainStatusBar::cMainStatusBar(QWidget *parent) : inherited(parent)
{
	m_pHintLabel = new QLabel("", this);
	addPermanentWidget(m_pHintLabel);

	m_pCursorPosLabel = new QLabel("", this);
	addPermanentWidget(m_pCursorPosLabel);
}

void cMainStatusBar::showCursorPos(double x, double y)
{
	char szBuffer[128];
	sprintf(szBuffer, "%.3f, %.3f", x, y);
	m_pCursorPosLabel->setText(szBuffer);
}

void cMainStatusBar::showHint(char *pszHint)
{
	m_pHintLabel->setText(pszHint);
}
