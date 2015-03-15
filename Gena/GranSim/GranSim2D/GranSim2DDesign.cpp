/* GranSim2DDesign.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2007 - initial creation
	23 Sep 2008 - project tree items appearance and updating
	10 Oct 2008 - reflecting wall creation functionality
	11 Oct 2008 - linear/point particle source creation functionality
	13 Oct 2008 - status bar text from design; object selection
	16 Oct 2008 - splitted "object common form" and "geometry object common form"; Particle size and shape form
	17 Oct 2008 - particle material form; particle source object hierarchy
	02 Nov 2008 - design file saving
	10 Nov 2008 - particle types (size and shape)
	15 Nov 2008 - materials for reflecting objects, particle types
	24 Nov 2008 - file saving/loading update
	10 Dec 2008 - simulation stuff (checking for sim, particle genaeration)
	12 Dec 2008 - material for particle type and particle type for particle sources loading from file
	15 Dec 2008 - particle generation; simulation (initial edition)
	04 Jan 2009 - reflecting rectangle functionality (base)
	05 Jan 2009 - objects deletion
	28 Mar 2009 - split design and editor/renderer stuff
	10 Apr 2009 - particle shape vertices re-organised (each vertex corresponds to known shape incline now)
	27 May 2009 - particle states (CURRENT, TR etc) for efolution in time domain
	20 Jul 2009 - Velocity Verlet time integration
	25 Aug 2009 - Simulation cells approach
*/

#include "stdafx.h"
#include "GranSim2DDesign.h"
#include "GuiAPI.h"
#include "RandomGen.h"
#include "Calculations.h"
#include <math.h>

static char *PSZ_EMPTY = "";
static char *PSZ_NONAME = "noname";
static char *PSZ_GEOMETRY = "Geometry";
static char *PSZ_MATERIAL = "Material";
static char *PSZ_PARTICLE_TYPE = "Particle Type";
static char *PSZ_INTENSITY = "Intensity";

static char *PSZ_DESIGN_KEYWORD = "DESIGN";
static char *PSZ_SIMULATION_KEYWORD = "SIMULATION";
static char *PSZ_SIMULATE_UNTIL_KEYWORD = "SIMULATE_UNTIL";
static char *PSZ_TIME_INTEGRATION_METHOD_KEYWORD = "TIME_INTEGRATION_METHOD";
static char *PSZ_IMPLICIT_EULER = "IMPLICIT_EULER";
static char *PSZ_VERLET = "VERLET";
static char *PSZ_PREDICTOR_CORRECTOR = "PREDICTOR_CORRECTOR";
static char *PSZ_TRAPEZOID = "TRAPEZOID";
static char *PSZ_RUNGE_KUTTA_2 = "RUNGE_KUTTA_2";
static char *PSZ_RUNGE_KUTTA_4 = "RUNGE_KUTTA_4";
static char *PSZ_MATERIAL_KEYWORD = "MATERIAL";
static char *PSZ_PARTICLE_TYPE_KEYWORD = "PARTICLE_TYPE";
static char *PSZ_LINEAR_PARTICLE_SOURCE_KEYWORD = "LINEAR_PARTICLE_SOURCE";
static char *PSZ_POINT_PARTICLE_SOURCE_KEYWORD = "POINT_PARTICLE_SOURCE";
static char *PSZ_NAME_KEYWORD = "NAME";
static char *PSZ_COLOR_KEYWORD = "COLOR";
static char *PSZ_GEOMETRY_KEYWORD = "GEOMETRY";
static char *PSZ_POISSON_RATIO_KEYWORD = "POISSON_RATIO";
static char *PSZ_YOUNG_MODULE_KEYWORD = "YOUNG_MODULE";
static char *PSZ_DENSITY_KEYWORD = "DENSITY";
static char *PSZ_SQUARENESS_KEYWORD = "SQUARENESS";
static char *PSZ_ASPECT_RATIO_KEYWORD = "ASPECT_RATIO";
static char *PSZ_SIZE_TYPE_KEYWORD = "SIZE_TYPE";
static char *PSZ_FIXED_KEYWORD = "FIXED";
static char *PSZ_UNIFORM_KEYWORD = "UNIFORM";
static char *PSZ_GAUSS_KEYWORD = "GAUSS";
static char *PSZ_FIXED_SIZE_KEYWORD = "FIXED_SIZE";
static char *PSZ_FROM_SIZE_KEYWORD = "FROM_SIZE";
static char *PSZ_TO_SIZE_KEYWORD = "TO_SIZE";
static char *PSZ_MEDIAN_KEYWORD = "MEDIAN";
static char *PSZ_SIGMA_KEYWORD = "SIGMA";
static char *PSZ_FLOW_TYPE_KEYWORD = "FLOW_TYPE";
static char *PSZ_POISSON_FLOW_KEYWORD = "POISSON_FLOW";
static char *PSZ_UNIFORM_FLOW_KEYWORD = "UNIFORM_FLOW";
static char *PSZ_FLOW_INTENSITY_KEYWORD = "FLOW_INTENSITY";
static char *PSZ_WALL_KEYWORD = "WALL";
static char *PSZ_RECTANGLE_KEYWORD = "RECTANGLE";

static char *PSZ_END_KEYWORD = "END";

static char *PSZ_RGBA_FORMAT_STRING = "%hd, %hd, %hd, %hd";

CRandomGen g_RandomGen;

// Simulation data and options
IGuiAPI *g_pGui = NULL;

void SParticleShapeVertex::RecalcAux()
{
	m_Sine = sin(Incline);
	m_Cosine = cos(Incline);
}

///////////////////////////////////////////////////////////////////////////
// CListOf

template <class T>
CListOf<T>::CListOf()
{
}

//virtual
template <class T>
CListOf<T>::~CListOf()
{
	DeleteAll();
}

template <class T>
void CListOf<T>::DeleteAll()
{
	for (iterator p = begin(); p != end(); p++)
	{
		delete *p;
	}
	resize(0);
}

template <class T>
void CListOf<T>::RemoveObject(T *pObjectToRemove)
{
	for (iterator p = begin(); p != end(); p++)
	{
		T *pObject = *p;
		if (pObject == pObjectToRemove)
		{
			erase(p);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDesignTextString

static const char *PSZ_EQUALS_KEYWORD = " = ";

void CDesignTextString::Set(char *pszParamName, char *pszTextValue)
{
	Init();
	*this += pszParamName;
	*this += PSZ_EQUALS_KEYWORD;
	char *pcBlank = strchr(pszTextValue, ' ');
	if (pcBlank)
		*this += '"';
	*this += pszTextValue;
	if (pcBlank)
		*this += '"';
}

void CDesignTextString::Set(char *pszParamName, SRGBA *pColor)
{
	Init();
	*this += pszParamName;
	*this += PSZ_EQUALS_KEYWORD;
	char szColor[64];
	*this += '"';
	sprintf(szColor, PSZ_RGBA_FORMAT_STRING, pColor->R(), pColor->G(), pColor->B(), pColor->A());
	*this += szColor;
	*this += '"';
}

void CDesignTextString::Set(char *pszParamName, double fValue)
{
	Init();
	*this += pszParamName;
	*this += PSZ_EQUALS_KEYWORD;
	char szValue[64];
	sprintf(szValue, "%e", fValue);
	*this += szValue;
}

/////////////////////////////////////////////////////////////////////////////
// SParticleState

//inline
void SParticleState::Set(SParticleState *pState)
{
	memcpy(this, pState, sizeof(SParticleState));
/*	m_Position.Set(state.m_Position);
	m_Velocity.Set(state.m_Velocity);
	m_Force.Set(state.m_Force);
	m_Fi = state.m_Fi;
	m_Omega = state.m_Omega;
	m_Cosine = state.m_Cosine;
	m_Sine = state.m_Sine;*/
}

void SParticleState::SetFi(double fi)
{
	NormalizeAngleRadians(fi);
	m_Fi = fi;
	m_Cosine = cos(fi);
	m_Sine = sin(fi);
}

/////////////////////////////////////////////////////////////////////////////
// CParticle2D

CParticle2D::CParticle2D()
{
	memset(m_State, 0, sizeof(m_State));
	m_pParticleType = NULL;
	m_fSize = m_M = m_I = 0;
	m_pParticleType = NULL;
	m_pDesign = NULL;
}

CParticle2D::CParticle2D(CParticle2D *pParticle)
{
	memset(m_State, 0, sizeof(m_State));
	m_pParticleType = pParticle->m_pParticleType;
	m_fSize = pParticle->m_fSize;
	m_State[CURRENT_STATE].Set(pParticle->m_State+CURRENT_STATE);
}

/*void CParticle2D::ZerothApproximation(double dt)
{
	m_State[NEXT_STATE].m_X = m_State[CURRENT_STATE].m_X + m_State[CURRENT_STATE].m_Vx*dt;
	m_State[NEXT_STATE].m_Y = m_State[CURRENT_STATE].m_Y + m_State[CURRENT_STATE].m_Vy*dt;
	m_State[NEXT_STATE].m_Vx = m_State[CURRENT_STATE].m_Vx;
	m_State[NEXT_STATE].m_Vy = m_State[CURRENT_STATE].m_Vy;
	SetFi(NEXT_STATE, m_State[CURRENT_STATE].m_Fi + m_State[CURRENT_STATE].m_Omega*dt);
	m_State[NEXT_STATE].m_Omega = m_State[CURRENT_STATE].m_Omega;
}*/

void CParticle2D::SetFi(StateEnum iState, double Fi)
{
	if (IsCircular())
		return;
	m_State[iState].SetFi(Fi);
}
LOGICAL CParticle2D::CanOverlap(StateEnum iState, CParticle2D *pParticle)
{
	double MaxRadiusSum = GetMaxDistanceToCenter() + pParticle->GetMaxDistanceToCenter();
	double dx = m_State[iState].m_Position.m_fX - pParticle->m_State[iState].m_Position.m_fX;
	if (fabs(dx) >  MaxRadiusSum)
		return LOGICAL_FALSE;
	double dy = m_State[iState].m_Position.m_fY - pParticle->m_State[iState].m_Position.m_fY;
	if (fabs(dy) >  MaxRadiusSum)
		return LOGICAL_FALSE;
	return ((dx*dx + dy*dy) <= MaxRadiusSum*MaxRadiusSum);
}

void CParticle2D::ShapeVertexToWorld(StateEnum iState, const SParticleShapeVertex *pShapeVertex, double &DstX, double &DstY)
{
	double x = pShapeVertex->x*m_fSize;
	double y = pShapeVertex->y*m_fSize;
	SParticleState *pState = m_State+iState;
	double cosine = m_State[iState].m_Cosine;
	double sine = m_State[iState].m_Sine;
	DstX = +x*pState->m_Cosine - y*pState->m_Sine + pState->m_Position.m_fX;
	DstY = +x*pState->m_Sine + y*pState->m_Cosine + pState->m_Position.m_fY;
}

// Function to be optimized
double CParticle2D::ParticleOverlap(StateEnum iState, CParticle2D *pParticle, int iShapeVertex)
{
	const SParticleShapeVertex *pShapeVertex = m_pParticleType->GetShapeVertex(iShapeVertex % N_PARTICLE_SHAPE_VERTICES);
	double x1, y1;
	ShapeVertexToWorld(iState, pShapeVertex, x1, y1);
	double Incline1 = pShapeVertex->Incline + GetFi(iState);
	NormalizeAngleRadians(Incline1);
	double DesiredIncline = Incline1 + PI - pParticle->GetFi(iState);
	NormalizeAngleRadians(DesiredIncline);
	const SParticleShapeVertex *pAnotherShapeVertex = pParticle->m_pParticleType->GetShapePointIncline(DesiredIncline);
	double x2, y2;
	pParticle->ShapeVertexToWorld(iState, pAnotherShapeVertex, x2, y2);
	double dx = x2-x1;
	double dy = y2-y1;
	return (dx*dx+dy*dy);
}

LOGICAL CParticle2D::IsOverlapped(StateEnum iState, CParticle2D *pParticle, SContact *pContact /*=NULL*/)
{
	// 1.Simple check first
	if (!CanOverlap(iState, pParticle))
		return LOGICAL_FALSE;

	double x1, y1, x2, y2;

	if (!pContact)
	{
		x1 = m_State[iState].m_Position.m_fX - pParticle->m_State[iState].m_Position.m_fX;
		y1 = m_State[iState].m_Position.m_fY - pParticle->m_State[iState].m_Position.m_fY;
		double MinRadiusSum = GetMinDistanceToCenter() + pParticle->GetMinDistanceToCenter();
		if ((x1*x1 + y1*y1) < MinRadiusSum*MinRadiusSum)
			return LOGICAL_TRUE; // very close to each other
	}
	
	// 2.Complicated check now

	// Find good enough zero-th approximation for further optimization
	const N_ANGLES = 8;
	double MinDistance = VERY_BIG_DOUBLE;
	int iBestShapeVertex;
	for (int i = 0; i < N_ANGLES; i++)
	{
		int iShapeVertex = i*N_PARTICLE_SHAPE_VERTICES/N_ANGLES;
		double distance = ParticleOverlap(iState, pParticle, iShapeVertex);
		if (distance < MinDistance)
		{
			MinDistance = distance;
			iBestShapeVertex = iShapeVertex;
		}
	}
	// Optimization itself
	int iLeft = iBestShapeVertex - N_PARTICLE_SHAPE_VERTICES/N_ANGLES-1;
	int iRight = iBestShapeVertex + N_PARTICLE_SHAPE_VERTICES/N_ANGLES+1;
	const double FI = 1.618;
	const double FI1 = 1.-FI;
	if (iLeft < 0)
	{
		iLeft += N_PARTICLE_SHAPE_VERTICES;
		iRight += N_PARTICLE_SHAPE_VERTICES;
	}
	int shift = (int)((iRight-iLeft)/FI+0.49999);
	int i1 = iRight - shift;
	double d1 = ParticleOverlap(iState, pParticle, i1);
	int i2 = iLeft + shift;
	double d2 = ParticleOverlap(iState, pParticle, i2);
	while ((iRight - iLeft) > 1)
	{
		if (d1 < d2)
		{
			iRight = i2;
			i2 = i1;
			d2 = d1;
			i1 = iRight - (int)((iRight-iLeft)/FI+0.49999);
			d1 = ParticleOverlap(iState, pParticle, i1);
		}
		else
		{
			iLeft = i1;
			i1 = i2;
			d1 = d2;
			i2 = iLeft + (int)((iRight-iLeft)/FI+0.49999);
			d2 = ParticleOverlap(iState, pParticle, i2);
		}
	}

	iBestShapeVertex = i1;
	d2 = ParticleOverlap(iState, pParticle, i1-1);
	if (d2 < d1)
	{
		d1 = d2;
		iBestShapeVertex = i1-1;
	}
	d2 = ParticleOverlap(iState, pParticle, i1+1);
	if (d2 < d1)
		iBestShapeVertex = i1+1;
	const SParticleShapeVertex *pShapeVertex = m_pParticleType->GetShapeVertex(iBestShapeVertex % N_PARTICLE_SHAPE_VERTICES);
	ShapeVertexToWorld(iState, pShapeVertex, x1, y1);

	double Incline1 = pShapeVertex->Incline + GetFi(iState);
	NormalizeAngleRadians(Incline1);
	double DesiredIncline = Incline1 + PI - pParticle->GetFi(iState);
	NormalizeAngleRadians(DesiredIncline);
	const SParticleShapeVertex *pAnotherShapeVertex = pParticle->m_pParticleType->GetShapePointIncline(DesiredIncline);
	pParticle->ShapeVertexToWorld(iState, pAnotherShapeVertex, x2, y2);

	// 3. Check of contact points are inside another particle
	// Sine and cosine of Incline1 = pShapeVertex->Incline + m_Fi (to avoid sin/cos calls, using trigonometric formulas)
	double sine = pShapeVertex->m_Sine*m_State[iState].m_Cosine+pShapeVertex->m_Cosine*m_State[iState].m_Sine;
	double cosine = pShapeVertex->m_Cosine*m_State[iState].m_Cosine-pShapeVertex->m_Sine*m_State[iState].m_Sine;
	if (cosine*(y2 - y1) < sine*(x2 - x1))
		return LOGICAL_FALSE;

	if (pContact)
	{
		pContact->Point1.Set(x1, y1);
		pContact->Point2.Set(x2, y2);
		// Real contact point
		pContact->ContactPoint.Set((pContact->Point2.X() + pContact->Point1.X())/2, (pContact->Point2.Y() + pContact->Point1.Y())/2);
		// Real overlapping(projection of vector between two contact points on normal to one contact point)
		pContact->Overlap = fabs(-(pContact->Point2.X() - pContact->Point1.X())*sine + (pContact->Point2.Y() - pContact->Point1.Y())*cosine);
		pContact->NormalForceDirection.Set(-sine, cosine);
		pContact->TangentialForceDirection.Set(cosine, sine);
	}

	return LOGICAL_TRUE;
}

LOGICAL CParticle2D::IsOverlapped(StateEnum iState, CLine2D *pLine, SContact *pContact /*=NULL*/)
{
	double h = fabs(pLine->GetPointHeightAbove(m_State[iState].m_Position.m_fX, m_State[iState].m_Position.m_fY));
	if (h > GetMaxDistanceToCenter())
		return LOGICAL_FALSE;
	double MutualAngle = pLine->GetAngleRadians() - GetFi(iState);
	NormalizeAngleRadians(MutualAngle);
	const SParticleShapeVertex *pShapeVertex = m_pParticleType->GetShapePointIncline(MutualAngle);
	if (h > pShapeVertex->DistanceToParticleCenter*m_fSize)
		return LOGICAL_FALSE;
	
	double x = pShapeVertex->x*m_fSize;
	double y = pShapeVertex->y*m_fSize;
	double x1, y1, x2, y2;
	// Rotate according to m_Fi
	if (IsCircular())
	{ // rotation is not needed
		x1 = x;
		y1 = y;
	}
	else
	{
		x1 = +x*m_State[iState].m_Cosine - y*m_State[iState].m_Sine;
		y1 = +x*m_State[iState].m_Sine + y*m_State[iState].m_Cosine;
	}
	// Second point is mirrored
	x2 = -x1;
	y2 = -y1;
	// Translate according to particle position
	x1 += m_State[iState].m_Position.m_fX;
	y1 += m_State[iState].m_Position.m_fY;
	x2 += m_State[iState].m_Position.m_fX;
	y2 += m_State[iState].m_Position.m_fY;

	double XProj1, YProj1, H1;
	pLine->GetPointProjection(x1, y1, XProj1, YProj1, H1);
	double XProj2, YProj2, H2;
	pLine->GetPointProjection(x2, y2, XProj2, YProj2, H2);
	if (H1*H2 > 0)
		return LOGICAL_FALSE; // heights of equal sign, i.e. both points are on the same side of the line -> no overlap
	
	if (pContact)
	{
		if (fabs(H1) < fabs(H2))
		{
			pContact->Point1.Set(x1, y1);
			pContact->Point2.Set(XProj1, YProj1);
		}
		else
		{
			pContact->Point1.Set(x2, y2);
			pContact->Point2.Set(XProj2, YProj2);
		}
	}
	return LOGICAL_TRUE;
}

//inline
double CParticle2D::GetMaxDistanceToCenter()
{
	return m_fSize*m_pParticleType->m_MaxDistanceToParticleCenter;
}

//inline
double CParticle2D::GetMinDistanceToCenter()
{
	return m_fSize*m_pParticleType->m_MinDistanceToParticleCenter;
}

//inline
LOGICAL CParticle2D::IsCircular()
{
	return m_pParticleType->IsCircular();
}

void CParticle2D::CopyNextToCurrentState()
{
	m_State[CURRENT_STATE].Set(m_State+NEXT_STATE);
}

void CParticle2D::CopyCurrentToPrevState()
{
	m_State[PREV_STATE].Set(m_State+CURRENT_STATE);
}

void CParticle2D::VelocityVerletCoords(double dt)
{
	SParticleState *pCurrentState = m_State+CURRENT_STATE;
	SVector2D a = pCurrentState->m_Force/m_M;
	a.m_fY -= m_pDesign->GetG();
	m_State[NEXT_STATE].m_Position = pCurrentState->m_Position + pCurrentState->m_Velocity*dt + a*dt*dt/2;
}

void CParticle2D::VelocityVerletVelocities(double dt)
{
	SParticleState *pCurrentState = m_State+CURRENT_STATE;
	SParticleState *pNextState = m_State+NEXT_STATE;
	SVector2D a = (pCurrentState->m_Force + pNextState->m_Force)/m_M;
	a.m_fY -= m_pDesign->GetG();
	pNextState->m_Velocity = pCurrentState->m_Velocity + a/2*dt;
}

/////////////////////////////////////////////////////////////////////////////
// CAttachedBlock

CAttachedBlock::CAttachedBlock(long ViewIndex, CGranSim2DDesign *pDesign, CGranSim2DObject *pObject)
{
	m_ViewIndex = ViewIndex;
	m_pDesign = pDesign;
	m_pObject = pObject;
}

CAttachedBlock::~CAttachedBlock()
{
}

///////////////////////////////////////////////////////////////////////////
// CGranSim2DObject

//static
CGranSim2DDesign *CGranSim2DObject::m_pDesign = NULL;
//static
int CGranSim2DObject::m_nObjectCount = 0;

CGranSim2DObject::CGranSim2DObject()
{
	m_szFullName[0] = 0;
	char szTmpBuffer[128];
	sprintf(szTmpBuffer, "<%s_%d>", PSZ_NONAME, ++m_nObjectCount);
	m_GivenName = szTmpBuffer;
	m_uCheckTime = 0;
}

//virtual
CGranSim2DObject::~CGranSim2DObject()
{
}

char *CGranSim2DObject::GetFullName()
{
	sprintf(m_szFullName, "%s '%s'", GetTypeDesc(), GetGivenName());
	return m_szFullName;
}

void CGranSim2DObject::SetGivenName(const char *pszName)
{
	if (m_GivenName == pszName)
		return;
	m_GivenName = pszName;
	g_pGui->DesignChanged();
}


///////////////////////////////////////////////////////////////////////////
// CGeometry2DObject

CGeometry2DObject::CGeometry2DObject()
{
	m_bSelected = LOGICAL_FALSE;
}

//virtual
CGeometry2DObject::~CGeometry2DObject()
{
}

void CGeometry2DObject::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a/* = 255 */)
{
	m_RGBA.Init(r, g, b, a);
	g_pGui->DesignChanged();
//	m_pRenderer->Redraw();
}

void CGeometry2DObject::SetColor(SRGBA *pRGBA)
{
	m_RGBA.Init(pRGBA);
	g_pGui->DesignChanged();
}

void CGeometry2DObject::SetGeometry(const char *pszGeometry)
{
	CGeometry2D *pGeometry = GetGeometry();
	pGeometry->Unformat(pszGeometry);
	pGeometry->RecalcExtents();
//	m_pRenderer->RebuildDisplayList(pGeometry);
	g_pGui->DesignChanged();
//	m_pRenderer->Redraw();
}

//virtual
void CGeometry2DObject::SaveToFile(FILE *fd)
{
	CDesignTextString str;
	// Name
	str.Set(PSZ_NAME_KEYWORD, GetGivenName());
	m_pDesign->WriteString(fd, &str);
	
	// Color
	str.Set(PSZ_COLOR_KEYWORD, GetRGBA());
	m_pDesign->WriteString(fd, &str);
	
	//Geometry
	CTextString GeomStr;
	GetGeometry()->Format(GeomStr);
	str.Set(PSZ_GEOMETRY_KEYWORD, GeomStr.GetText());
	m_pDesign->WriteString(fd, &str);
}

///////////////////////////////////////////////////////////////////////////
// CReflectingObject

CReflectingObject::CReflectingObject()
{
	m_pMaterial = NULL;
	m_RGBA.Init(0xFF, 0x00, 0x00); // reflecting objects are red. TODO: load initial color from application defaults
}

//virtual
CReflectingObject::~CReflectingObject()
{
}

//virtual
void CReflectingObject::SaveToFile(FILE *fd)
{
	CGeometry2DObject::SaveToFile(fd);

	CDesignTextString str;
	// Material
	if (m_pMaterial)
	{
		str.Set(PSZ_MATERIAL_KEYWORD, m_pMaterial->GetGivenName());
		m_pDesign->WriteString(fd, &str);
	}
}

//virtual
void CReflectingObject::DeleteFromDesign()
{
	m_pDesign->DeleteReflectingObject(this);
}

///////////////////////////////////////////////////////////////////////////
// CMaterial

static SFormDesc MaterialFormDescs[] = {
	{ OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 5 },
	{ MATERIAL_CONSTANTS_INDEX, "Material constants", 2 }
};

CMaterial::CMaterial()
{
	m_Nu = 0.3;
	m_E = 1.0E+8;
	m_Ro = 2000;
}

//virtual
CMaterial::~CMaterial()
{
}

//virtual
char *CMaterial::GetKeyWord()
{
	return PSZ_MATERIAL_KEYWORD;
}

//virtual
int CMaterial::GetFormCount()
{
	return sizeof(MaterialFormDescs)/sizeof(MaterialFormDescs[0]);
}

//virtual
SFormDesc *CMaterial::GetFormDesc(int iFormNumber)
{
	return MaterialFormDescs+iFormNumber;
}

//virtual
void CMaterial::SaveToFile(FILE *fd)
{
	CDesignTextString str;
	// Name
	str.Set(PSZ_NAME_KEYWORD, GetGivenName());
	m_pDesign->WriteString(fd, &str);

	// Material constants
	str.Set(PSZ_POISSON_RATIO_KEYWORD, m_Nu);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_YOUNG_MODULE_KEYWORD, m_E);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_DENSITY_KEYWORD, m_Ro);
	m_pDesign->WriteString(fd, &str);
}

//virtual
void CMaterial::DeleteFromDesign()
{
	m_pDesign->DeleteMaterial(this);
}

///////////////////////////////////////////////////////////////////////////
// CMaterializedObject

CMaterializedObject::CMaterializedObject()
{
	m_pMaterial = NULL;
}

//virtual
CMaterializedObject::~CMaterializedObject()
{
}

void CMaterializedObject::SetMaterial(CMaterial *pMaterial)
{
	m_pMaterial = pMaterial;
	if (m_pMaterial)
		m_MaterialName = m_pMaterial->GetGivenName();
	else
		m_MaterialName.Empty();
}


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
		return 1./(m_SizeSigma*SQRT_2PI);
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
		return min(0, m_SizeMedian - 5*m_SizeSigma);
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
		return g_RandomGen.GenGauss(0, VERY_BIG_DOUBLE, m_SizeMedian, m_SizeSigma);
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
	CParticleType *pType = (CParticleType *)pContext;
	double shape = Shape(x, pContext);
	return shape*(x*x+shape*shape/3);
}

void CParticleType::RecalcShapeVertices()
{
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
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].Incline = PI;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].R = 0.5/m_ShapeAspectRatio;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].Xk = 0;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/2].Yk = 0;

	// OX axis (90, 270 CW)
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].x = 0.5;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].y = 0;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].DistanceToParticleCenter = 0.5;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].Incline = PI/2.;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].R = 0.5;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].Xk = 0;
	m_Vertices[N_PARTICLE_SHAPE_VERTICES/4].Yk = 0;

	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].x = -0.5;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].y = 0;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].DistanceToParticleCenter = 0.5;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].Incline = 3.*PI/2.;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].R = 0.5;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].Xk = 0;
	m_Vertices[3*N_PARTICLE_SHAPE_VERTICES/4].Yk = 0;

	for (int i = 1; i < N_PARTICLE_SHAPE_VERTICES/4; i++)
	{
		double x, y, Incline;
		double DesiredIncline = i*360./N_PARTICLE_SHAPE_VERTICES;
		double leftX = 0, leftIncline = 0;
		double rightX = 1, rightIncline = -90;
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
			Incline = RADIANS_2_DEGREES(atan(dYdX));
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
		m_Vertices[i].Incline = DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[i].R = R/2;
		m_Vertices[i].Xk = +Xk/2;
		m_Vertices[i].Yk = +Yk/2;
		
		// 1st quadrant
		int j = N_PARTICLE_SHAPE_VERTICES/2 - i;
		m_Vertices[j].x = +x/2;
		m_Vertices[j].y = -y/2;
		m_Vertices[j].DistanceToParticleCenter = DistanceToParticleCenter;
		m_Vertices[j].Incline = PI - DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[j].R = R/2;
		m_Vertices[j].Xk = +Xk/2;
		m_Vertices[j].Yk = -Yk/2;
		
		// 2nd quadrant
		j = N_PARTICLE_SHAPE_VERTICES/2 + i;
		m_Vertices[j].x = -x/2;
		m_Vertices[j].y = -y/2;
		m_Vertices[j].DistanceToParticleCenter = DistanceToParticleCenter;
		m_Vertices[j].Incline = PI + DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[j].R = R/2;
		m_Vertices[j].Xk = -Xk/2;
		m_Vertices[j].Yk = -Yk/2;
		
		// 3rd quadrant
		j = N_PARTICLE_SHAPE_VERTICES - i;
		m_Vertices[j].x = -x/2;
		m_Vertices[j].y = y/2;
		m_Vertices[j].DistanceToParticleCenter = DistanceToParticleCenter;
		m_Vertices[j].Incline = PI2 - DEGREES_2_RADIANS(DesiredIncline);
		m_Vertices[j].R = R/2;
		m_Vertices[j].Xk = -Xk/2;
		m_Vertices[j].Yk = +Yk/2;
	}

	// Recalc other fields for future fun
	for (i = 0; i < N_PARTICLE_SHAPE_VERTICES; i++)
	{
		SParticleShapeVertex *pShapeVertex = m_Vertices+i;
		pShapeVertex->RecalcAux();
	}

	m_Mass = IntegralSimpson(Shape, this, 0, 1, 1.E-6)*4;
	m_InertiaMomentum = IntegralSimpson(InertiaMomentum, this, 0, 1, 1.E-6)*4; // 
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
const SParticleShapeVertex *CParticleType::GetShapePointIncline(double Incline)
{
	const double step = PI2/N_PARTICLE_SHAPE_VERTICES;
	int index = int(Incline/step + 0.49999) % N_PARTICLE_SHAPE_VERTICES;
	return m_Vertices+index;
}

//virtual
char *CParticleType::GetKeyWord()
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

///////////////////////////////////////////////////////////////////////////
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
	CTextString GeomStr;
	GetGeometry()->Format(GeomStr);
	str.Set(PSZ_GEOMETRY_KEYWORD, GeomStr.GetText());
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

	LOGICAL bSuccess = LOGICAL_FALSE;
	g_ProbeParticle.m_pParticleType = m_pParticleType;
	double ParticleSize;
	for (int iAttempt = 0; iAttempt < 100; iAttempt++)
	{
		LocateProbeParticle();
		g_ProbeParticle.SetFi(CURRENT_STATE, g_RandomGen.GenDouble(0, PI));
		ParticleSize = g_ProbeParticle.m_fSize = m_pParticleType->GenSize();
		if (!m_pDesign->IsFreeSpaceForParticle(CURRENT_STATE, &g_ProbeParticle))
			continue;
		if (m_pDesign->DoesParticleTouchReflectingObject(CURRENT_STATE, &g_ProbeParticle))
			continue;
		bSuccess = LOGICAL_TRUE;
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
char *CParticlePointSource::GetKeyWord()
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
// CReflectingWall

static SFormDesc ReflectingWallFormDescs[] = {
	{ GEOMETRY_OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 3 }, 
	{ LINE_GEOMETRY_FORM_INDEX, PSZ_GEOMETRY, 1 }, 
	{ REFLECTING_OBJECT_MATERIAL_FORM_INDEX, PSZ_MATERIAL, 2 } 
};

CReflectingWall::CReflectingWall(CLine2D *pLine)
{
	m_Line.Init(pLine);
}

//virtual
CReflectingWall::~CReflectingWall()
{
}

//virtual
char *CReflectingWall::GetKeyWord()
{
	return PSZ_WALL_KEYWORD;
}

//virtual
int CReflectingWall::GetFormCount()
{
	return sizeof(ReflectingWallFormDescs)/sizeof(ReflectingWallFormDescs[0]);
}

//virtual
SFormDesc *CReflectingWall::GetFormDesc(int iFormNumber)
{
	return ReflectingWallFormDescs+iFormNumber;
}

//virtual
int CReflectingWall::DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts)
{
	if (pParticle->IsOverlapped(iState, GetLine(), pContacts))
		return 1;
	return 0;
}

///////////////////////////////////////////////////////////////////////////
// CReflectingRectangle

static SFormDesc ReflectingRectangleFormDescs[] = {
	{ GEOMETRY_OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 3 }, 
	{ RECTANGLE_GEOMETRY_FORM_INDEX, PSZ_GEOMETRY, 1 }, 
	{ REFLECTING_OBJECT_MATERIAL_FORM_INDEX, PSZ_MATERIAL, 2 } 
};

CReflectingRectangle::CReflectingRectangle(CRectangle2D *pRectangle)
{
	m_Rectangle.Init(pRectangle);
}

//virtual
CReflectingRectangle::~CReflectingRectangle()
{
}

//virtual
char *CReflectingRectangle::GetKeyWord()
{
	return PSZ_RECTANGLE_KEYWORD;
}

//virtual
int CReflectingRectangle::GetFormCount()
{
	return sizeof(ReflectingRectangleFormDescs)/sizeof(ReflectingRectangleFormDescs[0]);
}

//virtual
SFormDesc *CReflectingRectangle::GetFormDesc(int iFormNumber)
{
	return ReflectingRectangleFormDescs+iFormNumber;
}

//virtual
int CReflectingRectangle::DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts)
{
	if (pContacts)
	{
		int nContacts = 0;
		if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideN(), pContacts))
		{
			++nContacts;
			++pContacts;
		}
		
		if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideE(), pContacts))
		{
			++nContacts;
			++pContacts;
		}
		
		if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideS(), pContacts))
		{
			++nContacts;
			++pContacts;
		}
		
		if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideW(), pContacts))
		{
			++nContacts;
			++pContacts;
		}
		return nContacts;
	}

	if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideN(), NULL))
		return 1;
		
	if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideE(), NULL))
		return 1;
		
	if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideS(), NULL))
		return 1;
		
	if (pParticle->IsOverlapped(iState, m_Rectangle.GetSideW(), NULL))
		return 1;

	return 0;
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
char *CParticleLinearSource::GetKeyWord()
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


///////////////////////////////////////////////////////////////////////////
// CSimCell_2D

CSimCell_2D::CSimCell_2D(CGranSim2DDesign *pDesign, double left, double right, double bottom, double top)
{
	m_pDesign = pDesign;
	m_Box.Init(left, right, bottom, top);
	m_pBiggestParticle = NULL;
	m_fBiggestParticleSize = 0;
}

//virtual
CSimCell_2D::~CSimCell_2D()
{
	for (int i = 0; i < GetParticleCount(); i++)
	{
		CParticle2D *pParticle = GetParticle(i);
		if (pParticle)
			delete pParticle;
	}
}

void CSimCell_2D::NewParticle(CParticle2D *pParticle)
{
	pParticle->m_iIndexInCell = GetParticleCount();
	pParticle->SetCell(this);
	m_Particles.AddTail(pParticle);
	if (!m_pBiggestParticle || m_pBiggestParticle->GetMaxDistanceToCenter() < pParticle->GetMaxDistanceToCenter())
	{
		m_pBiggestParticle = pParticle;
		m_fBiggestParticleSize = m_pBiggestParticle->GetMaxDistanceToCenter();
	}
}

void CSimCell_2D::RemoveParticle(CParticle2D *pParticle)
{
	m_Particles.CopyTailTo(pParticle->m_iIndexInCell);
	m_Particles[pParticle->m_iIndexInCell]->m_iIndexInCell = pParticle->m_iIndexInCell;
	m_Particles.CutTail();
	pParticle->SetCell(NULL);
	if (pParticle == m_pBiggestParticle)
	{
		m_pBiggestParticle = NULL;
		m_fBiggestParticleSize = 0;
		for (int i = 0; i < GetParticleCount(); i++)
		{
			CParticle2D *pParticle = GetParticle(i);
			if (pParticle->m_fSize > m_fBiggestParticleSize)
			{
				m_pBiggestParticle = pParticle;
				m_fBiggestParticleSize = pParticle->m_fSize;
			}
		}
		if (m_pBiggestParticle)
			m_fBiggestParticleSize = m_pBiggestParticle->GetMaxDistanceToCenter();
	}
}

void CSimCell_2D::RecalcForcesForAllParticles(CSimCell_2D *pCell, StateEnum state)
{
	if (!pCell->GetParticleCount())
		return;
	double fMaxDistance = m_fBiggestParticleSize + pCell->m_fBiggestParticleSize;
	SContact Contact; 
	SContact *pContact = &Contact;
	for (int i = 0; i < GetParticleCount(); i++)
	{
		CParticle2D *pParticle1 = GetParticle(i);
		for (int j = 0; j < pCell->GetParticleCount(); j++)
		{
			CParticle2D *pParticle2 = pCell->GetParticle(j);
			if (pParticle1 >= pParticle2)
				continue; // to avoid self-interaction and calculation of the same forces twice
			if (pParticle1->IsOverlapped(state, pParticle2, pContact))
			{
				// Force from Particle2 to Particle1
				// normal forces
				double F = m_pDesign->CalcNormalForce(pParticle1, pParticle2, pContact);
				SVector2D force = F*pContact->NormalForceDirection;
				pParticle1->m_State[state].m_Force += force;
				pParticle2->m_State[state].m_Force -= force;
				// tangential forces
				F = m_pDesign->CalcTangentialForce(pParticle1, pParticle2, pContact);
				force = F*pContact->TangentialForceDirection;
				pParticle1->m_State[state].m_Force += force;
				pParticle2->m_State[state].m_Force -= force;
				// TODO: add momentums calculations
			}
		} // end of Particle2 loop
	}
}

void CSimCell_2D::ZeroForcesForAllParticles(StateEnum state)
{
	for (int i = 0; i < GetParticleCount(); i++)
	{
		CParticle2D *pParticle = GetParticle(i);
		pParticle->m_State[state].m_Force.Zero();
	}
}

void CSimCell_2D::CommitNextState(CParticles2D &ParticlesOutOfCells)
{
	for (int i = 0; i < GetParticleCount(); i++)
	{
		CParticle2D *pParticle = GetParticle(i);
		pParticle->CopyNextToCurrentState();
		if (!IsInside(pParticle->m_State[CURRENT_STATE].m_Position))
			ParticlesOutOfCells.AddTail(pParticle);
	}
}


///////////////////////////////////////////////////////////////////////////
// CParticleSizeRenderer

CGranSim2DDesign::CGranSim2DDesign()
{
	CGranSim2DObject::m_pDesign = this;
	m_pIOFile = NULL;
	m_CurrentLoadingSection = IO_NONE_SECTION;
	m_CurrentRegime = STANDBY_REGIME;
	m_CurrentCreationMode = NONE_CREATION;
	m_G = 9.8E+3; //mm/s**2
	m_nTabs = 0;
	m_fSize = 100;//mm
	m_IntegrationMethod = VELOCITY_VERLET;
	m_Time = 0;
	m_TimeStep = 0;
	m_fSimulateUntilTime = 0;
}

//virtual
CGranSim2DDesign::~CGranSim2DDesign()
{
	FreeSimCells();
}

void CGranSim2DDesign::UpdateBoxAndSize()
{
	m_Box.Init();
	for (CListOf<CParticleSource>::iterator SourceIterator = m_ParticleSources.begin(); SourceIterator != m_ParticleSources.end(); SourceIterator++)
	{
		CParticleSource *pSource = *SourceIterator;
		UpdateBoxAndSize(pSource->GetGeometry());
	}
	for (CListOf<CReflectingObject>::iterator ObjectIterator = m_ReflectingObjects.begin(); ObjectIterator != m_ReflectingObjects.end(); ObjectIterator++)
	{
		CReflectingObject *pObject = *ObjectIterator;
		UpdateBoxAndSize(pObject->GetGeometry());
	}

	m_Box.Expand(1.e-10);
}

void CGranSim2DDesign::UpdateBoxAndSize(CGeometry2D *pGeometry)
{
	pGeometry->UpdateExtents(m_Box);
	double MaxCoord = 0;
	if (MaxCoord < fabs(m_Box.m_fLeft))
		MaxCoord = fabs(m_Box.m_fLeft);
	if (MaxCoord < fabs(m_Box.m_fRight))
		MaxCoord = fabs(m_Box.m_fRight);
	if (MaxCoord < fabs(m_Box.m_fTop))
		MaxCoord = fabs(m_Box.m_fTop);
	if (MaxCoord < fabs(m_Box.m_fBottom))
		MaxCoord = fabs(m_Box.m_fBottom);
	m_fSize = 1.1*MaxCoord*2; //10 percents bigger
}

CGeometry2DObject *CGranSim2DDesign::OnPointCreatedExternally(CPoint2D *pPoint, void *pWhere)
{
	CGeometry2DObject *pGeometryObject = 0;
	switch (m_CurrentCreationMode)
	{
		case PARTICLE_POINT_SOURCE_CREATION:
		{
			pGeometryObject = m_CurrentObject.pParticleSource = CreateNewPointSource(pPoint, pWhere, LOGICAL_TRUE);
			break;
		}
	}
	g_pGui->DesignChanged();
	return pGeometryObject;
}

CGeometry2DObject *CGranSim2DDesign::OnLineCreatedExternally(CLine2D *pLine, void *pWhere)
{
	CGeometry2DObject *pGeometryObject = 0;
	switch (m_CurrentCreationMode)
	{
		case REFLECTING_WALL_CREATION:
		{
			pGeometryObject = m_CurrentObject.pWall = CreateNewWall(pLine, pWhere, LOGICAL_TRUE);
			break;
		}
		case PARTICLE_LINEAR_SOURCE_CREATION:
		{
			pGeometryObject = m_CurrentObject.pParticleSource = CreateNewLinearSource(pLine, pWhere, LOGICAL_TRUE);
			break;
		}
	}
	g_pGui->DesignChanged();
	return pGeometryObject;
}

CGeometry2DObject *CGranSim2DDesign::OnRectangleCreatedExternally(CRectangle2D *pRectangle, void *pWhere)
{
	CGeometry2DObject *pGeometryObject = 0;
	switch (m_CurrentCreationMode)
	{
		case REFLECTING_RECTANGLE_CREATION:
		{
			pGeometryObject = m_CurrentObject.pRectangle = CreateNewRectangle(pRectangle, pWhere, LOGICAL_TRUE);
			break;
		}
	}
	g_pGui->DesignChanged();
	return pGeometryObject;
}

void CGranSim2DDesign::FreeSimCells()
{
	for (int i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (int j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i, j);
			if (pCell)
				delete pCell;
			m_SimCells(i, j) = NULL;
		}
	}
	m_SimCells.SetSizes(0, 0);
}

void CGranSim2DDesign::RebuildSimCells()
{
	int i, j;
	FreeSimCells();

	UpdateBoxAndSize();

	// Estimate ave size of particles
	double fSum = 0;
	double fIntensSum = 0;
	double fAveParticleSize = -1;
	for (CListOf<CParticleSource>::iterator SourceIt = m_ParticleSources.begin(); SourceIt != m_ParticleSources.end(); SourceIt++)
	{
		CParticleSource *pSource = *SourceIt;
		if (pSource->GetIntensity() > 0)
		{
			fSum += pSource->GetIntensity()*pSource->GetAveParticleSize();
			fIntensSum += pSource->GetIntensity();
			fAveParticleSize = fSum/fIntensSum;
		}
	}
	// TODO: check particles
	
	if (fAveParticleSize < 0)
	{
		// TODO: report error (cannot get sim cell sizes)
		return;
	}

	fAveParticleSize *= 3;
	m_nSimCellCols = (int)(m_Box.GetSizeX()/fAveParticleSize)+1;
	if (m_nSimCellCols > 100)
		m_nSimCellCols = 100;
	m_fSimCellWidth = m_Box.GetSizeX()/m_nSimCellCols;

	m_nSimCellRows = (int)(m_Box.GetSizeY()/fAveParticleSize)+1;
	if (m_nSimCellRows > 100)
		m_nSimCellRows = 100;
	m_fSimCellHeight = m_Box.GetSizeY()/m_nSimCellRows;
	
	LOGICAL bOK = m_SimCells.SetSizes(m_nSimCellCols, m_nSimCellRows);
	if (!bOK)
	{
		//TODO: return out-of-memory error
		return;
	}
	for (i = 0; i < m_SimCells.GetCols(); i++)
	{
		double left = m_Box.m_fLeft + i*m_fSimCellWidth;
		double right = m_Box.m_fLeft + (i+1)*m_fSimCellWidth;
		for (j = 0; j < m_SimCells.GetRows(); j++)
		{
			double top = m_Box.m_fBottom + (j+1)*m_fSimCellHeight;
			double bottom = m_Box.m_fBottom + j*m_fSimCellHeight;
			CSimCell_2D *pCell = new CSimCell_2D(this, left, right, bottom, top);
			if (!pCell)
			{
				//TODO: return out-of-memory error
				return;
			}
			m_SimCells(i, j) = pCell;
		}
	}
}

CSimCell_2D *CGranSim2DDesign::PointToCell(double x, double y)
{
	if (x < m_Box.m_fLeft || x > m_Box.m_fRight)
		return NULL;
	if (y < m_Box.m_fBottom || y > m_Box.m_fTop)
		return NULL;
	int iCol = (int)((x-m_Box.m_fLeft)/m_fSimCellWidth);
	if (iCol < -1)
		iCol = iCol;//debug;
	if (iCol == -1)
		iCol = 0;
	else if (iCol == m_SimCells.GetCols())
		iCol = m_SimCells.GetCols()-1;
	int iRow = (int)((y-m_Box.m_fBottom)/m_fSimCellHeight);
	if (iRow < -1)
		iRow = iRow;//debug;
	if (iRow == -1)
		iRow = 0;
	else if (iRow == m_SimCells.GetRows())
		iRow = m_SimCells.GetRows()-1;
	return m_SimCells(iCol, iRow);
}

LOGICAL CGranSim2DDesign::StartSimulation()
{
	if (!CheckForSimulation())
		return LOGICAL_FALSE;

	RebuildSimCells();
	
	for (CListOf<CParticleSource>::iterator SourceIt = m_ParticleSources.begin(); SourceIt != m_ParticleSources.end(); SourceIt++)
	{
		CParticleSource *pSource = *SourceIt;
		pSource->Simulation_Started();
	}
	m_CurrentRegime = SIMULATION_REGIME;
	m_Time = 0;
	m_TimeStep = 0.0001;
	return LOGICAL_TRUE;
}

void CGranSim2DDesign::StopSimulation()
{
	m_CurrentRegime = STANDBY_REGIME;
}

void CGranSim2DDesign::SetIntegrationMethod(IntergationMethodEnum method)
{
	if (m_IntegrationMethod == method)
		return;
	m_IntegrationMethod = method;
	g_pGui->DesignChanged();
}

void CGranSim2DDesign::SetSimulateUntilTime(double fSimulateUntilTime)
{
	if (fabs(m_fSimulateUntilTime - fSimulateUntilTime) < 1.e-6)
		return;
	m_fSimulateUntilTime = fSimulateUntilTime;
	g_pGui->DesignChanged();
}

LOGICAL CGranSim2DDesign::SimulateOneTimeStep()
{
	if ((m_Time+1.e-6) >= GetSimulateUntilTime())
		return LOGICAL_FALSE;
	OneTimeStep();

	return LOGICAL_TRUE;
}

double CGranSim2DDesign::OneTimeStep()
{
	int i, j;
	// 1. Clip time step to avoid too big overlap
	double dt = m_TimeStep;
	double MaxDT = GetSimulateUntilTime() - m_Time;
	if (m_Time+dt >= GetSimulateUntilTime())
		dt = GetSimulateUntilTime()-m_Time;
	
	for (CListOf<CParticleSource>::iterator SourceIterator = m_ParticleSources.begin(); SourceIterator != m_ParticleSources.end(); SourceIterator++)
	{
		CParticleSource *pSource = *SourceIterator;
		pSource->Simulation_ClipTimeStep(m_Time, dt);
	}

	double MaxVelocity = 0;
	CParticle2D *pFastestParticle = NULL;
	for (i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i,j);
			for (int k = 0; k < pCell->GetParticleCount(); k++)
			{
				CParticle2D *pParticle = pCell->GetParticle(k);
				double V = pParticle->GetVelocitySq();
				if (V > MaxVelocity)
				{
					MaxVelocity = V;
					pFastestParticle = pParticle;
				}
			}
		}
	}
	if (pFastestParticle)
	{
		double dTmax = pFastestParticle->GetMinDistanceToCenter()/(2*sqrt(MaxVelocity));
		if (m_TimeStep < dTmax)
			m_TimeStep *= 1.2;
		if (m_TimeStep > dTmax)
			m_TimeStep = dTmax;		
	}
	
	// Simulation itself

	switch (m_IntegrationMethod)
	{
	case VELOCITY_VERLET:
		VelocityVerlet(dt);
		break;
	case PREDICTOR_CORRECTOR:
		PredictorCorrector(dt);
		break;
	default:
		break;//debug
	}
	
	m_Time += dt;

	// Copy next state to current state and found particles crossing their sim cell boundaries
	for (i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i,j);
			pCell->CommitNextState(m_ParticlesOutOfCells);
		}
	}
	// Move particles between cells if necessary
	for (i = 0; i < m_ParticlesOutOfCells.GetSize(); i++)
	{
		CParticle2D *pParticle = m_ParticlesOutOfCells[i];
		CSimCell_2D *pOldCell = pParticle->GetCell();
		CSimCell_2D *pNewCell = PointToCell(pParticle->m_State[CURRENT_STATE].m_Position);
		if (pOldCell == pNewCell)
			pOldCell=pOldCell;//debug
		pNewCell = PointToCell(pParticle->m_State[CURRENT_STATE].m_Position);
		if (pOldCell->IsInside(pParticle->m_State[CURRENT_STATE].m_Position))
			pOldCell=pOldCell;//debug
		pOldCell->RemoveParticle(pParticle);
		if (pNewCell)
			pNewCell->NewParticle(pParticle);
		else
			delete pParticle;
	}
	m_ParticlesOutOfCells.RemoveAll();

	for (SourceIterator = m_ParticleSources.begin(); SourceIterator != m_ParticleSources.end(); SourceIterator++)
	{
		CParticleSource *pSource = *SourceIterator;
		CParticle2D *pParticle = pSource->Simulation_NewParticle(m_Time);
		if (pParticle)
		{
			//printf("New particle of type '%s' generated at (%f, %f)\n", pParticle->m_pParticleType->GetGivenName(), pParticle->m_State[CURRENT_STATE].m_Position.m_fX, pParticle->m_State[CURRENT_STATE].m_Position.m_fY);
			CSimCell_2D *pCell = PointToCell(pParticle->m_State[CURRENT_STATE].m_Position);
			pCell->NewParticle(pParticle);
		}
	}
	return dt;
}

void CGranSim2DDesign::VelocityVerlet(double &dt)
{
	int nParticles = 0;//debug
	int i, j;
	for (i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i,j);
			for (int k = 0; k < pCell->GetParticleCount(); k++)
			{
				CParticle2D *pParticle = pCell->GetParticle(k);
				pParticle->VelocityVerletCoords(dt);
				++nParticles;
			}
		}
	}
	DWORD s = GetTickCount();
	RecalcForcesForAllParticles(NEXT_STATE);
	DWORD e = GetTickCount();
	//printf("N = %d, t = %d msec\n", nParticles, e-s);
	for (i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i,j);
			for (int k = 0; k < pCell->GetParticleCount(); k++)
			{
				CParticle2D *pParticle = pCell->GetParticle(k);
				pParticle->VelocityVerletVelocities(dt);
			}
		}
	}
}

void CGranSim2DDesign::PredictorCorrector(double &dt)
{
}

void CGranSim2DDesign::RecalcForcesForAllParticles(StateEnum state)
{
	int i1, j1, i2, j2;
	CListOf<CReflectingObject>::iterator it;
	for (i1 = 0; i1 < m_SimCells.GetCols(); i1++)
	{
		for (j1 = 0; j1 < m_SimCells.GetRows(); j1++)
		{
			CSimCell_2D *pCell = m_SimCells(i1, j1);
			pCell->ZeroForcesForAllParticles(state);
		}
	}

	SContact Contact; 
	SContact *pContact = &Contact;
	for (i1 = 0; i1 < m_SimCells.GetCols(); i1++)
	{
		for (j1 = 0; j1 < m_SimCells.GetRows(); j1++)
		{
			CSimCell_2D *pCell1 = m_SimCells(i1,j1);
			if (!pCell1->GetParticleCount())
				continue;
			for (i2 = 0; i2 < m_SimCells.GetCols(); i2++)
			{
				for (j2 = 0; j2 < m_SimCells.GetRows(); j2++)
				{
					CSimCell_2D *pCell2 = m_SimCells(i2,j2);
					if (!pCell2->GetParticleCount())
						continue;
					pCell1->RecalcForcesForAllParticles(pCell2, state);
				}
			}
		}
	}
		// Objects-to-particle interaction
		for (it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
		{
			CReflectingObject *pReflectingObject = *it;
/*			if (pParticle1->IsOverlapped(state, pReflectingObject, pContact))
			{
				// Force from Particle2 to Particle1
				// normal forces
				double F = CalcNormalForce(pParticle1, pReflectingObject, pContact);
				SVector2D force = F*pContact->NormalForceDirection;
				pParticle1->m_State[state].m_Force += force;
				// tangential forces
				F = CalcTangentialForce(pParticle1, pParticle2, pContact);
				force = F*pContact->TangentialForceDirection;
				pParticle1->m_State[state].m_Force += force;
				// TODO: add momentums calculations
			}*/
		}
}

// returns abs value of normal force  (particle2 affects particle1, the normal direction is calculated in pContact)
double CGranSim2DDesign::CalcNormalForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact)
{
	return 1;
}

// returns abs value of tangential force  (particle2 affects particle1, the normal direction is calculated in pContact)
double CGranSim2DDesign::CalcTangentialForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact)
{
	return 0;
}

// returns abs value of normal force  (reflecting object affects particle1, the normal direction is calculated in pContact)
double CGranSim2DDesign::CalcNormalForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact)
{
	return 1;
}

// returns abs value of tangential force  (reflecting object affects particle1, the normal direction is calculated in pContact)
double CGranSim2DDesign::CalcTangentialForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact)
{
	return 0;
}

LOGICAL CGranSim2DDesign::LoadFromFile(char *pszFileName)
{
	FILE *fd = fopen(pszFileName, "r");
	if (!fd)
		return LOGICAL_FALSE;

	LOGICAL bSuccess = LoadFromFile(fd);
	fclose(fd);
	return bSuccess;
}

LOGICAL CGranSim2DDesign::LoadFromFile(FILE *fd)
{
	ClearAll();	
	if (!fd)
		return LOGICAL_FALSE;

	m_CurrentRegime = LOADING_REGIME;
	m_pIOFile = fd;
	m_CurrentObject.pObject = NULL;
	CTextString token;
//	TRACE("---------------------------------------------\n");
	IO_ERROR_CODE_ENUM EC;
	while (1)
	{
		EC = GetNextToken(token);
		if (EC == IO_ERROR_CODE_EOF)
			break;
		if (EC != IO_ERROR_CODE_OK)
			return LOGICAL_FALSE;
		if (strlen(token.GetText()) < 512)
		{
//			TRACE("%s\n", token.GetText());
		//	Sleep(100);
		}
		ProcessToken(token);
	}
//	TRACE("---------------------------------------------\n");

	// Set materials for all particle types
	for (CListOf<CParticleType>::iterator pPartTypeIterator = m_ParticleTypes.begin(); pPartTypeIterator != m_ParticleTypes.end(); pPartTypeIterator++)
	{
		CParticleType *pType = *pPartTypeIterator;
		CMaterial *pMaterial = FindMaterial(pType->GetMaterialName());
		pType->SetMaterial(pMaterial);
	}
	// Set materials for all reflecting objects
	for (CListOf<CReflectingObject>::iterator pReflectingObjectIterator = m_ReflectingObjects.begin(); pReflectingObjectIterator != m_ReflectingObjects.end(); pReflectingObjectIterator++)
	{
		CReflectingObject *pReflectingObject = *pReflectingObjectIterator;
		CMaterial *pMaterial = FindMaterial(pReflectingObject->GetMaterialName());
		pReflectingObject->SetMaterial(pMaterial);
	}
	// Set particle types for all particle sources types
	for (CListOf<CParticleSource>::iterator pPartSourceIterator = m_ParticleSources.begin(); pPartSourceIterator != m_ParticleSources.end(); pPartSourceIterator++)
	{
		CParticleSource *pSource = *pPartSourceIterator;
		CParticleType *pType = FindParticleType(pSource->GetParticleTypeName());
		pSource->SetParticleType(pType);
	}

	m_CurrentRegime = STANDBY_REGIME;
	
	return LOGICAL_TRUE;
}

double StringToDouble(const char *pszStr)
{
	double fValue = 0;
	sscanf(pszStr, "%lf", &fValue);
	return fValue;
}

double StringToDouble(CTextString &str)
{
	return StringToDouble(str.GetText());
}

void CGranSim2DDesign::ProcessToken(CTextString &token)
{
	if (token.IsEqualNoCase(PSZ_END_KEYWORD))
	{
		if (m_CurrentObject.pObject)
		{
			switch (m_CurrentLoadingSection)
			{
			case IO_MATERIAL_SECTION:
				g_pGui->MaterialCreated(m_CurrentObject.pMaterial, this); break;
			case IO_PARTICLE_TYPE_SECTION:
				g_pGui->ParticleTypeCreated(m_CurrentObject.pParticleType, this); break;
			case IO_PARTICLE_SOURCE_SECTION:
				if (m_CurrentCreationMode == PARTICLE_POINT_SOURCE_CREATION)
					g_pGui->ParticlePointSourceCreated(m_CurrentObject.pParticlePointSource, this);
				if (m_CurrentCreationMode == PARTICLE_LINEAR_SOURCE_CREATION)
					g_pGui->ParticleLinearSourceCreated(m_CurrentObject.pParticleLinearSource, this);
				break;
			case IO_WALL_SECTION:
				g_pGui->ReflectingWallCreated(m_CurrentObject.pWall, this); break;
			case IO_RECTANGLE_SECTION:
				g_pGui->ReflectingRectangleCreated(m_CurrentObject.pRectangle, this); break;
			}
		}
		m_CurrentObject.pObject = NULL;
		m_CurrentLoadingSection = IO_NONE_SECTION;
		m_CurrentCreationMode = NONE_CREATION;
	}
	else if (token.IsEqualNoCase(PSZ_DESIGN_KEYWORD))
	{
		m_CurrentLoadingSection = IO_DESIGN_SECTION;
	}
	else if (token.IsEqualNoCase(PSZ_SIMULATION_KEYWORD))
	{
		m_CurrentLoadingSection = IO_SIMULATION_SECTION;
	}
	else if (token.IsEqualNoCase(PSZ_TIME_INTEGRATION_METHOD_KEYWORD))
	{
		if (m_CurrentLoadingSection == IO_SIMULATION_SECTION)
		{
			GetNextToken(m_NextToken);
			if (m_NextToken.IsEqualNoCase(PSZ_IMPLICIT_EULER))
				m_IntegrationMethod = IMPLICIT_EULER;
			else if (m_NextToken.IsEqualNoCase(PSZ_VERLET))
				m_IntegrationMethod = VELOCITY_VERLET;
			else if (m_NextToken.IsEqualNoCase(PSZ_PREDICTOR_CORRECTOR))
				m_IntegrationMethod = PREDICTOR_CORRECTOR;
			else if (m_NextToken.IsEqualNoCase(PSZ_TRAPEZOID))
				m_IntegrationMethod = TRAPEZOID;
			else if (m_NextToken.IsEqualNoCase(PSZ_RUNGE_KUTTA_2))
				m_IntegrationMethod = RUNGE_KUTTA_2;
			else if (m_NextToken.IsEqualNoCase(PSZ_RUNGE_KUTTA_4))
				m_IntegrationMethod = RUNGE_KUTTA_4;
		}
	}
	else if (token.IsEqualNoCase(PSZ_SIMULATE_UNTIL_KEYWORD))
	{
		if (m_CurrentLoadingSection == IO_SIMULATION_SECTION)
		{
			GetNextToken(m_NextToken);
			SetSimulateUntilTime(StringToDouble(m_NextToken));
		}
	}
	else if (token.IsEqualNoCase(PSZ_MATERIAL_KEYWORD))
	{
		if (m_CurrentLoadingSection == IO_NONE_SECTION)
		{
			m_CurrentLoadingSection = IO_MATERIAL_SECTION;
			m_CurrentObject.pMaterial = CreateNewMaterial(LOGICAL_FALSE);
		}
		else if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
		{
			GetNextToken(m_CurrentObject.pParticleType->GetMaterialName());
		}
		else if (m_CurrentLoadingSection == IO_WALL_SECTION ||
				m_CurrentLoadingSection == IO_RECTANGLE_SECTION)
		{
			GetNextToken(m_CurrentObject.pReflectingObject->GetMaterialName());
		}
	}
	else if (token.IsEqualNoCase(PSZ_PARTICLE_TYPE_KEYWORD))
	{
		if (m_CurrentLoadingSection == IO_NONE_SECTION)
		{
			m_CurrentLoadingSection = IO_PARTICLE_TYPE_SECTION;
			m_CurrentObject.pParticleType = CreateNewParticleType(LOGICAL_FALSE);
		}
		else if (m_CurrentLoadingSection == IO_PARTICLE_SOURCE_SECTION)
		{
			GetNextToken(m_CurrentObject.pParticleSource->GetParticleTypeName());
		}
	}
	else if (token.IsEqualNoCase(PSZ_LINEAR_PARTICLE_SOURCE_KEYWORD))
	{
		m_CurrentLoadingSection = IO_PARTICLE_SOURCE_SECTION;
		m_CurrentCreationMode = PARTICLE_LINEAR_SOURCE_CREATION;
		m_CurrentObject.pParticleSource = CreateNewLinearSource(NULL, this, LOGICAL_FALSE);
	}
	else if (token.IsEqualNoCase(PSZ_POINT_PARTICLE_SOURCE_KEYWORD))
	{
		m_CurrentLoadingSection = IO_PARTICLE_SOURCE_SECTION;
		m_CurrentCreationMode = PARTICLE_POINT_SOURCE_CREATION;
		m_CurrentObject.pParticleSource = CreateNewPointSource(NULL, this, LOGICAL_FALSE);
	}
	else if (token.IsEqualNoCase(PSZ_WALL_KEYWORD))
	{
		m_CurrentLoadingSection = IO_WALL_SECTION;
		m_CurrentCreationMode = REFLECTING_WALL_CREATION;
		m_CurrentObject.pWall = CreateNewWall(NULL, this, LOGICAL_FALSE);
	}
	else if (token.IsEqualNoCase(PSZ_RECTANGLE_KEYWORD))
	{
		m_CurrentLoadingSection = IO_RECTANGLE_SECTION;
		m_CurrentCreationMode = REFLECTING_RECTANGLE_CREATION;
		m_CurrentObject.pRectangle = CreateNewRectangle(NULL, this, LOGICAL_FALSE);
	}
	else if (token.IsEqualNoCase(PSZ_POISSON_RATIO_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_MATERIAL_SECTION)
			m_CurrentObject.pMaterial->m_Nu = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_YOUNG_MODULE_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_MATERIAL_SECTION)
			m_CurrentObject.pMaterial->m_E = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_DENSITY_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_MATERIAL_SECTION)
			m_CurrentObject.pMaterial->m_Ro = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_NAME_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection != IO_NONE_SECTION)
			m_CurrentObject.pObject->SetGivenName(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_COLOR_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection != IO_NONE_SECTION)
		{
			unsigned char R = 0, G = 0, B = 0, A = 0;
			int n = sscanf(m_NextToken.GetText(), PSZ_RGBA_FORMAT_STRING, &R, &G, &B, &A);
			m_CurrentObject.pGeometryObject->SetColor(R, G, B, A);
		}
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_SIZE_TYPE_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->SetSizeType(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_FIXED_SIZE_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->m_SizeFixed = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_FROM_SIZE_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->m_SizeFrom = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_TO_SIZE_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->m_SizeTo = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_MEDIAN_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->m_SizeMedian = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_SIGMA_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->m_SizeSigma = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_SQUARENESS_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->SetShapeSquareness(StringToDouble(m_NextToken));
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_ASPECT_RATIO_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_TYPE_SECTION)
			m_CurrentObject.pParticleType->SetShapeAspectRatio(StringToDouble(m_NextToken));
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_FLOW_TYPE_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_SOURCE_SECTION)
			m_CurrentObject.pParticleSource->SetFlowType(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_FLOW_INTENSITY_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_SOURCE_SECTION)
			m_CurrentObject.pParticleSource->m_fIntensity = StringToDouble(m_NextToken);
		else
			ReportUnexpectedKeyword(token);
	}
	else if (token.IsEqualNoCase(PSZ_GEOMETRY_KEYWORD))
	{
		GetNextToken(m_NextToken);
		if (m_CurrentLoadingSection == IO_PARTICLE_SOURCE_SECTION ||
			m_CurrentLoadingSection == IO_WALL_SECTION ||
			m_CurrentLoadingSection == IO_RECTANGLE_SECTION)
		{
			m_CurrentObject.pGeometryObject->SetGeometry(m_NextToken);
			UpdateBoxAndSize(m_CurrentObject.pGeometryObject->GetGeometry());
		}
		else
			ReportUnexpectedKeyword(token);
	}
}

void CGranSim2DDesign::ReportUnexpectedKeyword(const char *pszKeyWord)
{
}

void CGranSim2DDesign::ClearAll()
{
	m_Materials.DeleteAll();
	m_ParticleTypes.DeleteAll();
	m_ParticleSources.DeleteAll();
	m_ReflectingObjects.DeleteAll();
	m_ParticleTypes.DeleteAll();
	FreeSimCells();
	CGranSim2DObject::m_nObjectCount = 0; // reset object counter
	m_Box.Init();
	g_pGui->DesignCleaned();
}

LOGICAL CGranSim2DDesign::CheckForSimulation()
{
	CTextString CheckStatus;
	CheckStatus.Empty();
	int nErrors = 0;
	for (CListOf<CParticleSource>::iterator pSourceIt = m_ParticleSources.begin(); pSourceIt != m_ParticleSources.end(); pSourceIt++)
	{
		CParticleSource *pSource = *pSourceIt;
		if (pSource->GetParticleType())
		{
			if (!pSource->GetParticleType()->GetMaterial())
			{
				CheckStatus += "Material is not chosen for particle type'";
				CheckStatus += pSource->GetParticleType()->GetGivenName();
				CheckStatus += "'\n";
				++nErrors;
			}
		}
		else
		{
			CheckStatus += "Particle type is not chosen for particle source '";
			CheckStatus += pSource->GetGivenName();
			CheckStatus += "'\n";
			++nErrors;
		}
		if (nErrors >= 10)
			break;
	}
	if (nErrors)
	{
		if (nErrors >= 10)
			CheckStatus += "\n\n\t...and probably other errors.";
		CheckStatus += "\n\nCannot simulate. Please fix that and try again.";
		g_pGui->CannotSimulate(CheckStatus.GetText());
	}

	return (nErrors == 0); // everything is OK
}

CMaterial *CGranSim2DDesign::FindMaterial(CTextString &MaterialName)
{
	for (CListOf<CMaterial>::iterator it = m_Materials.begin(); it != m_Materials.end(); it++)
	{
		CMaterial *pMaterial = *it;
		if (MaterialName == pMaterial->GetGivenName())
			return pMaterial;
	}
	return NULL;
}

CParticleType *CGranSim2DDesign::FindParticleType(CTextString &TypeName)
{
	for (CListOf<CParticleType>::iterator it = m_ParticleTypes.begin(); it != m_ParticleTypes.end(); it++)
	{
		CParticleType *pType = *it;
		if (TypeName == pType->GetGivenName())
			return pType;
	}
	return NULL;
}

LOGICAL CGranSim2DDesign::SaveToFile(FILE *fd)
{
	if (!fd)
		return LOGICAL_FALSE;
	
	m_CurrentRegime = SAVING_REGIME;
	m_nTabs = 0;
	SaveDesignSettings(fd);
	SaveSimulationSettings(fd);
	SaveMaterials(fd);
	SaveParticleTypes(fd);
	SaveParticleSources(fd);
	SaveReflectingObjects(fd);
	m_CurrentRegime = STANDBY_REGIME;

	return LOGICAL_TRUE;
}

void CGranSim2DDesign::WriteTabs(FILE *fd)
{
	for (int i = 0; i < m_nTabs; i++)
		fprintf(fd, "\t");
}

void CGranSim2DDesign::WriteString(FILE *fd, char *pszString)
{
	WriteTabs(fd);
	fprintf(fd, "%s\n", pszString);
}

void CGranSim2DDesign::SaveDesignSettings(FILE *fd)
{
	WriteString(fd, PSZ_DESIGN_KEYWORD);
	IncTabs();

	DecTabs();
	WriteString(fd, PSZ_END_KEYWORD);
}

void CGranSim2DDesign::SaveSimulationSettings(FILE *fd)
{
	WriteString(fd, PSZ_SIMULATION_KEYWORD);
	IncTabs();

	CDesignTextString str;

	str.Set(PSZ_SIMULATE_UNTIL_KEYWORD, GetSimulateUntilTime());
	WriteString(fd, &str);

	char *pszValue = PSZ_EMPTY;
	switch(m_IntegrationMethod)
	{
	case IMPLICIT_EULER:
		pszValue = PSZ_IMPLICIT_EULER; break;
	case VELOCITY_VERLET:
		pszValue = PSZ_VERLET; break;
	case PREDICTOR_CORRECTOR:
		pszValue = PSZ_PREDICTOR_CORRECTOR; break;
	case TRAPEZOID:
		pszValue = PSZ_TRAPEZOID; break;
	case RUNGE_KUTTA_2:
		pszValue = PSZ_RUNGE_KUTTA_2; break;
	case RUNGE_KUTTA_4:
		pszValue = PSZ_RUNGE_KUTTA_4; break;
	}
	// Name
	str.Set(PSZ_TIME_INTEGRATION_METHOD_KEYWORD, pszValue);
	WriteString(fd, &str);

	DecTabs();
	WriteString(fd, PSZ_END_KEYWORD);
}

void CGranSim2DDesign::SaveMaterials(FILE *fd)
{
	for (CListOf<CMaterial>::iterator it = m_Materials.begin(); it != m_Materials.end(); it++)
	{
		CMaterial *pMaterial = *it;
		WriteString(fd, pMaterial->GetKeyWord());
		IncTabs();
		pMaterial->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void CGranSim2DDesign::SaveParticleTypes(FILE *fd)
{
	for (CListOf<CParticleType>::iterator it = m_ParticleTypes.begin(); it != m_ParticleTypes.end(); it++)
	{
		CParticleType *pType = *it;
		WriteString(fd, pType->GetKeyWord());
		IncTabs();
		pType->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void CGranSim2DDesign::SaveParticleSources(FILE *fd)
{
	for (CListOf<CParticleSource>::iterator it = m_ParticleSources.begin(); it != m_ParticleSources.end(); it++)
	{
		CParticleSource *pSource = *it;
		WriteString(fd, pSource->GetKeyWord());
		IncTabs();
		pSource->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void CGranSim2DDesign::SaveReflectingObjects(FILE *fd)
{
	for (CListOf<CReflectingObject>::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
	{
		CReflectingObject *pObject = *it;
		WriteString(fd, pObject->GetKeyWord());
		IncTabs();
		pObject->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void CGranSim2DDesign::DeleteMaterial(CMaterial *pMaterialToBeDeleted)
{
	// Remove this material from all particle types
	for (CListOf<CParticleType>::iterator pTypeIterator = m_ParticleTypes.begin(); pTypeIterator != m_ParticleTypes.end(); pTypeIterator++)
	{
		CParticleType *pType = *pTypeIterator;
		if (pType->GetMaterial() == pMaterialToBeDeleted)
			pType->SetMaterial(NULL);
	}
	
	// Remove material from material list
	m_Materials.RemoveObject(pMaterialToBeDeleted);
}

void CGranSim2DDesign::DeleteParticleType(CParticleType *pParticleTypeToBeDeleted)
{
	// Remove this particle type from all particle sources
	for (CListOf<CParticleSource>::iterator pSourceIterator = m_ParticleSources.begin(); pSourceIterator != m_ParticleSources.end(); pSourceIterator++)
	{
		CParticleSource *pSource = *pSourceIterator;
		if (pSource->GetParticleType() == pParticleTypeToBeDeleted)
			pSource->SetParticleType(NULL);
	}
	
	// Remove particle type from parttype list
	m_ParticleTypes.RemoveObject(pParticleTypeToBeDeleted);
}

void CGranSim2DDesign::DeleteParticleSource(CParticleSource *pParticleSourceToBeDeleted)
{
	// Remove particle source from source list
	m_ParticleSources.RemoveObject(pParticleSourceToBeDeleted);
}

void CGranSim2DDesign::DeleteReflectingObject(CReflectingObject *pReflectingObjectToBeDeleted)
{
	// Remove object from object list
	m_ReflectingObjects.RemoveObject(pReflectingObjectToBeDeleted);
}

LOGICAL CGranSim2DDesign::IsFreeSpaceForParticle(StateEnum iState, CParticle2D *pParticle)
{
	for (int i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (int j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i,j);
			for (int k = 0; k < pCell->GetParticleCount(); k++)
			{
				CParticle2D *pParticle2 = pCell->GetParticle(k);
				if (pParticle2->IsOverlapped(iState, pParticle))
					return LOGICAL_FALSE;
			}
		}
	}
	return LOGICAL_TRUE;
}

LOGICAL CGranSim2DDesign::DoesParticleTouchReflectingObject(StateEnum iState, CParticle2D *pParticle)
{
	for (CListOf<CReflectingObject>::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
	{
		CReflectingObject *pObject = *it;
		if (pObject->DoesContactWithParticle(iState, pParticle, NULL))
			return LOGICAL_TRUE;
	}
	return LOGICAL_FALSE;
}

void CGranSim2DDesign::VisitAllMaterials(IObjectVisitor *pVisitor)
{
	for (CListOf<CMaterial>::iterator it = m_Materials.begin(); it != m_Materials.end(); it++)
	{
		CMaterial *pMaterial = *it;
		pVisitor->Visit(pMaterial);
	}
}

void CGranSim2DDesign::VisitAllParticleTypes(IObjectVisitor *pVisitor)
{
	for (CListOf<CParticleType>::iterator it = m_ParticleTypes.begin(); it != m_ParticleTypes.end(); it++)
	{
		CParticleType *pType = *it;
		pVisitor->Visit(pType);
	}
}

CGranSim2DObject *CGranSim2DDesign::GetObjectAt(double WorldX, double WorldY, double tolerance)
{
	double MinDistance = VERY_BIG_DOUBLE;
	CGranSim2DObject *pNearestObject = NULL;
	for (CListOf<CReflectingObject>::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
	{
		CReflectingObject *pObject = *it;
		CGeometry2D *pGeometry = pObject->GetGeometry();
		if (pGeometry)
		{
			double distance = pGeometry->CalcDistance(WorldX, WorldY);
			if (distance < tolerance && distance < MinDistance)
			{
				MinDistance = distance;
				pNearestObject = pObject;
			}
		}
	}
	for (CListOf<CParticleSource>::iterator it2 = m_ParticleSources.begin(); it2 != m_ParticleSources.end(); it2++)
	{
		CParticleSource *pObject = *it2;
		CGeometry2D *pGeometry = pObject->GetGeometry();
		if (pGeometry)
		{
			double distance = pGeometry->CalcDistance(WorldX, WorldY);
			if (distance < tolerance && distance < MinDistance)
			{
				MinDistance = distance;
				pNearestObject = pObject;
			}
		}
	}
	return pNearestObject;
}

CParticleType *CGranSim2DDesign::CreateNewParticleType(LOGICAL bUpdateGUI)
{
	CParticleType *pNewType = new CParticleType();
	m_ParticleTypes.push_back(pNewType);
	if (bUpdateGUI)
	{
		g_pGui->ParticleTypeCreated(pNewType, this);
		g_pGui->DesignChanged();
	}
	return pNewType;
}

CMaterial *CGranSim2DDesign::CreateNewMaterial(LOGICAL bUpdateGUI)
{
	CMaterial *pNewMaterial = new CMaterial();
	m_Materials.push_back(pNewMaterial);
	if (bUpdateGUI)
	{
		g_pGui->MaterialCreated(pNewMaterial, this);
		g_pGui->DesignChanged();
	}
	return pNewMaterial;
}

CParticlePointSource *CGranSim2DDesign::CreateNewPointSource(CPoint2D *pPoint, void *pWhere, LOGICAL bUpdateGUI)
{
	CParticlePointSource *pSource = new CParticlePointSource(pPoint);
	m_ParticleSources.push_back(pSource);
	if (bUpdateGUI)
	{
		g_pGui->ParticlePointSourceCreated(pSource, pWhere);
		g_pGui->DesignChanged();
	}
	return pSource;
}

CParticleLinearSource *CGranSim2DDesign::CreateNewLinearSource(CLine2D *pLine, void *pWhere, LOGICAL bUpdateGUI)
{
	CParticleLinearSource *pSource = new CParticleLinearSource(pLine);
	m_ParticleSources.push_back(pSource);
	if (bUpdateGUI)
	{
		g_pGui->ParticleLinearSourceCreated(pSource, pWhere);
		g_pGui->DesignChanged();
	}
	return pSource;
}

CReflectingWall *CGranSim2DDesign::CreateNewWall(CLine2D *pLine, void *pWhere, LOGICAL bUpdateGUI)
{
	CReflectingWall *pWall = new CReflectingWall(pLine);
	m_ReflectingObjects.push_back(pWall);
	if (bUpdateGUI)
	{
		g_pGui->ReflectingWallCreated(pWall, pWhere);
		g_pGui->DesignChanged();
	}
	return pWall;
}

CReflectingRectangle *CGranSim2DDesign::CreateNewRectangle(CRectangle2D *pRectangle, void *pWhere, LOGICAL bUpdateGUI)
{
	CReflectingRectangle *pRect = new CReflectingRectangle(pRectangle);
	m_ReflectingObjects.push_back(pRect);
	if (bUpdateGUI)
	{
		g_pGui->ReflectingRectangleCreated(pRect, pWhere);
		g_pGui->DesignChanged();
	}
	return pRect;
}

IO_ERROR_CODE_ENUM CGranSim2DDesign::GetNextToken(CTextString &token)
{
	const char *pszDelimeters = " \t\r\n,";
	token.Empty();
	LOGICAL bSlash = LOGICAL_FALSE, bDoubleSlash = LOGICAL_FALSE;
	LOGICAL bQuote = LOGICAL_FALSE;
	LOGICAL bBlank = LOGICAL_FALSE;
	while (1)
	{
		int ch = fgetc(m_pIOFile);
		if (ch == EOF)
		{
			if (token.IsEmpty())
				return IO_ERROR_CODE_EOF;
			else
				return IO_ERROR_CODE_OK;
		}
		char c = LOBYTE(LOWORD(ch));
		if (ch == 0x0D || ch == 0x0A)
			bSlash = bDoubleSlash = LOGICAL_FALSE;
		if (!bQuote && strchr(pszDelimeters, ch))
		{
			if (!token.IsEmpty())
			{
				if (token == "=")
				{ // '=' symbol, get next token
					token.Empty();
					continue;
				}
				// good token, return it
				return IO_ERROR_CODE_OK;
			}
		}
		else
		{
			if (!bQuote)
			{
				if (bDoubleSlash)
					continue; // skip everything till CRLF
				if (ch == '/')
				{
					if (bSlash)
					{
						bDoubleSlash = LOGICAL_TRUE;
						token.TrimRight(1);
						continue;
					}
					bSlash = LOGICAL_TRUE;
				}
				else
					bSlash = bDoubleSlash = LOGICAL_FALSE;
			}
			else
			{
				if (ch == ' ')
				{ // skip multiply blanks
					if (bBlank)
						continue;
					bBlank = LOGICAL_TRUE;
				}
				else
					bBlank = LOGICAL_FALSE;
			}
			if (ch == '\"')
			{
				if (bQuote)
					return IO_ERROR_CODE_OK;
				bQuote = LOGICAL_TRUE;
				continue;
			}
			token += c;
		}
	}

	return IO_ERROR_CODE_OK;
}



// Misc

