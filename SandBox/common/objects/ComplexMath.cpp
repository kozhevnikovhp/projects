/* ComplexMath.cpp - header file for complex number mathematics

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Feb 2009 - initial creation
	16 May 2009 - sqrt method(s)
*/

#include "stdafx.h"
#include <math.h>
#include "ComplexMath.h"
#include "Calculations.h"

LOGICAL operator == (const SComplex &c1, const SComplex &c2)
{ 
	return (fabs(c1.m_Re - c2.m_Re) < g_MachineEpsilon &&
		fabs(c1.m_Im - c2.m_Im) < g_MachineEpsilon);
}

LOGICAL operator != (const SComplex &c1, const SComplex &c2)
{ 
	return (fabs(c1.m_Re - c2.m_Re) > g_MachineEpsilon ||
		fabs(c1.m_Im - c2.m_Im) > g_MachineEpsilon);
}

SComplex operator + (const SComplex &c1, const SComplex &c2)
{
	SComplex ret;
	ret.m_Re = c1.m_Re + c2.m_Re;
	ret.m_Im = c1.m_Im + c2.m_Im;
	return ret;
}

SComplex operator + (const double &c1, const SComplex &c2)
{
	SComplex ret;
	ret.m_Re = c1 + c2.m_Re;
	ret.m_Im = c2.m_Im;
	return ret;
}

SComplex operator + (const SComplex &c1, const double &c2)
{
	SComplex ret;
	ret.m_Re = c1.m_Re + c2;
	ret.m_Im = c1.m_Im;
	return ret;
}

SComplex operator - (const SComplex &c1, const SComplex &c2)
{
	SComplex ret;
	ret.m_Re = c1.m_Re - c2.m_Re;
	ret.m_Im = c1.m_Im - c2.m_Im;
	return ret;
}

SComplex operator - (const double &c1, const SComplex &c2)
{
	SComplex ret;
	ret.m_Re = c1 - c2.m_Re;
	ret.m_Im = -c2.m_Im;
	return ret;
}

SComplex operator - (const SComplex &c1, const double &c2)
{
	SComplex ret;
	ret.m_Re = c1.m_Re - c2;
	ret.m_Im = c1.m_Im;
	return ret;
}

SComplex operator * (const SComplex &c1, const SComplex &c2)
{
	SComplex ret;
	ret.m_Re = c1.m_Re*c2.m_Re - c1.m_Im*c2.m_Im;
	ret.m_Im = c1.m_Re*c2.m_Im + c1.m_Im*c2.m_Re;
	return ret;
}

SComplex operator * (const double &c1, const SComplex &c2)
{
	SComplex ret;
	ret.m_Re = c1 * c2.m_Re;
	ret.m_Im = c1 * c2.m_Im;
	return ret;
}

SComplex operator * (const SComplex &c1, const double &c2)
{
	SComplex ret;
	ret.m_Re = c2 * c1.m_Re;
	ret.m_Im = c2 * c1.m_Im;
	return ret;
}

SComplex operator / (const SComplex &c1, const SComplex &c2)
{
	SComplex ret;
	return ret;
}

SComplex operator / (const SComplex &c1, const double &c2)
{
	SComplex ret;
	ret.m_Re = c1.m_Re/c2;
	ret.m_Im = c1.m_Im/c2;
	return ret;
}

void SComplex::operator += (const SComplex &c)
{
	m_Re += c.m_Re;
	m_Im += c.m_Im;
}

void SComplex::operator += (const double &c)
{
	m_Re += c;
}

void SComplex::operator -= (const SComplex &c)
{
	m_Re -= c.m_Re;
	m_Im -= c.m_Im;
}

void SComplex::operator -= (const double &c)
{
	m_Re -= c;
}

void SComplex::operator *= (const SComplex &c)
{
	m_Re = m_Re*c.m_Re - m_Im*c.m_Im;
	m_Im = m_Re*c.m_Im + m_Im*c.m_Re;
}

void SComplex::operator *= (const double &c)
{
	m_Re *= c;
	m_Im *= c;
}

SComplex SComplex::operator-() const
{
	SComplex ret;
	ret.m_Re = -m_Re;
	ret.m_Im = -m_Im;
	return ret;
}

//inline
double SComplex::cabs()
{
	return ::sqrt(cabs2());
}

//inline
LOGICAL SComplex::IsZero()
{
	return (fabs(m_Re) < g_MachineEpsilon && fabs(m_Im) < g_MachineEpsilon);
}

SComplex SComplex::csqrt()
{
	SComplex ret;
	double r = cabs();
	if (r < g_MachineEpsilon)
	{
		ret.Zero();
		return ret;
	}

	double ro = sqrt(r); //module
	double half_cosine = m_Re/(2*r);
	ret.m_Re = sqrt(0.5+half_cosine)*ro;
	ret.m_Im = sqrt(0.5-half_cosine)*ro;
	return ret;
}

SComplex cexp(double fi)
{
	SComplex ret;
	ret.m_Re = cos(fi);
	ret.m_Im = sin(fi);
	return ret;
}

SComplex SComplex::cexp()
{
	SComplex ret;
	ret.m_Re = 1; // !!! Implement exponent!!!
	ret.m_Im = 1;
	return ret;
}



/*************************************************************************
������� �������  ��������  ���������  �  ��������  �������,  ��������  LU-
�����������.

�������� ������ ������� �������� ���������,  �������  �������  ������  LU-
�����������. � ������, ����  �  ��������  �������  ����������  �������  ��
����, ������������ ��������  False,  ������������,  ���  �������  ��������
�����������.

�������� ������ ������ ������� ��������� � ���������� ��������.

������� ���������:
    A   -   LU-���������� ������� ������� � �����������  �����  (���������
            ������ ������������ RMatrixLU).
    Pivots- �������  ������������  �����  (���������  ������  ������������
            RMatrixLU).
    B   -   ������ ����� �������. ������ � ���������� ��������� [0..N-1]
    N   -   ����������� �������.

�������� ���������:
    X   -   ������� �������. ������ � ���������� ��������� [0..N-1]

���������:
    True, ���� ������� �� ��������� (��, ��������, ������ � �����������).
    False, ���� ������� ���������. � ����� ������ X �� �������� �������.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
LOGICAL cmatrixlusolve(const CMatrixOfComplex &a,
     const CVectorOfInt &pivots,
     CVectorOfComplex &b,
     int n,
     CVectorOfComplex &x)
{
    LOGICAL result;
    CVectorOfComplex y;
    int i;
    SComplex v;
    int i_;

    y.SetSize(n);
    x.SetSize(n);
    result = LOGICAL_TRUE;
    for (i = 0; i < n; i++)
    {
		SComplex aaa = a(i,i);
        if (aaa.IsZero() == 0.)
            return LOGICAL_FALSE;
    }
    
    //
    // pivots
    //
    for (i = 0; i < n; i++)
    {
        if (pivots(i) != i)
        {
            v = b(i);
            b(i) = b(pivots(i));
            b(pivots(i)) = v;
        }
    }
    
    //
    // Ly = b
    //
    y(0) = b(0);
    for (i = 1; i < n; i++)
    {
        v = 0.0;
        for(i_=0; i_< i; i_++)
        {
            v += a(i,i_)*y(i_);
        }
        y(i) = b(i)-v;
    }
    
    //
    // Ux = y
    //
    x(n-1) = y(n-1)/a(n-1,n-1);
    for (i = n-2; i >= 0; i--)
    {
        v = 0.0;
        for(i_=i+1; i_<=n-1;i_++)
        {
            v += a(i,i_)*x(i_);
        }
        x(i) = (y(i)-v)/a(i,i);
    }
    return result;
}


/*************************************************************************
������� �������  ��������  ���������

�������� ������ ������� �������� ��������� � �������������� LU-����������.
�������� ������ ������ ������� ��������� � ���������� ��������.

������� ���������:
    A   -   ������� �������.
            ������ � ���������� ��������� [0..N-1, 0..N-1].
    B   -   ������ �����.
            ������ � ���������� ��������� [0..N-1].
    N   -   ����������� �������.

�������� ���������:
    X   -   ������� �������. ������ � ���������� ��������� [0..N-1]

���������:
    True, ���� ������� �� ��������� (��, ��������, ������ � �����������).
    False, ���� ������� ���������. � ����� ������ X �� �������� �������.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
LOGICAL cmatrixsolve(CMatrixOfComplex &a,
     CVectorOfComplex &b,
     int n,
     CVectorOfComplex &x)
{
    LOGICAL result;
    CVectorOfInt pivots;

    cmatrixlu(a, n, n, pivots);
    result = cmatrixlusolve(a, pivots, b, n, x);
    return result;
}

/*************************************************************************
LU-���������� ����������� ������� ������ ���� ������� M x N

������������ ��������� LU-���������� ������������� ������� ������  ����  �
��������� ������� �������� �������� (� �������������� �����).

������� ���������:
    A       -   ������� A. ��������� ���������: [0..M-1, 0..N-1]
    M       -   ����� ����� � ������� A
    N       -   ����� �������� � ������� A

�������� ���������:
    A       -   ������� L � U � ���������� ����� (��. ����).
                ��������� ���������: [0..M-1, 0..N-1]
    Pivots  -   ������� ������������ � ���������� ����� (��. ����).
                ��������� ���������: [0..Min(M-1,N-1)]

������� A ��������������, ��� A = P * L * U, ��� P - ������� ������������,
������� L - ���������������� (��� ��������������������, ���� M>N) �������,
U - ����������������� (��� ���������������������, ���� M<N) �������.

���������� ���������� ����� �������� �� ������� ��� M=4, N=3:

                   (  1          )    ( U11 U12 U13  )
A = P1 * P2 * P3 * ( L21  1      )  * (     U22 U23  )
                   ( L31 L32  1  )    (         U33  )
                   ( L41 L42 L43 )

����� ������� L  �����  ������  M  x  Min(M,N),  �������  U  �����  ������
Min(M,N) x N, �������  P(i)  ����������  �����  ������������  �  ���������
������� �������� M x M ����� � �������� I � Pivots[I]

����������� ������ ��������� �������� ������ Pivots  �  ��������� �������,
����������  �������  A,  �  �����������  � ���������� ����� ������� L � U
(������ �������� ��� M=4, N=3):

 ( U11 U12 U13 )
 ( L21 U22 U23 )
 ( L31 L32 U33 )
 ( L41 L42 L43 )

��� �����, ��������� ��������� ������� L  ��  �����������.
���� N>M, �� �������������� �������� ������� ������ � ������������
���������.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     June 30, 1992
*************************************************************************/
void cmatrixlu(CMatrixOfComplex &a,
     int m,
     int n,
     CVectorOfInt &pivots)
{
    int i;
    int j;
    int jp;
    CVectorOfComplex t1;
    SComplex s;
    int i_;

    pivots.SetSize(min(m-1, n-1));
    t1.SetSize(max(m-1, n-1));
    
    //
    // Quick return if possible
    //
    if( m==0 || n==0 )
        return;
    for (j = 0; j < min(m, n); j++)
    {
        // Find pivot and test for singularity.
        //
        jp = j;
        for (i = j+1; i < m; i++)
        {
            if (cabs(a(i,j)) > cabs(a(jp,j)))
            {
                jp = i;
            }
        }
        pivots(j) = jp;
        if (a(jp,j) != 0)
        {
            //Apply the interchange to rows
            //
            if (jp != j)
            {
                for (i_=0; i_ < n; i_++)
                {
                    t1(i_) = a(j,i_);
                }
                for (i_=0; i_ < n; i_++)
                {
                    a(j,i_) = a(jp,i_);
                }
                for (i_=0; i_ < n; i_++)
                {
                    a(jp,i_) = t1(i_);
                }
            }
            
            //
            //Compute elements J+1:M of J-th column.
            //
            if (j<m)
            {
                jp = j+1;
                s = 1/a(j,j);
                for (i_=jp; i_ < m;i_++)
                {
                    a(i_,j) = s*a(i_,j);
                }
            }
        }
        if (j<min(m, n)-1)
        {
            //
            //Update trailing submatrix.
            //
            jp = j+1;
            for (i = j+1; i < m; i++)
            {
                s = a(i,j);
                for (i_=jp; i_ < n;i_++)
                {
                    a(i,i_) = a(i,i_) - s*a(j,i_);
                }
            }
        }
    }
}



