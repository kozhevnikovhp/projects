/* particle2d.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
    01 Sep 2010 - initial creation (moved out from another file)
*/

#include "particle2d.h"
#include "particle_type.h"
#include "gransim2d_design.h"
#include <math.h>
#include <string.h>

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
        common::geometry2d::NormalizeAngleRadians(fi);
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

bool CParticle2D::CanOverlap(StateEnum iState, CParticle2D *pParticle)
{
    double MaxRadiusSum = GetMaxDistanceToCenter() + pParticle->GetMaxDistanceToCenter();
    double dx = m_State[iState].m_Position.m_fX - pParticle->m_State[iState].m_Position.m_fX;
    if (fabs(dx) >  MaxRadiusSum)
        return false;
    double dy = m_State[iState].m_Position.m_fY - pParticle->m_State[iState].m_Position.m_fY;
    if (fabs(dy) >  MaxRadiusSum)
        return false;
    return ((dx*dx + dy*dy) <= MaxRadiusSum*MaxRadiusSum);
}

void CParticle2D::ShapeVertexToWorld(StateEnum iState, const sParticleShapeVertex *pShapeVertex, double &DstX, double &DstY)
{
    double x = pShapeVertex->x*m_fSize;
    double y = pShapeVertex->y*m_fSize;
    SParticleState *pState = m_State+iState;
    DstX = +x*pState->m_Cosine - y*pState->m_Sine + pState->m_Position.m_fX;
    DstY = +x*pState->m_Sine + y*pState->m_Cosine + pState->m_Position.m_fY;
}

// Function to be optimized
double CParticle2D::ParticleOverlap(StateEnum iState, CParticle2D *pParticle, int iShapeVertex)
{
	const sParticleShapeVertex *pShapeVertex = m_pParticleType->GetShapeVertex(iShapeVertex % N_PARTICLE_SHAPE_VERTICES);
    double x1, y1;
    ShapeVertexToWorld(iState, pShapeVertex, x1, y1);
    double Incline1 = pShapeVertex->Incline + GetFi(iState);
        common::geometry2d::NormalizeAngleRadians(Incline1);
        double DesiredIncline = Incline1 + common::calc::PI - pParticle->GetFi(iState);
        common::geometry2d::NormalizeAngleRadians(DesiredIncline);
	const sParticleShapeVertex *pAnotherShapeVertex = pParticle->m_pParticleType->GetShapePointIncline(DesiredIncline);
    double x2, y2;
    pParticle->ShapeVertexToWorld(iState, pAnotherShapeVertex, x2, y2);
    double dx = x2-x1;
    double dy = y2-y1;
    return (dx*dx+dy*dy);
}

bool CParticle2D::IsOverlapped(StateEnum iState, CParticle2D *pParticle, SContact *pContact /*=NULL*/)
{
    // 1.Simple check first
    if (!CanOverlap(iState, pParticle))
        return false;

    double x1, y1, x2, y2;

    if (!pContact)
    {
        x1 = m_State[iState].m_Position.m_fX - pParticle->m_State[iState].m_Position.m_fX;
        y1 = m_State[iState].m_Position.m_fY - pParticle->m_State[iState].m_Position.m_fY;
        double MinRadiusSum = GetMinDistanceToCenter() + pParticle->GetMinDistanceToCenter();
        if ((x1*x1 + y1*y1) < MinRadiusSum*MinRadiusSum)
            return true; // very close to each other
    }

    // 2.Complicated check now

    // Find good enough zero-th approximation for further optimization
	const int N_ANGLES = 8;
	double MinDistance = common::calc::VERY_BIG_DOUBLE;
	int iBestShapeVertex = 0;
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
	const sParticleShapeVertex *pShapeVertex = m_pParticleType->GetShapeVertex(iBestShapeVertex % N_PARTICLE_SHAPE_VERTICES);
    ShapeVertexToWorld(iState, pShapeVertex, x1, y1);

    double Incline1 = pShapeVertex->Incline + GetFi(iState);
        common::geometry2d::NormalizeAngleRadians(Incline1);
        double DesiredIncline = Incline1 + common::calc::PI - pParticle->GetFi(iState);
        common::geometry2d::NormalizeAngleRadians(DesiredIncline);
	const sParticleShapeVertex *pAnotherShapeVertex = pParticle->m_pParticleType->GetShapePointIncline(DesiredIncline);
    pParticle->ShapeVertexToWorld(iState, pAnotherShapeVertex, x2, y2);

    // 3. Check of contact points are inside another particle
    // Sine and cosine of Incline1 = pShapeVertex->Incline + m_Fi (to avoid sin/cos calls, using trigonometric formulas)
    double sine = pShapeVertex->m_Sine*m_State[iState].m_Cosine+pShapeVertex->m_Cosine*m_State[iState].m_Sine;
    double cosine = pShapeVertex->m_Cosine*m_State[iState].m_Cosine-pShapeVertex->m_Sine*m_State[iState].m_Sine;
    if (cosine*(y2 - y1) < sine*(x2 - x1))
        return false;

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

    return true;
}

bool CParticle2D::IsOverlapped(StateEnum iState, common::geometry2d::CLine2D *pLine, SContact *pContact /*=NULL*/)
{
    double h = fabs(pLine->GetPointHeightAbove(m_State[iState].m_Position.m_fX, m_State[iState].m_Position.m_fY));
    if (h > GetMaxDistanceToCenter())
        return false;
    double MutualAngle = pLine->GetAngleRadians() - GetFi(iState);
        common::geometry2d::NormalizeAngleRadians(MutualAngle);
	const sParticleShapeVertex *pShapeVertex = m_pParticleType->GetShapePointIncline(MutualAngle);
    if (h > pShapeVertex->DistanceToParticleCenter*m_fSize)
        return false;

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
        return false; // heights of equal sign, i.e. both points are on the same side of the line -> no overlap

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
    return true;
}

double CParticle2D::GetMaxDistanceToCenter()
{
    return m_fSize*m_pParticleType->m_MaxDistanceToParticleCenter;
}

double CParticle2D::GetMinDistanceToCenter()
{
    return m_fSize*m_pParticleType->m_MinDistanceToParticleCenter;
}

bool CParticle2D::IsCircular()
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
        common::geometry2d::SVector2D a = pCurrentState->m_Force/m_M;
    a.m_fY -= m_pDesign->GetG();
    m_State[NEXT_STATE].m_Position = pCurrentState->m_Position + pCurrentState->m_Velocity*dt + a*dt*dt/2;
}

void CParticle2D::VelocityVerletVelocities(double dt)
{
    SParticleState *pCurrentState = m_State+CURRENT_STATE;
    SParticleState *pNextState = m_State+NEXT_STATE;
        common::geometry2d::SVector2D a = (pCurrentState->m_Force + pNextState->m_Force)/m_M;
    a.m_fY -= m_pDesign->GetG();
    pNextState->m_Velocity = pCurrentState->m_Velocity + a/2*dt;
}

