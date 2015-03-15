/* ComplexMath.h - header file for complex number mathematics

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Feb 2009 - initial creation
	16 May 2009 - sqrt method(s)
*/

#ifndef __COMPLEX_MATH_H_INCLUDED__
#define __COMPLEX_MATH_H_INCLUDED__


#include "Calculations.h"

typedef struct SComplex
{
public:
	SComplex() { m_Re = m_Im = 0; }
	SComplex(const SComplex &c) { m_Re = c.m_Re; m_Im = c.m_Im; }
	SComplex(double Re) { Set(Re, 0.);  }
	SComplex(double Re, double Im) { Set(Re, Im);  }
	void Set(double Re, double Im) { m_Re = Re; m_Im = Im; }
	void Set(const SComplex &c) { m_Re = c.m_Re; m_Im = c.m_Im; }
	void Zero() { Set(0, 0); }
	inline LOGICAL IsZero();
    SComplex& operator = (const SComplex &c) { m_Re  = c.m_Re; m_Im  = c.m_Im; return *this; };
    SComplex& operator = (const double &c) { m_Re  = c; m_Im  = 0; return *this; };
	friend LOGICAL operator == (const SComplex &c1, const SComplex &c2);
	friend LOGICAL operator != (const SComplex &c1, const SComplex &c2);
	friend SComplex operator + (const SComplex &c1, const SComplex &c2);
	friend SComplex operator + (const double &c1, const SComplex &c2);
	friend SComplex operator + (const SComplex &c1, const double &c2);
	friend SComplex operator - (const SComplex &c1, const SComplex &c2);
	friend SComplex operator - (const double &c1, const SComplex &c2);
	friend SComplex operator - (const SComplex &c1, const double &c2);
	friend SComplex operator * (const SComplex &c1, const SComplex &c2);
	friend SComplex operator * (const double &c1, const SComplex &c2);
	friend SComplex operator * (const SComplex &c1, const double &c2);
	friend SComplex operator / (const SComplex &c1, const SComplex &c2);
	friend SComplex operator / (const SComplex &c1, const double &c2);
	void operator += (const SComplex &c);
	void operator += (const double &c);
	void operator -= (const SComplex &c);
	void operator -= (const double &c);
	void operator *= (const SComplex &c);
	void operator *= (const double &c);

	SComplex operator-() const;

	double Re() { return m_Re; }
	double Im() { return m_Im; }
	inline double cabs();
	friend double cabs(SComplex &c) { return c.cabs(); }
	double cabs2() { return m_Re*m_Re + m_Im*m_Im; }
	friend double cabs2(SComplex &c) { return c.cabs2(); }
	SComplex csqrt();
	friend SComplex csqrt(SComplex &c) { return c.csqrt(); }
	friend SComplex cexp(double fi);
	SComplex cexp();
	friend SComplex cexp(SComplex &c) { return c.cexp(); }

protected:
	double m_Re, m_Im;
} SComplex;

typedef CVectorOf<SComplex> CVectorOfComplex;
typedef CMatrixOf<SComplex> CMatrixOfComplex;

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
     CVectorOfComplex &x);


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
     CVectorOfComplex &x);

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
     CVectorOfInt &pivots);



#endif //__COMPLEX_MATH_H_INCLUDED__