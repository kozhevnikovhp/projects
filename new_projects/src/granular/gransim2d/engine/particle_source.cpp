#include "particle_source.h"
#include "design_text_string.h"
#include "text_const.h"
#include "particle2d.h"
#include "gransim2d_design.h"
#include "global.h"
#include "portable/portable.h"
#include <string.h>
#include <math.h>

using namespace common;
using namespace geometry2d;

////////////////////////////////////////////////////////////////////////////
// CParticleSource

//static
CParticle2D CParticleSource::g_ProbeParticle;

CParticleSource::CParticleSource()
{
	m_FlowType = PARTICLE_SOURCE_FLOW_TYPE_UNIFORM;
	m_fIntensity = 1;
	m_fNextGenTime = 0;
	m_pParticleType = NULL;
	m_RGBA.Init(0x00, 0xEF, 0x00); // particle sources are green. TODO: load initial color from application defaults
}

//virtual
CParticleSource::~CParticleSource()
{
}

void CParticleSource::SetFlowType(const char *pszType)
{
	if (!stricmp(pszType, PSZ_POISSON_FLOW_KEYWORD))
		m_FlowType = PARTICLE_SOURCE_FLOW_TYPE_POISSON;
	if (!stricmp(pszType, PSZ_UNIFORM_FLOW_KEYWORD))
		m_FlowType = PARTICLE_SOURCE_FLOW_TYPE_UNIFORM;
}

void CParticleSource::SetParticleType(CParticleType *pType)
{
	m_pParticleType = pType;
	if (m_pParticleType)
		m_ParticleTypeName = m_pParticleType->GetGivenName();
	else
		m_ParticleTypeName.Empty();
}

//virtual
void CParticleSource::SaveToFile(FILE *fd)
{
	CDesignTextString str;
	// Name
	str.Set(PSZ_NAME_KEYWORD, GetGivenName());
	m_pDesign->WriteString(fd, &str);

	// Color
	str.Set(PSZ_COLOR_KEYWORD, GetRGBA());
	m_pDesign->WriteString(fd, &str);

	// ParticleType
	if (m_pParticleType)
	{
		str.Set(PSZ_PARTICLE_TYPE_KEYWORD, m_pParticleType->GetGivenName());
		m_pDesign->WriteString(fd, &str);
	}

	// FlowType
	switch (m_FlowType)
	{
	case PARTICLE_SOURCE_FLOW_TYPE_UNIFORM:
		str.Set(PSZ_FLOW_TYPE_KEYWORD, PSZ_UNIFORM_FLOW_KEYWORD);
		break;
	case PARTICLE_SOURCE_FLOW_TYPE_POISSON:
		str.Set(PSZ_FLOW_TYPE_KEYWORD, PSZ_POISSON_FLOW_KEYWORD);
		break;
	default:
//		ASSERT(0);
		break;
	}
	m_pDesign->WriteString(fd, &str);
	// Intensity
	str.Set(PSZ_FLOW_INTENSITY_KEYWORD, m_fIntensity);
	m_pDesign->WriteString(fd, &str);

	//Geometry
	std::string GeomStr;
	GetGeometry()->Format(GeomStr);
	str.Set(PSZ_GEOMETRY_KEYWORD, GeomStr.c_str());
	m_pDesign->WriteString(fd, &str);
}

//virtual
void CParticleSource::DeleteFromDesign()
{
	m_pDesign->DeleteParticleSource(this);
}

void CParticleSource::Simulation_Started()
{
	m_fNextGenTime = g_RandomGen.GenDouble(0, 1./m_fIntensity);
}

void CParticleSource::Simulation_ClipTimeStep(double t, double &dt)
{
	if ((t+dt) > m_fNextGenTime)
		dt = m_fNextGenTime - t;
}

CParticle2D *CParticleSource::Simulation_NewParticle(double t)
{
	if (t < m_fNextGenTime)
		return NULL;
	if (m_FlowType == PARTICLE_SOURCE_FLOW_TYPE_UNIFORM)
		m_fNextGenTime += 1./m_fIntensity;
	else if (m_FlowType == PARTICLE_SOURCE_FLOW_TYPE_POISSON)
		m_fNextGenTime += -log(1.-g_RandomGen.Gen01())/m_fIntensity;

	bool bSuccess = false;
	g_ProbeParticle.m_pParticleType = m_pParticleType;
	double ParticleSize;
	for (int iAttempt = 0; iAttempt < 100; iAttempt++)
	{
		LocateProbeParticle();
		g_ProbeParticle.SetFi(CURRENT_STATE, g_RandomGen.GenDouble(0, common::calc::PI));
		ParticleSize = g_ProbeParticle.m_fSize = m_pParticleType->GenSize();
		if (!m_pDesign->IsFreeSpaceForParticle(CURRENT_STATE, &g_ProbeParticle))
			continue;
		if (m_pDesign->DoesParticleTouchReflectingObject(CURRENT_STATE, &g_ProbeParticle))
			continue;
		bSuccess = true;
		break;
	}
	if (bSuccess)
	{
		CParticle2D *pNewParticle = new CParticle2D(&g_ProbeParticle);
		double scale = ParticleSize/2;
		pNewParticle->m_M = m_pParticleType->GetMass()*scale*scale*m_pParticleType->m_pMaterial->m_Ro;
		pNewParticle->m_I = m_pParticleType->GetInertiaMomentum()*scale*scale*scale*scale*m_pParticleType->m_pMaterial->m_Ro;
		pNewParticle->m_pDesign = m_pDesign;
		return pNewParticle;
	}

	// TODO: increment error count and report the error
	return NULL;
}


///////////////////////////////////////////////////////////////////////////
// CParticlePointSource

static SFormDesc ParticlePointSourceFormDescs[] = {
	{ GEOMETRY_OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 5 },
	{ PARTICLE_SOURCE_TYPE_FORM_INDEX, PSZ_PARTICLE_TYPE, 2 },
	{ POINT_GEOMETRY_FORM_INDEX, PSZ_GEOMETRY, 2 },
	{ PARTICLE_SOURCE_INTENSITY_FORM_INDEX, PSZ_INTENSITY, 4 }
};

CParticlePointSource::CParticlePointSource(CPoint2D *pPoint)
{
	m_Point.Init(pPoint);
}

//virtual
CParticlePointSource::~CParticlePointSource()
{
}

void CParticlePointSource::SetPoint(CPoint2D *pPoint)
{
	m_Point.Set(pPoint);
}

//virtual
const char *CParticlePointSource::GetKeyWord()
{
	return PSZ_POINT_PARTICLE_SOURCE_KEYWORD;
}

//virtual
int CParticlePointSource::GetFormCount()
{
	return sizeof(ParticlePointSourceFormDescs)/sizeof(ParticlePointSourceFormDescs[0]);
}

//virtual
SFormDesc *CParticlePointSource::GetFormDesc(int iFormNumber)
{
	return ParticlePointSourceFormDescs+iFormNumber;
}

//virtual
void CParticlePointSource::LocateProbeParticle()
{
	g_ProbeParticle.SetPosition(CURRENT_STATE, m_Point.X(), m_Point.Y());
}

///////////////////////////////////////////////////////////////////////////
// CParticleLinearSource

static SFormDesc ParticleLinearSourceFormDescs[] = {
	{ GEOMETRY_OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 4 },
	{ PARTICLE_SOURCE_TYPE_FORM_INDEX, PSZ_PARTICLE_TYPE, 2 },
	{ LINE_GEOMETRY_FORM_INDEX, PSZ_GEOMETRY, 1 },
	{ PARTICLE_SOURCE_INTENSITY_FORM_INDEX, PSZ_INTENSITY, 4 }
};

CParticleLinearSource::CParticleLinearSource(CLine2D *pLine)
{
	m_Line.Init(pLine);
}

//virtual
CParticleLinearSource::~CParticleLinearSource()
{
}

//virtual
const char *CParticleLinearSource::GetKeyWord()
{
	return PSZ_LINEAR_PARTICLE_SOURCE_KEYWORD;
}

//virtual
int CParticleLinearSource::GetFormCount()
{
	return sizeof(ParticleLinearSourceFormDescs)/sizeof(ParticleLinearSourceFormDescs[0]);
}

//virtual
SFormDesc *CParticleLinearSource::GetFormDesc(int iFormNumber)
{
	return ParticleLinearSourceFormDescs+iFormNumber;
}

//virtual
void CParticleLinearSource::LocateProbeParticle()
{
	double s = g_RandomGen.Gen01();
	double x = m_Line.X1()*s + m_Line.X2()*(1.-s);
	double y = m_Line.Y1()*s + m_Line.Y2()*(1.-s);
	g_ProbeParticle.SetPosition(CURRENT_STATE, x, y);
}


