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
Решение системы  линейных  уравнений  с  матрицей  системы,  заданной  LU-
разложением.

Алгоритм решает систему линейных уравнений,  матрица  которой  задана  LU-
разложением. В случае, если  в  процессе  решения  произойдет  деление  на
ноль, возвращается значение  False,  обозначающее,  что  система  является
вырожденной.

Алгоритм решает только системы уравнений с квадратной матрицей.

Входные параметры:
    A   -   LU-разложение матрицы системы в упакованной  форме  (результат
            работы подпрограммы RMatrixLU).
    Pivots- таблица  перестановок  строк  (результат  работы  подпрограммы
            RMatrixLU).
    B   -   правая часть системы. Массив с нумерацией элементов [0..N-1]
    N   -   размерность системы.

Выходные параметры:
    X   -   решение системы. Массив с нумерацией элементов [0..N-1]

Результат:
    True, если система не вырождена (но, возможно, близка к вырожденной).
    False, если система вырождена. В таком случае X не содержит решение.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
LOGICAL cmatrixlusolve(const CMatrixOfComplex &a,
     const CVectorOfInt &pivots,
     CVectorOfComplex &b,
     int n,
     CVectorOfComplex &x);


/*************************************************************************
Решение системы  линейных  уравнений

Алгоритм решает систему линейных уравнений с использованием LU-разложения.
Алгоритм решает только системы уравнений с квадратной матрицей.

Входные параметры:
    A   -   Матрица системы.
            Массив с нумерацией элементов [0..N-1, 0..N-1].
    B   -   Правая часть.
            Массив с нумерацией элементов [0..N-1].
    N   -   размерность системы.

Выходные параметры:
    X   -   решение системы. Массив с нумерацией элементов [0..N-1]

Результат:
    True, если система не вырождена (но, возможно, близка к вырожденной).
    False, если система вырождена. В таком случае X не содержит решение.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
LOGICAL cmatrixsolve(CMatrixOfComplex &a,
     CVectorOfComplex &b,
     int n,
     CVectorOfComplex &x);

/*************************************************************************
LU-разложение комплексной матрицы общего вида размера M x N

Подпрограмма вычисляет LU-разложение прямоугольной матрицы общего  вида  с
частичным выбором ведущего элемента (с перестановками строк).

Входные параметры:
    A       -   матрица A. Нумерация элементов: [0..M-1, 0..N-1]
    M       -   число строк в матрице A
    N       -   число столбцов в матрице A

Выходные параметры:
    A       -   матрицы L и U в компактной форме (см. ниже).
                Нумерация элементов: [0..M-1, 0..N-1]
    Pivots  -   матрица перестановок в компактной форме (см. ниже).
                Нумерация элементов: [0..Min(M-1,N-1)]

Матрица A представляется, как A = P * L * U, где P - матрица перестановок,
матрица L - нижнетреугольная (или нижнетрапецоидальная, если M>N) матрица,
U - верхнетреугольная (или верхнетрапецоидальная, если M<N) матрица.

Рассмотрим разложение более подробно на примере при M=4, N=3:

                   (  1          )    ( U11 U12 U13  )
A = P1 * P2 * P3 * ( L21  1      )  * (     U22 U23  )
                   ( L31 L32  1  )    (         U33  )
                   ( L41 L42 L43 )

Здесь матрица L  имеет  размер  M  x  Min(M,N),  матрица  U  имеет  размер
Min(M,N) x N, матрица  P(i)  получается  путем  перестановки  в  единичной
матрице размером M x M строк с номерами I и Pivots[I]

Результатом работы алгоритма являются массив Pivots  и  следующая матрица,
замещающая  матрицу  A,  и  сохраняющая  в компактной форме матрицы L и U
(пример приведен для M=4, N=3):

 ( U11 U12 U13 )
 ( L21 U22 U23 )
 ( L31 L32 U33 )
 ( L41 L42 L43 )

Как видно, единичная диагональ матрицы L  не  сохраняется.
Если N>M, то соответственно меняются размеры матриц и расположение
элементов.

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