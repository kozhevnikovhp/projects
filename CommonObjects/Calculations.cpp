/* Calculations.h - implementation file for various calculations stuff

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	13 Dec 2008 - initial creation
*/

#include "stdafx.h"
#include "Calculations.h"
#include <math.h>

void SnapToGrid(double &fValue, double fGridSize)
{
	int n = round(fValue/fGridSize);
	fValue = n*fGridSize;
}

double sqr(double x)
{
	return x*x;
}

int sign(double x)
{
    if (x>0) 
		return 1;
	else
		if (x<0)
			return -1;
    return 0;
}

int round(double x)
{
	double f = floor(x);
	double c = ceil(x);
	if (fabs(x - f) < fabs(x - c))
		return (int)f;
	return (int)c;
}

double Gauss(double x, double mean, double sigma)
{
	double dx = (x - mean)/sigma;
	return exp(-0.5*dx*dx)/(sigma*SQRT_2PI);
}

double IntegralSimpson(PFN_DOUBLE_FROM_DOUBLE pfnTargetFunction, const void *pContext, const double a, const double b, const double epsilon)
{
    double h;
    double s;
    double s1;
    double s2;
    double s3;
    double x;

    s2 = 1;
    h = b - a;
    s = pfnTargetFunction(a, pContext) + pfnTargetFunction(b, pContext);
    do
    {
        s3 = s2;
        h = h/2;
        s1 = 0;
        x = a + h;
        do
        {
            s1 = s1 + 2*(pfnTargetFunction(x, pContext));
            x = x + 2*h;
        }
        while (x < b);
        s = s + s1;
        s2 = (s+s1)*h/3;
        x = fabs(s3-s2)/15;
    }
    while (x > epsilon);
    return s2;
}

