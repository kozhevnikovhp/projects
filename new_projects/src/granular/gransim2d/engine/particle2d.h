/* particle2d.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
    01 Sep 2010 - initial creation (moved out from another file)
*/
#ifndef __PARTICLE2D_H_INCLUDED__
#define __PARTICLE2D_H_INCLUDED__

#include "particle_shape_vertex.h"
#include "portable/types.h"
#include "geometry2d/geometry2d.h"
#include <vector>

// Forward declarations
class CSimCell_2D;
class cGranSim2DDesign;
class CParticleType;

typedef struct
{
    common::geometry2d::SVector2D Point1; //debug
    common::geometry2d::SVector2D Point2; //debug
    common::geometry2d::SVector2D ContactPoint;
    double Overlap;
    common::geometry2d::SVector2D NormalForceDirection; // oriented inside Particle1
    common::geometry2d::SVector2D TangentialForceDirection; // perpendicular to NormalForceDirection
} SContact;

typedef struct SParticleState
{
    common::geometry2d::SVector2D m_Position; // position
    common::geometry2d::SVector2D m_Velocity; // velocity
    common::geometry2d::SVector2D m_Force;
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

class CParticle2D
{
// Constructors/destructors
public:
    CParticle2D();
    CParticle2D(CParticle2D *pParticle);

// Public methods
public:
    bool CanOverlap(StateEnum iState, CParticle2D *pParticle);
    bool IsOverlapped(StateEnum iState, CParticle2D *pParticle, SContact *pContact);
    bool IsOverlapped(StateEnum iState, common::geometry2d::CLine2D *pLine, SContact *pContact);
	double GetMaxDistanceToCenter();
	double GetMinDistanceToCenter();
	bool IsCircular();
    void SetPosition(StateEnum iState, double x, double y) { m_State[iState].SetPosition(x, y); }
    void SetVelocity(StateEnum iState, double Vx, double Vy) { m_State[iState].SetVelocity(Vx, Vy); }
    double GetVelocitySq(StateEnum iState) { return m_State[iState].m_Velocity.GetAbsSq(); }
    double GetVelocitySq() { return m_State[CURRENT_STATE].m_Velocity.GetAbsSq(); }
    double GetVelocity(StateEnum iState) { return m_State[iState].m_Velocity.GetAbs(); }
    double GetVelocity() { return m_State[CURRENT_STATE].m_Velocity.GetAbs(); }
    double GetFi(StateEnum iState) { return m_State[iState].m_Fi; }
    void SetFi(StateEnum iState, double Fi);
	void SetOmega(StateEnum iState, double Omega) { m_State[iState].m_Omega = Omega; }
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
	cGranSim2DDesign *m_pDesign;
    CParticleType *m_pParticleType;
    CSimCell_2D *m_pCell;
    int m_iIndexInCell;

// Protected methods
protected:
	void ShapeVertexToWorld(StateEnum iState, const sParticleShapeVertex *pShapeVertex, double &DstX, double&DstY);
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
typedef std::vector<CParticle2D *> cParticles2D;


#endif // __PARTICLE2D_H_INCLUDED__
