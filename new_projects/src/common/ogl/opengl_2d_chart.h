/* opengl_2d_widget.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
*/

#ifndef __OPENGL_2D_CHART_H_INCLUDED__
#define __OPENGL_2D_CHART_H_INCLUDED__

#include "opengl_2d_widget.h"

namespace common {

namespace ogl {

class cOpenGL_Chart2D : public cOpenGL_2D_Widget
{
    Q_OBJECT
	typedef cOpenGL_2D_Widget inherited;
    
// Construction/destruction
public:
	cOpenGL_Chart2D(QWidget *parent);
	virtual ~cOpenGL_Chart2D();

// Public overridable methods
public:
	virtual void onRenderScene();
	virtual void onInitialized();
	virtual void onSetDefaultOptions();
	virtual bool IsIsotropic() { return false; }

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return (GetMaxXValue() - GetMinXValue()); }
	virtual double GetMinXValue() { return 0.; } // Should be overriden in derived classes according to available chart data
	virtual double GetMaxXValue() { return 1.; } // Should be overriden in derived classes according to available chart data
	virtual double GetWorldSizeY() { return (GetMaxYValue() - GetMinYValue()); }
	virtual double GetMinYValue() { return 0.; } // Should be overriden in derived classes according to available chart data
	virtual double GetMaxYValue() { return 1.; } // Should be overriden in derived classes according to available chart data
	virtual double GetDefaultWorldCenterX() { return (GetMaxXValue() + GetMinXValue())/2; }
	virtual double GetDefaultWorldCenterY() { return (GetMaxYValue() + GetMinYValue())/2; }
	virtual void RenderVerticalGrid();
	virtual void RenderHorizontalGrid();
	virtual void RenderCurves();
	virtual void RenderMarksForVertScale();
	virtual void RenderMarksForHorzScale();
	virtual void FormatXMark(char *pszMark, int MarkMaxLen, double value);
	virtual void FormatYMark(char *pszMark, int MarkMaxLen, double value);
	virtual int GetGridFontSize() { return 20; } // in pixels
	virtual int GetVertGridMinStep() { return 100; } // in pixels
	virtual QColor getGridColor() const { return Qt::gray; }
	virtual QColor getSubgridColor() const { return Qt::lightGray; }
	virtual QColor getGridMarksColor() const { return Qt::yellow; }
	// Event handlers

// Public methods
public:

// Protected methods
protected:

// Private methods
private:

// Public members
public:

// Protected members
protected:

// Private members
private:
	double m_fVertGridStep, m_fHorzGridStep;
	unsigned int m_RasterFontListBaseID;
};

} // namespace ogl

} // namespace common


#endif // __OPENGL_2D_CHART_H_INCLUDED__
