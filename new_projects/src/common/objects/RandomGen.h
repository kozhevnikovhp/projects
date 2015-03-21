/* RandomGen.h - header file for random number generation stuff

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	13 Dec 2008 - initial creation
*/


#ifndef __RANDOM_GEN_H_INCLUDED__
#define __RANDOM_GEN_H_INCLUDED__

class CRandomGen
{
//Constuctors/destructors
public:
	CRandomGen();
	virtual ~CRandomGen();

// Public methods
public:
	void Init();
	int GenInteger(int iFrom, int iTo);
	double GenDouble(double fFrom, double fTo);
	double Gen01();
	double GenGauss(double fFrom, double fTo, double fMean, double fSigma);

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

// Private methods
private:

// Private overridables
private:

// Private members
private:
};

#endif // __RANDOM_GEN_H_INCLUDED__

