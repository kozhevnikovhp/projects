#ifndef __LITHO_RENDERER_H_INCLUDED__
#define __LITHO_RENDERER_H_INCLUDED__

#include "OpenGLEditor.h"
#include "Calculations.h"
#include "ComplexMath.h"
#include <vector>

typedef struct
{
	double x, y;
	double fIntensity;
	SComplex fField;
	double *pRGBA;
} SImagePoint;

typedef struct
{
	double x, y;
	double Sq;
	double N, E, S, W;
	double dFi;
	double nz;
} SZone;

class CZones : public std::vector<SZone>
{
};

class CRectArea
{
public:
	CRectArea(CRectangle2D *pRect, double PhaseShift);
	CRectangle2D *GetRectangle() { return m_pRect; }
	double GetPhaseShift() { return m_PhaseShift; }
	
protected:
	CRectangle2D *m_pRect;
	double m_PhaseShift;
};

class CRectangles : public std::vector<CRectArea *>
{
};

// Forward declarations
class CProgressDlg;

typedef enum
{
	PSM_None,
	PSM_0,
	PSM_90,
	PSM_180,
	PSM_270
} PSM_Enum;

const int MOUSE_GRID_ROWS = 50;

class CLithoRenderer : public COpenGL_Editor2D
{
// Construction/destruction
public:
	CLithoRenderer();
	virtual ~CLithoRenderer();

// Public overridable methods
public:

// Protected overridable methods
protected:
	virtual unsigned char *GetBackgroundColor() { return ucBlackColor; }
	virtual double GetWorldSizeX() { return m_MaskSize*2; }
	virtual double GetWorldSizeY() { return GetWorldSizeX(); }
	virtual double GetDefaultWorldCenterX() { return 0; }
	virtual double GetDefaultWorldCenterY() { return 0; }
//	virtual double GetWorldScaleX() { return 1; }
//	virtual double GetWorldScaleY() { return 1; }
	virtual void OnRenderScene();
	virtual double GetGridSize() { return GetWorldSizeX()/100.; }
	// Event handlers
	virtual void OnCleanup();
	// Geometry creation/changing
	virtual void *OnRectangleCreated(CEditorRectangle2D *pRectangle);
	virtual void OnGeometryChanged(CEditorGeometry2D *pGeometry, CControlPoint2D *pControlPoint);

// Public methods
public:
	char *GetDisplayedText(int x, int y);
	void Simulate(CProgressDlg *pDlg);
	void ToggleMask() { m_bMaskVisible = !m_bMaskVisible; Redraw(); }
	void ToggleMaskGrid() { m_bMaskGridVisible = !m_bMaskGridVisible; Redraw(); }
	void ToggleImageGrid() { m_bImageGridVisible = !m_bImageGridVisible; Redraw(); }
	LOGICAL ReallocImage();
	PSM_Enum GetPSM() { return m_PSM_Regime; }
	void SetPSM(PSM_Enum PSM) { m_PSM_Regime = PSM; }

// Protected methods
protected:
	void RenderMask();
	void RenderImage();
	void RenderLegend();
	CRectArea *IsInAnyRect(double x, double y);
	void CreateCoordsInRange(double From, double To, double MaxCellSize, CVectorOfDouble &Dst);
	void CreateCoords(CVectorOfDouble &Dst, CVectorOfDouble &Src);
	void UpdateImagePoint(SImagePoint *pPoint, SZone *pZone);
	void Free();
	double *Intensity2RGBA(double fIntensity, double fMaxIntensity);

// Private methods
private:

// Public members
public:
	char m_szText[128];
	double m_WaveLength;
	double m_Distance;
	double m_MaskSize, m_ImageSize;
	double m_ImageAccuracy;
	double m_KVector;
	LOGICAL m_bPositiveMask; // TRUE if created rectangles are transparent for the light
	LOGICAL m_bMaskVisible, m_bMaskGridVisible, m_bImageGridVisible;
	SImagePoint *m_pImage;
	int m_nImageSize;
	CRectangles m_Rectangles;
	CZones m_Zones;
	double m_Colors[100][4];
	int m_nColors;

// Protected members
protected:
	PSM_Enum m_PSM_Regime;

// Private members
private:
};


#endif // __LITHO_RENDERER_H_INCLUDED__
