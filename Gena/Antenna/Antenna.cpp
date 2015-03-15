// Antenna.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "ComplexMath.h"
#include "Calculations.h"
#include "Bessel.h"

const int ARRAY_SIZE = 1024;
int main(int argc, char *argv[])
{
	const SComplex I(0., 1); // sqrt(-1)
	const double c = 2.99E+08;
	int N = 0, n = 0, m = 0, M = 0, j;
	SComplex Z;
	double f = 0, delta = 0, R = 0, h = 0;
	double H[ARRAY_SIZE];
	double mu[ARRAY_SIZE];
	double K[ARRAY_SIZE];
	SComplex F[ARRAY_SIZE];
	CMatrixOfComplex B;
	CVectorOfComplex FreeVector, D;

	if (argc < 3)
	{
		printf("Usage: antenna.exe <file_name>, <file_name>\n");
		return 1;
	}

	char *pszFileName = argv[1];
	FILE *fd = fopen(pszFileName, "r");
	if (!fd)
	{
		printf("Cannot open file %s\n", pszFileName);
		return 1;
	}

	double Hi = 0;
	while (1)
	{
		int nScanfed = fscanf(fd, "%lf\n", &Hi);
		if (nScanfed == EOF || nScanfed == 0)
			break;
		H[N] = Hi;
		printf("H[%d] = %lf\n", N+1, H[N]);
		++N;
	}
	printf("%d numbers read from file %s\n", N, pszFileName);
	if (!N)
	{
		printf("No data, execution canceled\n");
		return 1;
	}
	--N;

	pszFileName = argv[2];
	fd = fopen(pszFileName, "r");
	if (!fd)
	{
		printf("Cannot open file %s\n\n", pszFileName);
		return 1;
	}

	double MUi = 0;
	while (1)
	{
		int nScanfed = fscanf(fd, "%lf\n", &MUi);
		if (nScanfed == EOF || nScanfed == 0)
			break;
		mu[m] = MUi;
		printf("mu[%d] = %lf\n", m+1, mu[m]);
		++m;
	}
	printf("%d numbers read from file %s\n\n", m, pszFileName);
	
	// delta
	printf("delta = ");
	scanf("%lf", &delta);
	printf("\tOK, delta = %lf m\n", delta);
	
	// R
	printf("R = ");
	scanf("%lf", &R);
	printf("\tOK, R = %lf m\n", R);
	
	// h
	printf("h = ");
	scanf("%lf", &h);
	printf("\tOK, h = %lf m\n", h);
	
	// f
	printf("f = ");
	scanf("%lf", &f);
	printf("\tOK, f = %lf Hz\n", f);

	// M
	printf("M = ");
	scanf("%d", &M);
	printf("\tOK, M = %d\n", m);

	// Z
	double ReZ, ImZ;
	printf("Re(Z) = ");
	scanf("%lf", &ReZ);
	printf("\tOK, Re(Z) = %lf\n", ReZ);
	printf("Im(Z) = ");
	scanf("%lf", &ImZ);
	printf("\tOK, Im(Z) = %lf\n", ImZ);
	Z.Set(ReZ, ImZ);

	SComplex kappa0 = Z;
	kappa0 *= 2.*PI*f/c;

	SComplex k = 2.*PI*f/c*csqrt(1.-Z*Z);
//	double Jkr = besselj0(k*R);
	double Jkr = 1;//!!! TODO: Implement that
	for (m = 0; m < M; m++)
	{
		K[m] = sqrt(sqr(2*PI*f/c)-sqr(mu[m]/R));
	}

	SComplex Cs = H[0] + H[N]*cexp(I*kappa0*delta*N);
	for (n = 1; n <= (N/2-1); n++)
	{
		Cs += 2*H[2*n]*cexp(I*kappa0*delta*2*n);
	}
	for (n = 1; n <= N/2; n++)
	{
		Cs += 4*H[2*n-1]*cexp(I*kappa0*delta*(2*n-1));
	}

	int L = ceil(h + c/f)/delta;
	for (j = 0; j <= N; j++)
	{
		F[j] = H[j]+2*I*Cs*cexp(I*kappa0*j*delta);
	}

	for (int p = 0; p < M; p++)
	{
		for (m = 0; m < M; m++)
		{
			B(p,m) = 0;
			for (j = L; j <= N; j++)
			{
				B(p,m) += cexp(K[m]-K[p]*j*delta);
			}
		}
		FreeVector(p) = 0;
		for (j = L; j <= N; j++)
		{
			FreeVector(p) += F[j]*cexp(-K[p]*j*delta);
		}
	}

	LOGICAL bSuccess = cmatrixsolve(B, FreeVector, M, D);
	if (!bSuccess)
	{
		printf("ERROR: The equation system is not compatible.\nSolution does not exist.\n");
		return 1;
	}

	double sum = 0;
	for (m = 0; m < M; m++)
	{
		sum += K[m]*cabs2(D[m])/cabs2(Cs);
	}
	double eta = 1./(1+sum*kappa0.Im()/(2*PI*cabs(k)*fabs(k.Re())*sqr(fabs(Jkr))));
	printf("Eta = %6.1f%%\n", eta*100);
	
	return 0;
}
