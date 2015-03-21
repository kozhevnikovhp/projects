//********************************************************************************
//********************************************************************************
//**										**
//**	File:		write_isf.cpp						**
//**	Contents:	Write ISF file                                    	**
//**										**
//**	Author:		Guy de Burgh						**
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

#define RS_LINE 0
#define RS_ARC 1
#define RS_CW 0
#define RS_CCW 1
#define RS_SOLID 0
#define RS_VOID 1

#define RS_BEGIN 0
#define RS_END 1

// -------------------------------------------------
// Forward declarations

int wi_write_isf(char *sPath, int AreaID);
void wi_write_area(ID_AREAS *al, double Scale, FILE *fpI);
void wi_write_rt(ID_AREAS *al, double Scale, FILE *fpI);
void wi_get_scale(double &Scale, int &iUPI);
long wi_scale_units(double x, double units);

int wi_read_layer(char *sPath);
int wi_add_to_layer_list(char *sLyrName, char *sLyrIndx);
void wi_debug_layer_list();
int wi_get_layer_index(char *sName);
void wi_layer_free();

int wi_read_outline(char *sPath);
int wi_add_to_outline_list(char *sLA, char *sX1, char *sY1, char *sX2, char *sY2, char *sXc, char *sYc, char *sRad, char *sCCW);
void wi_debug_outline_list();
void wi_outline_free();

void wi_id_shapes();
int wi_reorder_save_area();
int wi_reorder_save_edge(int iLA, float fx1, float fy1, float fx2, float fy2, float fxc, float fyc, float fRad, int iCCW, int iBE);

void wi_convert_shapes();
void wi_debug_reorder_shapes();
int wi_add_new_area(char *sNetName);
void wi_update_shapes();
void wi_debug_reorder_outline_list();
int wi_countUnused();

void wi_identify_oe();
double wi_CalculateArea(ID_OLAREA *area);
double wi_CalcArea(ID_AREAS *area);

// -------------------------------------------------
// External declarations

extern void error_message(char *sEW, char *str);
extern void error_log(char *str);
extern int rs_add_new_segment(float fx1, float fy1, float fx2, float fy2);
extern void rs_convert_arc_to_segs(float fx1, float fy1, float fx2, float fy2, float fxc, float fyc, float fRad, int iCCW);
extern void rs_convert_circle_to_segs(float fxc, float fyc, float fRad);
extern void rs_convert_rectangle_to_segs(float fx1, float fy1, float fx2, float fy2);

// -------------------------------------------------
// Global declarations

ID_ALLLAYERS *id_LayerList;
ID_ALLLAYERS *id_LayerList_b;

ID_ALLOUTLINE *id_OutLnList;
ID_ALLOUTLINE *id_OutLnList_b;

ID_OLAREA *id_OLAreaList;
ID_OLAREA *id_OLAreaList_b;

ID_ALLAREAS *id_OutlineList;
ID_ALLAREAS *id_OutlineList_b;
extern ID_AREAS *current_al;

extern ID_ALLAREAS *id_AreaList;
extern char sUnits[SI_SMLINE];

extern char sLogStr[];
extern FILE *fp_log;
extern int test_code;

extern S34 *g_pMesh; // meshed area as list of right isosceles triangles and rectangles

// -------------------------------------------------
// Write ISF file

int wi_write_isf(char *sPath, int AreaID)
{
#if 1
   float delta;
#else
   int iNumVer;
   ID_OLAREA *oal;
   ID_OLEDGE *osl;
#endif
   int iNum;
   int iFirst;
   float fLLx, fLLy, fURx, fURy;
   long LLx, LLy, URx, URy;
   double Scale = 1.0;
   int iUPI = 254000;
   char sIsfFile[SI_SMLINE];
   char sAreaIDFile[SI_SMLINE];
   double dArea;
   ID_ALLAREAS *as;
   ID_AREAS *al;
   ID_VOIDS *vd;
   FILE *fpI;
   FILE *fpA;

   // Update Scale based on seg.txt units
   wi_get_scale(Scale, iUPI);

   sprintf(sIsfFile, "%s/seg_areas.isf", sPath);
   fpI = fopen(sIsfFile, "w");
   if (fpI == NULL)
   {
      error_message("E01", sIsfFile);		// unable to write file
      return(-1);
   }

   // Header
   fprintf(fpI, "ISF: 2.0\n");
   fprintf(fpI, "I: Allegro\n");
   fprintf(fpI, "U: %d\n", iUPI);
   fprintf(fpI, "W: 2540\n");
   fprintf(fpI, "D: 7112\n");
   fprintf(fpI, "Q: 7112\n");

   // Board outline
#if 0

   iNumVer = 0;
   for (oal = id_OLAreaList; oal != NULL; oal = oal->next)
   {
      if (oal->bOE)
      {
         for (osl = oal->edge; osl != NULL; osl = osl->next)
         {
            iNumVer++;
         }
      }
   }

   fLLx = 0.0;
   fLLy = 0.0;
   fURx = 0.0;
   fURy = 0.0;
   iFirst = 0;
   for (oal = id_OLAreaList; oal != NULL; oal = oal->next)
   {
      if ( !(oal->bOE) )
         continue;

      for (osl = oal->edge; osl != NULL; osl = osl->next)
      {
         if (iFirst == 0)
         {
            fLLx = osl->fx1;
            fLLy = osl->fy1;
            fURx = osl->fx2;
            fURy = osl->fy2;
            iFirst = 1;
         }

         if (fLLx > osl->fx1)
            fLLx = osl->fx1;

         if (fLLx > osl->fx2)
            fLLx = osl->fx2;

         if (fLLy > osl->fy1)
            fLLy = osl->fy1;

         if (fLLy > osl->fy2)
            fLLy = osl->fy2;

         if (fURx < osl->fx1)
            fURx = osl->fx1;

         if (fURx < osl->fx2)
            fURx = osl->fx2;

         if (fURy < osl->fy1)
            fURy = osl->fy1;

         if (fURy < osl->fy2)
            fURy = osl->fy2;
      }

      LLx = wi_scale_units(fLLx, Scale);
      LLy = wi_scale_units(fLLy, Scale);
      URx = wi_scale_units(fURx, Scale);
      URy = wi_scale_units(fURy, Scale);
      fprintf(fpI, "B: %d,%d,%d,%d,%d\n", (iNumVer + 1), LLx, LLy, URx, URy);

      for (osl = oal->edge; osl != NULL; osl = osl->next)
      {
         LLx = wi_scale_units(osl->fx1, Scale);
         LLy = wi_scale_units(osl->fy1, Scale);
         fprintf(fpI, "VER: %d,%d\n", LLx, LLy);
      }

      // Last entry is the first entry
      osl = oal->edge;
      LLx = wi_scale_units(osl->fx1, Scale);
      LLy = wi_scale_units(osl->fy1, Scale);
      fprintf(fpI, "VER: %d,%d\n", LLx, LLy);
   }

#else

   // Make this the boundary rectangle that contains all the areas,
   // then add 10%

   fLLx = 0.0;
   fLLy = 0.0;
   fURx = 0.0;
   fURy = 0.0;
   iFirst = 0;
   for (as = id_AreaList; as != NULL; as = as->next)
   {
      for (al = as->areas; al != NULL; al = al->next)
      {
         if (iFirst == 0)
         {
            fLLx = al->fLLx;
            fLLy = al->fLLy;
            fURx = al->fURx;
            fURy = al->fURy;
            iFirst = 1;
         }

         if (fLLx > al->fLLx)
            fLLx = al->fLLx;

         if (fLLy > al->fLLy)
            fLLy = al->fLLy;

         if (fURx < al->fURx)
            fURx = al->fURx;

         if (fURy < al->fURy)
            fURy = al->fURy;
      }
   }

   delta = (fURx - fLLx) * (float) 0.05;
   fURx += delta;
   fLLx -= delta;

   delta = (fURy - fLLy) * (float) 0.05;
   fURy += delta;
   fLLy -= delta;

   LLx = wi_scale_units(fLLx, Scale);
   LLy = wi_scale_units(fLLy, Scale);
   URx = wi_scale_units(fURx, Scale);
   URy = wi_scale_units(fURy, Scale);
   fprintf(fpI, "B: 5,%d,%d,%d,%d\n", LLx, LLy, URx, URy);
   fprintf(fpI, "VER: %d,%d\n", LLx, LLy);
   fprintf(fpI, "VER: %d,%d\n", LLx, URy);
   fprintf(fpI, "VER: %d,%d\n", URx, URy);
   fprintf(fpI, "VER: %d,%d\n", URx, LLy);
   fprintf(fpI, "VER: %d,%d\n", LLx, LLy);
#endif

   // Nets
   if (AreaID != -1)
   {
      // one area only
      for (as = id_AreaList; as != NULL; as = as->next)
      {
         for (al = as->areas; al != NULL; al = al->next)
         {
            if (al->id == AreaID)
            {
               fprintf(fpI, "N: %s\n", as->sNetName);
               wi_write_area(al, Scale, fpI);           // Original area

               // write out voids in this area
               for (vd = al->voids; vd != NULL; vd = vd->next)
               {
                  wi_write_area(vd->voidarea, Scale, fpI);
               }

               wi_write_rt(al, Scale, fpI);             // Rectangles/triangles
               break;
            }
         }
      }
   }
   else
   {
      // all areas
      for (as = id_AreaList; as != NULL; as = as->next)
      {
         fprintf(fpI, "N: %s\n", as->sNetName);

         for (al = as->areas; al != NULL; al = al->next)
         {
            wi_write_area(al, Scale, fpI);
         }
      }

      // AreaID list
      sprintf(sAreaIDFile, "%s/areaID.txt", sPath);
      fpA = fopen(sAreaIDFile, "w");
      fprintf(fpA, "AreaID            Net                         Layer     Num Voids  Num Vertices  Area\n"); 
      fprintf(fpA, "======            ===                         =====     =========  ============  ====\n"); 

      for (as = id_AreaList; as != NULL; as = as->next)
      {
         for (al = as->areas; al != NULL; al = al->next)
         {
            if (al->iType == RS_VOID)
               continue;

            iNum = 0;
            for (vd = al->voids; vd != NULL; vd = vd->next)
            {
               iNum++;
            }

            dArea = wi_CalcArea(al);
            fprintf(fpA, "%5d %30s %15s     %5d       %5d       %.2f\n", al->id, as->sNetName, al->sLyrName, iNum, al->iNumVer, dArea);
         }
      }

      fclose(fpA);
   }

   fclose(fpI);

   return(0);
}


void wi_write_area(ID_AREAS *al, double Scale, FILE *fpI)
{
   int iLayer;
   long LLx, LLy, URx, URy;
   ID_VERS *vs;

   LLx = wi_scale_units(al->fLLx, Scale);
   LLy = wi_scale_units(al->fLLy, Scale);
   URx = wi_scale_units(al->fURx, Scale);
   URy = wi_scale_units(al->fURy, Scale);
   iLayer = wi_get_layer_index(al->sLyrName);
   fprintf(fpI, "A: %d,%d,%d,%d,%d,%d,SOLID,", (al->iNumVer + 1), LLx, LLy, URx, URy, iLayer);

   if (al->iType == RS_SOLID)
      fprintf(fpI, "POS\n");
   else
      fprintf(fpI, "NEG\n");

   for (vs = al->segver; vs != NULL; vs = vs->next)
   {
      LLx = wi_scale_units(vs->fx1, Scale);
      LLy = wi_scale_units(vs->fy1, Scale);
      fprintf(fpI, "VER: %d,%d\n", LLx, LLy);
   }

   // Last entry is the first entry
   vs = al->segver;
   LLx = wi_scale_units(vs->fx1, Scale);
   LLy = wi_scale_units(vs->fy1, Scale);
   fprintf(fpI, "VER: %d,%d\n", LLx, LLy);
}


void wi_write_rt(ID_AREAS *al, double Scale, FILE *fpI)
{
   int iLayer;
   long left, top, right, bottom;
   long LLx, LLy, URx, URy;

   S34 *p34 = g_pMesh; // that is head of linked list returned from MeshArea() 
   if (p34 == NULL)
      return;

   iLayer = wi_get_layer_index(al->sLyrName);

   // Write out rectangles/triangles
   while (p34)
   {
      left = wi_scale_units((double) p34->left, Scale);
      top = wi_scale_units((double) p34->top, Scale);
      right = wi_scale_units((double) p34->right, Scale);
      bottom = wi_scale_units((double) p34->bottom, Scale);

      // I suspect that "left, top, right, bottom" = minX, maxY, maxX, minY
      // So may not need this as "left" is probably < "right", and "bottom" < "top"
      LLx = left;
      URx = right;
      if (right < left)
      {
         LLx = right;
         URx = left;
      }

      LLy = bottom;
      URy = top;
      if (top < bottom)
      {
         LLy = top;
         URy = bottom;
      }

      switch (p34->type)
      {
         case TYPE_RECTANGLE:
            // LT->LB->RB->RT->LT
            fprintf(fpI, "A: 5,%d,%d,%d,%d,%d,SOLID,POS\n", LLx, LLy, URx, URy, iLayer);
            fprintf(fpI, "VER: %d,%d\n", left, top);
            fprintf(fpI, "VER: %d,%d\n", left, bottom);
            fprintf(fpI, "VER: %d,%d\n", right, bottom);
            fprintf(fpI, "VER: %d,%d\n", right, top);
            fprintf(fpI, "VER: %d,%d\n", left, top);
         break;
         case TYPE_NW:
            // LT->LB->RT->LT
            fprintf(fpI, "A: 4,%d,%d,%d,%d,%d,SOLID,POS\n", LLx, LLy, URx, URy, iLayer);
            fprintf(fpI, "VER: %d,%d\n", left, top);
            fprintf(fpI, "VER: %d,%d\n", left, bottom);
            fprintf(fpI, "VER: %d,%d\n", right, top);
            fprintf(fpI, "VER: %d,%d\n", left, top);
         break;
         case TYPE_NE:
            // LT->RB->RT->LT
            fprintf(fpI, "A: 4,%d,%d,%d,%d,%d,SOLID,POS\n", LLx, LLy, URx, URy, iLayer);
            fprintf(fpI, "VER: %d,%d\n", left, top);
            fprintf(fpI, "VER: %d,%d\n", right, bottom);
            fprintf(fpI, "VER: %d,%d\n", right, top);
            fprintf(fpI, "VER: %d,%d\n", left, top);
         break;
         case TYPE_SE:
            // RT->LB->RB->RT
            fprintf(fpI, "A: 4,%d,%d,%d,%d,%d,SOLID,POS\n", LLx, LLy, URx, URy, iLayer);
            fprintf(fpI, "VER: %d,%d\n", right, top);
            fprintf(fpI, "VER: %d,%d\n", left, bottom);
            fprintf(fpI, "VER: %d,%d\n", right, bottom);
            fprintf(fpI, "VER: %d,%d\n", right, top);
         break;
         case TYPE_SW:
            // RB->LB->LT->RB
            fprintf(fpI, "A: 4,%d,%d,%d,%d,%d,SOLID,POS\n", LLx, LLy, URx, URy, iLayer);
            fprintf(fpI, "VER: %d,%d\n", right, bottom);
            fprintf(fpI, "VER: %d,%d\n", left, bottom);
            fprintf(fpI, "VER: %d,%d\n", left, top);
            fprintf(fpI, "VER: %d,%d\n", right, bottom);
         break;
      }

      p34 = p34->pNext;
   }
}

// -------------------------------------------------

// To be updated...
void wi_get_scale(double &Scale, int &iUPI)
{
   if (strcmp(sUnits, "mils") == 0)
   {
      Scale = 0.0001;
      iUPI = 254000;
   }
   else if ( (strcmp(sUnits, "milimeters") == 0) || (strcmp(sUnits, "mm") == 0) )
   {
      Scale = 0.0001;
      iUPI = 254000;
   }
   else if (strcmp(sUnits, "inches") == 0)
   {
      Scale = 0.0001;
      iUPI = 254000;
   }
   else
   {
      Scale = 0.0001;
      iUPI = 254000;
   }
}

long wi_scale_units(double x, double units)
{
    long y;
    double z;

    z = (double)(x / units);
    if (x < 0)
        z -= (double)0.5;
    else
        z += (double)0.5;

    y = (long)z;
    return (y);
}

// -------------------------------------------------

int wi_read_layer(char *sPath)
{
   int i, j, k;
   int res = 0;
   char sLine[SI_MAXLINE];
   char sLyrFile[SI_SMLINE];
   char sLyrName[SI_SMLINE];
   char sLyrIndx[SI_SMLINE];
   FILE *fp2;

   // Read lay.txt
   sprintf(sLyrFile, "%s/lay.txt", sPath);
   fp2 = fopen(sLyrFile, "r");
   if (fp2 == NULL)
   {
      error_message("E02", sLyrFile);		// unable to read file
      return(-1);
   }

   id_LayerList = NULL;
   id_LayerList_b = NULL;

   // S!GND1!003!YES!4.500000!595900.000000 mho/cm!COPPER!YES!1.440000 mil!EMBEDDED_PLANE!
   //
   // posn           Value
   // ----           -----
   //   1            Layer name
   //   2            Layer index

   while (fgets(sLine, SI_MAXLINE, fp2) != NULL)
   {
      if (sLine[0] == 'S')
      {
         sLyrName[0] = '\0';
         sLyrIndx[0] = '\0';

         for (i = 0, j = 0, k = 0; i < (int) strlen(sLine); i++)
         {
	    if (sLine[i] == '!')
	    {
	       j++;
	       k = 0;
	    }
	    else
	    {
	       if (j == 1)
	       {
	          sLyrName[k] = sLine[i];
	          k++;
	          sLyrName[k] = '\0';
	       }
	       else if (j == 2)
	       {
	          sLyrIndx[k] = sLine[i];
	          k++;
	          sLyrIndx[k] = '\0';
	       }
            }
         }

         res = wi_add_to_layer_list(sLyrName, sLyrIndx);
      }
   }

   fclose(fp2);

   if (test_code == 4)
      wi_debug_layer_list();

   return(res);
}


int wi_add_to_layer_list(char *sLyrName, char *sLyrIndx)
{
   int iN;
   int iIndx;
   ID_ALLLAYERS *new_lb;

   /* Create structure */
   (ID_ALLLAYERS *) SI_ALLOC_STRUCT2(ID_ALLLAYERS, new_lb);

   if (new_lb == NULL)
      return(-1);

   if (sLyrIndx[0] != '\0')
      iN = sscanf(sLyrIndx, "%d", &iIndx);
   else
      iIndx = 0;

   new_lb->sName = SI_strsave(sLyrName);
   new_lb->iIndx = iIndx;
   new_lb->next = NULL;

   /* Link */

   if (id_LayerList == NULL)
   {
      id_LayerList = new_lb;
      id_LayerList_b = new_lb;
   }
   else
   {
      id_LayerList_b->next = new_lb;
      id_LayerList_b = new_lb;
   }

   return(0);
}


void wi_debug_layer_list()
{
   ID_ALLLAYERS *sl;

   fprintf(fp_log, "\n");
   fprintf(fp_log, "Layers\n");

   for (sl = id_LayerList; sl != NULL; sl = sl->next)
   {
      if (sl->sName[0] != '\0')
         fprintf(fp_log, "%3d %s\n", sl->iIndx, sl->sName);
      else
         fprintf(fp_log, "%3d (no name)\n", sl->iIndx);
   }

   fprintf(fp_log, "\n");
}


int wi_get_layer_index(char *sName)
{
   ID_ALLLAYERS *sl;

   for (sl = id_LayerList; sl != NULL; sl = sl->next)
   {
      if (strcmp(sl->sName, sName) == 0)
         return(sl->iIndx);
   }

   return(0);
}


void wi_layer_free()
{
   ID_ALLLAYERS *sl, *sln;

   sl = id_LayerList;
   while (sl != NULL)
   {
      sln = sl->next;
      free(sl);
      sl = sln;
   }
}

// -------------------------------------------------

int wi_read_outline(char *sPath)
{
   int i, j, k;
   int res = 0;
   char sLine[SI_MAXLINE];
   char sOutFile[SI_SMLINE];
   char sLA[SI_SMLINE];
   char sX1[SI_SMLINE];
   char sY1[SI_SMLINE];
   char sX2[SI_SMLINE];
   char sY2[SI_SMLINE];
   char sXc[SI_SMLINE];
   char sYc[SI_SMLINE];
   char sRad[SI_SMLINE];
   char sCCW[SI_SMLINE];
   FILE *fp2;

   // Read outline.txt
   sprintf(sOutFile, "%s/outline.txt", sPath);
   fp2 = fopen(sOutFile, "r");
   if (fp2 == NULL)
   {
      error_message("E02", sOutFile);		// unable to read file
      return(-1);
   }

   id_OutLnList = NULL;
   id_OutLnList_b = NULL;

   // S!BOARD GEOMETRY!OUTLINE!LINE!5324.410!54.720!5324.410!105.910!0.000!!!!!NOTCONNECT!
   // S!BOARD GEOMETRY!OUTLINE!ARC!5324.410!54.720!5393.310!-14.170!5393.300!54.720!68.890!0.000!COUNTERCLOCKWISE!NOTCONNECT!
   //
   // posn           Value
   // ----           -----
   //   3            LINE | ARC | [CIRCLE, RECTANGLE]
   //   4            x1
   //   5            y1
   //   6            x2
   //   7            y2
   //   8            xc
   //   9            y3
   //   10           radius
   //   11           ? (ignore)
   //   12           CLOCKWISE | COUNTERCLOCKWISE (if 3 = ARC)

   while (fgets(sLine, SI_MAXLINE, fp2) != NULL)
   {
      if (sLine[0] == 'S')
      {
         sLA[0] = '\0';
         sX1[0] = '\0';
         sY1[0] = '\0';
         sX2[0] = '\0';
         sY2[0] = '\0';
         sXc[0] = '\0';
         sYc[0] = '\0';
         sRad[0] = '\0';
         sCCW[0] = '\0';

         for (i = 0, j = 0, k = 0; i < (int) strlen(sLine); i++)
         {
	    if (sLine[i] == '!')
	    {
	       j++;
	       k = 0;
	    }
	    else
	    {
	       if (j == 3)
	       {
	          sLA[k] = sLine[i];
	          k++;
	          sLA[k] = '\0';
	       }
	       else if (j == 4)
	       {
	          sX1[k] = sLine[i];
	          k++;
	          sX1[k] = '\0';
	       }
	       else if (j == 5)
	       {
	          sY1[k] = sLine[i];
	          k++;
	          sY1[k] = '\0';
	       }
	       else if (j == 6)
	       {
	          sX2[k] = sLine[i];
	          k++;
	          sX2[k] = '\0';
	       }
	       else if (j == 7)
	       {
	          sY2[k] = sLine[i];
	          k++;
	          sY2[k] = '\0';
	       }
	       else if (j == 8)
	       {
	          sXc[k] = sLine[i];
	          k++;
	          sXc[k] = '\0';
	       }
	       else if (j == 9)
	       {
	          sYc[k] = sLine[i];
	          k++;
	          sYc[k] = '\0';
	       }
	       else if (j == 10)
	       {
	          sRad[k] = sLine[i];
	          k++;
	          sRad[k] = '\0';
	       }
	       else if (j == 12)
	       {
	          sCCW[k] = sLine[i];
	          k++;
	          sCCW[k] = '\0';
	       }
            }
         }

         res = wi_add_to_outline_list(sLA, sX1, sY1, sX2, sY2, sXc, sYc, sRad, sCCW);
      }
   }

   fclose(fp2);

   if (test_code == 5)
      wi_debug_outline_list();

   // Identify and individualize each area
   // Populates structure id_OLAreaList
   wi_id_shapes();

   if (test_code == 5)
      wi_debug_reorder_shapes();

#if 0
   // Convert ARCs, CIRCLEs and RECTANGLEs to LINEs
   wi_convert_shapes();

   // Update Number of vertices and Boundary rectange
   wi_update_shapes();

   if (test_code == 5)
      wi_debug_reorder_outline_list();
#endif

   // Identify Outside Edge (if more that one outline identified)
   wi_identify_oe();

   if (test_code == 5)
      wi_debug_reorder_shapes();

   return(res);
}


int wi_add_to_outline_list(char *sLA, char *sX1, char *sY1, char *sX2, char *sY2,
      char *sXc, char *sYc, char *sRad, char *sCCW)
{
   int iN;
   int iLA;
   int iCCW;
   float fx1, fy1;
   float fx2, fy2;
   float fxc, fyc;
   float fRad;
   ID_ALLOUTLINE *new_lb;

   /* Create structure */
   (ID_ALLOUTLINE *) SI_ALLOC_STRUCT2(ID_ALLOUTLINE, new_lb);

   if (new_lb == NULL)
      return(-1);

   if (strcmp(sLA, "LINE") == 0)
      iLA = RS_LINE;
   else if (strcmp(sLA, "ARC") == 0)
      iLA = RS_ARC;
   else if (strcmp(sLA, "CIRCLE") == 0)
      iLA = RS_CIRCLE;
   else if (strcmp(sLA, "RECTANGLE") == 0)
      iLA = RS_RECTANGLE;
   else
      iLA = RS_LINE;

   if (strcmp(sCCW, "COUNTERCLOCKWISE") == 0)
      iCCW = RS_CCW;
   else
      iCCW = RS_CW;

   if (sX1[0] != '\0')
      iN = sscanf(sX1, "%f", &fx1);
   else
      fx1 = 0.0;

   if (sY1[0] != '\0')
      iN = sscanf(sY1, "%f", &fy1);
   else
      fy1 = 0.0;

   if (sX2[0] != '\0')
      iN = sscanf(sX2, "%f", &fx2);
   else
      fx2 = 0.0;

   if (sY2[0] != '\0')
      iN = sscanf(sY2, "%f", &fy2);
   else
      fy2 = 0.0;

   if (sXc[0] != '\0')
      iN = sscanf(sXc, "%f", &fxc);
   else
      fxc = 0.0;

   if (sYc[0] != '\0')
      iN = sscanf(sYc, "%f", &fyc);
   else
      fyc = 0.0;

   if (sRad[0] != '\0')
      iN = sscanf(sRad, "%f", &fRad);
   else
      fRad = 0.0;

   new_lb->iLA = iLA;
   new_lb->fx1 = fx1;
   new_lb->fy1 = fy1;
   new_lb->fx2 = fx2;
   new_lb->fy2 = fy2;
   new_lb->fxc = fxc;
   new_lb->fyc = fyc;
   new_lb->fRad = fRad;
   new_lb->iCCW = iCCW;
   new_lb->iUsed = 0;
   new_lb->next = NULL;

   /* Link */

   if (id_OutLnList == NULL)
   {
      id_OutLnList = new_lb;
      id_OutLnList_b = new_lb;
   }
   else
   {
      id_OutLnList_b->next = new_lb;
      id_OutLnList_b = new_lb;
   }

   return(0);
}


void wi_debug_outline_list()
{
   ID_ALLOUTLINE *sl;

   fprintf(fp_log, "\n");
   fprintf(fp_log, "Outline (org)\n");
   for (sl = id_OutLnList; sl != NULL; sl = sl->next)
   {
      if (sl->iLA == RS_LINE)
      {
         fprintf(fp_log, "LINE ");
      }
      else if (sl->iLA == RS_ARC)
      {
         fprintf(fp_log, "ARC ");

         if (sl->iCCW == RS_CCW)
            fprintf(fp_log, "COUNTERCLOCKWISE ");
         else
            fprintf(fp_log, "CLOCKWISE ");
      }
      else if (sl->iLA == RS_CIRCLE)
      {
         fprintf(fp_log, "CIRCLE ");
      }
      else if (sl->iLA == RS_RECTANGLE)
      {
         fprintf(fp_log, "RECTANGLE ");
      }

      fprintf(fp_log, "(%f %f) (%f %f) (%f %f  %f)\n",
            sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad);
   }

   fprintf(fp_log, "\n");
}


void wi_outline_free()
{
   ID_ALLOUTLINE *sl, *sln;

   sl = id_OutLnList;
   while (sl != NULL)
   {
      sln = sl->next;
      free(sl);
      sl = sln;
   }
}

// -------------------------------------------------

// Identify and individualize each area
// Populates structure id_OLAreaList
void wi_id_shapes()
{
   //int iStartOfArea;
   int iEndOfArea;
   int iSafety;
   int iSegmentAdded;
   int iMakeNewArea;
   bool bNotDone;
   ID_ALLOUTLINE *sl;

   id_OLAreaList = NULL;
   id_OLAreaList_b = NULL;

   // First time through 
   // Identify "one line" voids (circles and rectangles)
   for (sl = id_OutLnList; sl != NULL; sl = sl->next)
   {
      if ( (sl->iLA == RS_ARC) && (sl->fx1 == sl->fx2) && (sl->fy1 == sl->fy2) )
      {
         // If ARC and first and last points are the same => circle

         // Make a new area
         wi_reorder_save_area();
         // Save feature that makes the area
         wi_reorder_save_edge(sl->iLA, sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW, RS_BEGIN);

         sl->iUsed = 1;         // mark line as used
         iEndOfArea = 1;
      }
      else if (sl->iLA == RS_CIRCLE)
      {
         // CIRCLE

         // Make a new area
         wi_reorder_save_area();
         // Save feature that makes the area
         wi_reorder_save_edge(sl->iLA, sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW, RS_BEGIN);

         sl->iUsed = 1;         // mark line as used
         iEndOfArea = 1;
      }
      else if (sl->iLA == RS_RECTANGLE)
      {
         // RECTANGLE

         // Make a new area
         wi_reorder_save_area();
         // Save feature that makes the area
         wi_reorder_save_edge(sl->iLA, sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW, RS_BEGIN);

         sl->iUsed = 1;         // mark line as used
         iEndOfArea = 1;
      }
   }

   // Reset iMakeNewArea flag
   // If first time through
   //   make a new area
   //   add first segment to that area
   //   mark it as used
   // If all other times through
   //   If iMakeNewArea flag is set
   //      make a new area
   //   Reset iSsegmentAdded flag
   //   Is segment connected to either end of first or last segment in current area list?
   //   If yes, add it to the start of the current area list if segment is connected
   //     to the first, or to the end if the segment is connected to the end.
   //     Mark segment as used.
   //     Set iSsegmentAdded flag
   //   If no,
   //     do nothing
   //   If iSegmentAdded flag is not set
   //      Set iMakeNewArea flag
   //   else
   //      Reset iMakeNewArea flag
   //
   //   Have all segments been accounted for?
   //   If yes, exit
   //   else, Go to next segment
   //
   iSafety = wi_countUnused();
   iSafety *= iSafety;

   iMakeNewArea = 1;

   bNotDone = true;
   while(bNotDone)
   {
      // Assume no segment will be added
      iSegmentAdded = 0;

      for (sl = id_OutLnList; sl != NULL; sl = sl->next)
      {
         if (sl->iUsed == 1)
            continue;

         if (test_code == 5)
            fprintf(fp_log, "Processing = (%f %f) (%f %f)\n", sl->fx1, sl->fy1, sl->fx2, sl->fy2);

         if (iMakeNewArea == 1)
         {
            // If first time through, or make a new area is required
            // Make a new area
            if (test_code == 5)
               fprintf(fp_log, "New area\n");

            wi_reorder_save_area();

            // Save first edge
            if (test_code == 5)
               fprintf(fp_log, "First edge\n");

            wi_reorder_save_edge(sl->iLA, sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW, RS_BEGIN);
            iSegmentAdded = 1;       // set segment added flag
            sl->iUsed = 1;           // mark line as used
            iMakeNewArea = 0;
         }
         else
         {
            if ( ((sl->fx1 == id_OLAreaList_b->edge->fx1) && (sl->fy1 == id_OLAreaList_b->edge->fy1))
              || ((sl->fx1 == id_OLAreaList_b->edge->fx2) && (sl->fy1 == id_OLAreaList_b->edge->fy2))
              || ((sl->fx2 == id_OLAreaList_b->edge->fx1) && (sl->fy2 == id_OLAreaList_b->edge->fy1))
              || ((sl->fx2 == id_OLAreaList_b->edge->fx2) && (sl->fy2 == id_OLAreaList_b->edge->fy2)) )
            {
               // Segment is connected to the first segment in current area list
               // Save edge at beginning of list
               if (test_code == 5)
                  fprintf(fp_log, "Connected to first\n");

               wi_reorder_save_edge(sl->iLA, sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW, RS_BEGIN);
               iSegmentAdded = 1;       // set segment added flag
               sl->iUsed = 1;           // mark line as used
            }
            else if ( ((sl->fx1 == id_OLAreaList_b->edge_b->fx1) && (sl->fy1 == id_OLAreaList_b->edge_b->fy1))
                   || ((sl->fx1 == id_OLAreaList_b->edge_b->fx2) && (sl->fy1 == id_OLAreaList_b->edge_b->fy2))
                   || ((sl->fx2 == id_OLAreaList_b->edge_b->fx1) && (sl->fy2 == id_OLAreaList_b->edge_b->fy1))
                   || ((sl->fx2 == id_OLAreaList_b->edge_b->fx2) && (sl->fy2 == id_OLAreaList_b->edge_b->fy2)) )
            {
               // Segment is connected to the last segment in current area list
               // Save edge at end of list
               if (test_code == 5)
                  fprintf(fp_log, "Connected to last\n");

               wi_reorder_save_edge(sl->iLA, sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW, RS_END);
               iSegmentAdded = 1;       // set segment added flag
               sl->iUsed = 1;           // mark line as used
            }
         }

         if (test_code == 5)
         {
            wi_debug_reorder_shapes();
            fprintf(fp_log, "\n");
         }
      }

      if (iSegmentAdded == 0)
         iMakeNewArea = 1;      // No segment added so start a new area
      else
         iMakeNewArea = 0;

      if (wi_countUnused == 0)
         bNotDone = false;

      // So that we don't loop forever
      iSafety--;
      if (iSafety < 0)
         bNotDone = false;
   }
}


// Count of lines not yet used.
int wi_countUnused()
{
   int i;
   ID_ALLOUTLINE *sl;

   i = 0;
   for (sl = id_OutLnList; sl != NULL; sl = sl->next)
   {
      if (sl->iUsed == 0)
      {
         i++;
      }
   }

   return(i);
}


// Add to end of list
// id_OLAreaList_b will point to this just added structure
int wi_reorder_save_area()
{
   ID_OLAREA *new_as;

   // Create new area
   (ID_OLAREA *) SI_ALLOC_STRUCT2(ID_OLAREA, new_as);

   if (new_as == NULL)
      return(-1);

   new_as->bOE = false;
   new_as->fArea = 0.0;
   new_as->edge = NULL;
   new_as->edge_b = NULL;
   new_as->next = NULL;

   // Link

   if (id_OLAreaList == NULL)
   {
      id_OLAreaList = new_as;
      id_OLAreaList_b = new_as;
   }
   else
   {
      id_OLAreaList_b->next = new_as;
      id_OLAreaList_b = new_as;
   }

   return(0);
}


// Save in structure id_OLAreaList_b
// Either at the beginning or at the end of the list (see iBE)
int wi_reorder_save_edge(int iLA, float fx1, float fy1, float fx2, float fy2, float fxc, float fyc, float fRad, int iCCW, int iBE)
{
   ID_OLEDGE *new_as;

   // Create new structure
   (ID_OLEDGE *) SI_ALLOC_STRUCT2(ID_OLEDGE, new_as);

   if (new_as == NULL)
      return(-1);

   new_as->iLA = iLA;
   new_as->fx1 = fx1;
   new_as->fy1 = fy1;
   new_as->fx2 = fx2;
   new_as->fy2 = fy2;
   new_as->fxc = fxc;
   new_as->fyc = fyc;
   new_as->fRad = fRad;
   new_as->iCCW = iCCW;
   new_as->next = NULL;

   // Link

   if (id_OLAreaList_b->edge == NULL)
   {
      id_OLAreaList_b->edge = new_as;
      id_OLAreaList_b->edge_b = new_as;
   }
   else
   {
      if (iBE == RS_END)
      {
         // Add to end of list
         id_OLAreaList_b->edge_b->next = new_as;
         id_OLAreaList_b->edge_b = new_as;
      }
      else
      {
         // Add to beginning of list
         new_as->next = id_OLAreaList_b->edge;
         id_OLAreaList_b->edge = new_as;
      }
   }

   return(0);
}


void wi_debug_reorder_shapes()
{
   ID_OLAREA *al;
   ID_OLEDGE *sl;

   fprintf(fp_log, "\n");
   fprintf(fp_log, "Outline identified shapes\n");

      for (al = id_OLAreaList; al != NULL; al = al->next)
      {
         if (al->bOE)
            fprintf(fp_log, "Outside edge\n");

         fprintf(fp_log, "Area = %f\n", al->fArea);

         for (sl = al->edge; sl != NULL; sl = sl->next)
         {
            if (sl->iLA == RS_LINE)
            {
               fprintf(fp_log, "  LINE ");
            }
            else if (sl->iLA == RS_ARC)
            {
               fprintf(fp_log, "  ARC ");

               if (sl->iCCW == RS_CCW)
                  fprintf(fp_log, "COUNTERCLOCKWISE ");
               else
                  fprintf(fp_log, "CLOCKWISE ");
            }
            else if (sl->iLA == RS_CIRCLE)
            {
               fprintf(fp_log, "  CIRCLE ");
            }
            else if (sl->iLA == RS_RECTANGLE)
            {
               fprintf(fp_log, "  RECTANGLE ");
            }

            fprintf(fp_log, "(%f %f) (%f %f) (%f %f  %f)\n",
                  sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad);
         }
      }

   fprintf(fp_log, "\n");
}

// -------------------------------------------------

// Convert ARCs, CIRCLEs and RECTANGLEs to LINEs
void wi_convert_shapes()
{
   int iStartOfArea;
   int iEndOfArea;
   float first_x, first_y;
   float last_x, last_y;
   ID_ALLOUTLINE *sl;

   id_OutlineList = NULL;
   id_OutlineList_b = NULL;

   iStartOfArea = 0;
   iEndOfArea = 0;

   // First time through 
   for (sl = id_OutLnList; sl != NULL; sl = sl->next)
   {
      if (iStartOfArea == 0)
      {
         wi_add_new_area("OutLine");

         // First and Last coords in area list
         first_x = sl->fx1;
         first_y = sl->fy1;
         last_x = sl->fx2;
         last_y = sl->fy2;

         iStartOfArea = 1;
      }

      if ( (sl->iLA == RS_ARC) && (sl->fx1 == sl->fx2) && (sl->fy1 == sl->fy2) )
      {
         // If ARC and first and last points are the same => circle
         rs_convert_circle_to_segs(sl->fxc, sl->fyc, sl->fRad);
         sl->iUsed = 1;         // mark line as used
         iEndOfArea = 1;
      }
      else if (sl->iLA == RS_CIRCLE)
      {
         // CIRCLE
         rs_convert_circle_to_segs(sl->fxc, sl->fyc, sl->fRad);
         sl->iUsed = 1;         // mark line as used
         iEndOfArea = 1;
      }
      else if (sl->iLA == RS_RECTANGLE)
      {
         // RECTANGLE
         rs_convert_rectangle_to_segs(sl->fx1, sl->fy1, sl->fx2, sl->fy2);
         sl->iUsed = 1;         // mark line as used
         iEndOfArea = 1;
      }
      else
      {
         if (sl->iLA == RS_ARC)
         {
            // ARC
            rs_convert_arc_to_segs(sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW);
         }
         if (sl->iLA == RS_LINE)
         {
            rs_add_new_segment(sl->fx1, sl->fy1, sl->fx2, sl->fy2);
         }
      }

      if (iEndOfArea == 1)
      {
         iStartOfArea = 0;
         iEndOfArea = 0;
      }
   }

   return;
}


// GUY should be almost the same as rs_add_new_area (id_OutlineList vs id_AreaList)
int wi_add_new_area(char *sNetName)
{
   ID_ALLAREAS *as;
   ID_ALLAREAS *new_as;
   ID_AREAS *last_al;
   ID_AREAS *new_al;

   // Does net already exist?
   for (as = id_OutlineList; as != NULL; as = as->next)
   {
      if (strcmp(as->sNetName, sNetName) == 0)
         break;
   }

   // Does structure already exist?
   if (as == NULL)
   {
      // Create structure
      (ID_ALLAREAS *) SI_ALLOC_STRUCT2(ID_ALLAREAS, new_as);

      if (new_as == NULL)
         return(-1);

      new_as->sNetName = SI_strsave(sNetName);
      new_as->areas = NULL;
      new_as->next = NULL;

      // Link

      if (id_OutlineList == NULL)
      {
         id_OutlineList = new_as;
         id_OutlineList_b = new_as;
      }
      else
      {
         id_OutlineList_b->next = new_as;
         id_OutlineList_b = new_as;
      }

      // Update "as" pointer
      as = new_as;
   }

   // structure "as" now points to current net

   // Add new area to end of list
   // Find end of list areas on this net
   // last_al = as->areas_b
   for (current_al = as->areas; current_al != NULL; current_al = current_al->next)
   {
      last_al = current_al;
   }

   // Create new area
   (ID_AREAS *) SI_ALLOC_STRUCT2(ID_AREAS, new_al);

   if (new_al == NULL)
      return(-1);

   new_al->sLyrName = NULL;
   new_al->iType = 0;
   new_al->iNumVer = 0;
   new_al->fLLx = 0.0;
   new_al->fLLy = 0.0;
   new_al->fURx = 0.0;
   new_al->fURy = 0.0;
   new_al->segver = NULL;
   new_al->next = NULL;

   // Link to end of list
   if (as->areas == NULL)
      as->areas = new_al;
   else
      last_al->next = new_al;

   // Update "current_al" pointer
   current_al = new_al;

   // structure "current_al" now points to current area

   return(0);
}

// -------------------------------------------------

// Identify Outside Edge (if more that one outline identified)
void wi_identify_oe()
{
   int iNum;
   int iCount;
   int iCurCount;
   double fArea;
   double fCurArea;
   ID_OLAREA *al;
   ID_OLAREA *saved_al;
   ID_OLEDGE *sl;

   // Count number of areas
   iNum = 0;
   for (al = id_OLAreaList; al != NULL; al = al->next)
   {
      iNum++;
   }

   if (iNum == 1)
      id_OLAreaList->bOE = true;
   else
   {
      // Calculate the area of each identified area
      // The largest is the outside edge (don't use this for now)
      fArea = 0.0;
      iCount = 0;

      saved_al = NULL;
      for (al = id_OLAreaList; al != NULL; al = al->next)
      {
         fCurArea = wi_CalculateArea(al);
         al->fArea = fCurArea;
#if 0
         if (fCurArea > fArea)
         {
            fArea = fCurArea;
            saved_al = al;
         }
#endif
         iCurCount = 0;
         for (sl = al->edge; sl != NULL; sl = sl->next)
         {
            iCurCount++;
         }

         if (iCurCount > iCount)
         {
            iCount = iCurCount;
            saved_al = al;
         }
      }

      if (saved_al != NULL)
         saved_al->bOE = true;
      else
         id_OLAreaList->bOE = true;

      // Area with the most edges wins
   }

   return;
}


double
wi_CalculateArea(ID_OLAREA *area)
{
   double rArea = 0.0;
   ID_OLEDGE *sl;

   for (sl = area->edge; sl != NULL; sl = sl->next)
   {
      rArea += ((double) sl->fx1) * ((double) sl->fy2);
      rArea -= ((double) sl->fx2) * ((double) sl->fy1);
   }

   rArea *= 0.5;

   // always return positive value
   if (rArea < 0.0)
      rArea = -rArea;

   return(rArea);
}


double
wi_CalcArea(ID_AREAS *area)
{
   double rArea = 0.0;
   ID_VERS *sl;

   for (sl = area->segver; sl != NULL; sl = sl->next)
   {
      rArea += ((double) sl->fx1) * ((double) sl->fy2);
      rArea -= ((double) sl->fx2) * ((double) sl->fy1);
   }

   rArea *= 0.5;

   // always return positive value
   if (rArea < 0.0)
      rArea = -rArea;

   return(rArea);
}

// -------------------------------------------------

// Update Number of vertices and Boundary rectange
void wi_update_shapes()
{
   int iNum = 0;
   float fXmin = 0.0;
   float fYmin = 0.0;
   float fXmax = 0.0;
   float fYmax = 0.0;
   ID_ALLAREAS *as;
   ID_AREAS *al;
   ID_VERS *vs;

   for (as = id_OutlineList; as != NULL; as = as->next)
   {
      for (al = as->areas; al != NULL; al = al->next)
      {
         iNum = 0;
         for (vs = al->segver; vs != NULL; vs = vs->next)
         {
            if (iNum == 0)
            {
               fXmin = vs->fx1;
               fYmin = vs->fy1;
               fXmax = vs->fx1;
               fYmax = vs->fy1;
            }

            if (fXmin > vs->fx1)
               fXmin = vs->fx1;

            if (fYmin > vs->fy1)
               fYmin = vs->fy1;

            if (fXmax < vs->fx1)
               fXmax = vs->fx1;

            if (fYmax < vs->fy1)
               fYmax = vs->fy1;

            iNum++;
         }

         al->fLLx = fXmin;
         al->fLLy = fYmin;
         al->fURx = fXmax;
         al->fURy = fYmax;
         al->iNumVer = iNum;
      }
   }
}


void wi_debug_reorder_outline_list()
{
   ID_ALLAREAS *as;
   ID_AREAS *al;
   ID_VERS *vs;

   fprintf(fp_log, "Outline Re-ordered list\n");

   for (as = id_OutlineList; as != NULL; as = as->next)
   {
      fprintf(fp_log, "%s\n", as->sNetName);

      for (al = as->areas; al != NULL; al = al->next)
      {
         fprintf(fp_log, " Num edges = %d Boundary (%f %f) (%f %f)\n", al->iNumVer,
               al->fLLx, al->fLLy, al->fURx, al->fURy);

         for (vs = al->segver; vs != NULL; vs = vs->next)
         {
            fprintf(fp_log, "    (%f %f) (%f %f)\n", vs->fx1, vs->fy1, vs->fx2, vs->fy2);
         }
      }

      fprintf(fp_log, "\n");
   }

   fprintf(fp_log, "\n");
}

// -------------------------------------------------
