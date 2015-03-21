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

#include "gransim2d_design.h"
#include "design_text_string.h"
#include "text_const.h"
#include "global.h"
#include "calc/calc.h"
#include <math.h>
#include <string.h>
#include <utility>

using namespace common;
using namespace geometry2d;
using namespace portable;


/////////////////////////////////////////////////////////////////////////////
// CAttachedBlock

CAttachedBlock::CAttachedBlock(long ViewIndex, cGranSim2DDesign *pDesign, CGranSim2DObject *pObject)
{
	m_ViewIndex = ViewIndex;
	m_pDesign = pDesign;
	m_pObject = pObject;
}

CAttachedBlock::~CAttachedBlock()
{
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
const char *CReflectingWall::GetKeyWord()
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
const char *CReflectingRectangle::GetKeyWord()
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
// CParticleSizeRenderer

cGranSim2DDesign::cGranSim2DDesign()
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
cGranSim2DDesign::~cGranSim2DDesign()
{
	FreeSimCells();
}

void cGranSim2DDesign::UpdateBoxAndSize()
{
	m_Box.Init();
    for (cParticleSources::iterator SourceIterator = m_ParticleSources.begin(); SourceIterator != m_ParticleSources.end(); SourceIterator++)
	{
		CParticleSource *pSource = *SourceIterator;
		UpdateBoxAndSize(pSource->GetGeometry());
	}
    for (cReflectingObjects::iterator ObjectIterator = m_ReflectingObjects.begin(); ObjectIterator != m_ReflectingObjects.end(); ObjectIterator++)
	{
		CReflectingObject *pObject = *ObjectIterator;
		UpdateBoxAndSize(pObject->GetGeometry());
	}

	m_Box.Expand(1.e-10);
}

void cGranSim2DDesign::UpdateBoxAndSize(CGeometry2D *pGeometry)
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

CGeometry2DObject *cGranSim2DDesign::OnPointCreatedExternally(CPoint2D *pPoint, void *pWhere)
{
	CGeometry2DObject *pGeometryObject = 0;
	switch (m_CurrentCreationMode)
	{
    case PARTICLE_POINT_SOURCE_CREATION:
    {
        pGeometryObject = m_CurrentObject.pParticleSource = CreateNewPointSource(pPoint, pWhere, true);
        break;
    }
    default:
        break;
	}
	g_pGui->DesignChanged();
	return pGeometryObject;
}

CGeometry2DObject *cGranSim2DDesign::OnLineCreatedExternally(CLine2D *pLine, void *pWhere)
{
	CGeometry2DObject *pGeometryObject = 0;
	switch (m_CurrentCreationMode)
	{
        case REFLECTING_WALL_CREATION:
        {
            pGeometryObject = m_CurrentObject.pWall = CreateNewWall(pLine, pWhere, true);
            break;
        }
        case PARTICLE_LINEAR_SOURCE_CREATION:
        {
            pGeometryObject = m_CurrentObject.pParticleSource = CreateNewLinearSource(pLine, pWhere, true);
            break;
        }
        default:
            break;
	}
	g_pGui->DesignChanged();
	return pGeometryObject;
}

CGeometry2DObject *cGranSim2DDesign::OnRectangleCreatedExternally(CRectangle2D *pRectangle, void *pWhere)
{
	CGeometry2DObject *pGeometryObject = NULL;
	switch (m_CurrentCreationMode)
	{
		case REFLECTING_RECTANGLE_CREATION:
		{
			pGeometryObject = m_CurrentObject.pRectangle = CreateNewRectangle(pRectangle, pWhere, true);
			break;
		}
        default:
            break;
	}
	g_pGui->DesignChanged();
	return pGeometryObject;
}

void cGranSim2DDesign::FreeSimCells()
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

void cGranSim2DDesign::RebuildSimCells()
{
	int i, j;
	FreeSimCells();

	UpdateBoxAndSize();

	// Estimate ave size of particles
	double fSum = 0;
	double fIntensSum = 0;
	double fAveParticleSize = -1;
    for (cParticleSources::iterator SourceIt = m_ParticleSources.begin(); SourceIt != m_ParticleSources.end(); SourceIt++)
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
	
	bool bOK = m_SimCells.SetSizes(m_nSimCellCols, m_nSimCellRows);
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

CSimCell_2D *cGranSim2DDesign::PointToCell(double x, double y)
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

bool cGranSim2DDesign::StartSimulation()
{
	if (!CheckForSimulation())
		return false;

	RebuildSimCells();
	
    for (cParticleSources::iterator SourceIt = m_ParticleSources.begin(); SourceIt != m_ParticleSources.end(); SourceIt++)
	{
		CParticleSource *pSource = *SourceIt;
		pSource->Simulation_Started();
	}
	m_CurrentRegime = SIMULATION_REGIME;
	m_Time = 0;
	m_TimeStep = 0.0001;
	return true;
}

void cGranSim2DDesign::StopSimulation()
{
	m_CurrentRegime = STANDBY_REGIME;
}

void cGranSim2DDesign::SetIntegrationMethod(IntergationMethodEnum method)
{
	if (m_IntegrationMethod == method)
		return;
	m_IntegrationMethod = method;
	g_pGui->DesignChanged();
}

void cGranSim2DDesign::SetSimulateUntilTime(double fSimulateUntilTime)
{
	if (fabs(m_fSimulateUntilTime - fSimulateUntilTime) < 1.e-6)
		return;
	m_fSimulateUntilTime = fSimulateUntilTime;
	g_pGui->DesignChanged();
}

bool cGranSim2DDesign::SimulateOneTimeStep()
{
	if ((m_Time+1.e-6) >= GetSimulateUntilTime())
		return false;
	OneTimeStep();

	return true;
}

double cGranSim2DDesign::OneTimeStep()
{
	int i, j;
	// 1. Clip time step to avoid too big overlap
	double dt = m_TimeStep;
	if (m_Time+dt >= GetSimulateUntilTime())
		dt = GetSimulateUntilTime()-m_Time;
	
    cParticleSources::iterator SourceIterator;
    for (SourceIterator = m_ParticleSources.begin(); SourceIterator != m_ParticleSources.end(); SourceIterator++)
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
    int nParticlesOutOfCells = m_ParticlesOutOfCells.size();
    for (i = 0; i < nParticlesOutOfCells; i++)
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
    m_ParticlesOutOfCells.resize(0);

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

void cGranSim2DDesign::VelocityVerlet(double &dt)
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
//	DWORD s = GetTickCount();
	RecalcForcesForAllParticles(NEXT_STATE);
//	DWORD e = GetTickCount();
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

void cGranSim2DDesign::PredictorCorrector(double &dt)
{
}

void cGranSim2DDesign::RecalcForcesForAllParticles(StateEnum state)
{
	int i1, j1, i2, j2;
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
    for (cReflectingObjects::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
    {
		CReflectingObject *pReflectingObject = *it;
/*		if (pParticle1->IsOverlapped(state, pReflectingObject, pContact))
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
double cGranSim2DDesign::CalcNormalForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact)
{
	return 1;
}

// returns abs value of tangential force  (particle2 affects particle1, the normal direction is calculated in pContact)
double cGranSim2DDesign::CalcTangentialForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact)
{
	return 0;
}

// returns abs value of normal force  (reflecting object affects particle1, the normal direction is calculated in pContact)
double cGranSim2DDesign::CalcNormalForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact)
{
	return 1;
}

// returns abs value of tangential force  (reflecting object affects particle1, the normal direction is calculated in pContact)
double cGranSim2DDesign::CalcTangentialForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact)
{
	return 0;
}

bool cGranSim2DDesign::LoadFromFile(const std::string &FileName)
{
	return LoadFromFile(FileName.c_str());
}

bool cGranSim2DDesign::LoadFromFile(const char *pszFileName)
{
	FILE *fd = fopen(pszFileName, "r");
	if (!fd)
		return false;

	bool bSuccess = LoadFromFile(fd);
	fclose(fd);
	return bSuccess;
}

bool cGranSim2DDesign::LoadFromFile(FILE *fd)
{
	ClearAll();	
	if (!fd)
		return false;

	m_CurrentRegime = LOADING_REGIME;
	m_pIOFile = fd;
	m_CurrentObject.pObject = NULL;
    portable::CTextString token;
//	TRACE("---------------------------------------------\n");
	IO_ERROR_CODE_ENUM EC;
	while (1)
	{
		EC = GetNextToken(token);
		if (EC == IO_ERROR_CODE_EOF)
			break;
		if (EC != IO_ERROR_CODE_OK)
			return false;
		if (strlen(token.GetText()) < 512)
		{
//			TRACE("%s\n", token.GetText());
		//	Sleep(100);
		}
		ProcessToken(token);
	}
//	TRACE("---------------------------------------------\n");

	// Set materials for all particle types
    for (cParticleTypes::iterator pPartTypeIterator = m_ParticleTypes.begin(); pPartTypeIterator != m_ParticleTypes.end(); pPartTypeIterator++)
	{
		CParticleType *pType = *pPartTypeIterator;
		CMaterial *pMaterial = FindMaterial(pType->GetMaterialName());
		pType->SetMaterial(pMaterial);
	}
	// Set materials for all reflecting objects
    for (cReflectingObjects::iterator pReflectingObjectIterator = m_ReflectingObjects.begin(); pReflectingObjectIterator != m_ReflectingObjects.end(); pReflectingObjectIterator++)
	{
		CReflectingObject *pReflectingObject = *pReflectingObjectIterator;
		CMaterial *pMaterial = FindMaterial(pReflectingObject->GetMaterialName());
		pReflectingObject->SetMaterial(pMaterial);
	}
	// Set particle types for all particle sources types
    for (cParticleSources::iterator pPartSourceIterator = m_ParticleSources.begin(); pPartSourceIterator != m_ParticleSources.end(); pPartSourceIterator++)
	{
		CParticleSource *pSource = *pPartSourceIterator;
		CParticleType *pType = FindParticleType(pSource->GetParticleTypeName());
		pSource->SetParticleType(pType);
	}

	m_CurrentRegime = STANDBY_REGIME;
	
	return true;
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

void cGranSim2DDesign::ProcessToken(CTextString &token)
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
            default:
                break;
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
			m_CurrentObject.pMaterial = CreateNewMaterial(false);
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
			m_CurrentObject.pParticleType = CreateNewParticleType(false);
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
		m_CurrentObject.pParticleSource = CreateNewLinearSource(NULL, this, false);
	}
	else if (token.IsEqualNoCase(PSZ_POINT_PARTICLE_SOURCE_KEYWORD))
	{
		m_CurrentLoadingSection = IO_PARTICLE_SOURCE_SECTION;
		m_CurrentCreationMode = PARTICLE_POINT_SOURCE_CREATION;
		m_CurrentObject.pParticleSource = CreateNewPointSource(NULL, this, false);
	}
	else if (token.IsEqualNoCase(PSZ_WALL_KEYWORD))
	{
		m_CurrentLoadingSection = IO_WALL_SECTION;
		m_CurrentCreationMode = REFLECTING_WALL_CREATION;
		m_CurrentObject.pWall = CreateNewWall(NULL, this, false);
	}
	else if (token.IsEqualNoCase(PSZ_RECTANGLE_KEYWORD))
	{
		m_CurrentLoadingSection = IO_RECTANGLE_SECTION;
		m_CurrentCreationMode = REFLECTING_RECTANGLE_CREATION;
		m_CurrentObject.pRectangle = CreateNewRectangle(NULL, this, false);
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
			unsigned int R = 0, G = 0, B = 0, A = 0;
			int n = sscanf(m_NextToken.GetText(), PSZ_RGBA_FORMAT_STRING, &R, &G, &B, &A);
			m_CurrentObject.pGeometryObject->SetColor((unsigned char)(R), (unsigned char)(G), (unsigned char)(B), (unsigned char)(A));
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

void cGranSim2DDesign::ReportUnexpectedKeyword(const char *pszKeyWord)
{
}

void cGranSim2DDesign::ClearAll()
{
	m_Materials.resize(0);
	m_ParticleTypes.resize(0);
	m_ParticleSources.resize(0);
	m_ReflectingObjects.resize(0);
	m_ParticleTypes.resize(0);
	FreeSimCells();
	CGranSim2DObject::m_nObjectCount = 0; // reset object counter
	m_Box.Init();
	g_pGui->DesignCleaned();
}

bool cGranSim2DDesign::CheckForSimulation()
{
	CTextString CheckStatus;
	CheckStatus.Empty();
	int nErrors = 0;
    for (cParticleSources::iterator pSourceIt = m_ParticleSources.begin(); pSourceIt != m_ParticleSources.end(); pSourceIt++)
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

CMaterial *cGranSim2DDesign::FindMaterial(CTextString &MaterialName)
{
    for (cMaterials::iterator it = m_Materials.begin(); it != m_Materials.end(); it++)
	{
		CMaterial *pMaterial = *it;
		if (MaterialName == pMaterial->GetGivenName())
			return pMaterial;
	}
	return NULL;
}

CParticleType *cGranSim2DDesign::FindParticleType(CTextString &TypeName)
{
    for (cParticleTypes::iterator it = m_ParticleTypes.begin(); it != m_ParticleTypes.end(); it++)
	{
		CParticleType *pType = *it;
		if (TypeName == pType->GetGivenName())
			return pType;
	}
	return NULL;
}

bool cGranSim2DDesign::SaveToFile(FILE *fd)
{
	if (!fd)
		return false;
	
	m_CurrentRegime = SAVING_REGIME;
	m_nTabs = 0;
	SaveDesignSettings(fd);
	SaveSimulationSettings(fd);
	SaveMaterials(fd);
	SaveParticleTypes(fd);
	SaveParticleSources(fd);
	SaveReflectingObjects(fd);
	m_CurrentRegime = STANDBY_REGIME;

	return false;
}

void cGranSim2DDesign::WriteTabs(FILE *fd)
{
	for (int i = 0; i < m_nTabs; i++)
		fprintf(fd, "\t");
}

void cGranSim2DDesign::WriteString(FILE *fd, const char *pszString)
{
	WriteTabs(fd);
	fprintf(fd, "%s\n", pszString);
}

void cGranSim2DDesign::SaveDesignSettings(FILE *fd)
{
	WriteString(fd, PSZ_DESIGN_KEYWORD);
	IncTabs();

	DecTabs();
	WriteString(fd, PSZ_END_KEYWORD);
}

void cGranSim2DDesign::SaveSimulationSettings(FILE *fd)
{
	WriteString(fd, PSZ_SIMULATION_KEYWORD);
	IncTabs();

	CDesignTextString str;

	str.Set(PSZ_SIMULATE_UNTIL_KEYWORD, GetSimulateUntilTime());
	WriteString(fd, &str);

	const char *pszValue = PSZ_EMPTY;
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
	default:
		break;
	}
	// Name
	str.Set(PSZ_TIME_INTEGRATION_METHOD_KEYWORD, pszValue);
	WriteString(fd, &str);

	DecTabs();
	WriteString(fd, PSZ_END_KEYWORD);
}

void cGranSim2DDesign::SaveMaterials(FILE *fd)
{
	for (cMaterials::iterator it = m_Materials.begin(); it != m_Materials.end(); it++)
	{
		CMaterial *pMaterial = *it;
		WriteString(fd, pMaterial->GetKeyWord());
		IncTabs();
		pMaterial->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void cGranSim2DDesign::SaveParticleTypes(FILE *fd)
{
	for (cParticleTypes::iterator it = m_ParticleTypes.begin(); it != m_ParticleTypes.end(); it++)
	{
		CParticleType *pType = *it;
		WriteString(fd, pType->GetKeyWord());
		IncTabs();
		pType->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void cGranSim2DDesign::SaveParticleSources(FILE *fd)
{
	for (cParticleSources::iterator it = m_ParticleSources.begin(); it != m_ParticleSources.end(); it++)
	{
		CParticleSource *pSource = *it;
		WriteString(fd, pSource->GetKeyWord());
		IncTabs();
		pSource->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void cGranSim2DDesign::SaveReflectingObjects(FILE *fd)
{
	for (cReflectingObjects::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
	{
		CReflectingObject *pObject = *it;
		WriteString(fd, pObject->GetKeyWord());
		IncTabs();
		pObject->SaveToFile(fd);
		DecTabs();
		WriteString(fd, PSZ_END_KEYWORD);
	}
}

void cGranSim2DDesign::DeleteMaterial(CMaterial *pMaterialToBeDeleted)
{
	// Remove this material from all particle types
	for (cParticleTypes::iterator pTypeIterator = m_ParticleTypes.begin(); pTypeIterator != m_ParticleTypes.end(); pTypeIterator++)
	{
		CParticleType *pType = *pTypeIterator;
		if (pType->GetMaterial() == pMaterialToBeDeleted)
			pType->SetMaterial(NULL);
	}
	
	// Remove material from material list
	for (cMaterials::iterator pMaterialIterator = m_Materials.begin(); pMaterialIterator != m_Materials.end(); pMaterialIterator++)
	{
		CMaterial *pMaterial = *pMaterialIterator;
		if (pMaterial == pMaterialToBeDeleted)
		{
			m_Materials.erase(pMaterialIterator);
			delete pMaterial;
			break;
		}
	}
}

void cGranSim2DDesign::DeleteParticleType(CParticleType *pParticleTypeToBeDeleted)
{
	// Remove this particle type from all particle sources
	for (cParticleSources::iterator pSourceIterator = m_ParticleSources.begin(); pSourceIterator != m_ParticleSources.end(); pSourceIterator++)
	{
		CParticleSource *pSource = *pSourceIterator;
		if (pSource->GetParticleType() == pParticleTypeToBeDeleted)
			pSource->SetParticleType(NULL);
	}
	
	// Remove particle type from parttype list
	for (cParticleTypes::iterator pTypeIterator = m_ParticleTypes.begin(); pTypeIterator != m_ParticleTypes.end(); pTypeIterator++)
	{
		CParticleType *pType = *pTypeIterator;
		if (pType == pParticleTypeToBeDeleted)
		{
			m_ParticleTypes.erase(pTypeIterator);
			delete pType;
			break;
		}
	}
}

void cGranSim2DDesign::DeleteParticleSource(CParticleSource *pParticleSourceToBeDeleted)
{
	// Remove particle source from source list
	for (cParticleSources::iterator pSourceIterator = m_ParticleSources.begin(); pSourceIterator != m_ParticleSources.end(); pSourceIterator++)
	{
		CParticleSource *pSource = *pSourceIterator;
		if (pSource == pParticleSourceToBeDeleted)
		{
			m_ParticleSources.erase(pSourceIterator);
			delete pSource;
			break;
		}
	}
}

void cGranSim2DDesign::DeleteReflectingObject(CReflectingObject *pReflectingObjectToBeDeleted)
{
	// Remove object from object list
	for (cReflectingObjects::iterator pObjectIterator = m_ReflectingObjects.begin(); pObjectIterator != m_ReflectingObjects.end(); pObjectIterator++)
	{
		CReflectingObject *pObject = *pObjectIterator;
		if (pObject == pReflectingObjectToBeDeleted)
		{
			m_ReflectingObjects.erase(pObjectIterator);
			delete pObject;
			break;
		}
	}
}

bool cGranSim2DDesign::IsFreeSpaceForParticle(StateEnum iState, CParticle2D *pParticle)
{
	for (int i = 0; i < m_SimCells.GetCols(); i++)
	{
		for (int j = 0; j < m_SimCells.GetRows(); j++)
		{
			CSimCell_2D *pCell = m_SimCells(i,j);
			for (int k = 0; k < pCell->GetParticleCount(); k++)
			{
				CParticle2D *pParticle2 = pCell->GetParticle(k);
				if (pParticle2->IsOverlapped(iState, pParticle, NULL))
					return false;
			}
		}
	}
	return true;
}

bool cGranSim2DDesign::DoesParticleTouchReflectingObject(StateEnum iState, CParticle2D *pParticle)
{
	for (cReflectingObjects::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
	{
		CReflectingObject *pObject = *it;
		if (pObject->DoesContactWithParticle(iState, pParticle, NULL))
			return true;
	}
	return false;
}

void cGranSim2DDesign::VisitAllMaterials(IObjectVisitor *pVisitor)
{
	for (cMaterials::iterator it = m_Materials.begin(); it != m_Materials.end(); it++)
	{
		CMaterial *pMaterial = *it;
		pVisitor->Visit(pMaterial);
	}
}

void cGranSim2DDesign::VisitAllParticleTypes(IObjectVisitor *pVisitor)
{
	for (cParticleTypes::iterator it = m_ParticleTypes.begin(); it != m_ParticleTypes.end(); it++)
	{
		CParticleType *pType = *it;
		pVisitor->Visit(pType);
	}
}

CGranSim2DObject *cGranSim2DDesign::GetObjectAt(double WorldX, double WorldY, double tolerance)
{
	double MinDistance = calc::VERY_BIG_DOUBLE;
	CGranSim2DObject *pNearestObject = NULL;
	for (cReflectingObjects::iterator it = m_ReflectingObjects.begin(); it != m_ReflectingObjects.end(); it++)
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
	for (cParticleSources::iterator it2 = m_ParticleSources.begin(); it2 != m_ParticleSources.end(); it2++)
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

CParticleType *cGranSim2DDesign::CreateNewParticleType(bool bUpdateGUI)
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

CMaterial *cGranSim2DDesign::CreateNewMaterial(bool bUpdateGUI)
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

CParticlePointSource *cGranSim2DDesign::CreateNewPointSource(CPoint2D *pPoint, void *pWhere, bool bUpdateGUI)
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

CParticleLinearSource *cGranSim2DDesign::CreateNewLinearSource(CLine2D *pLine, void *pWhere, bool bUpdateGUI)
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

CReflectingWall *cGranSim2DDesign::CreateNewWall(CLine2D *pLine, void *pWhere, bool bUpdateGUI)
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

CReflectingRectangle *cGranSim2DDesign::CreateNewRectangle(CRectangle2D *pRectangle, void *pWhere, bool bUpdateGUI)
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

IO_ERROR_CODE_ENUM cGranSim2DDesign::GetNextToken(CTextString &token)
{
	const char *pszDelimeters = " \t\r\n,";
	token.Empty();
	bool bSlash = false, bDoubleSlash = false;
	bool bQuote = false;
	bool bBlank = false;
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
        char c = (char)ch;
		if (ch == 0x0D || ch == 0x0A)
			bSlash = bDoubleSlash = false;
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
						bDoubleSlash = true;
						token.TrimRight(1);
						continue;
					}
					bSlash = true;
				}
				else
					bSlash = bDoubleSlash = false;
			}
			else
			{
				if (ch == ' ')
				{ // skip multiply blanks
					if (bBlank)
						continue;
					bBlank = true;
				}
				else
					bBlank = true;
			}
			if (ch == '\"')
			{
				if (bQuote)
					return IO_ERROR_CODE_OK;
				bQuote = true;
				continue;
			}
			token += c;
		}
	}

	return IO_ERROR_CODE_OK;
}



// Misc

