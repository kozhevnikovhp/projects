//********************************************************************************
//********************************************************************************
//**										**
//**	File:		poly2rt.hpp						**
//**	Contents:	Polygon -> Rectangles & Right Isosceles Triangles	**
//**										**
//**	Author:		Guy de Burgh						**
//**										**
//**	Copyright (c) 2009 IBM                                                  **
//**	All rights, including trade secret rights, reserved.			**
//**										**
//********************************************************************************
//********************************************************************************

// --------------------------------------------------------

#define RS_LINE 0
#define RS_ARC 1
#define RS_CIRCLE 2
#define RS_RECTANGLE 3

#define RS_CW 0
#define RS_CCW 1

#define RS_SOLID 0
#define RS_VOID 1

// --------------------------------------------------------
// Structure Notes.
//   All structures are created the same way. With a pointer
//   to the head of the (top) of the list and a pointer to
//   the tail (bottom) of the list. The pointer to the tail
//   of the list has the extension "_b".
//   E.g. id_ShapeList and id_ShapeList_b
// --------------------------------------------------------

// All Shapes (from seg.txt)
// (id_ShapeList)
typedef struct cra_id_AS
{
   char	*sNetName;		// net name
   char	*sLyrName;		// layer name
   int iLA;                     // LINE, ARC or CIRCLE
   float fx1;
   float fy1;
   float fx2;
   float fy2;
   float fxc;                   // x center
   float fyc;                   // y center
   float fRad;                  // Radius
   float fUnk;                  // ? (not used)
   int iCCW;                    // Clockwise or CounterClockwise (for ARCs)
   int iType;                   // SOLID or VOID
   struct cra_id_AS *next;
} ID_ALLSHAPES;

// --------------------------------------------

// This is the above structure re-ordered.
// Ordered by Net.
// ARCs have been converted to LINEs.
// The boundary rectangle is determined.
// The number of line segments is determined.
// The vertex list (segver) is a closed loop, meaning that the first
//  point of the first entry (fx1, fy1) is the same as the last point
//  of the last entry (fx2, fy2).
// (id_AreaList)
typedef struct AllAreaList
{
   char	*sNetName;		// net name
   struct AreaList *areas;      // List of areas on this net
   struct AllAreaList *next;
} ID_ALLAREAS;

typedef struct AreaList
{
   int id;                      // Internal id
   char	*sLyrName;		// layer name
   int iType;                   // SOLID or VOID
   int iNumVer;                 // Number of entries in segver
   float fLLx;                  // Boundary rectangle
   float fLLy;
   float fURx;
   float fURy;
   struct VerList *segver;      // List of line segment vertices
   struct VoidList *voids;      // List of voids inside this area
   struct AreaList *next;
} ID_AREAS;

typedef struct VerList
{
   float fx1;
   float fy1;
   float fx2;
   float fy2;
   struct VerList *next;
} ID_VERS;

typedef struct VoidList
{
   struct AreaList *voidarea;
   struct VoidList *next;
} ID_VOIDS;

// --------------------------------------------

// All Layers (from lay.txt)
// (id_LayerList)
typedef struct cra_id_AL
{
   char	*sName;		        // layer name
   int iIndx;                   // Layer Index
   struct cra_id_AL *next;
} ID_ALLLAYERS;

// --------------------------------------------

// Board Outline (from outline.txt)
// (id_OutLnList)
typedef struct cra_id_OL
{
   int iLA;                     // LINE, ARC, CIRCLE or RECTANGLE
   float fx1;
   float fy1;
   float fx2;
   float fy2;
   float fxc;                   // x center
   float fyc;                   // y center
   float fRad;                  // Radius
   int iCCW;                    // Clockwise or CounterClockwise (for ARCs)
   int iUsed;                   // 1 = read and used this line
   struct cra_id_OL *next;
} ID_ALLOUTLINE;

// --------------------------------------------

// This is the above structure where each area has been properly identified.
// (id_OLAreaList)
typedef struct OLAreaList
{
   float first_x;               // First coord in Edge list
   float first_y;
   float last_x;                // Last coord in Edge list
   float last_y;
   bool bOE;                    // true if this area is the outside edge
   double fArea;                // Area
   struct OLEdgeList *edge;     // pointer to first entry in list of edges
   struct OLEdgeList *edge_b;   // pointer to last entry in list of edges
   struct OLAreaList *next;
} ID_OLAREA;

typedef struct OLEdgeList
{
   int iLA;                     // LINE, ARC, CIRCLE or RECTANGLE
   float fx1;
   float fy1;
   float fx2;
   float fy2;
   float fxc;                   // x center
   float fyc;                   // y center
   float fRad;                  // Radius
   int iCCW;                    // Clockwise or CounterClockwise (for ARCs)
   struct OLEdgeList *next;
} ID_OLEDGE;

// --------------------------------------------------------

extern int mp_polygon2rt(float fTolerance, int AreaID);
extern void mp_free_S34();

// --------------------------------------------------------
