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

#include "Geometry2D.h"

// Forward declarations
class CGranSim2DDesign;
class CGranSim2DObject;
class CParticleType;
class CSimCell_2D;

typedef struct
{
	double x;
	double y;
	double DistanceToParticleCenter;
	double Xk, Yk, R; // center and radius
	double Incline; //in radians
	double m_Sine, m_Cosine;
	void RecalcAux();
} SParticleShapeVertex;

typedef struct
{
	SVector2D Point1; //debug
	SVector2D Point2; //debug
	SVector2D ContactPoint;
	double Overlap;
	SVector2D NormalForceDirection; // oriented inside Particle1
	SVector2D TangentialForceDirection; // perpendicular to NormalForceDirection
} SContact;

template <class T> class CListOf : public std::vector<T *>
{
//Constuctors/destructors
public:
	CListOf();
	virtual ~CListOf();

// Public methods
public:
	void DeleteAll();
	void RemoveObject(T *pObject);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

};

typedef struct SParticleState
{
	SVector2D m_Position; // position
	SVector2D m_Velocity; // velocity
	SVector2D m_Force;
	double m_Omega; // rotation speed in radians/sec
	double m_Fi;
	double m_Sine, m_Cosine;

	inline void Set(SParticleState *pState);
	void SetPosition(double x, double y) { m_Position.Set(x, y); }
	void SetVelocity(double Vx, double Vy) { m_Velocity.Set(Vx, Vy); }
	void SetFi(double fi);
} SParticleState;

typedef enum
{
	PREV_STATE,
	CURRENT_STATE,
	NEXT_STATE,
	STOP_STATE
} StateEnum;

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

class CParticle2D
{
// Constructors/destructors
public:
	CParticle2D();
	CParticle2D(CParticle2D *pParticle);

// Public methods
public:
	LOGICAL CanOverlap(StateEnum iState, CParticle2D *pParticle);
	LOGICAL IsOverlapped(StateEnum iState, CParticle2D *pParticle, SContact *pContact = NULL);
	LOGICAL IsOverlapped(StateEnum iState, CLine2D *pLine, SContact *pContact = NULL);
	inline double GetMaxDistanceToCenter();
	inline double GetMinDistanceToCenter();
	inline LOGICAL IsCircular();
	void SetPosition(StateEnum iState, double x, double y) { m_State[iState].SetPosition(x, y); }
	void SetVelocity(StateEnum iState, double Vx, double Vy) { m_State[iState].SetVelocity(Vx, Vy); }
	double GetVelocitySq(StateEnum iState) { return m_State[iState].m_Velocity.GetAbsSq(); }
	double GetVelocitySq() { return m_State[CURRENT_STATE].m_Velocity.GetAbsSq(); }
	double GetVelocity(StateEnum iState) { return m_State[iState].m_Velocity.GetAbs(); }
	double GetVelocity() { return m_State[CURRENT_STATE].m_Velocity.GetAbs(); }
	double GetFi(StateEnum iState) { return m_State[iState].m_Fi; }
	void SetFi(StateEnum iState, double Fi);
	void SetOmega(StateEnum iState, double Omega) { m_State[iState].m_Omega; }
	double GetOmega(StateEnum iState) { return m_State[iState].m_Omega; }

	CSimCell_2D *GetCell() { return m_pCell; }
	void SetCell(CSimCell_2D *pCell) { m_pCell = pCell; }
	void CopyNextToCurrentState();
	void CopyCurrentToPrevState();
	void VelocityVerletCoords(double dt);
	void VelocityVerletVelocities(double dt);

// Public overridables
public:

// Public members
public:
	SParticleState m_State[STOP_STATE];

	double m_fSize;
	double m_M, m_I;
	CGranSim2DDesign *m_pDesign;
	CParticleType *m_pParticleType;
	CSimCell_2D *m_pCell;
	int m_iIndexInCell;

// Protected methods
protected:
	void ShapeVertexToWorld(StateEnum iState, const SParticleShapeVertex *pShapeVertex, double &DstX, double&DstY);
	double ParticleOverlap(StateEnum iState, CParticle2D *pParticle, int iShapeVertex);

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

typedef CVectorOf<CParticle2D *> CParticles2D;

class CDesignTextString : public CTextString
{
// Public methods
public:
	void Set(char *pszParamName, char *pszTextValue);
	void Set(char *pszParamName, SRGBA *pColor);
	void Set(char *pszParamName, double fValue);

// Public overridables
public:

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

typedef enum {
	DEFAULT_FORM_INDEX,
	SIM_SETTINGS_FORM_INDEX,
	OBJECT_COMMON_FORM_INDEX,
	MATERIAL_CONSTANTS_INDEX,
	PARTICLE_TYPE_SIZE_INDEX,
	PARTICLE_TYPE_SHAPE_INDEX,
	PARTICLE_TYPE_MATERIAL_INDEX,
	GEOMETRY_OBJECT_COMMON_FORM_INDEX,
	LINE_GEOMETRY_FORM_INDEX,
	RECTANGLE_GEOMETRY_FORM_INDEX,
	POINT_GEOMETRY_FORM_INDEX,
	PARTICLE_SOURCE_TYPE_FORM_INDEX,
	PARTICLE_SOURCE_INTENSITY_FORM_INDEX,
	REFLECTING_OBJECT_MATERIAL_FORM_INDEX,
	STOP_FORM_INDEX
} FormIndexEnum;

typedef struct
{
	FormIndexEnum FormIndex;
	char *pszTextDesc;
	int ImageIndex;
} SFormDesc;


class CGranSim2DObject
{
	friend class CGranSim2DDesign;
//Constuctors/destructors
public:
	CGranSim2DObject();
	virtual ~CGranSim2DObject();

// Public methods
public:
	static CGranSim2DDesign *GetDesign() { return m_pDesign; }
	char *GetFullName();
	char *GetGivenName() { return m_GivenName.GetText(); }
	void SetGivenName(const char *pszName);
	void SetGivenName(const CTextString &Name) { SetGivenName(Name.GetText()); }
	LOGICAL IsChecked(unsigned int uCheckTime) { return (m_uCheckTime == uCheckTime); }
	void Checked(unsigned int uCheckTime) { m_uCheckTime = uCheckTime; }

// Public overridables
public:
	virtual CGeometry2D *GetGeometry() { return NULL; }
	virtual char *GetTypeDesc() = 0;
	virtual int GetFormCount() = 0;
	virtual SFormDesc *GetFormDesc(int iFormNumber) = 0;
	virtual LOGICAL IsSelected() { return LOGICAL_FALSE; }
	virtual void Select() {}
	virtual void Unselect() {}
	virtual void SaveToFile(FILE *fd) = 0;
	virtual void DeleteFromDesign() = 0;

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	static CGranSim2DDesign *m_pDesign;
	static int m_nObjectCount;
	CTextString m_GivenName;
	unsigned int m_uCheckTime;
	char m_szFullName[256];

// Private methods
private:

// Private overridables
private:

// Private members
private:

};

typedef enum
{
	PARTICLE_SIZE_FIXED,
	PARTICLE_SIZE_UNIFORM,
	PARTICLE_SIZE_GAUSS
} PARTICLE_SIZE_TYPE_ENUM;

class CMaterial : public CGranSim2DObject
{
//Constuctors/destructors
public:
	CMaterial();
	virtual ~CMaterial();

// Public methods
public:

// Public overridables
public:
	virtual char *GetTypeDesc() { return "Material"; }
	virtual char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual void SaveToFile(FILE *fd);
	virtual void DeleteFromDesign();

// Public members
public:
	double m_Nu; // Poisson
	double m_E; // Young
	double m_Ro;

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

class CMaterializedObject
{
//Constuctors/destructors
public:
	CMaterializedObject();
	virtual ~CMaterializedObject();

// Public methods
public:
	CTextString &GetMaterialName() { return m_MaterialName; }
	CMaterial *GetMaterial() { return m_pMaterial; }
	void SetMaterial(CMaterial *pMaterial);

// Public overridables
public:

// Public members
public:
	CMaterial *m_pMaterial;
	CTextString m_MaterialName; // At loading time we store material name, and find material itself later (probably material does not exist yet)

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

const int N_PARTICLE_SHAPE_VERTICES = 360*1;

class CParticleType : public CGranSim2DObject, public CMaterializedObject
{
//Constuctors/destructors
public:
	CParticleType();
	virtual ~CParticleType();

// Public methods
public:
	double CalcMaxDistributionDensity();
	double GetMaxParticleSize();
	double GetMinParticleSize();
	double GetAveParticleSize();
	double GenSize();
	void RecalcShapeVertices();
//	void RebuildDisplayLists();
	void SetSizeType(const char *pszType);
	void SetSizeType(const CTextString &Type) { SetSizeType(Type.GetText()); }
	void SetShapeSquareness(double fSquareness);
	void SetShapeAspectRatio(double fAspectRatio);
	double GetShapeSquareness() { return m_ShapeSquareness; }
	double GetShapeAspectRatio() { return m_ShapeAspectRatio; }
	double GetMass() { return m_Mass; }
	double GetInertiaMomentum() { return m_InertiaMomentum; }
	LOGICAL IsCircular() { return m_bCircular; }
	const SParticleShapeVertex *GetShapePointIncline(double Incline);
	const SParticleShapeVertex *GetShapeVertex(int index) { return m_Vertices+index; }

// Public overridables
public:
	virtual char *GetTypeDesc() { return "Particle Type"; }
	virtual char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual void SaveToFile(FILE *fd);
	virtual void DeleteFromDesign();

// Public members
public:
	PARTICLE_SIZE_TYPE_ENUM m_SizeType;
	double m_SizeFixed;
	double m_SizeMedian;
	double m_SizeSigma; // standard deviation
	double m_SizeFrom, m_SizeTo;
	double m_MaxDistanceToParticleCenter, m_MinDistanceToParticleCenter;
	SParticleShapeVertex m_Vertices[N_PARTICLE_SHAPE_VERTICES];

// Protected methods
protected:
	void UpdateCircularFlag();

// Protected overridables
protected:

// Protected members
protected:
	double m_ShapeAspectRatio;
	unsigned int m_ShapeSquareness;
	LOGICAL m_bCircular;
	double m_Mass;
	double m_InertiaMomentum;
	
// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CAttachedBlock
{
	// Constructors-destructors
public:
	CAttachedBlock(long ViewIndex, CGranSim2DDesign *pDesign, CGranSim2DObject *pObject);
	~CAttachedBlock();

// Public methods
public:

// Public members
public:
	long m_ViewIndex;
	CGranSim2DDesign *m_pDesign;
	CGranSim2DObject *m_pObject;
};


class CGeometry2DObject : public CGranSim2DObject
{
//Constuctors/destructors
public:
	CGeometry2DObject();
	virtual ~CGeometry2DObject();

// Public methods
public:
	SRGBA *GetRGBA() { return &m_RGBA; }
	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	void SetColor(SRGBA *pRGBA);
	void SetGeometry(const char *pszGeometry);
	void SetGeometry(const CTextString &Geometry) { SetGeometry(Geometry.GetText()); }
	LOGICAL IsSelected() { return m_bSelected; }
//	void Select(void *pWhoSelects);
//	void Unselect(void *pWhoSelects);

// Public overridables
public:
	virtual char *GetKeyWord() = 0;
	virtual void SaveToFile(FILE *fd);

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	LOGICAL m_bSelected;
	SRGBA m_RGBA;

// Private methods
private:

// Private overridables
private:

// Private members
private:

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

typedef enum
{
	PARTICLE_SOURCE_FLOW_TYPE_UNIFORM,
	PARTICLE_SOURCE_FLOW_TYPE_POISSON
} PARTICLE_SOURCE_FLOW_TYPE_ENUM;

class CParticleSource : public CGeometry2DObject
{
//Constuctors/destructors
public:
	CParticleSource();
	virtual ~CParticleSource();

// Public methods
public:
	void SetFlowType(const char *pszType);
	void SetFlowType(const CTextString &Type) { SetFlowType(Type.GetText()); }
	void Simulation_Started();
	void Simulation_ClipTimeStep(double t, double &dt);
	CParticle2D *Simulation_NewParticle(double t);
	CTextString &GetParticleTypeName() { return m_ParticleTypeName; }
	CParticleType *GetParticleType() { return m_pParticleType; }
	void SetParticleType(CParticleType *pType);
	double GetMaxParticleSize() { return m_pParticleType->GetMaxParticleSize(); }
	double GetMinParticleSize() { return m_pParticleType->GetMinParticleSize(); }
	double GetAveParticleSize() { return m_pParticleType->GetAveParticleSize(); }
	double GetIntensity() { return m_fIntensity; }

// Public overridables
public:
	virtual void SaveToFile(FILE *fd);
	virtual void DeleteFromDesign();

// Public members
public:
	PARTICLE_SOURCE_FLOW_TYPE_ENUM m_FlowType;
	double m_fIntensity;
	double m_fNextGenTime; // time when the next particle will be generated

// Protected methods
protected:

// Protected overridables
protected:
	virtual void LocateProbeParticle() = 0;

// Protected members
protected:
	static CParticle2D g_ProbeParticle;
	CParticleType *m_pParticleType;
	CTextString m_ParticleTypeName; // At loading time we store particle type name, and find particle type itself later (probably particle type does not exist yet)
	
// Private methods
private:

// Private overridables
private:

// Private members
private:

};


class CParticlePointSource : public CParticleSource
{
//Constuctors/destructors
public:
	CParticlePointSource(CPoint2D *pPoint);
	virtual ~CParticlePointSource();

// Public methods
public:
	void SetPoint(CPoint2D *pPoint);

// Public overridables
public:
	virtual char *GetTypeDesc() { return "Particle Point Source"; }
	virtual char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual CGeometry2D *GetGeometry() { return &m_Point; }

// Public members
public:
	CPoint2D *GetPoint() { return &m_Point; }

// Protected methods
protected:

// Protected overridables
protected:
	virtual void LocateProbeParticle();

// Protected members
protected:
	CPoint2D m_Point;
	
// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CReflectingWall : public CReflectingObject
{
//Constuctors/destructors
public:
	CReflectingWall(CLine2D *pLine);
	virtual ~CReflectingWall();

// Public methods
public:

// Public overridables
public:
	virtual char *GetTypeDesc() { return "Wall"; }
	virtual char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual CGeometry2D *GetGeometry() { return &m_Line; }
	virtual int DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts);

// Public members
public:
	CLine2D *GetLine() { return &m_Line; }

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CLine2D m_Line;
	
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
	CReflectingRectangle(CRectangle2D *pRectangle);
	virtual ~CReflectingRectangle();

// Public methods
public:

// Public overridables
public:
	virtual char *GetTypeDesc() { return "Rectangle"; }
	virtual char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual CGeometry2D *GetGeometry() { return &m_Rectangle; }
	virtual int DoesContactWithParticle(StateEnum iState, CParticle2D *pParticle, SContact *pContacts);

// Public members
public:
	CRectangle2D *GetRectangle() { return &m_Rectangle; }

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CRectangle2D m_Rectangle;
	
// Private methods
private:

// Private overridables
private:

// Private members
private:

};

class CParticleLinearSource : public CParticleSource
{
//Constuctors/destructors
public:
	CParticleLinearSource(CLine2D *pLine);
	virtual ~CParticleLinearSource();

// Public overridables
public:
	virtual char *GetTypeDesc() { return "Particle Linear Source"; }
	virtual char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual CGeometry2D *GetGeometry() { return &m_Line; }

// Public members
public:
	CLine2D *GetLine() { return &m_Line; }

// Protected methods
protected:

// Protected overridables
protected:
	virtual void LocateProbeParticle();

// Protected members
protected:
	CLine2D m_Line;
	
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
	virtual LOGICAL Visit(CGranSim2DObject *pObject) = 0; // returns TRUE if further processing needed

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

class CSimCell_2D
{
//Constuctors/destructors
public:
	CSimCell_2D(CGranSim2DDesign *pDesign, double left, double right, double bottom, double top);
	virtual ~CSimCell_2D();

// Public methods
public:
	int GetParticleCount() { return m_Particles.GetSize(); }
	CParticle2D *GetParticle(int nParticle) { return m_Particles[nParticle]; }
	SExtents2D *GetBox() { return &m_Box; }
	void NewParticle(CParticle2D *pParticle);
	void RemoveParticle(CParticle2D *pParticle);
	void RecalcForcesForAllParticles(CSimCell_2D *pCell, StateEnum state);
	void ZeroForcesForAllParticles(StateEnum state);
	void CommitNextState(CParticles2D &ParticlesOutOfCells);
	LOGICAL IsInside(SVector2D &position) { return m_Box.IsInside(position); }
	LOGICAL IsInside(double x, double y) { return m_Box.IsInside(x, y); }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CParticles2D m_Particles;
	SExtents2D m_Box;
	CGranSim2DDesign *m_pDesign;
	CParticle2D *m_pBiggestParticle;
	double m_fBiggestParticleSize;
};

typedef CMatrixOf<CSimCell_2D *> CSimCells2D;

class CGranSim2DDesign
{
// Construction/destruction
public:
	CGranSim2DDesign();
	virtual ~CGranSim2DDesign();

// Public overridable methods
public:

// Protected overridable methods
protected:

// Public methods
public:
	const SExtents2D *GetBoundingBox() const { return &m_Box; }
	IntergationMethodEnum GetIntegrationMethod() { return m_IntegrationMethod; }
	void SetIntegrationMethod(IntergationMethodEnum method);

	LOGICAL SimulateOneTimeStep();
	double GetSimulateUntilTime() { return m_fSimulateUntilTime; }
	void SetSimulateUntilTime(double fSimulateUntilTime);
	double GetSimulationTime() const { return m_Time; }
	double GetSimulationTimeStep() const { return m_TimeStep; }
	double GetG() const { return m_G;	}
	REGIME_ENUM GetCurrentRegime() const { return m_CurrentRegime; }
	LOGICAL StartSimulation();
	void StopSimulation();
	LOGICAL IsSimulating() const { return GetCurrentRegime() == SIMULATION_REGIME; }
	LOGICAL LoadFromFile(char *pszFileName);
	LOGICAL LoadFromFile(FILE *fd);
	LOGICAL SaveToFile(FILE *fd);
	double GetSize() const { return m_fSize; }
	CGranSim2DObject *GetObjectAt(double WorldX, double WorldY, double tolerance);
	CParticleType *CreateNewParticleType(LOGICAL bUpdateGUI);
	CMaterial *CreateNewMaterial(LOGICAL bUpdateGUI);
	CParticlePointSource *CreateNewPointSource(CPoint2D *pPoint, void *pWhere, LOGICAL bUpdateGUI);
	CParticleLinearSource *CreateNewLinearSource(CLine2D *pLine, void *pWhere, LOGICAL bUpdateGUI);
	CReflectingWall *CreateNewWall(CLine2D *pLine, void *pWhere, LOGICAL bUpdateGUI);
	CReflectingRectangle *CreateNewRectangle(CRectangle2D *pRectangle, void *pWhere, LOGICAL bUpdateGUI);
	const CSimCells2D &GetSimCells() { return m_SimCells; }
	CSimCell_2D *PointToCell(double x, double y);
	CSimCell_2D *PointToCell(const SVector2D &position) { return PointToCell(position.m_fX, position.m_fY); }
	CListOf<CReflectingObject> &GetReflectingObjects() { return m_ReflectingObjects; }
	CListOf<CParticleSource> &GetParticleSources() { return m_ParticleSources; }
	CGranSim2DObject *GetCurrentObject() { return m_CurrentObject.pObject; }
	CGeometry2DObject *OnPointCreatedExternally(CPoint2D *pPoint, void *pWhere);
	CGeometry2DObject *OnLineCreatedExternally(CLine2D *pLine, void *pWhere);
	CGeometry2DObject *OnRectangleCreatedExternally(CRectangle2D *pRectangle, void *pWhere);
	void ClearAll();
	void WriteString(FILE *fd, char *pszString);
	void WriteString(FILE *fd, CTextString *pStr) { WriteString(fd, pStr->GetText()); }
	void VisitAllMaterials(IObjectVisitor *pVisitor);
	void VisitAllParticleTypes(IObjectVisitor *pVisitor);
	void DeleteMaterial(CMaterial *pMaterialToBeDeleted);
	void DeleteParticleType(CParticleType *pParticleTypeToBeDeleted);
	void DeleteParticleSource(CParticleSource *pParticleSourceToBeDeleted);
	void DeleteReflectingObject(CReflectingObject *pReflectingObjectToBeDeleted);

	LOGICAL IsFreeSpaceForParticle(StateEnum iState, CParticle2D *pParticle);
	LOGICAL DoesParticleTouchReflectingObject(StateEnum iState, CParticle2D *pParticle);

	void SetNoneCreationMode() { m_CurrentCreationMode = NONE_CREATION; }
	LOGICAL IsInCreationReflectingWall() { return (m_CurrentCreationMode == REFLECTING_WALL_CREATION); }
	void SetReflectingWallCreationMode() { m_CurrentCreationMode = REFLECTING_WALL_CREATION; }
	LOGICAL IsInCreationReflectingRectangle() { return (m_CurrentCreationMode == REFLECTING_RECTANGLE_CREATION); }
	void SetReflectingRectangleCreationMode() { m_CurrentCreationMode = REFLECTING_RECTANGLE_CREATION; }
	LOGICAL IsInCreationParticleLinearSource() { return (m_CurrentCreationMode == PARTICLE_LINEAR_SOURCE_CREATION); }
	void SetParticleLinearSourceCreationMode() { m_CurrentCreationMode = PARTICLE_LINEAR_SOURCE_CREATION; }
	LOGICAL IsInCreationParticlePointSource() { return (m_CurrentCreationMode == PARTICLE_POINT_SOURCE_CREATION); }
	void SetParticlePointSourceCreationMode() { m_CurrentCreationMode = PARTICLE_POINT_SOURCE_CREATION; }

	double CalcNormalForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact);
	double CalcTangentialForce(CParticle2D *pParticle1, CParticle2D *pParticle2, SContact *pContact);
	double CalcNormalForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact);
	double CalcTangentialForce(CParticle2D *pParticle1, CReflectingObject *pObject, SContact *pContact);

// Protected methods
protected:
	void UpdateBoxAndSize();
	void UpdateBoxAndSize(CGeometry2D *pGeometry);
	void IncTabs() { ++m_nTabs; }
	void DecTabs() { --m_nTabs; }
	void WriteTabs(FILE *fd);
	CMaterial *FindMaterial(CTextString &MaterialName);
	CParticleType *FindParticleType(CTextString &TypeName);
	void SaveDesignSettings(FILE *fd);
	void SaveSimulationSettings(FILE *fd);
	void SaveMaterials(FILE *fd);
	void SaveParticleTypes(FILE *fd);
	void SaveParticleSources(FILE *fd);
	void SaveReflectingObjects(FILE *fd);
	IO_ERROR_CODE_ENUM GetNextToken(CTextString &token);
	void ProcessToken(CTextString &token);
	void ReportUnexpectedKeyword(const char *pszKeyWord);
	void ReportUnexpectedKeyword(const CTextString &KeyWord) { ReportUnexpectedKeyword(KeyWord.GetText()); }
	LOGICAL CheckForSimulation();
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
	CListOf<CMaterial> m_Materials;
	CListOf<CParticleType> m_ParticleTypes;
	CListOf<CParticleSource> m_ParticleSources;
	CListOf<CReflectingObject> m_ReflectingObjects;
	CSimCells2D m_SimCells;
	double m_fSimCellHeight, m_fSimCellWidth;
	int m_nSimCellRows, m_nSimCellCols;
	CVectorOf<CParticle2D *> m_ParticlesOutOfCells; // temporary array
	int m_nTabs;
	CTextString m_NextToken;
	FILE *m_pIOFile;
	CREATION_MODE_ENUM m_CurrentCreationMode;
	IO_LOADING_SECTION_ENUM m_CurrentLoadingSection;
	UAnyObject m_CurrentObject;
	REGIME_ENUM m_CurrentRegime;
	double m_fSize;
	SExtents2D m_Box;
	double m_G;
	double m_Time, m_TimeStep;
	IntergationMethodEnum m_IntegrationMethod;
	double m_fSimulateUntilTime;
	
// Private members
private:
};

// Misc

#endif //__GRANSIM2D_DESIGN_H_INCLUDED__

