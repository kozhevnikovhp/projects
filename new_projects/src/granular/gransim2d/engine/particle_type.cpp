#include "particle_type.h"
#include "text_const.h"
#include "design_text_string.h"
#include "gransim2d_design.h"
#include "particle_shape_vertex.h"
#include "global.h"
#include "portable/portable.h"
#include "random/random.h"
#include <string.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////
// CParticleType

static SFormDesc ParticleTypeFormDescs[] = {
	{ OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 5 },
	{ PARTICLE_TYPE_SIZE_INDEX, "Size", 2 },
	{ PARTICLE_TYPE_SHAPE_INDEX, "Shape", 2 },
	{ PARTICLE_TYPE_MATERIAL_INDEX, PSZ_MATERIAL, 2 }
};

CParticleType::CParticleType()
{
	m_SizeType = PARTICLE_SIZE_FIXED;
	m_SizeFixed = 1.; // 1mm
	m_SizeMedian = 1.; // 1mm
	m_SizeSigma = 1.; // 1mm (standard deviation)
	m_SizeFrom = 0;
	m_SizeTo = 1.; // 1mm;
	m_ShapeAspectRatio = 1;
	m_ShapeSquareness = 2;
	m_pMaterial = NULL;
	RecalcShapeVertices();
}

//virtual
CParticleType::~CParticleType()
{
}

double CParticleType::CalcMaxDistributionDensity()
{
	switch (m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		return 1;
	case PARTICLE_SIZE_UNIFORM:
		return 1./(m_SizeTo - m_SizeFrom);
	case PARTICLE_SIZE_GAUSS:
		return 1./(m_SizeSigma*common::calc::SQRT_2PI);
	default:
		break;
	}
	return 1;
}

double CParticleType::GetMaxParticleSize()
{
	switch (m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		return m_SizeFixed;
	case PARTICLE_SIZE_UNIFORM:
		return m_SizeTo;
	case PARTICLE_SIZE_GAUSS:
		return m_SizeMedian + 5*m_SizeSigma;
	default:
		break;
	}
	return 1;
}

double CParticleType::GetMinParticleSize()
{
	switch (m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		return m_SizeFixed;
	case PARTICLE_SIZE_UNIFORM:
		return m_SizeFrom;
	case PARTICLE_SIZE_GAUSS:
		return std::min(0., m_SizeMedian - 5*m_SizeSigma);
	default:
		break;
	}
	return 1;
}

double CParticleType::GetAveParticleSize()
{
	switch (m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		return m_SizeFixed;
	case PARTICLE_SIZE_UNIFORM:
		return (m_SizeFrom + m_SizeTo)/2;
	case PARTICLE_SIZE_GAUSS:
		return m_SizeMedian;
	default:
		break;
	}
	return 1;
}

double CParticleType::GenSize()
{
	switch (m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		return m_SizeFixed;
	case PARTICLE_SIZE_UNIFORM:
		return g_RandomGen.GenDouble(m_SizeFrom, m_SizeTo);
	case PARTICLE_SIZE_GAUSS:
		return g_RandomGen.GenGauss(0, common::calc::VERY_BIG_DOUBLE, m_SizeMedian, m_SizeSigma);
	default:
		break;
	}
	return m_SizeFixed;
}

double PowerN(double x, int N)
{
	double xx = x;
	for (int i = 1; i< N; i++)
		xx *= x;
	return xx;
}

double Shape(const double x, const void *pContext)
{
	if (fabs(x-1.) < 1.E-6)
		return 0;
	CParticleType *pType = (CParticleType *)pContext;
	double xn = PowerN(x, pType->GetShapeSquareness()); // X**n
	double y = exp(1./pType->GetShapeSquareness()*log(1.-xn))/pType->GetShapeAspectRatio();
	return y;
}

double InertiaMomentum(const double x, const void *pContext)
{
	double shape = Shape(x, pContext);
	return shape*(x*x+shape*shape/3);
}

void CParticleType::RecalcShapeVertices()
{
	int i;
	// Vertices are in CCW order in array, starting from negative OY direction
	m_MaxDistanceToParticleCenter = 0.5;
	m_MinDistanceToParticleCenter = 0.5/m_ShapeAspectRatio;
	// OY axis (0, 180 CW)
	m_Vertices[0].x = 0.;
	m_Vertices[0].y = -0.5/m_ShapeAspectRatio;
	m_Vertices[0].DistanceToParticleCenter = 0.5/m_ShapeAspectRatio;
	m_Vertices[0].Incline = 0;
	m_Vertices[0].R = 0.5/m_ShapeAspectRatio;
	m_Vertices[0].Xk = 0;
	m_Vertices[0].Yk = 0;

	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].x = 0.;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].y = +0.5/m_ShapeAspectRatio;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].DistanceToParticleCenter = 0.5/m_ShapeAspectRatio;
		m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].Incline = common::calc::PI;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].R = 0.5/m_ShapeAspectRatio;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].Xk = 0;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].Yk = 0;

	// OX axis (90, 270 CW)
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].x = 0.5;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].y = 0;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].DistanceToParticleCenter = 0.5;
		m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].Incline = common::calc::PI/2.;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].R = 0.5;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].Xk = 0;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].Yk = 0;

	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].x = -0.5;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].y = 0;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].DistanceToParticleCenter = 0.5;
		m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].Incline = 3.*common::calc::PI/2.;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].R = 0.5;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].Xk = 0;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].Yk = 0;

		for (i = 1; i < N_PARTICLE_SHAPE_VERTICES/4; i++)
	{
		double x, y, Incline;
		double DesiredIncline = i*360./N_PARTICLE_SHAPE_VERTICES;
				double leftX = 0;
				double rightX = 1;
		double dYdX, d2YdX2;
		do
		{
			x = (leftX + rightX)/2;
			double xn1 = PowerN(x, m_ShapeSquareness-1); // X**(n-1)
			double xn = xn1*x; // X**n
			double xn2 = xn1/x; // X**(n-2)
			y = -exp(1./m_ShapeSquareness*log(1.-xn))/m_ShapeAspectRatio;
			dYdX = xn1*exp((1./m_ShapeSquareness-1.)*log(1.-xn))/m_ShapeAspectRatio;
			d2YdX2 = (m_ShapeSquareness-1.)*xn2*exp((1./m_ShapeSquareness-2.)*log(1.-xn))/m_ShapeAspectRatio;
						Incline = common::calc::RADIANS_2_DEGREES(atan(dYdX));
			if (Incline < DesiredIncline)
				leftX = x;
			else
				rightX = x;
		} while (fabs(Incline - DesiredIncline) > 0.0001);

		double dY = (1.+dYdX*dYdX)/d2YdX2;
		double dX = -dYdX*dY;
		double R = sqrt(dX*dX + dY*dY);
		double Xk = x + dX;
		double Yk = y + dY;

		double DistanceToParticleCenter = sqrt(x*x + y*y)/2;
		if (DistanceToParticleCenter > m_MaxDistanceToParticleCenter)
			m_MaxDistanceToParticleCenter = DistanceToParticleCenter;

		// 4th quadrant
		m_Vertices[i].x = +x/2;
		m_Vertices[i].y = +y/2;
		m_Vertices[i].DistanceToParticleCenter = DistanceToParticleCenter;
				m_Vertices[i].Incline = common::calc::DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[i].R = R/2;
		m_Vertices[i].Xk = +Xk/2;
		m_Vertices[i].Yk = +Yk/2;

		// 1st quadrant
		int j = N_PARTICLE_SHAPE_VERTICES/2 - i;
		m_Vertices[j].x = +x/2;
		m_Vertices[j].y = -y/2;
		m_Vertices[j].DistanceToParticleCenter = DistanceToParticleCenter;
				m_Vertices[j].Incline = common::calc::PI - common::calc::DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[j].R = R/2;
		m_Vertices[j].Xk = +Xk/2;
		m_Vertices[j].Yk = -Yk/2;

		// 2nd quadrant
		j = N_PARTICLE_SHAPE_VERTICES/2 + i;
		m_Vertices[j].x = -x/2;
		m_Vertices[j].y = -y/2;
		m_Vertices[j].DistanceToParticleCenter = DistanceToParticleCenter;
				m_Vertices[j].Incline = common::calc::PI + common::calc::DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[j].R = R/2;
		m_Vertices[j].Xk = -Xk/2;
		m_Vertices[j].Yk = -Yk/2;

		// 3rd quadrant
		j = N_PARTICLE_SHAPE_VERTICES - i;
		m_Vertices[j].x = -x/2;
		m_Vertices[j].y = y/2;
		m_Vertices[j].DistanceToParticleCenter = DistanceToParticleCenter;
				m_Vertices[j].Incline = common::calc::PI2 - common::calc::DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[j].R = R/2;
		m_Vertices[j].Xk = -Xk/2;
		m_Vertices[j].Yk = +Yk/2;
	}

	// Recalc other fields for future fun
	for (i = 0; i < N_PARTICLE_SHAPE_VERTICES; i++)
	{
		sParticleShapeVertex *pShapeVertex = m_Vertices+i;
		pShapeVertex->RecalcAux();
	}

	m_Mass = common::calc::IntegralSimpson(Shape, this, 0, 1, 1.E-6)*4;
	m_InertiaMomentum = common::calc::IntegralSimpson(InertiaMomentum, this, 0, 1, 1.E-6)*4; //
}

/*void CParticleType::RebuildDisplayLists()
{
	RecalcShapeVertices();
}*/

void CParticleType::SetSizeType(const char *pszType)
{
	if (!stricmp(pszType, PSZ_FIXED_KEYWORD))
		m_SizeType = PARTICLE_SIZE_FIXED;
	else if (!stricmp(pszType, PSZ_UNIFORM_KEYWORD))
		m_SizeType = PARTICLE_SIZE_UNIFORM;
	else if (!stricmp(pszType, PSZ_GAUSS_KEYWORD))
		m_SizeType = PARTICLE_SIZE_GAUSS;
}

void CParticleType::SetShapeSquareness(double fSquareness)
{
	m_ShapeSquareness = fSquareness;
	if (m_ShapeSquareness < 2)
		m_ShapeSquareness = 2;
	UpdateCircularFlag();
	RecalcShapeVertices();
}

void CParticleType::SetShapeAspectRatio(double fAspectRatio)
{
	m_ShapeAspectRatio = fAspectRatio;
	if (m_ShapeAspectRatio < 1)
		m_ShapeAspectRatio = 1;
	UpdateCircularFlag();
	RecalcShapeVertices();
}

// Incline in radians
const sParticleShapeVertex *CParticleType::GetShapePointIncline(double Incline)
{
	const double step = common::calc::PI2/N_PARTICLE_SHAPE_VERTICES;
	int index = int(Incline/step + 0.49999) % N_PARTICLE_SHAPE_VERTICES;
	return m_Vertices+index;
}

//virtual
const char *CParticleType::GetKeyWord()
{
	return PSZ_PARTICLE_TYPE_KEYWORD;
}

//virtual
int CParticleType::GetFormCount()
{
	return sizeof(ParticleTypeFormDescs)/sizeof(ParticleTypeFormDescs[0]);
}

//virtual
SFormDesc *CParticleType::GetFormDesc(int iFormNumber)
{
	return ParticleTypeFormDescs+iFormNumber;
}

//virtual
void CParticleType::SaveToFile(FILE *fd)
{
	CDesignTextString str;
	// Name
	str.Set(PSZ_NAME_KEYWORD, GetGivenName());
	m_pDesign->WriteString(fd, &str);

	// Size
	switch (m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		str.Set(PSZ_SIZE_TYPE_KEYWORD, PSZ_FIXED_KEYWORD);
		break;
	case PARTICLE_SIZE_UNIFORM:
		str.Set(PSZ_SIZE_TYPE_KEYWORD, PSZ_UNIFORM_KEYWORD);
		break;
	case PARTICLE_SIZE_GAUSS:
		str.Set(PSZ_SIZE_TYPE_KEYWORD, PSZ_GAUSS_KEYWORD);
		break;
	default:
//		ASSERT(0);
		break;
	}
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_FIXED_SIZE_KEYWORD, m_SizeFixed);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_FROM_SIZE_KEYWORD, m_SizeFrom);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_TO_SIZE_KEYWORD, m_SizeTo);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_MEDIAN_KEYWORD, m_SizeMedian);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_SIGMA_KEYWORD, m_SizeSigma);
	m_pDesign->WriteString(fd, &str);

	// Shape
	str.Set(PSZ_SQUARENESS_KEYWORD, m_ShapeSquareness);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_ASPECT_RATIO_KEYWORD, m_ShapeAspectRatio);
	m_pDesign->WriteString(fd, &str);

	// Material
	if (m_pMaterial)
	{
		str.Set(PSZ_MATERIAL_KEYWORD, m_pMaterial->GetGivenName());
		m_pDesign->WriteString(fd, &str);
	}
}

//virtual
void CParticleType::DeleteFromDesign()
{
	m_pDesign->DeleteParticleType(this);
}

void CParticleType::UpdateCircularFlag()
{
	m_bCircular = ((fabs(m_ShapeAspectRatio - 1.) < 0.001) && (fabs(m_ShapeSquareness - 2) < 0.001));
}

