/* WorldPlaneRenderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation

*/

#include "stdafx.h"
#include "World.h"
#include "WorldRenderer.h"
#include "ParametersDialog.h"

const char *pszUnknownFileName = "<unknown>";
const char *pszSectionName = "Values";

//const double PI = 3.1415926;

///////////////////////////////////////////////////////////////////////////
// CWorld

CWorld::CWorld()
{
	nFluxFromLeft = nFluxFromRight = 0;
	pRenderer_ = NULL;
}

CWorld::~CWorld()
{
}

int CWorld::LoadInt(char *pszValueName, int nDefaultValue)
{
	return AfxGetApp()->GetProfileInt(pszSectionName, pszValueName, nDefaultValue);
}

void CWorld::SaveInt(char *pszValueName, int nValue)
{
	AfxGetApp()->WriteProfileInt(pszSectionName, pszValueName, nValue);
}

double CWorld::LoadDouble(char *pszValueName, double fDefaultValue)
{
	int iDefaultValue = (int)(fDefaultValue*1000);
	int iValue = LoadInt(pszValueName, iDefaultValue);
	return iValue/1000.;
}

void CWorld::SaveDouble(char *pszValueName, double fValue)
{
	AfxGetApp()->WriteProfileInt(pszSectionName, pszValueName, (int)(fValue*1000));
}

// генерирует псевдослучайное число в диапазоне from...to
double CWorld::random(double from, double to)
{
	return (to-from)*rand()/RAND_MAX + from;
}

// Вычисляет квадрат расстояния между двумя частицами (между центрами)
double CWorld::Distance2(SParticle &p1, SParticle &p2)
{
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return (dx*dx+dy*dy);
}

// Определяет, сближаются ли частицы
BOOL CWorld::Approaching(SParticle &p1, SParticle &p2)
{
	double dVx, dVy;	// Скорости сближения по осям
	if (p1.x < p2.x)
		dVx = p1.vx - p2.vx;
	else
		dVx = p2.vx - p1.vx;

	if (p1.y < p2.y)
		dVy = p1.vy - p2.vy;
	else
		dVy = p2.vy - p1.vy;

	if (dVx > 0 && dVy > 0)
		return TRUE;	// Сближаются по обеим осям
	if (dVx < 0 && dVy < 0)
		return FALSE;	// Удаляются по обеим осям
	if (dVx > 0 && dVy < 0)
		return (dVx > dVy);
	return (dVy > dVx);
}

// Рассчитывает столкновение частиц
void CWorld::Collision(SParticle &p1, SParticle &p2)
{
	double factor = sqrt(1.-loss);
	
	// Old collision
/*	double dummy = p2.vx;
	p2.vx = p1.vx*factor;
	p1.vx = dummy*factor;
	dummy = p2.vy;
	p2.vy = p1.vy*factor;
	p1.vy = dummy*factor;*/

	// New collision
	double dx = p1.x-p2.x;
	double dx2 = dx*dx;
	double dy = p1.y-p2.y;
	double dy2 = dy*dy;
	double distance2 = dx2+dy2;
	double sin2 = dy2/distance2;
	double cos2 = dx2/distance2;
	double sincos = dx*dy/distance2;
	double dVx = p1.vx - p2.vx;
	double dVy = p1.vy - p2.vy;
	double V1x = dVx*sin2 - dVy*sincos + p2.vx;
	double V1y = dVy*cos2 - dVx*sincos + p2.vy;
	double V2x = dVx*cos2 + dVy*sincos + p2.vx;
	double V2y = dVy*sin2 + dVx*sincos + p2.vy;
	p1.vx = V1x*factor;
	p1.vy = V1y*factor;
	p2.vx = V2x*factor;
	p2.vy = V2y*factor;
	if (distance2 < 4*Geometry.Rparticle*Geometry.Rparticle)
	{ // overlapped particles
		double distance = sqrt(distance2);
		double NeedToMove = 2*Geometry.Rparticle - distance;
		double cosine = dx/distance;
		double sine = dy/distance;
		if (dy > 0)
		{
			p1.x += NeedToMove*cosine;
			p1.y += NeedToMove*sine;
		}
		else
		{
			p2.x -= NeedToMove*cosine;
			p2.y -= NeedToMove*sine;
		}
	}

/*	double angle = random(-PI/8, PI/8);
	double sine = sin(angle);
	double cosine = cos(angle);
	double vxnew = p1.vx*cosine + p1.vy*sine;
	double vynew = -p1.vx*sine + p1.vy*cosine;
	p1.vx = vxnew;
	p1.vy = vynew;
	vxnew = p2.vx*cosine + p2.vy*sine;
	vynew = -p2.vx*sine + p2.vy*cosine;
	p2.vx = vxnew;
	p2.vy = vynew;*/
}

// Задает случайный вектор скорости (модуль постоянный и равный Vinit)
void CWorld::RandomVelocity(SParticle &p)
{
	double angle = random(0, PI*2);			// направление движения частицы - случайный угол 0...2PI
	p.vx = Vinit * cos(angle);
	p.vy = Vinit * sin(angle);
}

// Вычисляет временнОй шаг исходя из скоростей частиц с таким расчетом, чтобы даже самая быстрая частица
// за этот шаг не проходила расстояние, большее половины своего радиуса (это нужно чтобы избежать
// пролета частиц сквозь границу ящика за один шаг, а также чтобы столкновения частиц между собой не были пропущены).
void CWorld::CalcTimeStep()
{
	double V, Vmax = 0;
	for (int i = 0; i < nParticles; i++)
	{
		V = GetVelocity(particle[i]);
		if (V > Vmax)
			Vmax = V;
	}
	if (Vmax == 0)
		Vmax = Vinit;
	if (Vmax == 0)
		Vmax = 0.1;
	DeltaTime =  (Geometry.Rparticle/8)/Vmax;
}

// Рассчитывает статистику
void CWorld::RecalcStat()
{
	int i;
	nRight = nLeft = 0;
	VaverageR = VaverageL = 0;
	// Сначала просто суммируем модули скоростей, попутно считая частицы в каждой половине
	for (i = 0; i < nParticles; i++)
	{
		if (particle[i].x > Geometry.Xcenter)
		{
			++nRight;
			VaverageR += GetVelocity(particle[i]);
		}
		else
		{
			++nLeft;
			VaverageL += GetVelocity(particle[i]);
		}
	}
	// Теперь делим на число частиц, чтобы найти среднее (не забывая про случай, когда частиц нет)
	if (nRight != 0)
		VaverageR = VaverageR/nRight;
	else
		VaverageR = 0;
	if (nLeft != 0)
		VaverageL = VaverageL/nLeft;
	else
		VaverageL = 0;
}

//  Первоначальное распределение частиц
BOOL CWorld::InitialDistribution()
{
	BOOL bTouching;		// соприкасаются ли частицы
	int i,j;			// переменные циклов
	Time = 0;			// Текущее время
	nTimeSteps = 0;		// Количество пройденных шагов по времени
	
	CParametersDialog dlg;
	dlg.m_nParticles = LoadInt("nParticles", 1000);
	dlg.m_G = LoadDouble("g", 9.8);
	dlg.m_LossRatio = LoadDouble("CollisionLossRatio", 0.01);
	dlg.m_RParticle = LoadDouble("RParticle", 0.01);
	dlg.m_Xleft = LoadDouble("Xleft", -1);
	dlg.m_Xright = LoadDouble("Xright", +1);
	dlg.m_Ybottom = LoadDouble("Ybottom", 0);
	dlg.m_Ytop = LoadDouble("Ytop", +1);
	dlg.m_WallXPosition = LoadDouble("WallXPosition", 0);
	dlg.m_WallThickness = LoadDouble("WallThickness", 0.05);
	dlg.m_HolePosition = LoadDouble("HolePosition", 0.7);
	dlg.m_HoleSize = LoadDouble("HoleSize", 0.2);
	dlg.m_Vinit = LoadDouble("Vinit", 0);
	dlg.m_BounceTop = LoadDouble("DeltaVtop", 1);
	dlg.m_BounceBottom = LoadDouble("DeltaVbottom", 1);
	dlg.m_BounceSide = LoadDouble("DeltaVside", 1);
	dlg.m_FileName = pszUnknownFileName;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	nParticles = dlg.m_nParticles;
	SaveInt("nParticles", nParticles);
	g = dlg.m_G;
	SaveDouble("g", g);
	loss = dlg.m_LossRatio;
	SaveDouble("CollisionLossRatio", loss);
	Geometry.Rparticle = dlg.m_RParticle;
	SaveDouble("RParticle", Geometry.Rparticle);
	Geometry.Xleft = dlg.m_Xleft;
	Geometry.ParticleXleft = Geometry.Xleft + Geometry.Rparticle;
	SaveDouble("Xleft", Geometry.Xleft);
	Geometry.Xright = dlg.m_Xright;
	Geometry.ParticleXright = Geometry.Xright - Geometry.Rparticle;
	SaveDouble("Xright", Geometry.Xright);
	Geometry.Ymax = dlg.m_Ytop;
	Geometry.ParticleYmax = Geometry.Ymax - Geometry.Rparticle;
	SaveDouble("Ytop", Geometry.Ymax);
	Geometry.Ymin = dlg.m_Ybottom;
	Geometry.ParticleYmin = Geometry.Ymin + Geometry.Rparticle;
	SaveDouble("Ybottom", Geometry.Ymin);
	Geometry.Xcenter = dlg.m_WallXPosition;
	SaveDouble("WallXPosition", Geometry.Xcenter);
	Geometry.WThickness = dlg.m_WallThickness;
	SaveDouble("WallThickness", Geometry.WThickness);
	Geometry.WLeft = Geometry.Xcenter - Geometry.WThickness/2;
	Geometry.WRight = Geometry.Xcenter + Geometry.WThickness/2;
	Geometry.ParticleWLeft = Geometry.WLeft - Geometry.Rparticle;
	Geometry.ParticleWRight = Geometry.WRight + Geometry.Rparticle;
	Geometry.HoleSize = dlg.m_HoleSize;
	Geometry.HolePosition = dlg.m_HolePosition;
	Geometry.YGapTop = Geometry.HolePosition + Geometry.HoleSize/2;
	Geometry.YGapBottom = Geometry.HolePosition - Geometry.HoleSize/2;
	Geometry.ParticleHoleTop = Geometry.YGapTop-Geometry.Rparticle;
	Geometry.ParticleHoleBottom = Geometry.YGapBottom+Geometry.Rparticle;
	SaveDouble("HolePosition", dlg.m_HolePosition);
	SaveDouble("HoleSize", dlg.m_HoleSize);
	Vinit = dlg.m_Vinit;
	SaveDouble("Vinit", Vinit);
	DeltaVbottom = dlg.m_BounceBottom;
	SaveDouble("DeltaVbottom", DeltaVbottom);
	DeltaVtop = dlg.m_BounceTop;
	SaveDouble("DeltaVtop", DeltaVtop);
	DeltaVside = dlg.m_BounceSide;
	SaveDouble("DeltaVside", DeltaVside);
	FileName = dlg.m_FileName;
	nFluxFromLeft = nFluxFromRight = 0;

	srand(time(NULL));	// запуск генератора случайных чисел
	// сначала в левой половине
	for (i = 0; i < nParticles/2; i++)
	{
		do
		{
			bTouching = FALSE;
			particle[i].x = random(Geometry.Xleft+Geometry.Rparticle, Geometry.ParticleWLeft);
			particle[i].y = random(Geometry.Ymin+Geometry.Rparticle, Geometry.Ymax-Geometry.Rparticle);
			for (j = 0; j<i; j++)
			{
				if (InContact(particle[i], particle[j]))
				{
					bTouching = TRUE;
					break;
				}
			}
		} while (bTouching);
		// координаты определили, теперь скорость
		RandomVelocity(particle[i]);
		particle[i].color = 0; // red
	}

	// теперь в правой половине
	for (i = nParticles/2; i < nParticles; i++)
	{
		do
		{
			bTouching = FALSE;
			particle[i].x = random(Geometry.ParticleWRight, Geometry.Xright-Geometry.Rparticle);
			particle[i].y = random(Geometry.Ymin+Geometry.Rparticle, Geometry.Ymax-Geometry.Rparticle);
			for (j = nParticles/2; j<i; j++)
			{
				if (InContact(particle[i], particle[j]))
				{
					bTouching = TRUE;
					break;
				}
			}
		} while (bTouching);
		// координаты определили, теперь скорость
		RandomVelocity(particle[i]);
		particle[i].color = 1; // blue
	}
	CalcTimeStep();
	RecalcStat();
	FILE *fd = fopen(FileName, "w");
	if (fd)
	{
		fprintf(fd, "Particles:\n");
		fprintf(fd, "nParticles = %d\n", nParticles);
		fprintf(fd, "RParticle = %g m\n", Geometry.Rparticle);

		fprintf(fd, "\nCollisions:\n");
		fprintf(fd, "CollisionLossRatio = %g\n", loss);
		fprintf(fd, "DeltaVbottom = %g m/sec\n", DeltaVbottom);
		fprintf(fd, "DeltaVtop = %g m/sec\n", DeltaVtop);
		fprintf(fd, "DeltaVside = %g m/sec\n", DeltaVside);

		fprintf(fd, "\nGeometry:\n");
		fprintf(fd, "Xleft = %g m\n", Geometry.Xleft);
		fprintf(fd, "Xright = %g m\n", Geometry.Xright);
		fprintf(fd, "Ytop = %g m\n", Geometry.Ymax);
		fprintf(fd, "Ybottom = %g m\n", Geometry.Ymin);
		fprintf(fd, "WallXPosition = %g m\n", Geometry.Xcenter);
		fprintf(fd, "WallThickness = %g m\n", Geometry.WThickness);
		fprintf(fd, "HolePosition = %g m\n", Geometry.HolePosition);
		fprintf(fd, "HoleSize = %g m\n", Geometry.HoleSize);

		fprintf(fd, "\nInitial distribution:\n");
		fprintf(fd, "Vinit = %g m/sec\n", Vinit);

		fprintf(fd, "\nOther:\n");
		fprintf(fd, "g = %g m/sec**2\n", g);

		fprintf(fd, "\nTime\tnLeft\t<V>left\t<flux>left\t\tnRight\t<V>right\t<flux>right\n");
		fclose(fd);
	}
	pRenderer_->SetDefaultView();

	return TRUE;
}

void CWorld::OnIdle()
{
	// Записываем всю статистику в файл
	if ((nTimeSteps % 1000) == 0)
		WriteStat();

	BOOL bChanged = OneTimeStep();
	if (bChanged)
	{
		// Пересчитать статистику
		RecalcStat();
		CalcTimeStep();
	}
	if ((nTimeSteps % 5) == 0)
	{
		pRenderer_->Redraw();
	}
}

void CWorld::MoveParticle(SParticle &p, double dt)
{
	BOOL IsLeftBefore = p.x < Geometry.Xcenter;
	// По оси Х
	p.x = p.x + p.vx*dt;
	BOOL IsLeftAfter = p.x < Geometry.Xcenter;
	if (IsLeftAfter != IsLeftBefore)
	{
		if (IsLeftBefore)
			++nFluxFromLeft;
		else
			++nFluxFromRight;
	}
	// По оси Y
	p.y = p.y + p.vy*dt - g*dt*dt/2;
	p.vy = p.vy - g*dt;
}

// Записывает накопленную статистику в файл
void CWorld::WriteStat()
{
	FILE *fd = fopen(FileName, "a");
	if (fd)
	{
		double FluxFromLeft, FluxFromRight;
		if (LastTimeWritten == Time)
		{
			FluxFromLeft = FluxFromRight = 0;
		}
		else
		{
			FluxFromLeft = nFluxFromLeft/(Time-LastTimeWritten);
			FluxFromRight = nFluxFromRight/(Time-LastTimeWritten);
		}

		fprintf(fd, "%7.2f\t%d\t%7.2f\t%7.2f\t\t%d\t%7.2f\t%7.2f\n", Time, nLeft, VaverageL, FluxFromLeft, nRight, VaverageR, FluxFromRight);
		fclose(fd);
		LastTimeWritten = Time;
		nFluxFromLeft = nFluxFromRight = 0;
	}
}

// Метод сортировки частиц по Х
static int CompareParticleProc(const void *elem1, const void *elem2)
{
	SParticle *p1 = (SParticle *)elem1;
	SParticle *p2 = (SParticle *)elem2;
    if (p1->y < p2->y)
		return -1;
	else
		return 1;
}

// Выполняет моделирование одного временнОго шага
// Возвращает TRUE если в результате этого шага случилось хотя бы одно столкновение частицы
// с другой частицей или со стенками (в результате чего изменилась скорость хоть одной частицы)
// или хоть одна частица перелетела из одной половины в другую
BOOL CWorld::OneTimeStep()
{
	int i, j;
	double dt = DeltaTime/2;
	
	for (i = 0; i < nParticles; i++)
	{
		MoveParticle(particle[i], DeltaTime);
	}
	
	// Проверяем столкновение частиц между собой
	::qsort(particle, nParticles, sizeof(SParticle), CompareParticleProc);
	for (i = 0; i < nParticles; i++)
	{
		for (j = i+1; j < nParticles; j++)
		{
			if (particle[j].y - particle[i].y > 2*Geometry.Rparticle)
				break;
			if (InContact(particle[i], particle[j]) && Approaching(particle[i], particle[j]))
			{
				Collision(particle[i], particle[j]);
			}
		}
	}

	// Теперь проверяем столкновение частиц со стенками
	for (i = 0; i < nParticles; i++)
	{
		CorrectParticleByGeometry(particle[i]);
	}

	Time = Time+DeltaTime;
	nTimeSteps = nTimeSteps+1;
	return TRUE;
}

BOOL CWorld::CorrectParticleByGeometry(SParticle &p)
{
	BOOL bCorrected = FALSE;
	double xx = p.x;
	double yy = p.y;
	double vx = p.vx;
	double vy = p.vy;
	if (yy > Geometry.ParticleYmax && vy > 0)
	{ // Верхняя стенка
		p.y = Geometry.ParticleYmax;
		BounceY(p);
		p.vy -= DeltaVtop;
		bCorrected = TRUE;
	}
	else if (yy < Geometry.ParticleYmin && vy < 0)
	{ // Нижняя стенка
		p.y = Geometry.ParticleYmin;
		BounceY(p);
		p.vy += DeltaVbottom;
		bCorrected = TRUE;
	}
	if (xx < Geometry.ParticleXleft && vx < 0)
	{ // левая стенка
		p.x = Geometry.ParticleXleft;
		BounceX(p);
		p.vx += DeltaVside;
		bCorrected = TRUE;
	}
	else if (xx > Geometry.ParticleXright && vx > 0)
	{ // правая стенка
		p.x = Geometry.ParticleXright;
		BounceX(p);
		p.vx -= DeltaVside;
		bCorrected = TRUE;
	}
	else if (xx > Geometry.ParticleWLeft && xx < Geometry.ParticleWRight)
	{// Частица может находиться внутри перегородки
		if (xx > Geometry.WLeft && xx < Geometry.WRight)
		{
			if (yy > Geometry.ParticleHoleTop && vy > 0)
			{ // Верхняя граница дырки
				p.y = Geometry.ParticleHoleTop;
				BounceY(p);
				p.vy -= DeltaVtop;
				bCorrected = TRUE;
			}
			else if (yy < Geometry.ParticleHoleBottom && vy < 0)
			{ // Нижняя граница дырки
				p.y = Geometry.ParticleHoleBottom;
				BounceY(p);
				p.vy += DeltaVbottom;
				bCorrected = TRUE;
			}
		}
		else if (yy > Geometry.YGapTop || yy < Geometry.YGapBottom)
		{ // Частица не попала в дырку в перегородке
			if (xx < Geometry.Xcenter && vx > 0)
			{
				p.x = Geometry.ParticleWLeft;
				BounceX(p);
				// и добавляем импульса влево
				p.vx -= DeltaVside;
				bCorrected = TRUE;
			}
			else if (xx > Geometry.Xcenter && vx < 0)
			{ 
				p.x = Geometry.ParticleWRight;
				BounceX(p);
				// и добавляем импульса вправо
				p.vx += DeltaVside;
				bCorrected = TRUE;
			}
		}
	}
	return bCorrected;
}