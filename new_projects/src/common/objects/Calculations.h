/* Calculations.h - header file for various calculations stuff

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	13 Dec 2008 - initial creation
*/


#ifndef __CALCULATIONS_H_INCLUDED__
#define __CALCULATIONS_H_INCLUDED__

#include "Portable.h"

const double PI = 3.1415926535897932384626433832795;
const double PI2 = 2*PI;
const double SQRT_2PI = 2.506628274631000502415765284811;

#define DEGREES_2_RADIANS(x) (PI/180.*(x))
#define RADIANS_2_DEGREES(x) (180./PI*(x))

const double VERY_BIG_INTEGER = 0x1FFFFFFF;
const double VERY_BIG_DOUBLE = +1.e+30;

const double g_MachineEpsilon = 5E-16;
const double g_MaxRealNumber = 1E+300;
const double g_MinRealNumber = 1E-300;

static int CompareAscending(const void *elem1, const void *elem2)
{
	double *p1 = (double *)elem1;
	double *p2 = (double *)elem2;
	if (*p1 > *p2)
		return +1;
	return -1;
}

static int CompareDescending(const void *elem1, const void *elem2)
{
	return CompareAscending(elem2, elem1);
}

template<class T> class CVectorOf
{
// Constructors/destructors
public:
	CVectorOf()
	{
		m_pData = NULL;
		m_nSize = 0;
		m_nAllocated = 0;
	}
	virtual ~CVectorOf()
	{
		if (m_pData)
			free(m_pData);
	}

// Public methods
	int GetSize() { return m_nSize; }
	LOGICAL SetSize(int nSize)
	{
		m_nSize = nSize;
		return ReallocData();
	}
    T& operator()(int iIndex) { return m_pData[iIndex]; }
    const T& operator()(int iIndex) const  { return m_pData[iIndex]; }
	T operator[] (int iIndex)  { return m_pData[iIndex]; }
	const T &operator[] (int iIndex) const  { return m_pData[iIndex]; }
	void AddTail(T NewElement)
	{
		ReallocData();
		m_pData[m_nSize++] = NewElement;
	}
	void CutTail()
	{
		if (m_nSize)
			--m_nSize;
	}
	void CopyTailTo(int iIndex)
	{
		if (m_nSize)
			memcpy(m_pData+iIndex, m_pData+m_nSize-1, sizeof(T));
	}
	void RemoveAll() { m_nSize = 0; }

	void SortAscending()
	{
		if (m_nSize)
			qsort(m_pData, m_nSize, sizeof(T), CompareAscending);
	}

	void SortDescending()
	{
		if (m_nSize)
			qsort(m_pData, m_nSize, sizeof(T), CompareDescending);
	}

// Protected methods
protected:
	LOGICAL ReallocData()
	{
		if ((m_nSize+1) <= m_nAllocated)
			return LOGICAL_TRUE; // Allocated space is sufficient, do nothing
		LOGICAL bSuccess = LOGICAL_TRUE;
		int nAllocated = m_nAllocated;
		if (nAllocated)
			nAllocated *= 2;
		else
			nAllocated = 1;
		T *pData = (T *)realloc(m_pData, sizeof(T)*nAllocated);
		if (pData)
		{
			m_pData = pData;
			m_nAllocated = nAllocated;
		}
		else
		{
			// process errors
			bSuccess = LOGICAL_FALSE;
		}
		return bSuccess;
	}

// Protected members
protected:
	int m_nAllocated;
	int m_nSize;
	T *m_pData;
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
	LOGICAL SetSizes(int nCols, int nRows)
	{
		m_nRows = nRows;
		m_nCols = nCols;
		return ReallocData();
	}
 
	const T& operator() (int iCol, int iRow) const { return m_pData[iCol*m_nRows+iRow]; }
	T& operator() (int iCol, int iRow) { return m_pData[iCol*m_nRows+iRow]; }
 
// Protected methods
protected:
	LOGICAL ReallocData()
	{
		if (m_nRows*m_nCols <= m_nAllocated)
			return LOGICAL_TRUE; // Allocated space is sufficient, do nothing
		LOGICAL bSuccess = LOGICAL_TRUE;
		T *pData = (T *)realloc(m_pData, sizeof(T)*m_nRows*m_nCols);
		if (pData)
		{
			m_pData = pData;
			m_nAllocated = m_nRows*m_nCols;
		}
		else
		{
			// process errors
			bSuccess = LOGICAL_FALSE;
		}
		return bSuccess;
	}

// Protected members
protected:
	int m_nAllocated;
	int m_nRows, m_nCols;
	T *m_pData;
};

typedef CVectorOf<double> CVectorOfDouble;
typedef CVectorOf<int> CVectorOfInt;
typedef CVectorOf<unsigned int> CVectorOfUInt;
typedef CMatrixOf<double> CMatrixOfDouble;
typedef CMatrixOf<int> CMatrixOfInt;
typedef CMatrixOf<unsigned int> CMatrixOfUInt;

void SnapToGrid(double &fValue, double fGridSize);

double sqr(double x);
int sign(double x);
int round(double x);

double Gauss(double x, double mean, double sigma);

typedef double  (* PFN_DOUBLE_FROM_DOUBLE)(const double x, const void *pContext);
double IntegralSimpson(PFN_DOUBLE_FROM_DOUBLE pfnTargetFunction, const void *pContext, const double a, const double b, const double epsilon);


#endif //__CALCULATIONS_H_INCLUDED__
