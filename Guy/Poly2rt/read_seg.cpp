//********************************************************************************
//********************************************************************************
//**										**
//**	File:		read_seg.cpp						**
//**	Contents:	Read seg.txt                                    	**
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

// -------------------------------------------------
// Forward declarations

int read_seg(char *sSegFile);
int rs_add_to_shape_list(char *sNetName, char *sLyrName, char *sLA,
      char *sX1, char *sY1, char *sX2, char *sY2, char *sX3, char *sY3, char *sX4, char *sY4,
      char *sCCW, char *sType);
void rs_debug_shape_list();
void rs_free();
int rs_reorder_shapes();
int rs_add_new_area(char *sNetName, char *sLyrName, int iType, int id);
int rs_add_new_segment(float fx1, float fy1, float fx2, float fy2);
void rs_update_shapes();
void rs_debug_reorder_shape_list();
void rs_convert_arc_to_segs(float fx1, float fy1, float fx2, float fy2, float fxc, float fyc, float fRad, int iCCW);
void rs_convert_circle_to_segs(float fxc, float fyc, float fRad);
void rs_convert_rectangle_to_segs(float fx1, float fy1, float fx2, float fy2);
void rs_match_voids_to_area();
int rs_add_void_to_area(ID_VOIDS **vl, ID_AREAS *voids);

// -------------------------------------------------
// External declarations

extern void error_message(char *sEW, char *str);
extern void error_log(char *str);

// -------------------------------------------------
// Global declarations

ID_ALLSHAPES *id_ShapeList;
ID_ALLSHAPES *id_ShapeList_b;
char sUnits[SI_SMLINE];

ID_ALLAREAS *id_AreaList;
ID_ALLAREAS *id_AreaList_b;
ID_AREAS *current_al;

extern char sLogStr[];
extern FILE *fp_log;
extern int test_code;

// -------------------------------------------------
// Read seg.txt file

int read_seg(char *sSegFile)
{
   int i, j, k;
   int res = 0;
   char sLine[SI_MAXLINE];
   char sEtch[SI_SMLINE];
   char sLyrName[SI_SMLINE];
   char sNetName[SI_SMLINE];
   char sLA[SI_SMLINE];
   char sX1[SI_SMLINE];
   char sY1[SI_SMLINE];
   char sX2[SI_SMLINE];
   char sY2[SI_SMLINE];
   char sX3[SI_SMLINE];
   char sY3[SI_SMLINE];
   char sX4[SI_SMLINE];
   char sY4[SI_SMLINE];
   char sCCW[SI_SMLINE];
   char sType[SI_SMLINE];
   FILE *fp2;

   sUnits[0] = '\0';

   // Read seg.txt
   fp2 = fopen(sSegFile, "r");
   if (fp2 == NULL)
   {
      error_message("E02", sSegFile);		// unable to read file
      return(-1);
   }

   id_ShapeList = NULL;
   id_ShapeList_b = NULL;

   // S!ETCH!VCC!VCC!LINE!-61.0!-67.0!-61.0!196.0!0.0!!!!!SHAPE!413.38 pF!2.11 nH!2.26 ohms!0.93424 ns!3.50 mohms!
   // S!ETCH!VCC!VCC!ARC!2859.0!14.0!2737.0!27.0!2798.1!21.3!61.4!0.0!COUNTERCLOCKWISE!SHAPE!413.38 pF!2.11 nH!2.26 ohms!0.93424 ns!3.50 mohms!
   // S!ETCH!L5!GND!ARC!2498.0!83.8!2500.8!77.0!2498.0!79.8!4.0!0.0!CLOCKWISE!SHAPE!19.68 pF!0.43 nH!4.68 ohms!0.09217 ns!1.81 mohms!
   //
   // posn           Value
   // ----           -----
   //   1            ETCH
   //   2            Layer name
   //   3            Net name (might be empty, ignore if empty)
   //   4            LINE | ARC | CIRCLE
   //   5            x1
   //   6            y1
   //   7            x2
   //   8            y2
   //   9            xc
   //   10           yc
   //   11           radius
   //   12           ? (ignore)
   //   13           CLOCKWISE | COUNTERCLOCKWISE (if 4 = ARC)
   //   14           SHAPE | VOID

   while (fgets(sLine, SI_MAXLINE, fp2) != NULL)
   {
      if (sLine[0] == 'J')
      {
         // Get units
         sUnits[0] = '\0';

         for (i = 0, j = 0, k = 0; i < (int) strlen(sLine); i++)
         {
	    if (sLine[i] == '!')
	    {
	       j++;
	       k = 0;
	    }
	    else
	    {
	       if (j == 8)
	       {
	          sUnits[k] = sLine[i];
	          k++;
	          sUnits[k] = '\0';
	       }
            }
         }
      }
      else if (sLine[0] == 'S')
      {
         sEtch[0] = '\0';
         sLyrName[0] = '\0';
         sNetName[0] = '\0';
         sLA[0] = '\0';
         sX1[0] = '\0';
         sY1[0] = '\0';
         sX2[0] = '\0';
         sY2[0] = '\0';
         sX3[0] = '\0';
         sY3[0] = '\0';
         sX4[0] = '\0';
         sY4[0] = '\0';
         sCCW[0] = '\0';
         sType[0] = '\0';

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
	          sEtch[k] = sLine[i];
	          k++;
	          sEtch[k] = '\0';
	       }
	       else if (j == 2)
	       {
	          sLyrName[k] = sLine[i];
	          k++;
	          sLyrName[k] = '\0';
	       }
	       else if (j == 3)
	       {
	          sNetName[k] = sLine[i];
	          k++;
	          sNetName[k] = '\0';
	       }
	       else if (j == 4)
	       {
	          sLA[k] = sLine[i];
	          k++;
	          sLA[k] = '\0';
	       }
	       else if (j == 5)
	       {
	          sX1[k] = sLine[i];
	          k++;
	          sX1[k] = '\0';
	       }
	       else if (j == 6)
	       {
	          sY1[k] = sLine[i];
	          k++;
	          sY1[k] = '\0';
	       }
	       else if (j == 7)
	       {
	          sX2[k] = sLine[i];
	          k++;
	          sX2[k] = '\0';
	       }
	       else if (j == 8)
	       {
	          sY2[k] = sLine[i];
	          k++;
	          sY2[k] = '\0';
	       }
	       else if (j == 9)
	       {
	          sX3[k] = sLine[i];
	          k++;
	          sX3[k] = '\0';
	       }
	       else if (j == 10)
	       {
	          sY3[k] = sLine[i];
	          k++;
	          sY3[k] = '\0';
	       }
	       else if (j == 11)
	       {
	          sX4[k] = sLine[i];
	          k++;
	          sX4[k] = '\0';
	       }
	       else if (j == 12)
	       {
	          sY4[k] = sLine[i];
	          k++;
	          sY4[k] = '\0';
	       }
	       else if (j == 13)
	       {
	          sCCW[k] = sLine[i];
	          k++;
	          sCCW[k] = '\0';
	       }
	       else if (j == 14)
	       {
	          sType[k] = sLine[i];
	          k++;
	          sType[k] = '\0';
	       }
	    }
         }

         if ( (strcmp(sEtch, "ETCH") == 0) && (sNetName[0] != '\0') )
         {
            if ( (strcmp(sType, "SHAPE") == 0) || (strcmp(sType, "VOID") == 0) )
               res = rs_add_to_shape_list(sNetName, sLyrName, sLA, sX1, sY1, sX2, sY2, sX3, sY3, sX4, sY4, sCCW, sType);
         }
      }
   }

   fclose(fp2);

   if (test_code == 2)
      rs_debug_shape_list();

   // Identify and individualize each area
   rs_reorder_shapes();

   // Update Number of vertices and Boundary rectangle
   rs_update_shapes();

   // Identify the voids for each area
   rs_match_voids_to_area();

   if (test_code == 2)
      rs_debug_reorder_shape_list();

   return(res);
}


int
rs_add_to_shape_list(char *sNetName, char *sLyrName, char *sLA,
      char *sX1, char *sY1, char *sX2, char *sY2, char *sX3, char *sY3, char *sX4, char *sY4,
      char *sCCW, char *sType)
{
   int iN;
   int iLA;
   int iCCW;
   int iType;
   float fx1, fy1;
   float fx2, fy2;
   float fxc, fyc;
   float fRad, fUnk;
   ID_ALLSHAPES *new_lb;

   /* Create structure */
   (ID_ALLSHAPES *) SI_ALLOC_STRUCT2(ID_ALLSHAPES, new_lb);

   if (new_lb == NULL)
      return (-1);

   if (strcmp(sLA, "ARC") == 0)
      iLA = RS_ARC;
   else if (strcmp(sLA, "CIRCLE") == 0)
      iLA = RS_CIRCLE;
   else
      iLA = RS_LINE;

   if (strcmp(sCCW, "COUNTERCLOCKWISE") == 0)
      iCCW = RS_CCW;
   else
      iCCW = RS_CW;

   if (strcmp(sType, "SHAPE") == 0)
      iType = RS_SOLID;
   else
      iType = RS_VOID;

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

   if (sX3[0] != '\0')
      iN = sscanf(sX3, "%f", &fxc);
   else
      fxc = 0.0;

   if (sY3[0] != '\0')
      iN = sscanf(sY3, "%f", &fyc);
   else
      fyc = 0.0;

   if (sX4[0] != '\0')
      iN = sscanf(sX4, "%f", &fRad);
   else
      fRad = 0.0;

   if (sY4[0] != '\0')
      iN = sscanf(sY4, "%f", &fUnk);
   else
      fUnk = 0.0;

   new_lb->sNetName = SI_strsave(sNetName);
   new_lb->sLyrName = SI_strsave(sLyrName);
   new_lb->iLA = iLA;
   new_lb->fx1 = fx1;
   new_lb->fy1 = fy1;
   new_lb->fx2 = fx2;
   new_lb->fy2 = fy2;
   new_lb->fxc = fxc;
   new_lb->fyc = fyc;
   new_lb->fRad = fRad;
   new_lb->fUnk = fUnk;
   new_lb->iCCW = iCCW;
   new_lb->iType = iType;
   new_lb->next = NULL;

   /* Link */

   if (id_ShapeList == NULL)
   {
      id_ShapeList = new_lb;
      id_ShapeList_b = new_lb;
   }
   else
   {
      id_ShapeList_b->next = new_lb;
      id_ShapeList_b = new_lb;
   }

   return (0);
}


void rs_debug_shape_list()
{
   int iStartOfArea;
   int iEndOfArea;
   int iType = 0;
   int iLA = 0;
   float fx1 = 0.0;
   float fy1 = 0.0;
   char sNetName[SI_SMLINE];
   char sLyrName[SI_SMLINE];
   ID_ALLSHAPES *sl;

   sNetName[0] = '\0';
   sLyrName[0] = '\0';

   fprintf(fp_log, "\n");

   fprintf(fp_log, "Units = %s\n", sUnits);
   fprintf(fp_log, "\n");

#if 0
   fprintf(fp_log, "Shapes (org)\n");
   for (sl = id_ShapeList; sl != NULL; sl = sl->next)
   {
      fprintf(fp_log, "%s ", sl->sNetName);
      fprintf(fp_log, "%s ", sl->sLyrName);

      if (sl->iType == RS_SOLID)
         fprintf(fp_log, "Solid ");
      else
         fprintf(fp_log, "Void ");

      if (sl->iLA == RS_ARC)
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
      else
      {
         fprintf(fp_log, "LINE ");
      }

      fprintf(fp_log, "(%f %f) (%f %f) (%f %f) (%f %f)\n",
            sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->fUnk);
   }

   fprintf(fp_log, "\n");
#endif

   fprintf(fp_log, "Shapes\n");
   iStartOfArea = 0;
   iEndOfArea = 0;
   for (sl = id_ShapeList; sl != NULL; sl = sl->next)
   {
      if (iStartOfArea == 0)
      {
         // initialize if starting a new area
         strcpy(sNetName, sl->sNetName);
         strcpy(sLyrName, sl->sLyrName);
         iType = sl->iType;
         iLA = sl->iLA;
         fx1 = sl->fx1;
         fy1 = sl->fy1;

         iStartOfArea = 1;
      }

      // -----------------------
      // Write out the info
      fprintf(fp_log, "%s ", sl->sNetName);
      fprintf(fp_log, "%s ", sl->sLyrName);

      if (sl->iType == RS_SOLID)
         fprintf(fp_log, "Solid ");
      else
         fprintf(fp_log, "Void ");

      if (sl->iLA == RS_ARC)
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
      else
      {
         fprintf(fp_log, "LINE ");
      }

      fprintf(fp_log, "(%f %f) (%f %f) (%f %f) (%f %f)\n",
            sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->fUnk);

      // -----------------------
      // Are we at the end of an area?

      if ( (sl->iLA == RS_ARC) && (sl->fx1 == sl->fx2) && (sl->fy1 == sl->fy2) )
      {
         // If ARC and first and last points are the same -> circle
         iEndOfArea = 1;
      }
      else if (sl->iLA == RS_CIRCLE)
      {
         // If CIRCLE
         iEndOfArea = 1;
      }
      else
      {
         if ( (strcmp(sNetName, sl->sNetName) == 0)
           && (strcmp(sLyrName, sl->sLyrName) == 0)
           && (iType == sl->iType) )
         {
            // Net name, layer name, and shape type are the same

            // Is first vertex equal to last vertex?
            // If so -> end of area
            if ( (fx1 == sl->fx2) && (fy1 == sl->fy2) )
               iEndOfArea = 1;
         }
         else
         {
            iEndOfArea = 1;
         }
      }

      if (iEndOfArea == 1)
      {
         iStartOfArea = 0;
         iEndOfArea = 0;
         fprintf(fp_log, "\n");
      }
   }

   fprintf(fp_log, "\n");
}


void rs_free()
{
   ID_ALLSHAPES *sl, *sln;
   ID_ALLAREAS *a0, *a0n;
   ID_AREAS *a1, *a1n;
   ID_VERS *a2, *a2n;
   ID_VOIDS *a3, *a3n;

   // Free id_ShapeList
   sl = id_ShapeList;
   while (sl != NULL)
   {
      sln = sl->next;
      free(sl);
      sl = sln;
   }

   // Free id_AreaList
   a0 = id_AreaList;
   while (a0 != NULL)
   {
      a1 = a0->areas;
      while (a1 != NULL)
      {
         a2 = a1->segver;
         while (a2 != NULL)
         {
            a2n = a2->next;
            free(a2);
            a2 = a2n;
         }

         a3 = a1->voids;
         while (a3 != NULL)
         {
            a3n = a3->next;
            free(a3);
            a3 = a3n;
         }

         a1n = a1->next;
         free(a1);
         a1 = a1n;
      }

      a0n = a0->next;
      free(a0);
      a0 = a0n;
   }
}

// -------------------------------------------------

int rs_reorder_shapes()
{
   int iStartOfArea;
   int iEndOfArea;
   int id;
   int iType = 0;
   int iLA = 0;
   float fx1 = 0.0;
   float fy1 = 0.0;
   char sNetName[SI_SMLINE];
   char sLyrName[SI_SMLINE];
   ID_ALLSHAPES *sl;

   id_AreaList = NULL;
   id_AreaList_b = NULL;

   sNetName[0] = '\0';
   sLyrName[0] = '\0';

   iStartOfArea = 0;
   iEndOfArea = 0;
   id = 0;
   for (sl = id_ShapeList; sl != NULL; sl = sl->next)
   {
      if (iStartOfArea == 0)
      {
         // initialize if starting a new area
         strcpy(sNetName, sl->sNetName);
         strcpy(sLyrName, sl->sLyrName);
         iType = sl->iType;
         iLA = sl->iLA;
         fx1 = sl->fx1;
         fy1 = sl->fy1;

         iStartOfArea = 1;

         // Create new area (net, layer and type)
         rs_add_new_area(sl->sNetName, sl->sLyrName, sl->iType, id);
         id++;
      }

      // -----------------------
      // Are we at the end of an area?

      if ( (sl->iLA == RS_ARC) && (sl->fx1 == sl->fx2) && (sl->fy1 == sl->fy2) )
      {
         // If ARC and first and last points are the same => circle
         rs_convert_circle_to_segs(sl->fxc, sl->fyc, sl->fRad);
         iEndOfArea = 1;
      }
      else if (sl->iLA == RS_CIRCLE)
      {
         // If CIRCLE
         rs_convert_circle_to_segs(sl->fxc, sl->fyc, sl->fRad);
         iEndOfArea = 1;
      }
      else
      {
         if ( (strcmp(sNetName, sl->sNetName) == 0)
           && (strcmp(sLyrName, sl->sLyrName) == 0)
           && (iType == sl->iType) )
         {
            // Net name, layer name, and shape type are the same

            // Is first vertex equal to last vertex?
            // If so -> end of area
            if ( (fx1 == sl->fx2) && (fy1 == sl->fy2) )
               iEndOfArea = 1;

            if (sl->iLA == RS_LINE)
            {
               rs_add_new_segment(sl->fx1, sl->fy1, sl->fx2, sl->fy2);
            }
            else
            {
               // ARC (but not a circle)
               rs_convert_arc_to_segs(sl->fx1, sl->fy1, sl->fx2, sl->fy2, sl->fxc, sl->fyc, sl->fRad, sl->iCCW);
            }
         }
         else
         {
            // Add segment
            rs_add_new_segment(sl->fx1, sl->fy1, sl->fx2, sl->fy2);
            iEndOfArea = 1;
         }
      }

      if (iEndOfArea == 1)
      {
         iStartOfArea = 0;
         iEndOfArea = 0;
      }
   }

   return(0);
}


int rs_add_new_area(char *sNetName, char *sLyrName, int iType, int id)
{
   ID_ALLAREAS *as;
   ID_ALLAREAS *new_as;
   ID_AREAS *last_al;
   ID_AREAS *new_al;

   // Does net already exist?
   for (as = id_AreaList; as != NULL; as = as->next)
   {
      if (strcmp(as->sNetName, sNetName) == 0)
         break;
   }

   if (as == NULL)
   {
      // Net does not exist
      // Create structure
      (ID_ALLAREAS *) SI_ALLOC_STRUCT2(ID_ALLAREAS, new_as);

      if (new_as == NULL)
         return(-1);

      new_as->sNetName = SI_strsave(sNetName);
      new_as->areas = NULL;
      new_as->next = NULL;

      // Link

      if (id_AreaList == NULL)
      {
         id_AreaList = new_as;
         id_AreaList_b = new_as;
      }
      else
      {
         id_AreaList_b->next = new_as;
         id_AreaList_b = new_as;
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

   new_al->id = id;
   new_al->sLyrName = SI_strsave(sLyrName);
   new_al->iType = iType;
   new_al->iNumVer = 0;
   new_al->fLLx = 0.0;
   new_al->fLLy = 0.0;
   new_al->fURx = 0.0;
   new_al->fURy = 0.0;
   new_al->segver = NULL;
   new_al->voids = NULL;
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


// Add new segment vertices to "current_al" structure
int rs_add_new_segment(float fx1, float fy1, float fx2, float fy2)
{
   ID_VERS *vs;
   ID_VERS *last_vs;
   ID_VERS *new_vs;

   //last_vs = current_al->segver_b;
   for (vs = current_al->segver; vs != NULL; vs = vs->next)
   {
      last_vs = vs;
   }

   (ID_VERS *) SI_ALLOC_STRUCT2(ID_VERS, new_vs);

   if (new_vs == NULL)
      return(-1);

   new_vs->fx1 = fx1;
   new_vs->fy1 = fy1;
   new_vs->fx2 = fx2;
   new_vs->fy2 = fy2;
   new_vs->next = NULL;

   // Link to end of list
   if (current_al->segver == NULL)
      current_al->segver = new_vs;
   else
      last_vs->next = new_vs;

   return(0);
}


void rs_debug_reorder_shape_list()
{
   ID_ALLAREAS *as;
   ID_AREAS *al;
   ID_VOIDS *vl;
   ID_VERS *vs;

   fprintf(fp_log, "Re-ordered list\n");

   for (as = id_AreaList; as != NULL; as = as->next)
   {
      fprintf(fp_log, "%s\n", as->sNetName);

      for (al = as->areas; al != NULL; al = al->next)
      {
         fprintf(fp_log, "  %d %s ", al->id, al->sLyrName);

         if (al->iType == RS_SOLID)
            fprintf(fp_log, "Solid");
         else
            fprintf(fp_log, "Void");

         fprintf(fp_log, " Num edges = %d Boundary (%f %f) (%f %f)\n", al->iNumVer,
               al->fLLx, al->fLLy, al->fURx, al->fURy);

         fprintf(fp_log, "  List of voids attached to area:");
         if (al->voids == NULL)
         {
            fprintf(fp_log, " (none)\n");
         }
         else
         {
            for (vl = al->voids; vl != NULL; vl = vl->next)
            {
               if (vl->voidarea != NULL)
                  fprintf(fp_log, " %d", (vl->voidarea)->id);
            }
            fprintf(fp_log, "\n");
         }

#if 0
         for (vl = al->voids; vl != NULL; vl = vl->next)
         {
            if (vl->voidarea != NULL)
            {
               fprintf(fp_log, " void %d", (vl->voidarea)->id);

               if ((vl->voidarea)->iType == RS_SOLID)
                  fprintf(fp_log, "Solid (bad!)");

               fprintf(fp_log, " Num edges = %d Boundary (%f %f) (%f %f)\n", (vl->voidarea)->iNumVer,
                     (vl->voidarea)->fLLx, (vl->voidarea)->fLLy, (vl->voidarea)->fURx, (vl->voidarea)->fURy);
            }
         }
#endif

         fprintf(fp_log, "  Vertex list:");
         for (vs = al->segver; vs != NULL; vs = vs->next)
         {
            fprintf(fp_log, "    (%f %f) (%f %f)\n", vs->fx1, vs->fy1, vs->fx2, vs->fy2);
         }


         fprintf(fp_log, "\n");
      }

      fprintf(fp_log, "\n");
   }

   fprintf(fp_log, "\n");
}

// -------------------------------------------------

// Update Number of vertices and Boundary rectangle
void rs_update_shapes()
{
   int iNum = 0;
   float fXmin = 0.0;
   float fYmin = 0.0;
   float fXmax = 0.0;
   float fYmax = 0.0;
   ID_ALLAREAS *as;
   ID_AREAS *al;
   ID_VERS *vs;

   for (as = id_AreaList; as != NULL; as = as->next)
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

// -------------------------------------------------

void rs_convert_arc_to_segs(float fx1, float fy1, float fx2, float fy2, float fxc, float fyc, float fRad, int iCCW)
{
   int i;
   int iNumInter;
   float fP1x, fP1y;
   float fP2x, fP2y;
   double alpha1, alpha2;
   double ArcAngle;
   double angle;
   double NewAngle;
   double beta;
   float circPt_x[10];
   float circPt_y[10];

   // if order is CW, reverse start/end coords to make order CCW
   if (iCCW == RS_CCW)
   {
      fP1x = fx1;
      fP1y = fy1;
      fP2x = fx2;
      fP2y = fy2;
   }
   else
   {
      fP1x = fx2;
      fP1y = fy2;
      fP2x = fx1;
      fP2y = fy1;
   }

   // Calculate angle between center and each ARC end (alpha1, alpha2)

   alpha1 = SI_Angle(fxc, fyc, fP1x, fP1y, 0);
   alpha2 = SI_Angle(fxc, fyc, fP2x, fP2y, 0);

   // Angle of Arc
   ArcAngle = alpha2 - alpha1;
   if (ArcAngle < 0.0)
      ArcAngle += 360.0;

   if (test_code == 3)
   {
      fprintf(fp_log, "ARC : P1 = (%f %f) P2 = (%f %f) C = (%f %f) Rad = (%f)\n", fP1x, fP1y, fP2x, fP2y, fxc, fyc, fRad);
      fprintf(fp_log, "  alpha1 = %f,  alpha2 = %f,  ArcAngle = %f\n", alpha1, alpha2, ArcAngle);
   }

   if (ArcAngle <= 45.0)
   {
      // if ARC angle is less than, or equal to, 45 degrees,
      // then add 1 segment only
      rs_add_new_segment(fx1, fy1, fx2, fy2);
   }
   else
   {
      // Number of intermediate points on ARC
      iNumInter = (int) (ArcAngle / 45.0);

      // Angle between intermediate points
      beta = ArcAngle / (double) (iNumInter + 1);

      if (test_code == 3)
      {
         fprintf(fp_log, "  iNumInter = %d, beta = %f\n", iNumInter, beta);
      }

      // Start of Arc
      circPt_x[0] = fP1x;
      circPt_y[0] = fP1y;

      // Intermediate points
      for (i = 1; i < (iNumInter + 1); i++)
      {
         angle = alpha1 + (((double) i) * beta);
         NewAngle = angle * PIx2 / 360.0;
         circPt_x[i] = fxc + (fRad * (float) cos(NewAngle) );
         circPt_y[i] = fyc + (fRad * (float) sin(NewAngle) );
      }

      // End of Arc
      circPt_x[iNumInter + 1] = fP2x;
      circPt_y[iNumInter + 1] = fP2y;

      if (test_code == 3)
      {
         for (i = 0; i < (iNumInter + 1); i++)
         {
            fprintf(fp_log, "  segment (%f %f) - (%f %f)\n", circPt_x[i], circPt_y[i], circPt_x[i + 1], circPt_y[i + 1]);
         }
      }

      // Now write out segments
      if (iCCW == RS_CCW)
      {
         for (i = 0; i < (iNumInter + 1); i++)
         {
            rs_add_new_segment(circPt_x[i], circPt_y[i], circPt_x[i + 1], circPt_y[i + 1]);
         }
      }
      else
      {
         for (i = (iNumInter + 1); i > 0; i--)
         {
            rs_add_new_segment(circPt_x[i], circPt_y[i], circPt_x[i - 1], circPt_y[i - 1]);
         }
      }
   }
}


void rs_convert_circle_to_segs(float fxc, float fyc, float fRad)
{
   int i, j;
   float fD;
   float circPt_x[8];
   float circPt_y[8];

   // CCW list starting at 0 degrees. 8 points.
   fD = fRad * (float) SQRT2 / (float) 2.0;
   circPt_x[0] = fxc + fRad;
   circPt_y[0] = fyc;

   circPt_x[1] = fxc + fD;
   circPt_y[1] = fyc + fD;

   circPt_x[2] = fxc;
   circPt_y[2] = fyc + fRad;

   circPt_x[3] = fxc - fD;
   circPt_y[3] = fyc + fD;

   circPt_x[4] = fxc - fRad;
   circPt_y[4] = fyc;

   circPt_x[5] = fxc - fD;
   circPt_y[5] = fyc - fD;

   circPt_x[6] = fxc;
   circPt_y[6] = fyc - fRad;

   circPt_x[7] = fxc + fD;
   circPt_y[7] = fyc - fD;

   for (i = 0; i < 8; i++)
   {
      j = i + 1;        // next in list
      if (j > 7)
         j = 0;

      rs_add_new_segment(circPt_x[i], circPt_y[i], circPt_x[j], circPt_y[j]);
   }
}


void rs_convert_rectangle_to_segs(float fx1, float fy1, float fx2, float fy2)
{
   rs_add_new_segment(fx1, fy1, fx1, fy2);
   rs_add_new_segment(fx1, fy2, fx2, fy2);
   rs_add_new_segment(fx2, fy2, fx2, fy1);
   rs_add_new_segment(fx2, fy1, fx1, fy1);
}

// -------------------------------------------------

void rs_match_voids_to_area()
{
   int ret;
   char *sLyrName;
   ID_ALLAREAS *as;
   ID_AREAS *al;
   ID_AREAS *voids;

   for (as = id_AreaList; as != NULL; as = as->next)
   {
      for (al = as->areas; al != NULL; al = al->next)
      {
         // Is this a solid area?
         if (al->iType == RS_SOLID)
         {
            // get layer name
            sLyrName = al->sLyrName;

            // Now find all the voids that are inside this area
            for (voids = as->areas; voids != NULL; voids = voids->next)
            {
               // Must be a void
               if (voids->iType != RS_VOID)
                  continue;

               // Must be on same layer as the solid area
               if (strcmp(voids->sLyrName, sLyrName) != 0)
                  continue;

               // Is void inside solid area?
               if (voids->segver != NULL)
               {
                  if (SI_isPointinArea(al, (voids->segver)->fx1, (voids->segver)->fy1))
                  {
                     // Void is inside area
                     // Add "voids" to "al->voids" list
                     ret = rs_add_void_to_area(&(al->voids), voids);
                  }
               }
            }
         }
      }
   }
}


int rs_add_void_to_area(ID_VOIDS **vl, ID_AREAS *voids)
{
   ID_VOIDS *new_vs;

   (ID_VOIDS *) SI_ALLOC_STRUCT2(ID_VOIDS, new_vs);

   if (new_vs == NULL)
      return(-1);

   new_vs->voidarea = voids;

   // Link
   new_vs->next = *vl;
   *vl = new_vs;

   return(0);
}


// -------------------------------------------------
