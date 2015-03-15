#include "stdafx.h"
#include "LithoRenderer.h"
#include <math.h>
#include "Calculations.h"
#include "ProgressDlg.h"


typedef struct
{
	double fSaturation;
	double rgba[4];
} SSaturationColorEntry;

SSaturationColorEntry SaturationColorMap[] = {
	{ 0.00, {0.00, 0.00, 0.30} },  // dark blue
	{ 0.20, {0.00, 0.00, 0.65} },  // bright blue
	{ 0.40, {0.00, 0.70, 0.00} },  // green
	{ 0.60, {0.60, 0.00, 0.00} },  // red
	{ 0.75, {0.80, 0.70, 0.00} },  // bright red
	{ 0.90, {1.00, 1.00, 0.00} },  // yellow
	{ 1.01, {1.00, 1.00, 1.00} }   // white
};

CRectArea::CRectArea(CRectangle2D *pRect, double PhaseShift)
{
	m_pRect = pRect;
	m_PhaseShift = DEGREES_2_RADIANS(PhaseShift);
}

CLithoRenderer::CLithoRenderer()
{
	EnableEditing(LOGICAL_TRUE);
	m_WaveLength = 0.193; //193 nm
	m_Distance = 1; // 0.1 mm = 100 um 
	m_MaskSize = 2;
	m_ImageSize = 2;
	m_ImageAccuracy = m_ImageSize/100;
	m_bPositiveMask = LOGICAL_TRUE;
	m_pImage = NULL;
	m_bMaskVisible = LOGICAL_TRUE;
	m_bMaskGridVisible = m_bImageGridVisible = LOGICAL_FALSE;
	ReallocImage();

	m_nColors = sizeof(m_Colors)/sizeof(m_Colors[0]);
	for (int i = 0; i < m_nColors; i++)
	{
		double fSaturation = (double)i/(m_nColors-1);
		int j1 = 1;
		while (SaturationColorMap[j1].fSaturation < fSaturation)
		{
			j1++;
		}
		int j = j1-1;
		double fPrevSaturation = SaturationColorMap[j].fSaturation;
		double fNextSaturation = SaturationColorMap[j1].fSaturation;
		double DeltaS = fNextSaturation-fPrevSaturation;
		double k = (fSaturation-fPrevSaturation)/DeltaS;

		m_Colors[i][0] = SaturationColorMap[j].rgba[0] + k*(SaturationColorMap[j1].rgba[0]-SaturationColorMap[j].rgba[0]);
		m_Colors[i][1] = SaturationColorMap[j].rgba[1] + k*(SaturationColorMap[j1].rgba[1]-SaturationColorMap[j].rgba[1]);
		m_Colors[i][2] = SaturationColorMap[j].rgba[2] + k*(SaturationColorMap[j1].rgba[2]-SaturationColorMap[j].rgba[2]);
		m_Colors[i][3] = 1.;
	}
	m_PSM_Regime = PSM_0;
	SetEditMode(RECTANGLE_EDIT_MODE);
}

//virtual
CLithoRenderer::~CLithoRenderer()
{
	Free();
	if (m_pImage)
		free(m_pImage);
}

void CLithoRenderer::Free()
{
	for (CRectangles::iterator RectIt = m_Rectangles.begin(); RectIt != m_Rectangles.end(); RectIt++)
	{
		CRectArea *pArea = *RectIt;
		delete pArea;
	}

	m_Rectangles.resize(0);
}

//virtual
void CLithoRenderer::OnCleanup()
{
	Free();
}

//virtual
void CLithoRenderer::OnRenderScene()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_SMOOTH);
	glShadeModel(GL_SMOOTH);

	RenderImage();

	if (IsEditingEnabled())
	{ // editing
		RenderMask();
		COpenGL_Editor2D::OnRenderScene();
	}
	else
	{ // simulated
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		
		glStencilFunc(GL_NEVER, 1, 0xFFFFFFFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		COpenGL_Editor2D::OnRenderScene();
		
		if (m_bPositiveMask)
			glStencilFunc(GL_NOTEQUAL, 1, 0xFFFFFFFF);
		else
			glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		RenderMask();
		
		glDisable(GL_STENCIL_TEST);

		if (m_bMaskGridVisible)
		{
			glColor4d(0.2, 0.2, 0.2, 1);
			for (CZones::iterator it = m_Zones.begin(); it != m_Zones.end(); it++)
			{
				SZone *pZone = it;
				glBegin(GL_LINE_LOOP);
					glVertex2d(pZone->W, pZone->S);
					glVertex2d(pZone->W, pZone->N);
					glVertex2d(pZone->E, pZone->N);
					glVertex2d(pZone->E, pZone->S);
				glEnd();
			}
		}
	}
	RenderLegend();
}

void CLithoRenderer::RenderLegend()
{
	const int nLegendSize = 20;
	double h = 0.5*GetFrustrumHeight()/nLegendSize;
	double w = 20.*Get1PixelExtentsX();
	glBegin(GL_QUADS);
		for (int i=0; i<nLegendSize; i++)
		{
			double y = GetFrustrumBottom() + i*h;
			double y1 = GetFrustrumBottom() + (i+1)*h;
			double V = (double)i/nLegendSize;
			double V1 = (double)(i+1)/nLegendSize;
			glColor4dv(Intensity2RGBA(V, 1));
			glVertex2d(GetFrustrumLeft(),	y);
			glVertex2d(GetFrustrumLeft()+w,	y);
			glColor4dv(Intensity2RGBA(V1, 1));
			glVertex2d(GetFrustrumLeft()+w,	y1);
			glVertex2d(GetFrustrumLeft(),	y1);
		}
	glEnd();
}

//virtual
void *CLithoRenderer::OnRectangleCreated(CEditorRectangle2D *pRectangle)
{
	pRectangle->SetFilled();
	CRectArea *pArea = NULL;
	switch (m_PSM_Regime)
	{
		case PSM_0:
			pArea = new CRectArea(pRectangle->GetConnectedRectangle(), 0);
			pRectangle->GetRGBA()->Init(0xFF, 0x00, 0x00);
			break;
		case PSM_90:
			pArea = new CRectArea(pRectangle->GetConnectedRectangle(), 90);
			pRectangle->GetRGBA()->Init(0x00, 0xFF, 0x00);
			break;
		case PSM_180:
			pArea = new CRectArea(pRectangle->GetConnectedRectangle(), 180);
			pRectangle->GetRGBA()->Init(0xFF, 0x00, 0xFF);
			break;
		case PSM_270:
			pArea = new CRectArea(pRectangle->GetConnectedRectangle(), 270);
			pRectangle->GetRGBA()->Init(0x00, 0xFF, 0xFF);
			break;
		default:
			ASSERT(0);
	}
	m_Rectangles.push_back(pArea);
	return NULL;
}

//virtual
void CLithoRenderer::OnGeometryChanged(CEditorGeometry2D *pGeometry, CControlPoint2D *pControlPoint)
{
}

char *CLithoRenderer::GetDisplayedText(int x, int y)
{
	double WorldX = ScreenToWorldX(x);
	double WorldY = ScreenToWorldY(y);
	sprintf(m_szText, "%8.3f %8.3f", WorldX, WorldY);
	return m_szText;
}

void CLithoRenderer::CreateCoordsInRange(double From, double To, double MaxCellSize, CVectorOfDouble &Dst)
{
	double delta = To - From;
	if (delta < m_WaveLength/10000)
		return;
	if (delta > MaxCellSize)
	{
		int n = (int)(delta/MaxCellSize + 0.49999);
		double step = delta/n;
		for (int j = 1; j < n; j++)
		{
			Dst.AddTail(From + j*step);
		}
	}
	Dst.AddTail(To);
}

void CLithoRenderer::CreateCoords(CVectorOfDouble &Dst, CVectorOfDouble &Src)
{
	double MaxCellSize = m_WaveLength/4;
	Src.SortAscending();
	double PrevCoord = -m_MaskSize/2;
	Dst.AddTail(PrevCoord);
	for (int i = 0; i < Src.GetSize(); i++)
	{
		double coord = Src[i];
		if (coord < -m_MaskSize/2 && coord > +m_MaskSize/2)
			continue;
		CreateCoordsInRange(PrevCoord, coord, MaxCellSize, Dst);
		PrevCoord = coord;
	}
	CreateCoordsInRange(PrevCoord, +m_MaskSize/2, MaxCellSize, Dst);
}

void CLithoRenderer::Simulate(CProgressDlg *pDlg)
{
	EnableEditing(LOGICAL_FALSE);	
	SetEditMode(NONE_EDIT_MODE);
	SetPSM(PSM_None);
	RebuildAllDisplayLists(LOGICAL_TRUE);
	if (!ReallocImage())
		return;

	if (pDlg)
	{
		pDlg->SetStatus("Fresnel Zones Creation");
		pDlg->SetRange(0, 100);
	}
	int i, j;
	CVectorOf<double> Xs, RawXs;
	CVectorOf<double> Ys, RawYs;
	for (CRectangles::iterator RectIt = m_Rectangles.begin(); RectIt != m_Rectangles.end(); RectIt++)
	{
		CRectArea *pArea = *RectIt;
		CRectangle2D *pRectangle = pArea->GetRectangle();
		RawXs.AddTail(pRectangle->XCenter() - pRectangle->Width()/2);
		RawXs.AddTail(pRectangle->XCenter() + pRectangle->Width()/2);
		RawYs.AddTail(pRectangle->YCenter() - pRectangle->Height()/2);
		RawYs.AddTail(pRectangle->YCenter() + pRectangle->Height()/2);
	}
	
	CreateCoords(Xs, RawXs);
	CreateCoords(Ys, RawYs);
	
	m_KVector = PI2/m_WaveLength;
	double gain = m_ImageSize/m_MaskSize;
	m_Zones.resize(0);
	for (i = 0; i < Xs.GetSize()-1; i++)
	{
		double x1 = Xs[i];
		double x2 = Xs[i+1];
		double xc = (x1+x2)/2;
		double xSq = xc*xc;
		for (int j = 0; j < Ys.GetSize()-1; j++)
		{
			double y1 = Ys[j];
			double y2 = Ys[j+1];
			double yc = (y1+y2)/2;
			CRectArea *pArea = IsInAnyRect(xc, yc);
			if ((pArea != NULL) == m_bPositiveMask)
			{
				SZone zone;
				zone.x = xc;
				zone.y = yc;
				zone.Sq = (x2-x1)*(y2-y1);
				zone.N = y2;
				zone.S = y1;
				zone.E = x2;
				zone.W = x1;
				zone.dFi = 0;
				double DistanceToCenterMask = sqrt(xSq+yc*yc);
				double DistanceToCenterImage = DistanceToCenterMask*gain;
				double dS = DistanceToCenterMask - DistanceToCenterImage;
				double dD = m_Distance*(sqrt(1.+dS*dS/(m_Distance*m_Distance))-1);
				zone.dFi += +m_KVector*dD;

				zone.dFi -= pArea->GetPhaseShift();
				TRACE("dFi = %f\n", zone.dFi);
				double ray_len = sqrt((xc*xc+yc*yc)*(1.-gain)+m_Distance*m_Distance);
				zone.nz = m_Distance/ray_len;
				m_Zones.push_back(zone);
			}
		}
	}
	// Update image points
	char szStatus[128];
	for (i = 0; i < m_nImageSize; i++)
	{
		if (pDlg)
		{
			sprintf(szStatus, "Aerial image calculation, %5.1f %% done", i*100./m_nImageSize);
			pDlg->SetStatus(szStatus);
			pDlg->SetPos((int)(i*100./m_nImageSize));
			if (pDlg->CheckCancelButton())
				break;
		}
		for (j = 0; j < m_nImageSize; j++)
		{
			SImagePoint *pPoint = m_pImage + i*(m_nImageSize+1) + j;
			for (CZones::iterator ZoneIt = m_Zones.begin(); ZoneIt != m_Zones.end(); ZoneIt++)
			{
				SZone *pZone = ZoneIt;
				UpdateImagePoint(pPoint, pZone);
			}
		}
	}

	// Calc image intensity
	double fMinIntensity = +VERY_BIG_DOUBLE;
	double fMaxIntensity = -VERY_BIG_DOUBLE;
	for (i = 0; i < m_nImageSize; i++)
	{
		for (j = 0; j < m_nImageSize; j++)
		{
			SImagePoint *pPoint = m_pImage + i*(m_nImageSize+1) + j;
			pPoint->fIntensity = pPoint->fField.cabs2();
			if (pPoint->fIntensity > fMaxIntensity)
				fMaxIntensity = pPoint->fIntensity;
			else if (pPoint->fIntensity < fMinIntensity)
				fMinIntensity = pPoint->fIntensity;
		}
	}

	for (i = 0; i < m_nImageSize; i++)
	{
		for (j = 0; j < m_nImageSize; j++)
		{
			SImagePoint *pPoint = m_pImage + i*(m_nImageSize+1) + j;
			pPoint->pRGBA = Intensity2RGBA(pPoint->fIntensity, fMaxIntensity);
		}
	}
}

double *CLithoRenderer::Intensity2RGBA(double fIntensity, double fMaxIntensity)
{
	int iColor = (int)(fIntensity*m_nColors/fMaxIntensity-0.4999);
	return m_Colors[iColor];
}

void CLithoRenderer::UpdateImagePoint(SImagePoint *pPoint, SZone *pZone)
{
	double dx = pZone->x - pPoint->x;
	double dy = pZone->y - pPoint->y;
	double dxy2 = dx*dx+dy*dy;
	double r = sqrt(dxy2 + m_Distance*m_Distance);
	double dr = r - m_Distance;
	double dFi = pZone->dFi - m_KVector*dr;
	double angle_corr = 1.-sqrt(dxy2)/r;
	SComplex expo;
	double Re = cos(dFi)*pZone->Sq/r*pZone->nz*angle_corr;
	double Im = sin(dFi)*pZone->Sq/r*pZone->nz*angle_corr;
	expo.Set(Re, Im);
	pPoint->fField += expo;
}

CRectArea *CLithoRenderer::IsInAnyRect(double x, double y)
{
	for (CRectangles::iterator RectIt = m_Rectangles.begin(); RectIt != m_Rectangles.end(); RectIt++)
	{
		CRectArea *pArea = *RectIt;
		CRectangle2D *pRectangle = pArea->GetRectangle();
		if (pRectangle->IsPointInside(x, y))
			return pArea;
	}
	return NULL;
}

void CLithoRenderer::RenderMask()
{
	if (!m_bMaskVisible)
		return;
	glColor4d(1,1,1, 0.2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
		glVertex2d(-m_MaskSize/2, -m_MaskSize/2);
		glVertex2d(-m_MaskSize/2, +m_MaskSize/2);
		glVertex2d(+m_MaskSize/2, +m_MaskSize/2);
		glVertex2d(+m_MaskSize/2, -m_MaskSize/2);
	glEnd();
	glDisable(GL_BLEND);
}

void CLithoRenderer::RenderImage()
{
	if (!m_pImage)
		return;
	double scale = m_MaskSize/m_ImageSize;
	StoreModelViewMatrix();
	glScaled(scale, scale, 1);
	
	glBegin(GL_QUADS);
	for (int i = 0; i < m_nImageSize; i++)
	{
		for (int j = 0; j < m_nImageSize; j++)
		{
			SImagePoint *pPoint = m_pImage + i*(m_nImageSize+1) + j;
			glColor4dv(pPoint->pRGBA);
			glVertex2d(pPoint->x, pPoint->y);
			pPoint = m_pImage + (i+1)*(m_nImageSize+1) + j;
			glColor4dv(pPoint->pRGBA);
			glVertex2d(pPoint->x, pPoint->y);
			pPoint = m_pImage + (i+1)*(m_nImageSize+1) + (j+1);
			glColor4dv(pPoint->pRGBA);
			glVertex2d(pPoint->x, pPoint->y);
			pPoint = m_pImage + i*(m_nImageSize+1) + (j+1);
			glColor4dv(pPoint->pRGBA);
			glVertex2d(pPoint->x, pPoint->y);
		}
	}
	glEnd();
	
	if (m_bImageGridVisible)
	{
		glColor3d(0.5, 0.5, 0.5);
		for (int i = 0; i < m_nImageSize; i++)
		{
			for (int j = 0; j < m_nImageSize; j++)
			{
				glBegin(GL_LINE_LOOP);
				SImagePoint *pPoint = m_pImage + i*(m_nImageSize+1) + j;
				glVertex2d(pPoint->x, pPoint->y);
				pPoint = m_pImage + (i+1)*(m_nImageSize+1) + j;
				glVertex2d(pPoint->x, pPoint->y);
				pPoint = m_pImage + (i+1)*(m_nImageSize+1) + (j+1);
				glVertex2d(pPoint->x, pPoint->y);
				pPoint = m_pImage + i*(m_nImageSize+1) + (j+1);
				glVertex2d(pPoint->x, pPoint->y);
				glEnd();
			}
		}
	}
	RestoreModelViewMatrix();
}

LOGICAL CLithoRenderer::ReallocImage()
{
	int nImageSize = (int)(m_ImageSize/m_ImageAccuracy);
	SImagePoint *pImage = (SImagePoint *)realloc(m_pImage, sizeof(SImagePoint)*(nImageSize+1)*(nImageSize+1));
	if (!pImage)
	{
		MessageBox(NULL, "Out of memory", "Error!", MB_ICONSTOP);
		return LOGICAL_FALSE;
	}
	m_pImage = pImage;
	m_nImageSize = nImageSize;
	for (int i = 0; i <= m_nImageSize; i++)
	{
		double x = i*m_ImageAccuracy - m_ImageSize/2;
		for (int j = 0; j <= m_nImageSize; j++)
		{
			double y = j*m_ImageAccuracy - m_ImageSize/2;
			SImagePoint *pPoint = m_pImage + i*(m_nImageSize+1) + j;
			pPoint->x = x;
			pPoint->y = y;
			pPoint->fIntensity = 0;
			pPoint->fField.Zero();
	//		pPoint->rgba.Init(125*(cos(25*(x+y)/m_ImageSize)) + 128, 125*(cos(25*y/m_ImageSize)) + 128, 125*(sin(15*x/m_ImageSize)) + 128);
			pPoint->pRGBA = m_Colors[0];
		}
	}
	return LOGICAL_TRUE;
}
