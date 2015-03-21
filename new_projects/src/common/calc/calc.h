/* Calculations.h - header file for various calculations stuff

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
        13 Dec 2008 - initial creation
*/


#ifndef __CALCULATIONS_H_INCLUDED__
#define __CALCULATIONS_H_INCLUDED__

#include "portable/types.h"

namespace common {

namespace calc {

const double PI = 3.1415926535897932384626433832795;
const double PI2 = 2*PI;
const double SQRT_2PI = 2.506628274631000502415765284811;

double DEGREES_2_RADIANS(double x);
double RADIANS_2_DEGREES(double x);

const double VERY_BIG_INTEGER = 0x1FFFFFFF;
const double VERY_BIG_DOUBLE = +1.e+30;

const double g_MachineEpsilon = 5E-16;
const double g_MaxRealNumber = 1E+300;
const double g_MinRealNumber = 1E-300;

void SnapToGrid(double &fValue, double fGridSize);

double sqr(double x);
int sign(double x);
int round(double x);

double Gauss(double x, double mean, double sigma);

typedef double  (* PFN_DOUBLE_FROM_DOUBLE)(const double x, const void *pContext);
double IntegralSimpson(PFN_DOUBLE_FROM_DOUBLE pfnTargetFunction, const void *pContext, const double a, const double b, const double epsilon);

} // namespace calc

} // namespace common

#endif //__CALCULATIONS_H_INCLUDED__
