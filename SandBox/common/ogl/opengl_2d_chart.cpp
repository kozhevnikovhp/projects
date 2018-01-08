/* opengl_2d_widget.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
*/

#include "opengl_2d_chart.h"
#include <stdio.h>
#include <math.h>

namespace common {

namespace ogl {

///////////////////////////////////////////////////////////////////////////
// cOpenGL_Chart2D

cOpenGL_Chart2D::cOpenGL_Chart2D(QWidget *parent) : inherited(parent)
{
}

cOpenGL_Chart2D::~cOpenGL_Chart2D()
{
}


//virtual
void cOpenGL_Chart2D::onRenderScene()
{
	//inherited::onRenderScene();
	RenderVerticalGrid();
	RenderHorizontalGrid();
	RenderCurves();
	RenderMarksForVertScale();
	RenderMarksForHorzScale();
}

//virtual
void cOpenGL_Chart2D::onInitialized()
{
//	m_RasterFontListBaseID = Create2DRasterFont("Arial", GetGridFontSize());
}

//virtual
void cOpenGL_Chart2D::onSetDefaultOptions()
{
}

// Rounds grid_step to (0.1, 0.2, 0.25, 0.4, 0.5, 0.75) * 10**N
static void RoundGridStep(double &grid_step, double &subgrid_step)
{
	size_t i, iBest = 0;
	int j;
	double normalized_steps[] = { 1, 2, 2.5, 4, 5, 7.5, 10 };
	double subgrid_factor[]   = { 4, 4, 5.0, 4, 5, 3.0, 4 };
	int nPower = (int)floor(log10(grid_step));
	if (nPower > 0)
	{
		for (j = 0; j < nPower; j++)
			grid_step /= 10.;
	}
	else if (nPower < 0)
	{
		for (j = 0; j < -nPower; j++)
			grid_step *= 10.;

	}

	for (i = 1; i < sizeof(normalized_steps)/sizeof(normalized_steps[0]); i++)
	{
		if (grid_step < normalized_steps[i] &&
			grid_step > normalized_steps[i-1])
		{
			if ((grid_step - normalized_steps[i-1]) < (normalized_steps[i] - grid_step))
				grid_step = normalized_steps[i-1];
			else
				grid_step = normalized_steps[i];
			iBest = i;
			break;
		}
	}

	if (nPower > 0)
	{
		for (j = 0; j < nPower; j++)
			grid_step *= 10.;
	}
	else if (nPower < 0)
	{
		for (j = 0; j < -nPower; j++)
			grid_step /= 10.;
	}
	subgrid_step = grid_step/subgrid_factor[iBest];
}

//virtual
void cOpenGL_Chart2D::RenderVerticalGrid()
{
	if (m_WindowWidth <= 0)
		return;
	setColor3f(getGridColor());
	if (m_WindowHeight <= 0)
		return;
	int i;
	double subgrid_step;
	m_fVertGridStep = GetFrustrumWidth()/(m_WindowWidth/(2*GetVertGridMinStep()));
	RoundGridStep(m_fVertGridStep, subgrid_step);

	// subgrid
	setColor3f(getSubgridColor());
	glLineStipple(1, 0x8888);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		for (i = (int)floor(GetFrustrumLeft()/subgrid_step); i < ceil(GetFrustrumRight()/subgrid_step); i++)
		{
			double value = i*subgrid_step;
			glVertex2d(value, GetFrustrumBottom());
			glVertex2d(value, GetFrustrumTop());
		}
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	// grid
	setColor3f(getGridColor());
	glBegin(GL_LINES);
		for (i = (int)floor(GetFrustrumLeft()/m_fVertGridStep); i < ceil(GetFrustrumRight()/m_fVertGridStep); i++)
		{
			double value = i*m_fVertGridStep;
			glVertex2d(value, GetFrustrumBottom());
			glVertex2d(value, GetFrustrumTop());
		}
	glEnd();
}

//virtual
void cOpenGL_Chart2D::RenderHorizontalGrid()
{
	if (m_WindowHeight <= 0)
		return;

	int i;
	double subgrid_step;
	m_fHorzGridStep = GetFrustrumHeight()/(m_WindowHeight/(3*GetGridFontSize()));
	RoundGridStep(m_fHorzGridStep, subgrid_step);

	// subgrid
	setColor3f(getSubgridColor());
	glLineStipple(1, 0x8888);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		for (i = (int)floor(GetFrustrumBottom()/subgrid_step); i < ceil(GetFrustrumTop()/subgrid_step); i++)
		{
			double value = i*subgrid_step;
			glVertex2d(GetFrustrumRight(), value);
			glVertex2d(GetFrustrumLeft(), value);
		}
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	// grid
	setColor3f(getGridColor());
	glBegin(GL_LINES);
		for (i = (int)floor(GetFrustrumBottom()/m_fHorzGridStep); i < ceil(GetFrustrumTop()/m_fHorzGridStep); i++)
		{
			double value = i*m_fHorzGridStep;
			glVertex2d(GetFrustrumRight(), value);
			glVertex2d(GetFrustrumLeft(), value);
		}
	glEnd();
}

// default function
double func(double x)
{
	return 0.5*(x*sin(x*x*x*x*20)+1);
}

//virtual
void cOpenGL_Chart2D::RenderCurves() //default curve
{
	setColor3f(Qt::red);
	double step = GetWorldSizeX()/200;
	glBegin(GL_LINE_STRIP);
		double x1 = GetMinXValue();
		double y1 = func(x1);
		glVertex2d(x1, y1);
		double x2 = x1+step;
		while (x2 <= GetMaxXValue())
		{
			double y2 = func(x2);
			glVertex2d(x2, y2);
			x1 = x2;
			x2 += step;
		}
	glEnd();
}

//virtual
void cOpenGL_Chart2D::RenderMarksForVertScale()
{
	setColor3f(getGridMarksColor());
	glListBase(m_RasterFontListBaseID);
	double MarkX = ScreenToWorldX(3);
	char szValue[32];
	double Shift = (ScreenToWorldY(GetGridFontSize()) - ScreenToWorldY(0))/2;
	for (int i = (int)floor(GetFrustrumBottom()/m_fHorzGridStep); i < ceil(GetFrustrumTop()/m_fHorzGridStep); i++)
	{
		double fValue = i*m_fHorzGridStep;
		FormatYMark(szValue, sizeof(szValue)/sizeof(szValue[0]), fValue);
		glRasterPos2d(MarkX, fValue+Shift);
		draw2DText(szValue);
	}
}

//virtual
void cOpenGL_Chart2D::RenderMarksForHorzScale()
{
	setColor3f(getGridMarksColor());
	glListBase(m_RasterFontListBaseID);
	double MarkY = ScreenToWorldY(m_WindowHeight-2);
	char szValue[32];
	for (int i = (int)floor(GetFrustrumLeft()/m_fVertGridStep); i < ceil(GetFrustrumRight()/m_fVertGridStep); i++)
	{
		double fValue = i*m_fVertGridStep;
		FormatXMark(szValue, sizeof(szValue)/sizeof(szValue[0]), fValue);
		double Shift = ScreenToWorldX(GetGridFontSize()/2*strlen(szValue)/2) - ScreenToWorldX(0);
		glRasterPos2d(fValue-Shift, MarkY);
		draw2DText(szValue);
	}
}

//virtual
void cOpenGL_Chart2D::FormatXMark(char *pszMark, int MarkMaxLen, double value)
{
	sprintf(pszMark, "%g", value);
}

//virtual
void cOpenGL_Chart2D::FormatYMark(char *pszMark, int MarkMaxLen, double value)
{
	sprintf(pszMark, "%g", value);
}


} // namespace ogl

} // namespace common


