//********************************************************************************
//********************************************************************************
//**										**
//**	File:		mesh_poly.cpp						**
//**	Contents:	Polygon -> rectangle / right isosceles triangle        	**
//**										**
//**	Author:		tbd         						**
//**										**
//**	Copyright (c) 2009 IBM      						**
//**	All rights, including trade secret rights, reserved.			**
//**										**
//********************************************************************************
//********************************************************************************

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

// Windows
#include <stdlib.h>
#include <time.h>
// Unix
//   #include <malloc.h>
//   #include <sys/time.h>

#include "common_dec.hpp"
#include "mesh_poly.hpp"

// -------------------------------------------------
// Forward declarations

int mp_polygon2rt(float fTolerance);

// -------------------------------------------------
// External declarations

extern void error_message(char *sEW, char *str);
extern void error_log(char *str);

// -------------------------------------------------
// Global declarations

S34 *g_pMesh = NULL;
extern ID_ALLAREAS *id_AreaList;
extern ID_ALLAREAS *id_AreaList_b;

extern char sLogStr[];
extern FILE *fp_log;
extern int test_code;


SSectionArray g_Sections;
float g_StartX, g_StartY, g_fGridSize;
extern S34 *g_pMesh; // meshed area as list of right isosceles triangles and rectangles

// Debug macros, empty for main project
#ifndef ASSERT
	#define ASSERT
#endif

#ifndef TRACE
	#define TRACE
#endif

#ifndef max
	#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

void CalcBox(AreaList *pArea)
{
	if (!pArea)
		return;
	if (pArea->iNumVer && pArea->segver)
	{
		VerList *pVertex = pArea->segver;
		pArea->fLLx = pArea->fURx = pVertex->fx1;
		pArea->fLLy = pArea->fURy = pVertex->fy1;
		for (int i = 1; i < pArea->iNumVer; i++)
		{
			pVertex = pVertex->next;
			if (pVertex->fx1 > pArea->fURx)
				pArea->fURx = pVertex->fx1;
			if (pVertex->fx1 < pArea->fLLx)
				pArea->fLLx = pVertex->fx1;
			if (pVertex->fy1 > pArea->fURy)
				pArea->fURy = pVertex->fy1;
			if (pVertex->fy1 < pArea->fLLy)
				pArea->fLLy = pVertex->fy1;
		}
	}
}

void ReportOutOfMemoryError()
{
	// TODO: implement this
	exit(0);
}

void CleanSections(SSectionArray *pSectionArray)
{
	SI_FREE(pSectionArray->pSections, SSection*);
	pSectionArray->pSections = NULL;
	pSectionArray->nAllocated = pSectionArray->nSections = 0;
}

SSection *SSectionArray::FindSection(int i1, int j1, int i2, int j2, AreaList *pArea)
{
	for (int i = 0; i < nSections; i++)
	{
		SSection *pSection = pSections+i;
		if (!pSection->bAlive)
			continue;
		if (pArea)
		{
			if (pSection->pArea != pArea)
				continue; // pArea == NULL means ANY AREA, i.e. no area check is needed
		}
		if (pSection->i1 != i1)
			continue;
		if (pSection->i2 != i2)
			continue;
		if (pSection->j1 != j1)
			continue;
		if (pSection->j2 != j2)
			continue;
		// Found! Return it.
		return pSection;
	}
	return NULL;
}

SSection *SSectionArray::AllocNewSection(int i1, int j1, int i2, int j2, AreaList *pArea)
{
	if (nSections >= nAllocated)
	{
		if (nAllocated == 0)
			nAllocated = 1;
		nAllocated *= 2;
		pSections = (SSection *)realloc(pSections, nAllocated*sizeof(SSection));
	}
	if (!pSections)
	{
		ReportOutOfMemoryError();
		return NULL;
	}

	if (abs(i1-i2) > 1)
		i1=i1;//debug
	ASSERT(abs(i1-i2) <= 1);
	if (abs(j1-j2) > 1)
		i1=i1;//debug
	ASSERT(abs(j1-j2) <= 1);
	
	SSection *pSection = pSections + nSections;
	pSection->i1 = i1;
	pSection->j1 = j1;
	pSection->i2 = i2;
	pSection->j2 = j2;
	pSection->x1 = g_StartX + i1*g_fGridSize;
	pSection->y1 = g_StartY + j1*g_fGridSize;
	pSection->x2 = g_StartX + i2*g_fGridSize;
	pSection->y2 = g_StartY + j2*g_fGridSize;
	pSection->pArea = pArea;
	pSection->bAlive = 1;

	++nSections;
	return pSection;
}

SSection *SSectionArray::NewSectionIfNotExist(int i1, int j1, int i2, int j2, AreaList *pArea)
{
	SSection *pOppositeSection = FindSection(i2, j2, i1, j1, pArea);
	if (pOppositeSection)
	{
		pOppositeSection->bAlive = 0;
		return NULL;
	}
	return AllocNewSection(i1, j1, i2, j2, pArea);
}

void SObstacleArray2D::UpdateBounds(SSection *pSection)
{
	if (pSection->i1 > MaxI)
		MaxI = pSection->i1;
	if (pSection->i1 < MinI)
		MinI = pSection->i1;
	if (pSection->i2 > MaxI)
		MaxI = pSection->i2;
	if (pSection->i2 < MinI)
		MinI = pSection->i2;

	if (pSection->j1 > MaxJ)
		MaxJ = pSection->j1;
	if (pSection->j1 < MinJ)
		MinJ = pSection->j1;
	if (pSection->j2 > MaxJ)
		MaxJ = pSection->j2;
	if (pSection->j2 < MinJ)
		MinJ = pSection->j2;
}

SObstacle *SObstacleArray2D::GetObstacle(int i, int j)
{
	if (i < MinI || i > MaxI)
		return &m_SuperObstacle;
	ASSERT(i >= MinI && i <= MaxI);

	if (j < MinJ || j > MaxJ)
		return &m_SuperObstacle;
	ASSERT(j >= MinJ && j <= MaxJ);

	return pData+(i-MinI)*SizeJ+(j-MinJ);
}

void SObstacleArray2D::AcceptSection(SSection *pSection)
{
	if (!pSection)
		return;
	int i = min(pSection->i1, pSection->i2);
	int j = min(pSection->j1, pSection->j2);
	SObstacle *pObstacle = GetObstacle(i, j);
	if (pSection->i1 == pSection->i2)
	{
		TRACE("Obstacle[%d,%d] = W\n", i, j);
		pObstacle->W = 1;
	}
	else if (pSection->j1 == pSection->j2)
	{
		TRACE("Obstacle[%d,%d] = S\n", i, j);
		pObstacle->S = 1;
	}
	else if (pSection->i2-pSection->i1 == pSection->j2-pSection->j1)
	{
		TRACE("Obstacle[%d,%d] = SWNE\n", i, j);
		pObstacle->SWNE = 1;
	}
	else if (pSection->i2-pSection->i1 == pSection->j1-pSection->j2)
	{
		TRACE("Obstacle[%d,%d] = SENW\n", i, j);
		pObstacle->SENW = 1;
	}
	else
		ASSERT(0);
}

SObstacle *SObstacleArray2D::AllocData()
{
	SizeI = MaxI-MinI+1;
	SizeJ = MaxJ-MinJ+1;
	pData = (SObstacle *)calloc(SizeI*SizeJ, sizeof(SObstacle));
	return pData;
}

void SObstacleArray2D::FreeData()
{
	SI_FREE(pData, SObstacle*);
}

void SwapIntegers(int &i1, int &i2)
{
	int dummy = i2;
	i2 = i1;
	i1 = dummy;
}

void SwapFloats(float &f1, float &f2)
{
	float dummy = f2;
	f2 = f1;
	f1 = dummy;
}

void Reverse(SSection *pSection)
{
	SwapIntegers(pSection->i1, pSection->i2);
	SwapIntegers(pSection->j1, pSection->j2);
	SwapFloats(pSection->x1, pSection->x2);
	SwapFloats(pSection->y1, pSection->y2);
}

float Distance2Point2Point(float x1, float y1, float x2, float y2)
{
	float dx = x2-x1;
	float dy = y2-y1;
	return dx*dx+dy*dy;
}

float Distance2LineToPoint(float x1, float y1, float x2, float y2, float x, float y)
{
	float a2 = Distance2Point2Point(x, y, x1, y1);
	if (a2 == 0.)
		return 0.;
	float b2 = Distance2Point2Point(x, y, x2, y2);
	if (b2 == 0.)
		return 0.;
	float c2 = Distance2Point2Point(x1, y1, x2, y2);
	if (c2 == 0.)
		return a2; // v1==v2

	// ordinary triangle
	float foo1 = b2-c2-a2;
	float foo2 = a2-c2-b2;

	if (foo1>0 || foo2>0)
	{
		return min(a2, b2);
	}
	else
	{
		float d2=foo1*foo1/(4.f*c2);
		return a2-d2;
	}
}

void GenSectionsVerList(AreaList *pArea, float tolerance)
{
	if (!pArea)
		return;
	if (!pArea->segver)
		return;

	CalcBox(pArea);
	if ((pArea->fURx - pArea->fLLx) < tolerance)
		return;
	if ((pArea->fURy - pArea->fLLy) < tolerance)
		return;

	g_fGridSize = tolerance;
	int nStartSection = g_Sections.nSections; // store start point for sections reversing

	// Recalc orientation
	float S = 0;
	VerList *pSegment = pArea->segver;
	float FromX = pSegment->fx1;
	float FromY = pSegment->fy1;
    int k;
	for (k = 0; k < pArea->iNumVer; k++)
	{
		S += pSegment->fx2 * pSegment->fy1;
		S -= pSegment->fy2 * pSegment->fx1;
		pSegment = pSegment->next;
	}
	int ori;
	if (S > 0)
		ori = RS_CW;
	else if (S < 0)
		ori = RS_CCW;
	else
		return; // Polygon of no area

	SSection *pSection;
	pSegment = pArea->segver;

	for (k = 0; k < pArea->iNumVer; k++)
	{
		TRACE("x,y = (%f, %f)\n", pSegment->fx1, pSegment->fy1);
		int FromI = (int)floor((pSegment->fx1 - g_StartX)/g_fGridSize+0.4999999);
		int ToI = (int)floor((pSegment->fx2 - g_StartX)/g_fGridSize+0.4999999);
		int dI = ToI-FromI;
		int FromJ = (int)floor((pSegment->fy1 - g_StartY)/g_fGridSize+0.4999999);
		int ToJ = (int)floor((pSegment->fy2 - g_StartY)/g_fGridSize+0.4999999);
		int dJ = ToJ-FromJ;

		if (dI || dJ)
		{
			if (abs(dI) == abs(dJ))
			{ // diagonal
				int i1, i2, j1, j2, n = abs(ToJ - FromJ); // repeat n times
				for (int m = 0; m < n; m++)
				{
					if (ToI > FromI)
					{
						i1 = FromI+m;
						i2 = FromI+m+1;
					}
					else
					{
						i1 = FromI-m;
						i2 = FromI-m-1;
					}

					if (ToJ > FromJ)
					{
						j1 = FromJ+m;
						j2 = FromJ+m+1;
					}
					else
					{
						j1 = FromJ-m;
						j2 = FromJ-m-1;
					}

					pSection = g_Sections.NewSectionIfNotExist(i1, j1, i2, j2, pArea);
				}
			}
			else if (dJ != 0 && dI == 0)
			{ // vertical
				int j1, j2, n = abs(ToJ - FromJ); // repeat n times
				for (int m = 0; m < n; m++)
				{
					if (ToJ > FromJ)
					{
						j1 = FromJ+m;
						j2 = FromJ+m+1;
					}
					else
					{
						j1 = FromJ-m;
						j2 = FromJ-m-1;
					}

					pSection = g_Sections.NewSectionIfNotExist(ToI, j1, ToI, j2, pArea);
				}
			}
			else if (dI != 0 && dJ == 0)
			{ // horizontal
				int i1, i2, n = abs(ToI - FromI); // repeat n times
				for (int m = 0; m < n; m++)
				{
					if (ToI > FromI)
					{
						i1 = FromI+m;
						i2 = FromI+m+1;
					}
					else
					{
						i1 = FromI-m;
						i2 = FromI-m-1;
					}

					pSection = g_Sections.NewSectionIfNotExist(i1, ToJ, i2, ToJ, pArea);
				}
			}
			else
			{// common case
				int i1, j1, i2, j2, n = max(abs(dI), abs(dJ)) - 1;
				for (int m = 0; m < n; m++)
				{
					if (abs(dJ) > abs(dI))
					{
						if (ToJ > FromJ)
						{
							j1 = FromJ+1;
						}
						else
						{
							j1 = FromJ-1;
						}
						float x1 = g_StartX + FromI*g_fGridSize;
						float y1 = g_StartY + FromJ*g_fGridSize;
						float Dist1 = Distance2LineToPoint(FromX, FromY, pSegment->fx2, pSegment->fy2, x1, y1);
						if (dI > 0)
							i2 = FromI+1;
						else
							i2 = FromI-1;
						float x2 = g_StartX + i2*g_fGridSize;
						float y2 = g_StartY + j1*g_fGridSize;
						float Dist2 = Distance2LineToPoint(FromX, FromY, pSegment->fx2, pSegment->fy2, x2, y2);
						if (Dist2 < Dist1)
							i1 = i2;
						else
							i1 = FromI;

						pSection = g_Sections.NewSectionIfNotExist(FromI, FromJ, i1, j1, pArea);
					}
					else
					{
						if (ToI > FromI)
							i1 = FromI+1;
						else
							i1 = FromI-1;
						float x1 = g_StartX + FromI*g_fGridSize;
						float y1 = g_StartY + FromJ*g_fGridSize;
						float Dist1 = Distance2LineToPoint(FromX, FromY, pSegment->fx2, pSegment->fy2, x1, y1);
						if (dJ > 0)
							j2 = FromJ+1;
						else
							j2 = FromJ-1;
						float x2 = g_StartX + i1*g_fGridSize;
						float y2 = g_StartY + j2*g_fGridSize;
						float Dist2 = Distance2LineToPoint(FromX, FromY, pSegment->fx2, pSegment->fy2, x2, y2);
						if (Dist2 < Dist1)
							j1 = j2;
						else
							j1 = FromJ;

						pSection = g_Sections.NewSectionIfNotExist(FromI, FromJ, i1, j1, pArea);
					}	
					FromI = i1;
					FromJ = j1;
				}
				
				pSection = g_Sections.NewSectionIfNotExist(FromI, FromJ, ToI, ToJ, pArea);

				FromI = ToI;
				FromJ = ToJ;
			}
			FromX = pSegment->fx2;
			FromY = pSegment->fy2;
		}
		pSegment = pSegment->next;
	}

	if (ori == RS_CW && pArea->iType == RS_VOID ||
		ori == RS_CCW && pArea->iType == RS_SOLID)
	{
		// Reverse oll sections to have metal on right hand going from start to end through rge section
		for (k = nStartSection; k < g_Sections.nSections; k++)
		{
			pSection = g_Sections.pSections+k;
			Reverse(pSection);
		}
	}

}

void GenSections(AreaList *pArea, float tolerance)
{
	if (!pArea)
		return;
	if (!pArea->segver)
		return;

	g_StartX = pArea->segver->fx1;
	g_StartY = pArea->segver->fy1;

	// Own vertices first
	GenSectionsVerList(pArea, tolerance);
	// Cutouts now
	VoidList *pVoid = pArea->voids;
	while (pVoid)
	{
		GenSectionsVerList(pVoid->voidarea, tolerance);
		pVoid = pVoid->next;
	}
}

static int CompareSections(const void *elem1, const void *elem2)
{
	SSection *p1 = (SSection *)elem1;
	SSection *p2 = (SSection *)elem2;
	int Sum1 = p1->i1+p1->i2;
	int Sum2 = p2->i1+p2->i2;
	if (Sum1 > Sum2)
		return +1;
	else if (Sum1 < Sum2)
		return -1;
	else
	{
		Sum1 = p1->j1+p1->j2;
		Sum2 = p2->j1+p2->j2;
		if (Sum1 > Sum2)
			return +1;
		else if (Sum1 < Sum2)
			return -1;
	}
	return 0;
}

 // This method does all the work
S34 *MeshArea(AreaList *pArea, float tolerance)
{
	if (!pArea)
		return NULL;

	int i, j, k, m;
	SSection *pSection, *pAnotherSection;
	int bObstacleFound;

	/* recalc boxes */
	CalcBox(pArea);

	CleanSections(&g_Sections);
	GenSections(pArea, tolerance);
	
	// "Annihilate" opposite sections
	if (!g_Sections.pSections)
		return NULL;
	::qsort(g_Sections.pSections, g_Sections.nSections, sizeof(SSection), CompareSections);
	for (i = 0; i < g_Sections.nSections; i++)
	{
		SSection *pSection1 = g_Sections.pSections+i;
		if (!pSection1->bAlive)
			continue;
		int bOppositeFound = 0;
		for (j = i+1; j < g_Sections.nSections; j++)
		{
			SSection *pSection2 = g_Sections.pSections+j;
			if (!pSection2->bAlive)
				continue;
			if (pSection1->i1 == pSection2->i2 &&
				pSection1->j1 == pSection2->j2 &&
				pSection1->i2 == pSection2->i1 &&
				pSection1->j2 == pSection2->j1)
			{
				bOppositeFound = 1;
				break;
			}
			else if (pSection1->i1 != pSection2->i1 ||
				pSection1->j1 != pSection2->j1 ||
				pSection1->i2 != pSection2->i2 ||
				pSection1->j2 != pSection2->j2)
				break;
		}
		if (bOppositeFound)
		{
			pSection1->bAlive = 0;
			for (j = i+1; j < g_Sections.nSections; j++)
			{
				SSection *pSection2 = g_Sections.pSections+j;
				if (!pSection2->bAlive)
					continue;
				if (pSection1->i1 == pSection2->i1 &&
					pSection1->j1 == pSection2->j1 &&
					pSection1->i2 == pSection2->i2 &&
					pSection1->j2 == pSection2->j2 ||
					pSection1->i1 == pSection2->i2 &&
					pSection1->j1 == pSection2->j2 &&
					pSection1->i2 == pSection2->i1 &&
					pSection1->j2 == pSection2->j1)
				{
					pSection2->bAlive = 0;
				}
				else
				{
					i = j-1;
					break;
				}
			}
		}
	}
	
	// Make 2D-array of cells (obstacles)
	SObstacleArray2D obstacles;
	obstacles.m_SuperObstacle.S = obstacles.m_SuperObstacle.SENW = obstacles.m_SuperObstacle.W = obstacles.m_SuperObstacle.SWNE = 1; // Super-obstacle!
	memset(&obstacles, 0, sizeof(SObstacleArray2D));
	SObstacle *pObstacle;


	for (i = 0; i < g_Sections.nSections; i++)
	{
		pSection = g_Sections.pSections+i;
		if (!pSection->bAlive)
			continue;
		obstacles.UpdateBounds(pSection);
	}
	if (!obstacles.AllocData())
	{
		ReportOutOfMemoryError();
		return NULL;
	}

	// Fill the array of obstacles
	for (i = 0; i < g_Sections.nSections; i++)
	{
		pSection = g_Sections.pSections+i;
		if (!pSection->bAlive)
			continue;
		obstacles.AcceptSection(pSection);
	}

	S34 *p34Head = NULL;
	// Triangles first
	for (i = 0; i < g_Sections.nSections; i++)
	{
		pSection = g_Sections.pSections+i;
		if (!pSection->bAlive)
			continue;
		if (pSection->IsHorizontal() ||
			pSection->IsVertical())
			continue;
		pSection->bAlive = 0; // exclude this section from further processing
		int MinI = min(pSection->i1, pSection->i2);
		int MaxI = max(pSection->i1, pSection->i2);
		int MinJ = min(pSection->j1, pSection->j2);
		int MaxJ = max(pSection->j1, pSection->j2);

		SSection *pAnotherSection;

		// Make other sections and obstacles
		if (pSection->IsSWNE())
		{
			for (k = 1; ; k++)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1+k, pSection->j1+k, pSection->i2+k, pSection->j2+k, NULL);
				if (!pAnotherSection)
					break;
				bObstacleFound = 0;
				for (m = 0; m <= k; m++)
				{
					pObstacle = obstacles.GetObstacle(pSection->i1+k, pSection->j1+m);
					if (pObstacle->W || pObstacle->SENW ||
						(m != k && pObstacle->SWNE) ||
						(m != 0 && pObstacle->S))
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				pAnotherSection->bAlive = 0;
				MaxI = pSection->i2+k;
				MaxJ = pSection->j2+k;
			}
			for (k = MinI; k < MaxI; k++)
			{
				pAnotherSection = g_Sections.FindSection(k+1, MinJ, k, MinJ, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(k, MinJ, k+1, MinJ, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
			for (k = MinJ; k < MaxJ; k++)
			{
				pAnotherSection = g_Sections.FindSection(MaxI, k+1, MaxI, k, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(MaxI, k, MaxI, k+1, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
		}
		else if (pSection->IsNESW())
		{
			for (k = 1; ; k++)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1+k, pSection->j1+k, pSection->i1+k-1, pSection->j1+k-1, NULL);
				if (!pAnotherSection)
					break;
				bObstacleFound = 0;
				for (m = 0; m <= k; m++)
				{
					pObstacle = obstacles.GetObstacle(pSection->i2+m, pSection->j2+k);
					if (pObstacle->S || pObstacle->SENW ||
						(m != k && pObstacle->SWNE) ||
						(m != 0 && pObstacle->W))
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				pAnotherSection->bAlive = 0;
				MaxI = pSection->i1+k;
				MaxJ = pSection->j1+k;
			}
			for (k = MaxI; k > MinI; k--)
			{
				pAnotherSection = g_Sections.FindSection(k-1, MaxJ, k, MaxJ, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(k, MaxJ, k-1, MaxJ, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
			for (k = MaxJ; k > MinJ; k--)
			{
				pAnotherSection = g_Sections.FindSection(MinI, k-1, MinI, k, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(MinI, k, MinI, k-1, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
		}
		else if (pSection->IsSENW())
		{
			for (k = 1; ; k++)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1+k, pSection->j1-k, pSection->i2+k, pSection->j2-k, NULL);
				if (!pAnotherSection)
					break;
				bObstacleFound = 0;
				for (m = 0; m <= k; m++)
				{
					pObstacle = obstacles.GetObstacle(pSection->i2+k, pSection->j1-m);
					if (pObstacle->W || pObstacle->SWNE ||
						(m != k && pObstacle->SENW))
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				pAnotherSection->bAlive = 0;
				MaxI = pSection->i1+k;
				MinJ = pSection->j1-k;
			}
			for (k = MinJ; k < MaxJ; k++)
			{
				pAnotherSection = g_Sections.FindSection(MaxI, k+1, MaxI, k, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(MaxI, k, MaxI, k+1, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
			for (k = MaxI; k > MinI; k--)
			{
				pAnotherSection = g_Sections.FindSection(k-1, MaxJ, k, MaxJ, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(k, MaxJ, k-1, MaxJ, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
		}
		else if (pSection->IsNWSE())
		{
			for (k = 1; ; k++)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1+k, pSection->j1-k, pSection->i1+k+1, pSection->j1-k-1, NULL);
				if (!pAnotherSection)
					break;
				bObstacleFound = 0;
				for (m = 0; m <= k; m++)
				{
					pObstacle = obstacles.GetObstacle(pSection->i1+m, pSection->j2-k);
					if (pObstacle->SWNE ||
						(m != k && pObstacle->SENW) ||
						(m != 0 && pObstacle->W))
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				pAnotherSection->bAlive = 0;
				MaxI = pSection->i2+k;
				MinJ = pSection->j2-k;
			}
			for (k = MaxJ; k > MinJ; k--)
			{
				pAnotherSection = g_Sections.FindSection(MinI, k-1, MinI, k, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(MinI, k, MinI, k-1, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
			for (k = MinI; k < MaxI; k++)
			{
				pAnotherSection = g_Sections.FindSection(k+1, MinJ, k, MinJ, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(k, MinJ, k+1, MinJ, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
		}
		else
			ASSERT(0);
			
		S34 *p34 = (S34 *)malloc(sizeof(S34));
		p34->pNext = p34Head;
		p34Head = p34;

		pSection = g_Sections.pSections+i; // AllocNewSection() calls realloc therefore pSection can point to wrong data!!!
		if (pSection->IsSWNE())
			p34->type = TYPE_SE;
		else if (pSection->IsNESW())
			p34->type = TYPE_NW;
		else if (pSection->IsSENW())
			p34->type = TYPE_NE;
		else if (pSection->IsNWSE())
			p34->type = TYPE_SW;
		else
			ASSERT(0);
		p34->left = g_StartX + MinI*g_fGridSize;
		p34->right = g_StartX + MaxI*g_fGridSize;
		p34->bottom = g_StartY + MinJ*g_fGridSize;
		p34->top = g_StartY + MaxJ*g_fGridSize;
	}

	// Rectangles now
	for (i = 0; i < g_Sections.nSections; i++)
	{
		pSection = g_Sections.pSections+i;
		if (!pSection->bAlive)
			continue;
		if (!(pSection->IsHorizontal() ||
			pSection->IsVertical()))
		{
			ASSERT(0); // Vertical and horizontal sections only can survive now
			continue;
		}

		int MinI = min(pSection->i1, pSection->i2);
		int MaxI = max(pSection->i1, pSection->i2);
		int MinJ = min(pSection->j1, pSection->j2);
		int MaxJ = max(pSection->j1, pSection->j2);

		// Make other sections and obstacles
		if (pSection->IsSN())
		{
			// Northbound
			k = pSection->j1 + 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1, k, pSection->i1, k+1, NULL);
				if (pAnotherSection)
				{
					MaxJ = k+1;
					++k;
				}
				else
					break;
			}
			// Southbound
			k = pSection->j1 - 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1, k, pSection->i1, k+1, NULL);
				if (pAnotherSection)
				{
					MinJ = k;
					--k;
				}
				else
					break;
			}
			// Eastbound
			k = pSection->i1 + 1;
			while (1)
			{
				bObstacleFound = 0;
				for (m = MinJ; m < MaxJ; m++)
				{
					pObstacle = obstacles.GetObstacle(k, m);
					if (pObstacle->W || pObstacle->SENW || pObstacle->SWNE)
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				else
				{
					++MaxI;
					++k;
				}
			}
			++MaxI;
		}
		else if (pSection->IsNS())
		{
			// Northbound
			k = pSection->j1 + 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1, k, pSection->i1, k-1, NULL);
				if (pAnotherSection)
				{
					MaxJ = k;
					++k;
				}
				else
					break;
			}
			// Southbound
			k = pSection->j1 - 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(pSection->i1, k, pSection->i1, k-1, NULL);
				if (pAnotherSection)
				{
					MinJ = k-1;
					--k;
				}
				else
					break;
			}
			// Westbound
			k = pSection->i1-1;
			while (1)
			{
				int bObstacleFound = 0;
				for (m = MinJ; m < MaxJ; m++)
				{
					pObstacle = obstacles.GetObstacle(k, m);
					if (pObstacle->W || pObstacle->SENW || pObstacle->SWNE)
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				else
				{
					--MinI;
					--k;
				}
			}
			--MinI;
		}
		else if (pSection->IsWE())
		{
			// Eastbound
			k = pSection->i1 + 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(k, pSection->j1, k+1, pSection->j1, NULL);
				if (pAnotherSection)
				{
					MaxI = k+1;
					++k;
				}
				else
					break;
			}
			// Westbound
			k = pSection->i1 - 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(k, pSection->j1, k+1, pSection->j1, NULL);
				if (pAnotherSection)
				{
					MinI = k;
					--k;
				}
				else
					break;
			}
			// Southbound
			k = pSection->j1-1;
			while (1)
			{
				int bObstacleFound = 0;
				for (m = MinI; m < MaxI; m++)
				{
					pObstacle = obstacles.GetObstacle(m, k);
					if (pObstacle->S || pObstacle->SENW || pObstacle->SWNE)
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				else
				{
					--MinJ;
					--k;
				}
			}
			--MinJ;
		}
		else if (pSection->IsEW())
		{
			// Eastbound
			k = pSection->i1 + 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(k, pSection->j1, k-1, pSection->j1, NULL);
				if (pAnotherSection)
				{
					MaxI = k;
					++k;
				}
				else
					break;
			}
			// Westbound
			k = pSection->i1 - 1;
			while (1)
			{
				pAnotherSection = g_Sections.FindSection(k, pSection->j1, k-1, pSection->j1, NULL);
				if (pAnotherSection)
				{
					MinI = k-1;
					--k;
				}
				else
					break;
			}
			// Northbound
			k = pSection->j1+1;
			while (1)
			{
				int bObstacleFound = 0;
				for (m = MinI; m < MaxI; m++)
				{
					pObstacle = obstacles.GetObstacle(m, k);
					if (pObstacle->S || pObstacle->SENW || pObstacle->SWNE)
					{
						bObstacleFound = 1;
						break;
					}
				}
				if (bObstacleFound)
					break;
				else
				{
					++MaxJ;
					++k;
				}
			}
			++MaxJ;
		}
		else
			ASSERT(0);

		// Make other sections and obstacles
		if (MinI == MaxI)
		{
			MinI = MinI;//debug
		}
		else if (MinJ == MaxJ)
		{
			MinJ = MinJ; // debug
		}
		else
		{
			for (k = MinI; k < MaxI; k++)
			{
				// South side
				pAnotherSection = g_Sections.FindSection(k+1, MinJ, k, MinJ, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(k, MinJ, k+1, MinJ, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
				// North side
				pAnotherSection = g_Sections.FindSection(k, MaxJ, k+1, MaxJ, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(k+1, MaxJ, k, MaxJ, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
			for (k = MinJ; k < MaxJ; k++)
			{
				// West side
				pAnotherSection = g_Sections.FindSection(MinI, k, MinI, k+1, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(MinI, k+1, MinI, k, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
				// East side
				pAnotherSection = g_Sections.FindSection(MaxI, k+1, MaxI, k, NULL);
				if (pAnotherSection)
					pAnotherSection->bAlive = 0; // exclude this section from further processing
				else
				{
					pAnotherSection = g_Sections.AllocNewSection(MaxI, k, MaxI, k+1, NULL);
					obstacles.AcceptSection(pAnotherSection);
				}
			}
			S34 *p34 = (S34 *)malloc(sizeof(S34));
			p34->type = TYPE_RECTANGLE;
			p34->pNext = p34Head;
			p34Head = p34;

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// It is not used here. It may be necessary in the future.
			pSection = g_Sections.pSections+i; // AllocNewSection() calls realloc therefore pSection can point to wrong data!!!
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			
			p34->left = g_StartX + MinI*g_fGridSize;
			p34->right = g_StartX + MaxI*g_fGridSize;
			p34->bottom = g_StartY + MinJ*g_fGridSize;
			p34->top = g_StartY + MaxJ*g_fGridSize;
		}
	}


#ifndef __DO_NOT_FREE_SECTIONS__
	CleanSections(&g_Sections);
#endif

	obstacles.FreeData();
	return p34Head;
}

// -------------------------------------------------

void mp_debug_rect_aspect_ratio(S34 *pR, float fw, float fh, float fAR, int iNumR)
{
   FILE *fp2;

   fp2 = fopen("rect_aspect.txt", "a");
   fprintf(fp2, "\n");
   fprintf(fp2, "Rectangle: left = %f  right = %f  bottom = %f  top = %f\n", pR->left, pR->right, pR->bottom, pR->top);
   fprintf(fp2, "  Width = %f  Height = %f  Aspect ratio = %f  Num Rect reqd = %d\n", fw, fh, fAR, iNumR);
   fclose(fp2);
}


void mp_add_rectangle(float left, float right, float bottom, float top)
{
#if 0
   FILE *fp2;

   fp2 = fopen("rect_aspect.txt", "a");
   fprintf(fp2, "  Adding Rectangle: left = %f  right = %f  bottom = %f  top = %f\n", left, right, bottom, top);
   fclose(fp2);
#endif

   S34 *p34 = (S34 *)malloc(sizeof(S34));
   p34->type = TYPE_RECTANGLE;
   p34->left = left;
   p34->right = right;
   p34->bottom = bottom;
   p34->top = top;
   p34->pNext = g_pMesh;
   g_pMesh = p34;
}


// Rectangles must have a less than or equal to 10:1 aspect ratio.
// If a rectangle exceeds that ratio then split the rectangle into smaller rectangles
// where each smaller rectangle's aspect ratio is <= 10:1
void mp_rect_aspect_check()
{
   int i;
   int iNumCuts;
   int iNumR;
   int iWH;
   float fw;
   float fh;
   float fAR = 0.0;
   float fAspect = 10.0;
   float fLong;
   float fShort;
   float fNew;
   float fStart, fEnd;
   S34 *p34;

   for (p34 = g_pMesh; p34 != NULL; p34 = p34->pNext)
   {
      if (p34->type == TYPE_RECTANGLE)
      {
         // Rectangle width and height
         fw = p34->right - p34->left;
         fh = p34->top - p34->bottom;

         if (fw > fh)
         {
            fLong = fw;
            fShort = fh;
            iWH = 0;            // Width > Height
         }
         else
         {
            fLong = fh;
            fShort = fw;
            iWH = 1;            // Height > Width
         }

         // Aspect ratio
         fAR = fLong / fShort;

         if (fAR > fAspect)
         {
            // Aspect ratio exceeds 10:1

            // Determine how many cuts in existing rectangl to make.
            // Determine how many smaller rectangles to make.
            iNumCuts = int (fLong / (fShort * fAspect));
            iNumR = iNumCuts + 1;

            //mp_debug_rect_aspect_ratio(p34, fw, fh, fAR, iNumR);

            if (iWH == 0)
            {
               // Width > Height

               // New rectangle width. (Divide original width by iNumR.)
               fNew = fw / (float) iNumR;

               // Make new rectangles
               // First rectangle is the existing rectangle with modified width.
               p34->right = p34->left + fNew;

               // Now for the rest of the rectangles. Create new ones.
               fStart = p34->left + fNew;
               for (i = 0; i < iNumCuts; i++)
               {
                  fEnd = fStart + fNew;
                  mp_add_rectangle(fStart, fEnd, p34->bottom, p34->top);
                  fStart = fEnd;
               }
            }
            else
            {
               // Height > Width

               // New rectangle height. (Divide original width by iNumR.)
               fNew = fh / (float) iNumR;

               // Make new rectangles
               // First rectangle is the existing rectangle with modified height.
               p34->top = p34->bottom + fNew;

               // Now for the rest of the rectangles. Create new ones.
               fStart = p34->bottom + fNew;
               for (i = 0; i < iNumCuts; i++)
               {
                  fEnd = fStart + fNew;
                  mp_add_rectangle(p34->left, p34->right, fStart, fEnd);
                  fStart = fEnd;
               }
            }
         }
      }
   }
}

// -------------------------------------------------

// If these 2 shapes touch (side overlap) then return true, else return false.
bool mp_do_shapes_touch(S34 *p1, S34 *p2)
{
   int i, j;
   int iValid1, iValid2;
   float fx1, fy1;
   float fx2, fy2;
   float fx3, fy3;
   float fx4, fy4;
   float fmin, fmax;
   float fmin2, fmax2;

   // Edges that overlap are either vertical or horizontal.
   // For each vertical (or horizontal) edge on shape p1 see if the edge
   // overlaps a vertical (or horizontal) edge on shape p2.

   // Check horizontal edges. (There are 2: top and bottom)
   // Find horizontal edges for shape p1
   for (i = 0; i < 2; i++)
   {
      iValid1 = 1;      // Assume valid edge will be found

      if (i == 0)
      {
         // Top edge
         fx1 = p1->left;
         fy1 = p1->top;
         fx2 = p1->right;
         fy2 = p1->top;

         // SW and SE triangles do not have this edge
         if ( (p1->type == TYPE_SW) || (p1->type == TYPE_SE) )
            iValid1 = 0;
      }
      else
      {
         // Bottom edge
         fx1 = p1->left;
         fy1 = p1->bottom;
         fx2 = p1->right;
         fy2 = p1->bottom;

         // NW and NE triangles do not have this edge
         if ( (p1->type == TYPE_NW) || (p1->type == TYPE_NE) )
            iValid1 = 0;
      }

      if (iValid1 == 0)
         continue;

      // Find horizontal edges for shape p2
      for (j = 0; j < 2; j++)
      {
         iValid2 = 1;      // Assume valid edge will be found

         if (j == 0)
         {
            // Top edge
            fx3 = p2->left;
            fy3 = p2->top;
            fx4 = p2->right;
            fy4 = p2->top;

            // SW and SE triangles do not have this edge
            if ( (p2->type == TYPE_SW) || (p2->type == TYPE_SE) )
               iValid2 = 0;
         }
         else
         {
            // Bottom edge
            fx3 = p2->left;
            fy3 = p2->bottom;
            fx4 = p2->right;
            fy4 = p2->bottom;

            // NW and NE triangles do not have this edge
            if ( (p2->type == TYPE_NW) || (p2->type == TYPE_NE) )
               iValid2 = 0;
         }

         // If valid edges found, check for overlap
         // between (fx1, fy1)-(fx2, fy2) and (fx3, fy3)-(fx4, fy4).
         // Horizontal check, so the Y coords of the 2 edges must be the same for an overlap,
         // then check for X coords of one edge to be between X coords of the other edge.
         if ( (iValid1 == 1) && (iValid2 == 1) )
         {
            // Are the Y coords the same?
            if (SI_IsEqual(fy1, fy3))
            {
               // Is one of the X coords of one edge between the X coords of the other edge.
               fmin = fx3;
               fmax = fx4;
               if (fx3 > fx4)
               {
                  fmax = fx3;
                  fmin = fx4;
               }

               if ( ((fmin < fx1) && (fx1 < fmax)) || ((fmin < fx2) && (fx2 < fmax)) )
                  return(true);

               // Is there an exact overlap? I.e., do the 2 edges exactly overlap?
               // Compare end points
               fmin2 = fx1;
               fmax2 = fx2;
               if (fx1 > fx2)
               {
                  fmax2 = fx1;
                  fmin2 = fx2;
               }

               if ( (SI_IsEqual(fmin, fmin2)) && (SI_IsEqual(fmax, fmax2)) )
                  return(true);
            }
         }
      }
   }

   // Check vertical edges. (There are 2: left and right)
   // Find vertical edges for shape p1
   for (i = 0; i < 2; i++)
   {
      iValid1 = 1;      // Assume valid edge will be found

      if (i == 0)
      {
         // Left edge
         fx1 = p1->left;
         fy1 = p1->top;
         fx2 = p1->left;
         fy2 = p1->bottom;

         // NE and SE triangles do not have this edge
         if ( (p1->type == TYPE_NE) || (p1->type == TYPE_SE) )
            iValid1 = 0;
      }
      else
      {
         // Right edge
         fx1 = p1->right;
         fy1 = p1->top;
         fx2 = p1->right;
         fy2 = p1->bottom;

         // NW and SW triangles do not have this edge
         if ( (p1->type == TYPE_NW) || (p1->type == TYPE_SW) )
            iValid1 = 0;
      }

      if (iValid1 == 0)
         continue;

      // Find vertical edges for shape p2
      for (j = 0; j < 2; j++)
      {
         iValid2 = 1;      // Assume valid edge will be found

         if (j == 0)
         {
            // Left edge
            fx3 = p2->left;
            fy3 = p2->top;
            fx4 = p2->left;
            fy4 = p2->bottom;

            // NE and SE triangles do not have this edge
            if ( (p2->type == TYPE_NE) || (p2->type == TYPE_SE) )
               iValid2 = 0;
         }
         else
         {
            // Right edge
            fx3 = p2->right;
            fy3 = p2->top;
            fx4 = p2->right;
            fy4 = p2->bottom;

            // NW and SW triangles do not have this edge
            if ( (p2->type == TYPE_NW) || (p2->type == TYPE_SW) )
               iValid2 = 0;
         }

         // If valid edges found, check for overlap
         // between (fx1, fy1)-(fx2, fy2) and (fx3, fy3)-(fx4, fy4).
         // Vertical check, so the X coords of the 2 edges must be the same for an overlap,
         // then check for Y coords of one edge to be between Y coords of the other edge.
         if ( (iValid1 == 1) && (iValid2 == 1) )
         {
            // Are the X coords the same?
            if (SI_IsEqual(fx1, fx3))
            {
               // Is one of the X coords of one edge between the X coords of the other edge.
               fmin = fy3;
               fmax = fy4;
               if (fy3 > fy4)
               {
                  fmax = fy3;
                  fmin = fy4;
               }

               if ( ((fmin < fy1) && (fy1 < fmax)) || ((fmin < fy2) && (fy2 < fmax)) )
                  return(true);

               // Is there an exact overlap? I.e., do the 2 edges exactly overlap?
               // Compare end points
               fmin2 = fy1;
               fmax2 = fy2;
               if (fy1 > fy2)
               {
                  fmax2 = fy1;
                  fmin2 = fy2;
               }

               if ( (SI_IsEqual(fmin, fmin2)) && (SI_IsEqual(fmax, fmax2)) )
                  return(true);
            }
         }
      }
   }

   return(false);
}


int mp_add_shape_to_list(MP_TOUCH *pS, int id)
{
   MP_SHAPE *new_lb;
   MP_SHAPE *sl;

   for (sl = (*pS).pShapes; sl != NULL; sl = sl->pNext)
   {
      if (sl->id == id)
         return(0);             // already exists
   }

   (MP_SHAPE *) SI_ALLOC_STRUCT2(MP_SHAPE, new_lb);

   if (new_lb == NULL)
      return(-1);

   new_lb->id = id;

   // Link
   new_lb->pNext = (*pS).pShapes;
   (*pS).pShapes = new_lb;

   return(0);
}


// Returns
//  0 = No orphan shapes found
//  1 = Orphan shapes found
int mp_id_groups(MP_TOUCH *pOrphan, int iNum)
{
   int i, j;
   int iOrphan = 0;
   MP_SHAPE *sl;

   // (To get to this function we know that all shapes have connected shapes.)
   // Mark first shape with group id of 1
   pOrphan[0].iGroup = 1;

   // Now mark all shapes that are connected directly or indirectly to the first
   // shape with the same id.
   for (j = 0; j < iNum; j++)
   {
      for (i = 0; i < iNum; i++)
      {
         if (pOrphan[i].iGroup == 0)
            continue;

         // Mark all unmarked connected shapes (ie shapes without a group id)
         // with the same group id as the current shape.
         for (sl = pOrphan[i].pShapes; sl != NULL; sl = sl->pNext)
         {
            if (pOrphan[sl->id].iGroup == 0) 
               pOrphan[sl->id].iGroup = pOrphan[i].iGroup;
         }
      }

      // If all the shapes are connected (directly and indirectly) then iGroup will
      // be set to 1. So if any iGroup is set to 0 there must be an orphan shape.
      iOrphan = 0;
      for (i = 0; i < iNum; i++)
      {
         if (pOrphan[i].iGroup == 0)
         {
            iOrphan = 1;           // Orphan shape(s) found
            break;
         }
      }

      if (iOrphan == 0)
         break;                 // all shapes connected
   }

   return(iOrphan);
}


void mp_debug_orphan_list(MP_TOUCH *pOrphan, int iNum)
{
   int i;
   MP_SHAPE *sl;
   S34 *p34;
   FILE *fp2;

   fp2 = fopen("orphan.txt", "w");

   for (i = 0; i < iNum; i++)
   {
      fprintf(fp2, "Shape %d\n", pOrphan[i].id);
      p34 = pOrphan[i].p34;

      switch (p34->type)
      {
         case TYPE_RECTANGLE:
            // LT->LB->RB->RT->LT
            fprintf(fp2, "   RECT     (%f %f) (%f %f) (%f %f) (%f %f)\n",
                  p34->left, p34->top, p34->left, p34->bottom, p34->right, p34->bottom, p34->right, p34->top);
         break;
         case TYPE_NW:
            // LT->LB->RT->LT
            fprintf(fp2, "   TRI (NW) (%f %f) (%f %f) (%f %f)\n",
                  p34->left, p34->top, p34->left, p34->bottom, p34->right, p34->top);
         break;
         case TYPE_NE:
            // LT->RB->RT->LT
            fprintf(fp2, "   TRI (NE) (%f %f) (%f %f) (%f %f)\n",
                  p34->left, p34->top, p34->right, p34->bottom, p34->right, p34->top);
         break;
         case TYPE_SE:
            // RT->LB->RB->RT
            fprintf(fp2, "   TRI (SE) (%f %f) (%f %f) (%f %f)\n",
                  p34->right, p34->top, p34->left, p34->bottom, p34->right, p34->bottom);
         break;
         case TYPE_SW:
            // RB->LB->LT->RB
            fprintf(fp2, "   TRI (SW) (%f %f) (%f %f) (%f %f)\n",
                  p34->right, p34->bottom, p34->left, p34->bottom, p34->left, p34->top);
         break;
      }

      fprintf(fp2, "   Group %d\n", pOrphan[i].iGroup);
      fprintf(fp2, "   Connected shapes");
      for (sl = pOrphan[i].pShapes; sl != NULL; sl = sl->pNext)
      {
         fprintf(fp2, " %d", sl->id);
      }
      fprintf(fp2, "\n");
   }
   fprintf(fp2, "\n");

   // Are there any orphan shapes?
   for (i = 0; i < iNum; i++)
   {
      if (pOrphan[i].iGroup == 0)
      {
         fprintf(fp2, "Orphan shape(s) found\n");
         break;
      }
   }

   fclose(fp2);
}


// Check for orphan shapes, and shapes that touch only one other shape
// and touch that other shape at a corner. 
// Return
//  0 = no orphans or corner touching shapes
//  1 = orphan or corner touching shape found
int mp_orphan_check()
{
   int i, j;
   int ret = 0;
   int iNum;
   MP_TOUCH *pOrphan;
   MP_SHAPE *pShape;
   MP_SHAPE *pShapeNext;
   S34 *p34;

   pOrphan = NULL;

   // Get number of shapes is g_pMesh
   p34 = g_pMesh;
   iNum = 0;
   while (p34)
   {
      iNum++;
      p34 = p34->pNext;
   }

   // Create structure for orphan shape check
   p34 = g_pMesh;
   pOrphan = SI_MALLOC_PRIMS(MP_TOUCH, (iNum + 1));
   for (i = 0; i < iNum; i++)
   {
      pOrphan[i].id = i;
      pOrphan[i].p34 = p34;
      pOrphan[i].iGroup = 0;
      pOrphan[i].pShapes = NULL;

      p34 = p34->pNext;
   }

   // For all shapes find all the shapes that it touches.
   // Touching means that one side overlaps a side on another shape.
   // Touching at a corner (i.e., a point) does not count as touching.
   for (i = 0; i < iNum; i++)
   {
      for (j = 0; j < iNum; j++)
      {
         if (i == j)
            continue;           // Do not check ourself

         if ( mp_do_shapes_touch(pOrphan[i].p34, pOrphan[j].p34) )
         {
            // if A touches B then B touches A. So add both.
            mp_add_shape_to_list(&(pOrphan[i]), j);
            mp_add_shape_to_list(&(pOrphan[j]), i);
         }
      }
   }

   // Check to see if there are any isolated shapes.
   // (If there are no connected shapes then shape is isolated.)
   for (i = 0; i < iNum; i++)
   {
      if (pOrphan[i].pShapes == NULL)
      {
         ret = 1;             // Isolated shape found
         break;
      }
   }

   // Now identify groups.
   if (ret == 0)
   {
      // Any shape that touches another shape belongs to the same group.
      //  0 = No orphan shapes found
      //  1 = Orphan shapes found
      ret = mp_id_groups(pOrphan, iNum);
   }

   //mp_debug_orphan_list(pOrphan, iNum);

   // Free
   for (i = 0; i < iNum; i++)
   {
      pShape = pOrphan[i].pShapes;
      while (pShape)
      {
          pShapeNext = pShape->pNext;
          SI_FREE(pShape, MP_SHAPE *);
          pShape = pShapeNext;
      }
   }
   SI_FREE(pOrphan, MP_TOUCH *);

   // Done
   return(ret);
}

// -------------------------------------------------
// Polygon to rectangle / right isosceles triangle

// Input is id_AreaList
// Output is g_pMesh 
// Return
//   0 = OK
//   1 = area is a void
//   2 = area not found
//   3 = area too small, no triangles/rectangles created
//   4 = orphan triangles/rectangles found
int mp_polygon2rt(float fTolerance, int AreaID)
{
	int res = 2; // ERROR - no such area

	g_pMesh = NULL;

	for (AllAreaList *pNet = id_AreaList; pNet; pNet = pNet->next)
	{
	   for (AreaList *pArea = pNet->areas; pArea; pArea = pArea->next)
	   {
			if (pArea->id == AreaID)
			{
				if (pArea->iType == RS_SOLID)
				{
					g_pMesh = MeshArea(pArea, fTolerance); // fTolerance is in the database units.
					if (g_pMesh)
                                        {
						res = 0; // OK
                                                if (mp_orphan_check() == 1)
                                                   res = 4; // orphan shape found

                                                mp_rect_aspect_check(); // rectangle aspect ratio check
                                        }
					else
						res = 3; // Other ERROR, for example, area is too small and no triangles/rectangles created
				}
				else
					res = 1; // ERROR - area is not SOLID but VOID
				break;
			}
	   }
	}

	return (res);
}

// -------------------------------------------------

void mp_free_S34()
{
    S34 *p34 = g_pMesh;
    while (p34)
    {
        S34 *pNext34 = p34->pNext;
        SI_FREE(p34, S34*);
        p34 = pNext34;
    }
    g_pMesh = NULL;
}

// -------------------------------------------------

