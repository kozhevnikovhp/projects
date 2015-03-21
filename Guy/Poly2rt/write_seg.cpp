//********************************************************************************
//********************************************************************************
//**										**
//**	File:		write_seg.cpp						**
//**	Contents:	Write seg_new.txt                                    	**
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

// -------------------------------------------------
// Forward declarations

int ws_write_seg(char *sFile, int AreaID);

// -------------------------------------------------
// External declarations

extern void error_message(char *sEW, char *str);
extern void error_log(char *str);

// -------------------------------------------------
// Global declarations

extern char sLogStr[];
extern FILE *fp_log;
extern int test_code;

extern S34 *g_pMesh; // meshed area as list of right isosceles triangles and rectangles

// -------------------------------------------------
// Write seg_new.txt file
// Returns
//   0 = All OK
//  -1 = Cannot open sFile
int ws_write_seg(char *sFile, int AreaID)
{
   int res = 0;
   FILE *fps;

   // GUY To be written

   if (AreaID == -1)
      return(0);

   fps = fopen(sFile, "w");
   if (fps == NULL)
   {
      error_message("E01", sFile);
      return(-1);
   }

   S34 *p34 = g_pMesh; // that is head of linked list returned from MeshArea() 
   while (p34)
   {
      switch (p34->type)
      {
         case TYPE_RECTANGLE:
            // LT->LB->RB->RT->LT
            fprintf(fps, "RECT     (%f %f) (%f %f) (%f %f) (%f %f)\n",
                  p34->left, p34->top, p34->left, p34->bottom, p34->right, p34->bottom, p34->right, p34->top);
         break;
         case TYPE_NW:
            // LT->LB->RT->LT
            fprintf(fps, "TRI (NW) (%f %f) (%f %f) (%f %f)\n",
                  p34->left, p34->top, p34->left, p34->bottom, p34->right, p34->top);
         break;
         case TYPE_NE:
            // LT->RB->RT->LT
            fprintf(fps, "TRI (NE) (%f %f) (%f %f) (%f %f)\n",
                  p34->left, p34->top, p34->right, p34->bottom, p34->right, p34->top);
         break;
         case TYPE_SE:
            // RT->LB->RB->RT
            fprintf(fps, "TRI (SE) (%f %f) (%f %f) (%f %f)\n",
                  p34->right, p34->top, p34->left, p34->bottom, p34->right, p34->bottom);
         break;
         case TYPE_SW:
            // RB->LB->LT->RB
            fprintf(fps, "TRI (SW) (%f %f) (%f %f) (%f %f)\n",
                  p34->right, p34->bottom, p34->left, p34->bottom, p34->left, p34->top);
         break;
      }

      p34 = p34->pNext;
   }

   fclose(fps);

   return(res);
}

// -------------------------------------------------

