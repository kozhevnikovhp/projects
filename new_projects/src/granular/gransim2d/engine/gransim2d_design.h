/* GranSim2DDesign.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
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

#ifndef __GRANSIM2D_DESIGN_H_INCLUDED__
#define __GRANSIM2D_DESIGN_H_INCLUDED__

#include "portable/rgba.h"
#include "geometry2d/geometry2d.h"
#include "material.h"
#include "particle_type.h"
#include "particle2d.h"
#include "particle_source.h"
#include "simcell_2d.h"
#include "widget_desc.h"
#include <vector>
#include <stdio.h>

// Forward declarations
class cGranSim2DDesign;
class CGranSim2DObject;
class CSimCell_2D;

typedef enum
{
	IMPLICIT_EULER,
	VELOCITY_VERLET,
	TRAPEZOID,
	PREDICTOR_CORRECTOR,
	RUNGE_KUTTA_2,
	RUNGE_KUTTA_4,
	UNKNOWN_TIME_INTEGRATION_METHOD
} IntergationMethodEnum;


class CAttachedBlock
{
	// Constructors-destructors
public:
	CAttachedBlock(long ViewIndex, cGranSim2DDesign *pDesign, CGranSim2DObject *pObject);
	~CAttachedBlock();

// Public methods
public:

// Public members
public:
	long m_ViewIndex;
	cGranSim2DDesign *m_pDesign;
	CGranSim2DObject *m_pObject;
};


class CReflectingObject : public CGeometry2DObject, public CMaterializedObject
{
//Constuctors/destructors
public:
	CReflectingObject();
	virtual ~CReflectingObject();

// Public methods
public:

// Public overridables
public:
	virtual const char *GetKeyWord() = 0;
	virtual void SaveToFile(FILE *fd);
	virtual void DeleteFromDesign();
	virtual int DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts) = 0;

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

// Private methods
private:

// Private overridables
private:

// Private members
private:

};
typedef std::vector<CReflectingObject *> cReflectingObjects;

class CReflectingWall : public CReflectingObject
{
//Constuctors/destructors
public:
    CReflectingWall(common::geometry2d::CLine2D *pLine);
	virtual ~CReflectingWall();

// Public methods
public:

// Public overridables
public:
        virtual const char *GetTypeDesc() { return "Wall"; }
        virtual const char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
        virtual common::geometry2d::CGeometry2D *GetGeometry() { return &m_Line; }
	virtual int DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts);

// Public members
public:
        common::geometry2d::CLine2D *GetLine() { return &m_Line; }

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
        common::geometry2d::CLine2D m_Line;
	
// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CReflectingRectangle : public CReflectingObject
{
//Constuctors/destructors
public:
        CReflectingRectangle(common::geometry2d::CRectangle2D *pRectangle);
	virtual ~CReflectingRectangle();

// Public methods
public:

// Public overridables
public:
    virtual const char *GetTypeDesc() { return "Rectangle"; }
    virtual const char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
        virtual common::geometry2d::CGeometry2D *GetGeometry() { return &m_Rectangle; }
	virtual int DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts);

// Public members
public:
        common::geometry2d::CRectangle2D *GetRectangle() { return &m_Rectangle; }

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
        common::geometry2d::CRectangle2D m_Rectangle;
	
// Private methods
private:

// Private overridables
private:

// Private members
private:

};

/////////////////////////////////////////////////////////////////////
// IObjectVisitor

class IObjectVisitor
{
// Public overridables
public:
	virtual bool Visit(CGranSim2DObject *pObject) = 0; // returns TRUE if further processing needed

// Protected overridables
protected:
};

typedef enum
{
	STANDBY_REGIME,
	SIMULATION_REGIME,
	SAVING_REGIME,
	LOADING_REGIME
} REGIME_ENUM;

typedef enum 
{
	NONE_CREATION,
	REFLECTING_WALL_CREATION,
	REFLECTING_RECTANGLE_CREATION,
	PARTICLE_LINEAR_SOURCE_CREATION,
	PARTICLE_POINT_SOURCE_CREATION
} CREATION_MODE_ENUM;

typedef enum
{
	IO_ERROR_CODE_OK,
	IO_ERROR_CODE_GENERIC,
	IO_ERROR_CODE_EOF,
	IO_ERROR_CODE_OUTOFMEMORY
} IO_ERROR_CODE_ENUM;

typedef enum
{
	IO_NONE_SECTION,
	IO_DESIGN_SECTION,
	IO_SIMULATION_SECTION,
	IO_MATERIAL_SECTION,
	IO_PARTICLE_TYPE_SECTION,
	IO_PARTICLE_SOURCE_SECTION,
	IO_WALL_SECTION,
	IO_RECTANGLE_SECTION
} IO_LOADING_SECTION_ENUM;

typedef union
{
	CGranSim2DObject *pObject;
	CGeometry2DObject *pGeometryObject;
	CMaterial *pMaterial;
	CParticleType *pParticleType;
	CParticleSource *pParticleSource;
	CParticleLinearSource *pParticleLinearSource;
	CParticlePointSource *pParticlePointSource;
	CReflectingObject *pReflectingObject;
	CReflectingWall *pWall;
	CReflectingRectangle *pRectangle;
} UAnyObject;

class cGranSim2DDesign
{
// Construction/destruction
public:
	cGranSim2DDesign();
	virtual ~cGranSim2DDesign();

// Public overridable methods
public:

// Protected overridable methods
protected:

// Public methods
public:
	const common::geometry2d::SExtents2D *GetBoundingBox() const { return &m_Box; }
	IntergationMethodEnum GetIntegrationMethod() { return m_IntegrationMethod; }
	void SetIntegrationMethod(IntergationMethodEnum method);

	bool SimulateOneTimeStep();
	double GetSimulateUntilTime() { return m_fSimulateUntilTime; }
	void SetSimulateUntilTime(double fSimulateUntilTime);
	double GetSimulationTime() const { return m_Time; }
	double GetSimulationTimeStep() const { return m_TimeStep; }
	double GetG() const { return m_G;	}
	REGIME_ENUM GetCurrentRegime() const { return m_CurrentRegime; }
	bool StartSimulation();
	void StopSimulation();
	bool IsSimulating() const { return GetCurrentRegime() == SIMULATION_REGIME; }
	bool LoadFromFile(const std::string &FileName);
	bool LoadFromFile(const char *pszFileName);
	bool LoadFromFile(FILE *fd);
	bool SaveToFile(FILE *fd);
	double GetSize() const { return m_fSize; }
	CGranSim2DObject *GetObjectAt(double WorldX, double WorldY, double tolerance);
	CParticleType *CreateNewParticleType(bool bUpdateGUI);
	CMaterial *CreateNewMaterial(bool bUpdateGUI);
	CParticlePointSource *CreateNewPointSource(common::geometry2d::CPoint2D *pPoint, void *pWhere, bool bUpdateGUI);
	CParticleLinearSource *CreateNewLinearSource(common::geometry2d::CLine2D *pLine, void *pWhere, bool bUpdateGUI);
	CReflectingWall *CreateNewWall(common::geometry2d::CLine2D *pLine, void *pWhere, bool bUpdateGUI);
	CReflectingRectangle *CreateNewRectangle(common::geometry2d::CRectangle2D *pRectangle, void *pWhere, bool bUpdateGUI);
	const CSimCells2D &GetSimCells() { return m_SimCells; }
	CSimCell_2D *PointToCell(double x, double y);
	CSimCell_2D *PointToCell(const common::geometry2d::SVector2D &position) { return PointToCell(position.m_fX, position.m_fY); }
	cReflectingObjects &GetReflectingObjects() { return m_ReflectingObjects; }
	cParticleSources &GetParticleSources() { return m_ParticleSources; }
	CGranSim2DObject *GetCurrentObject() { return m_CurrentObject.pObject; }
	CGeometry2DObject *OnPointCreatedExternally(common::geometry2d::CPoint2D *pPoint, void *pWhere);
	CGeometry2DObject *OnLineCreatedExternally(common::geometry2d::CLine2D *pLine, void *pWhere);
	CGeometry2DObject *OnRectangleCreatedExternally(common::geometry2d::CRectangle2D *pRectangle, void *pWhere);
	void ClearAll();
    void WriteString(FILE *fd, const char *pszString);
    void WriteString(FILE *fd, common::portable::CTextString *pStr) { WriteString(fd, pStr->GetText()); }
	void VisitAllMaterials(IObjectVisitor *pVisitor);
	void VisitAllParticleTypes(IObjectVisitor *pVisitor);
	void DeleteMaterial(CMaterial *pMaterialToBeDeleted);
	void DeleteParticleType(CParticleType *pParticleTypeToBeDeleted);
	void DeleteParticleSource(CParticleSource *pParticleSourceToBeDeleted);
	void DeleteReflectingObject(CReflectingObject *pReflectingObjectToBeDeleted);

	bool IsFreeSpaceForParticle(StateEnum iState, CParticle2D *pParticle);
	bool DoesParticleTouchReflectingObject(StateEnum iState, CParticle2D *pParticle);

	void SetNoneCreationMode() { m_CurrentCreationMode = NONE_CREATION; }
	bool IsInCreationReflectingWall() { return (m_CurrentCreationMode == REFLECTING_WALL_CREATION); }
	void SetReflectingWallCreationMode() { m_CurrentCreationMode = REFLECTING_WALL_CREATION; }
	bool IsInCreationReflectingRectangle() { return (m_CurrentCreationMode == REFLECTING_RECTANGLE_CREATION); }
	void SetReflectingRectangleCreationMode() { m_CurrentCreationMode = REFLECTING_RECTANGLE_CREATION; }
	bool IsInCreationParticleLinearSource() { return (m_CurrentCreationMode == PARTICLE_LINEAR_SOURCE_CREATION); }
	void SetParticleLinearSourceCreationMode() { m_CurrentCreationMode = PARTICLE_LINEAR_SOURCE_CREATION; }
	bool IsInCreationParticlePointSource() { return (m_CurrentCreationMode == PARTICLE_POINT_SOURCE_CREATION); }
	void SetParticlePointSourceCreationMode() { m_CurrentCreationMode = PARTICLE_POINT_SOURCE_CREATION; }

	double CalcNormalForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact);
	double CalcTangentialForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact);
	double CalcNormalForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact);
	double CalcTangentialForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact);

// Protected methods
protected:
    void UpdateBoxAndSize();
    void UpdateBoxAndSize(common::geometry2d::CGeometry2D *pGeometry);
	void IncTabs() { ++m_nTabs; }
	void DecTabs() { --m_nTabs; }
    void WriteTabs(FILE *fd);
    CMaterial *FindMaterial(common::portable::CTextString &MaterialName);
    CParticleType *FindParticleType(common::portable::CTextString &TypeName);
	void SaveDesignSettings(FILE *fd);
	void SaveSimulationSettings(FILE *fd);
	void SaveMaterials(FILE *fd);
	void SaveParticleTypes(FILE *fd);
	void SaveParticleSources(FILE *fd);
	void SaveReflectingObjects(FILE *fd);
    IO_ERROR_CODE_ENUM GetNextToken(common::portable::CTextString &token);
    void ProcessToken(common::portable::CTextString &token);
	void ReportUnexpectedKeyword(const char *pszKeyWord);
    void ReportUnexpectedKeyword(const common::portable::CTextString &KeyWord) { ReportUnexpectedKeyword(KeyWord.GetText()); }
	bool CheckForSimulation();
	void FreeSimCells();
	void RebuildSimCells();
	double OneTimeStep();
	void VelocityVerlet(double &dt);
	void PredictorCorrector(double &dt);
	void RecalcForcesForAllParticles(StateEnum state);

// Private methods
private:

// Public members
public:

// Protected members
protected:
    cMaterials m_Materials;
    cParticleTypes m_ParticleTypes;
    cParticleSources m_ParticleSources;
    cReflectingObjects m_ReflectingObjects;
	CSimCells2D m_SimCells;
	double m_fSimCellHeight, m_fSimCellWidth;
	int m_nSimCellRows, m_nSimCellCols;
    cParticles2D m_ParticlesOutOfCells; // temporary array
	int m_nTabs;
    common::portable::CTextString m_NextToken;
	FILE *m_pIOFile;
	CREATION_MODE_ENUM m_CurrentCreationMode;
	IO_LOADING_SECTION_ENUM m_CurrentLoadingSection;
	UAnyObject m_CurrentObject;
	REGIME_ENUM m_CurrentRegime;
    double m_fSize;
    common::geometry2d::SExtents2D m_Box;
	double m_G;
	double m_Time, m_TimeStep;
	IntergationMethodEnum m_IntegrationMethod;
	double m_fSimulateUntilTime;
	
// Private members
private:
};

// Misc

#endif //__GRANSIM2D_DESIGN_H_INCLUDED__

