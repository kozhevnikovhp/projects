#ifndef __MESH_POLY_HPP_INCLUDED__
#define __MESH_POLY_HPP_INCLUDED__

typedef enum
{
	ORI_UNKNOWN,
	ORI_CW,
	ORI_CCW
} OrientationEnum;

typedef enum
{
	TYPE_UNKNOWN,
	TYPE_RECTANGLE,
	TYPE_NE,
	TYPE_NW,
	TYPE_SW,
	TYPE_SE
} TYPE_ENUM;

typedef struct S34
{
	float left, top, right, bottom; // 
	TYPE_ENUM type;
	S34 *pNext;
} S34;

typedef struct
{
	float x1, y1;
	int i1, j1;
	float x2, y2;
	int i2, j2;
	int bAlive; // flag showing if processing of this section is needed or not
	AreaList *pArea; // area created by this section

	int IsHorizontal()
	{
		return (j1 == j2);
	}
	int IsWE()
	{
		return IsHorizontal() && (i2 > i1);
	}
	int IsEW()
	{
		return IsHorizontal() && (i2 < i1);
	}
	int IsVertical()
	{
		return (i1 == i2);
	}
	int IsSN()
	{
		return IsVertical() && (j2 > j1);
	}
	int IsNS()
	{
		return IsVertical() && (j2 < j1);
	}
	int IsSWNE()
	{
		return ((i2-i1) == (j2-j1) && (i2 > i1));
	}
	int IsNESW()
	{
		return ((i2-i1) == (j2-j1) && (i1 > i2));
	}
	int IsNWSE()
	{
		return ((i2-i1) == (j1-j2) && (i2 > i1));
	}
	int IsSENW()
	{
		return ((i2-i1) == (j1-j2) && (i1 > i2));
	}
} SSection;

typedef struct
{
	SSection *pSections;
	int nSections, nAllocated;

	SSection *FindSection(int i1, int j1, int i2, int j2, AreaList *pArea);
	SSection *AllocNewSection(int i1, int j1, int i2, int j2, AreaList *pArea);
	SSection *NewSectionIfNotExist(int i1, int j1, int i2, int j2, AreaList *pArea);
} SSectionArray;

typedef struct
{
	int S, W, SWNE, SENW;
} SObstacle;

typedef struct
{
	SObstacle *pData;
	int MinI, MaxI;
	int MinJ, MaxJ;
	int SizeI, SizeJ;
	
	void UpdateBounds(SSection *pSection);
	SObstacle *GetObstacle(int i, int j);
	void AcceptSection(SSection *pSection);
	SObstacle *AllocData();
	void FreeData();
	
	SObstacle m_SuperObstacle; // ^-) this obstacle will be filled with all 1s for all kinds of obstacle to break growth in any direction,
							   // and returned when out-of bound obstacle will be queried (to avoid crash) instead of assert

} SObstacleArray2D;


typedef struct sTouching
{
        int id;                 // shape (from S34 *g_pMesh)
	S34 *p34;
        int iGroup;             // Group id that this shape belongs to
	struct sShape *pShapes;        // List of shapes touching this shape
} MP_TOUCH;


typedef struct sShape
{
        int id;                 // shape id
	struct sShape *pNext;
} MP_SHAPE;


extern SSectionArray g_Sections;
extern void CleanSections(SSectionArray *pSectionArray);
extern S34 *MeshArea(AreaList *pArea, float tolerance); // this method does all the work

#endif // __MESH_POLY_HPP_INCLUDED__
