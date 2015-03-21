#ifndef PARTICLE_TYPE_H
#define PARTICLE_TYPE_H

#include "gransim2d_object.h"
#include "materialized_object.h"
#include "particle_shape_vertex.h"
#include <vector>

const int N_PARTICLE_SHAPE_VERTICES = 360*1;

typedef enum
{
	PARTICLE_SIZE_FIXED,
	PARTICLE_SIZE_UNIFORM,
	PARTICLE_SIZE_GAUSS
} PARTICLE_SIZE_TYPE_ENUM;

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
	void SetSizeType(const common::portable::CTextString &Type) { SetSizeType(Type.GetText()); }
	void SetShapeSquareness(double fSquareness);
	void SetShapeAspectRatio(double fAspectRatio);
	double GetShapeSquareness() { return m_ShapeSquareness; }
	double GetShapeAspectRatio() { return m_ShapeAspectRatio; }
	double GetMass() { return m_Mass; }
	double GetInertiaMomentum() { return m_InertiaMomentum; }
	bool IsCircular() { return m_bCircular; }
	const sParticleShapeVertex *GetShapePointIncline(double Incline);
	const sParticleShapeVertex *GetShapeVertex(int index) { return m_Vertices+index; }

// Public overridables
public:
	virtual const char *GetTypeDesc() { return "Particle Type"; }
	virtual const char *GetKeyWord();
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
	sParticleShapeVertex m_Vertices[N_PARTICLE_SHAPE_VERTICES];

// Protected methods
protected:
	void UpdateCircularFlag();

// Protected overridables
protected:

// Protected members
protected:
	double m_ShapeAspectRatio;
	unsigned int m_ShapeSquareness;
	bool m_bCircular;
	double m_Mass;
	double m_InertiaMomentum;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};
typedef std::vector<CParticleType *> cParticleTypes;


#endif // PARTICLE_TYPE_H
