#include "simcell_2d.h"
#include "gransim2d_design.h"
#include "geometry2d/geometry2d.h"

///////////////////////////////////////////////////////////////////////////
// CSimCell_2D

CSimCell_2D::CSimCell_2D(cGranSim2DDesign *pDesign, double left, double right, double bottom, double top)
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
    m_Particles.push_back(pParticle);
    if (!m_pBiggestParticle || m_pBiggestParticle->GetMaxDistanceToCenter() < pParticle->GetMaxDistanceToCenter())
    {
        m_pBiggestParticle = pParticle;
        m_fBiggestParticleSize = m_pBiggestParticle->GetMaxDistanceToCenter();
    }
}

void CSimCell_2D::RemoveParticle(CParticle2D *pParticleToRemove)
{
    int nParticles = GetParticleCount();
    if (nParticles < 1)
        return; // TODO! That is strange, place assert here
    CParticle2D *pTailParticle = m_Particles[nParticles-1];
    m_Particles[pParticleToRemove->m_iIndexInCell] = pTailParticle;
    pTailParticle->m_iIndexInCell = pParticleToRemove->m_iIndexInCell;
    pParticleToRemove->SetCell(NULL);
    m_Particles.resize(nParticles-1);
    if (pParticleToRemove == m_pBiggestParticle)
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
                common::geometry2d::SVector2D force = F*pContact->NormalForceDirection;
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

void CSimCell_2D::CommitNextState(cParticles2D &ParticlesOutOfCells)
{
    for (int i = 0; i < GetParticleCount(); i++)
    {
        CParticle2D *pParticle = GetParticle(i);
        pParticle->CopyNextToCurrentState();
        if (!IsInside(pParticle->m_State[CURRENT_STATE].m_Position))
            ParticlesOutOfCells.push_back(pParticle);
    }
}

