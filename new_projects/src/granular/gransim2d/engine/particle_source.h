#ifndef PARTICLE_SOURCE_H
#define PARTICLE_SOURCE_H

#include "geometry2d_object.h"
#include "particle_type.h"

// Forward declarations
class CParticle2D;

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
	void SetFlowType(const common::portable::CTextString &Type) { SetFlowType(Type.GetText()); }
	void Simulation_Started();
	void Simulation_ClipTimeStep(double t, double &dt);
	CParticle2D *Simulation_NewParticle(double t);
	common::portable::CTextString &GetParticleTypeName() { return m_ParticleTypeName; }
	CParticleType *GetParticleType() { return m_pParticleType; }
	void SetParticleType(CParticleType *pType);
	double GetMaxParticleSize() { return m_pParticleType->GetMaxParticleSize(); }
	double GetMinParticleSize() { return m_pParticleType->GetMinParticleSize(); }
	double GetAveParticleSize() { return m_pParticleType->GetAveParticleSize(); }
	double GetIntensity() { return m_fIntensity; }

// Public overridables
public:
	virtual const char *GetKeyWord() = 0;
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
	common::portable::CTextString m_ParticleTypeName; // At loading time we store particle type name, and find particle type itself later (probably particle type does not exist yet)

// Private methods
private:

// Private overridables
private:

// Private members
private:

};
typedef std::vector<CParticleSource *> cParticleSources;

class CParticlePointSource : public CParticleSource
{
//Constuctors/destructors
public:
	CParticlePointSource(common::geometry2d::CPoint2D *pPoint);
	virtual ~CParticlePointSource();

// Public methods
public:
	void SetPoint(common::geometry2d::CPoint2D *pPoint);

// Public overridables
public:
	virtual const char *GetTypeDesc() { return "Particle Point Source"; }
	virtual const char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual common::geometry2d::CGeometry2D *GetGeometry() { return &m_Point; }

// Public members
public:
	common::geometry2d::CPoint2D *GetPoint() { return &m_Point; }

// Protected methods
protected:

// Protected overridables
protected:
	virtual void LocateProbeParticle();

// Protected members
protected:
	common::geometry2d::CPoint2D m_Point;

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
	CParticleLinearSource(common::geometry2d::CLine2D *pLine);
	virtual ~CParticleLinearSource();

// Public overridables
public:
	virtual const char *GetTypeDesc() { return "Particle Linear Source"; }
	virtual const char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual common::geometry2d::CGeometry2D *GetGeometry() { return &m_Line; }

// Public members
public:
	common::geometry2d::CLine2D *GetLine() { return &m_Line; }

// Protected methods
protected:

// Protected overridables
protected:
	virtual void LocateProbeParticle();

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


#endif // PARTICLE_SOURCE_H
