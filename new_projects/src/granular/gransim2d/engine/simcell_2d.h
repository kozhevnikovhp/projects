#ifndef SIMCELL_2D_H
#define SIMCELL_2D_H

#include "particle2d.h"
#include <stdlib.h>

// Forward declarations
class cGranSim2DDesign;

class CSimCell_2D
{
//Constuctors/destructors
public:
	CSimCell_2D(cGranSim2DDesign *pDesign, double left, double right, double bottom, double top);
    virtual ~CSimCell_2D();

// Public methods
public:
    int GetParticleCount() { return m_Particles.size(); }
    CParticle2D *GetParticle(int nParticle) { return m_Particles[nParticle]; }
    common::geometry2d::SExtents2D *GetBox() { return &m_Box; }
    void NewParticle(CParticle2D *pParticle);
    void RemoveParticle(CParticle2D *pParticle);
    void RecalcForcesForAllParticles(CSimCell_2D *pCell, StateEnum state);
    void ZeroForcesForAllParticles(StateEnum state);
    void CommitNextState(cParticles2D &ParticlesOutOfCells);
    bool IsInside(common::geometry2d::SVector2D &position) { return m_Box.IsInside(position); }
    bool IsInside(double x, double y) { return m_Box.IsInside(x, y); }

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
    cParticles2D m_Particles;
    common::geometry2d::SExtents2D m_Box;
	cGranSim2DDesign *m_pDesign;
    CParticle2D *m_pBiggestParticle;
    double m_fBiggestParticleSize;
};

template<class T> class CMatrixOf
{
// Constructors/destructors
public:
        CMatrixOf()
        {
                m_pData = NULL;
                m_nRows = m_nCols = 0;
                m_nAllocated = 0;
        }
        virtual ~CMatrixOf()
        {
                if (m_pData)
                        free(m_pData);
        }

// Public methods
        int GetRows() const { return m_nRows; }
        int GetCols() const { return m_nCols; }
        bool SetSizes(int nCols, int nRows)
        {
                m_nRows = nRows;
                m_nCols = nCols;
                return ReallocData();
        }

        const T& operator() (int iCol, int iRow) const { return m_pData[iCol*m_nRows+iRow]; }
        T& operator() (int iCol, int iRow) { return m_pData[iCol*m_nRows+iRow]; }

// Protected methods
protected:
        bool ReallocData()
        {
                if (m_nRows*m_nCols <= m_nAllocated)
                        return true; // Allocated space is sufficient, do nothing
                bool bSuccess = true;
                T *pData = (T *)realloc(m_pData, sizeof(T)*m_nRows*m_nCols);
                if (pData)
                {
                        m_pData = pData;
                        m_nAllocated = m_nRows*m_nCols;
                }
                else
                {
                        // process errors
                        bSuccess = false;
                }
                return bSuccess;
        }

// Protected members
protected:
        int m_nAllocated;
        int m_nRows, m_nCols;
        T *m_pData;
};

typedef CMatrixOf<CSimCell_2D *> CSimCells2D;

#endif // SIMCELL_2D_H
