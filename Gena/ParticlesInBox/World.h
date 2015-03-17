/* World.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation

*/

#ifndef __WORLD_H_INCLUDED__
#define __WORLD_H_INCLUDED__

#include <math.h>

// Forward declarations
class CWorldRenderer;

const int nMaxParticles = 65536;		// Максимальное число частиц

struct SParticle
{
	double x,y;		// position
	double vx, vy;	// velocity
	int color;		// 0 частица родилась слева, 1 - справа
};

struct SGeometry
{
	double Xleft, ParticleXleft;			// Левая граница (в метрах)
	double Xcenter, ParticleWLeft, ParticleWRight;			// Перегородка (в метрах)
	double WThickness;		// Wall thickness
	double WLeft, WRight;
	double YGapTop;			// Верхний край перегородки
	double YGapBottom;		// Нижний край перегородки
	double ParticleHoleTop, ParticleHoleBottom;
	double HoleSize, HolePosition;
	double Xright, ParticleXright;			// Правая граница (в метрах)
	double Ymax, ParticleYmax;			// Верхняя граница (в метрах)
	double Ymin, ParticleYmin;			// Нижняя граница (в метрах)
	double Rparticle;		// Радиус одной частицы  (в метрах)
};


class CWorld
{
// Construction/destruction
public:
	CWorld();
	virtual ~CWorld();

// Public overridable methods
public:

// Protected overridable methods
protected:

// Public methods
public:
	BOOL InitialDistribution();
	void OnIdle();
	int GetParticleCount() { return nParticles; }
	SParticle *GetParticle(int iParticle) { return particle + iParticle; }
	SGeometry *GetGeometry() { return &Geometry; }
	void SetRenderer(CWorldRenderer *pRenderer) { pRenderer_ = pRenderer; }

// Public members
public:
	double			Time;				// Текущее время (в секундах)
	double			DeltaTime;			// Шаг по времени (в секундах)
	unsigned long	nRight;				// Количество частиц в правой части
	unsigned long	nLeft;				// Количество частиц в левой части
	double			VaverageR;			// Средняя скорость частиц в правой части
	double			VaverageL;			// Средняя скорость частиц в левой части
	int nParticles;			// Реальное число частиц


// Protected methods
protected:
	int LoadInt(char *pszValueName, int nDefaultValue);
	void SaveInt(char *pszValueName, int nValue);
	double LoadDouble(char *pszValueName, double fDefaultValue);
	void SaveDouble(char *pszValueName, double fValue);
	double random(double from, double to);
	double Distance2(SParticle &p1, SParticle &p2);
	double Distance(SParticle &p1, SParticle &p2) { return sqrt(Distance2(p1, p2)); }
	BOOL InContact(SParticle &p1, SParticle &p2) { return (Distance(p1, p2) < 2*Geometry.Rparticle); }
	BOOL Approaching(SParticle &p1, SParticle &p2);
	double GetVelocity(SParticle &p) { return sqrt(p.vx*p.vx + p.vy*p.vy); }
	void Collision(SParticle &p1, SParticle &p2);
	void RandomVelocity(SParticle &p);
	void CalcTimeStep();
	void RecalcStat();
	void MoveParticle(SParticle &p, double dt);
	void BounceX(SParticle &p) { p.vx = -p.vx; }
	void BounceY(SParticle &p) { p.vy = -p.vy; }
	void WriteStat();
	BOOL OneTimeStep();
	BOOL CorrectParticleByGeometry(SParticle &p);


// Private methods
private:

// Protected members
protected:
	SParticle particle[nMaxParticles];		// Сами частицы
	SGeometry Geometry;
	double Vinit;			// Начальная скорость всех частиц (в метрах/сек)
	// Collisions
	double DeltaVtop;		// Добавок скорости, который получает частица, ударяясь о верхнюю стенку
	double DeltaVbottom;	// Добавок скорости, который получает частица, ударяясь о нижнюю стенку
	double DeltaVside;		// Добавок скорости, который получает частица, ударяясь о боковую стенку
	double loss;			// Коэффициент диссипации кинетической энергии

	double g;				// Ускорение свободного падения (в м/с**2)

	CString FileName;		// Имя файла с записанной статистикой

	// Вычисляемые величины (переменные)
	unsigned long	nTimeSteps;			// Количество пройденных шагов по времени

	double			LastTimeWritten;	// Время, которое было записано в файл последний раз
	int				nFluxFromLeft;
	int				nFluxFromRight;
	CWorldRenderer *pRenderer_;

// Private members
private:
};


#endif //__WORLD_H_INCLUDED__

