/* RandomGen.cpp - implementation file for random number generation stuff

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	13 Dec 2008 - initial creation
*/

#include "stdafx.h"
#include "RandomGen.h"
#include "Calculations.h"
#include <time.h> // for time() method. TODO: remove


///////////////////////////////////////////////////////////////////
// CRandomGen

CRandomGen::CRandomGen()
{
	Init();
}

//virtual
CRandomGen::~CRandomGen()
{
}

void CRandomGen::Init()
{
	srand(time(NULL));
}

int CRandomGen::GenInteger(int iFrom, int iTo)
{
	int iRange = abs(iTo - iFrom) + 1; // "+1" because of "to" value is included
	int rnd = rand() % iRange;
	return (rnd + min(iFrom, iTo));
}

double CRandomGen::GenDouble(double fFrom, double fTo)
{
	return (fTo - fFrom)*rand()/RAND_MAX + fFrom;
}

double CRandomGen::Gen01()
{
	return ((double)rand())/RAND_MAX;
}

double CRandomGen::GenGauss(double fFrom, double fTo, double fMean, double fSigma)
{
	double Xmin = max(min(fFrom, fTo), fMean - 5.*fSigma);
	double Xmax = min(max(fFrom, fTo), fMean + 5.*fSigma);
	double x, y, Ymax = Gauss(Xmin, fMean, fSigma);
	if ((fMean > Xmin) && (fMean < Xmax))
	{
		y = Gauss(fMean, fMean, fSigma);
		if (y > Ymax)
			Ymax = y;
	}
	y = Gauss(Xmax, fMean, fSigma);
	if (y > Ymax)
		Ymax = y;

	do
	{
		x = GenDouble(Xmin, Xmax);
		y = GenDouble(0, Ymax);
	} while (y > Gauss(x, fMean, fSigma));
	
	return x;
}
