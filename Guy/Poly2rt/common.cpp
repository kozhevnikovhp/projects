// ------------------------------------------------
//
// Common stuff
//
// ------------------------------------------------

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

// ------------------------------------------------

#include "common_def.hpp"
#include "poly2rt.hpp"

// ------------------------------------------------

bool SI_isPointinArea(ID_AREAS *area, float xt, float yt);
bool SI_isPointOnLine(double U0x, double U0y, double U1x, double U1y, double Px, double Py);
double SI_Angle(float xs, float ys, float xe, float ye, int iFlag);
void SI_heapsort(unsigned int N, HS_SORT *sHSsort, int (*compare)(int i1, int i2, int iSortType), int iSortType );
long SI_iabs(long x);
int SI_indx(char *s, char *t);
int SI_IsEqual(float x, float y);
bool SI_IsEqualD(double x, double y);
void SI_remove_crlf(char *ln);
int SI_blank(char *ln);
void SI_remove_comments(char *ln);
void SI_remove_leading_spaces(char *ln);
void SI_remove_trailing_spaces(char *ln);
void SI_remove_quotes(char *ln);
void SI_replace_char_with_space(char *ln, char ch);
char *SI_strsave(char *stg);
void *SI_sp_alloc(unsigned int size);
void *SI_sp_malloc(size_t size);
void g_debug(char *fn, char *str);

// ------------------------------------------------

// ***************************************************************************
// *                                                                         *
// *   inpoly.C                                                              *
// *                                                                         *
// *   Copyright (c) 1995-1996 Galacticomm, Inc.  Freeware source code.      *
// *                                                                         *
// *   Please feel free to use this source code for any purpose, commercial  *
// *   or otherwise, as long as you don't restrict anyone else's use of      *
// *   this source code.  Please give credit where credit is due.            *
// *                                                                         *
// *   Point-in-polygon algorithm, created especially for World-Wide Web     *
// *   servers to process image maps with mouse-clickable regions.           *
// *                                                                         *
// *   http://www.visibone.com/inpoly/inpoly.c.txt                           *
// *                                                                         *
// *                                       6/19/95 - Bob Stein & Craig Yap   *
// *                                       stein@gcomm.com                   *
// *                                       craig@cse.fau.edu                 *
// *                                                                         *
// ***************************************************************************

// Returns true if point (xt, yt) is inside shape (area).
// Does not take into account voids inside the shape.
bool SI_isPointinArea(ID_AREAS *area, float xt, float yt)
{
   bool inside = false;
   double xnew, ynew;
   double xold, yold;
   double x1, y1;
   double x2, y2;
   double targetX, targetY;
   ID_VERS *ver;

   // Need at least 3 edges to make an area
   if (area->iNumVer < 3)
      return(false);

   // Boundary box check
   if (xt > area->fURx)
      return(false);
   else if (yt > area->fURy)
      return(false);
   else if (xt < area->fLLx)
      return(false);
   else if (yt < area->fLLy)
      return(false);

   // Now see if point is inside area
   targetX = (double) xt;
   targetY = (double) yt;

   for (ver = area->segver; ver != NULL; ver = ver->next)
   {
      xold = ver->fx1;
      yold = ver->fy1;
      xnew = ver->fx2;
      ynew = ver->fy2;

      if (SI_isPointOnLine(xold, yold, xnew, ynew, targetX, targetY))
         return(true);      // Point is on the line, so it's inside

      if ((xnew > xold) || ((xnew == xold) && (ynew > yold)))
      {
         x1 = xold;
         x2 = xnew;
         y1 = yold;
         y2 = ynew;
      }
      else
      {
         x1 = xnew;
         x2 = xold;
         y1 = ynew;
         y2 = yold;
      }

      // Drawing a line up (vertical) from the target point, check how
      // many edges of the area are crossed (odd = in, even = out)

      // Ignore vertical edges
      if (x1 != x2)
      {
         if ((x1 < targetX) && (targetX <= x2) && ((targetY - y1) * (x2 - x1)) < ((y2 - y1) * (targetX - x1)))
	 {
	    inside = !inside;
         }
      }
   }

   return(inside);
}

// ----------------------------------------

// returns true if point (Px, Py) is on line (U0x, U0y)-(U1x, U1y)
bool SI_isPointOnLine(double U0x, double U0y, double U1x, double U1y, double Px, double Py)
{
   double xmin, ymin;
   double xmax, ymax;
   double m, c;
   double NewY;

   // Is point at either end of the line?
   //if ( (Px == U0x) && (Py == U0y) )
   if ( SI_IsEqualD(Px, U0x) && SI_IsEqualD(Py, U0y) )
      return(true);

   //if ( (Px == U1x) && (Py == U1y) )
   if ( SI_IsEqualD(Px, U1x) && SI_IsEqualD(Py, U1y) )
      return(true);

   // Find min max of line XY
   xmin = U0x;
   xmax = U1x;
   if (U0x > U1x)
   {
      xmin = U1x;
      xmax = U0x;
   }

   ymin = U0y;
   ymax = U1y;
   if (U0y > U1y)
   {
      ymin = U1y;
      ymax = U0y;
   }

   // Check vertical, horizontal and diagonal
   if (U0x == U1x)
   {
      // Vertical
      if (SI_IsEqualD(Px, U0x) && (ymin <= Py) && (Py <= ymax))
         return(true);
      else
         return(false);
   }
   else if (U0y == U1y)
   {
      // Horizontal
      if (SI_IsEqualD(Py, U0y) && (xmin <= Px) && (Px <= xmax))
         return(true);
      else
         return(false);
   }
   else
   {
      // diagonal
      m = ((U1y - U0y) / (U1x - U0x));
      c = U1y - (m * U1x);
      NewY = (m * Px) + c;

      //if (NewY == Py)
      if (SI_IsEqualD(NewY, Py))
         return(true);
      else
         return(false);
   }

   // Will never get here
   return(false);
}

// ------------------------------------------------

// If iFlag == 0
//    Returns the CCW angle (0 <= angle <= 359) from point (xs, ys) to point (xe, ye).
//    The point order is important. Reversing the start / end points will result in an
//    angle difference of 180 degrees.
// If iFlag = 1
//    Returns the CCW angle (0 <= angle <= 179) of the line with end points (xs, ys) and (xe, ye).
//    The point order does not matter.
//
double
SI_Angle(float xs, float ys, float xe, float ye, int iFlag)
{
   double  f, fDX, fDY, fDM;
   double  fxs, fys, fxe, fye;

   fxs = (double) xs;
   fys = (double) ys;
   fxe = (double) xe;
   fye = (double) ye;

   fDX = fxe - fxs;
   fDY = fye - fys;

   if (fDX == 0.0)
   {
      if (fye > fys)
         f = 90.0;
      else
         f = 270.0;
   }
   else if (fDY == 0.0)
   {
      if (fxe > fxs)
         f = 0.0;
      else
         f = 180.0;
   }
   else
   {
      fDM = fDY / fDX;

      if (fDM == 1.0)
         f = 45.0;
      else if (fDM == -1.0)
         f = -45.0;
      else
      {
         f = atan(fDM) * 360.0 / PIx2;
      }

      if (fDX < 0.0)
      {
         f += 180.0;
      }
      else if ( (fDY < 0.0) && (fDX > 0.0) )
      {
         f += 360.0;
      }
   }

   // To prevent angles very close to 180 (e.g., 179.8) when iFlag = 1,
   // or 360 when iFlag = 0
   f = (int)(f + 0.5);

   if (iFlag == 1)
   {
      if (f >= 180.0)
         f -= 180.0;
   }
   else
   {
      if (f >= 360.0)
         f -= 360.0;
   }

   return (f);				// 0 <= f <= 359  or  0 <= f <= 179
}

// ------------------------------------------------
// Heapsort
// Numerical Recipes in C, Chapter 8, p 247
void SI_heapsort(unsigned int N, HS_SORT *sHSsort, int (*compare)(int i1, int i2, int iSortType), int iSortType )
{
    int indxT;
    int indxt1;
    int indxt2;

 // int t;			/* the temporary value */
    unsigned int n;		/* heap indexes */
    unsigned int parent;	/* heap indexes */
    unsigned int index;		/* heap indexes */
    unsigned int child;		/* heap indexes */

    n = N;
    parent = N / 2;

    /* loop until array is sorted */
    for (;;)
    { 
        if (parent > 0)
       	{ 
            /* first stage - Sorting the heap */
         // t = arr[--parent];  		/* save old value to t */
	    indxT = sHSsort[--parent].iIndex;
        }
       	else
       	{
            /* second stage - Extracting elements in-place */
            n--;               			/* make the heap smaller */
            if (n == 0)
	        return;				/* When the heap is empty, we are done */

         // t = arr[n];         		/* save lost heap entry to temporary */
	    indxT = sHSsort[n].iIndex;

         // arr[n] = arr[0];   			/* save root entry beyond heap */
	    sHSsort[n].iIndex = sHSsort[0].iIndex;
        }

        /* insert operation - pushing t down the heap to replace the parent */
        index = parent;				/* start at the parent index */
        child = index * 2 + 1;			/* get its left child index */
        while (child < n)
       	{
            /* choose the largest child */
            if (child + 1 < n)
	    {
             // if ( arr[child + 1] > arr[child] )
	        indxt1 = sHSsort[child + 1].iIndex;
	        indxt2 = sHSsort[child].iIndex;
		if ( ((*compare)(indxt1, indxt2, iSortType)) == 1 )
	        {
                    child++;			/* right child exists and is bigger */
                }
	    }

            /* is the largest child larger than the entry? */
         // if (arr[child] > t)
	    indxt1 = sHSsort[child].iIndex;
	    if ( ((*compare)(indxt1, indxT, iSortType)) == 1 )
	    {
             // arr[index] = arr[child];	/* overwrite entry with child */
	        sHSsort[index].iIndex = sHSsort[child].iIndex;
                index = child;			/* move index to the child */
                child = index * 2 + 1;		/* get the left child and go around again */
            }
	    else
	    {
                break;				/* t's place is found */
            }
        }

        /* store the temporary value at its new location */
     // arr[index] = t; 
	sHSsort[index].iIndex = indxT;
    }
}

// ------------------------------------------------

long
SI_iabs(long x)
{
   if (x < 0)
      return (-x);
   else
      return (x);
}

// ------------------------------------------------

// return index of t[] in s[], -1 if none
int
SI_indx(char *s, char *t)
{
   int     i, j, k;

   for (i = 0; s[i] != '\0'; i++)
   {
      for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++)
      {
	 ;
      }
      if (t[k] == '\0')
      {
	 return (i);
      }
   }

   return (-1);
}


int
SI_IsEqual(float x, float y)
{
   float fDiff;

   fDiff = x - y;

   if ( (-FLOAT_TOL < fDiff) && (fDiff < FLOAT_TOL) )
      return(1);
   else
      return(0);
}


bool
SI_IsEqualD(double x, double y)
{
   double fDiff;

   fDiff = x - y;

   if ( (-FLOAT_TOLD < fDiff) && (fDiff < FLOAT_TOLD) )
      return(true);
   else
      return(false);
}


void
SI_remove_crlf(char *ln)
{
   int     i, j;
   char    tmp[SI_MAXLINE];

   j = 0;

   for (i = 0; ln[i] != '\0'; i++)
   {
      if ((ln[i] != '\r') && (ln[i] != '\n'))	// ignore CR & LF
      {
	 tmp[j] = ln[i];
	 j++;
      }
   }

   tmp[j] = '\0';

   strcpy(ln, tmp);
}


// returns 0 if blank line, else 1
int
SI_blank(char *ln)
{
   int     i, blnk;

   blnk = 0;

   for (i = 0; ln[i] != '\0'; i++)
   {
      if (!((ln[i] == ' ') || (ln[i] == '\n') || (ln[i] == '\t') || (ln[i] == '\r')))
      {
	 blnk = 1;
	 break;
      }
   }

   return (blnk);
}


void
SI_remove_comments(char *ln)
{
   int     i;

   for (i = 0; ln[i] != '\0'; i++)
   {
      if (ln[i] == '#')
      {
	 ln[i] = '\0';
	 break;
      }
   }
}


void
SI_remove_leading_spaces(char *ln)
{
   int     i, j, ok;
   char    tmp[SI_MAXLINE];

   ok = 1;
   j = 0;

   for (i = 0; ln[i] != '\0'; i++)
   {
      if (ok == 1)
      {
	 if (ln[i] != ' ')
	    ok = 0;
      }

      if (ok == 0)
      {
	 tmp[j] = ln[i];
	 j++;
      }
   }

   tmp[j] = '\0';

   strcpy(ln, tmp);
}


void
SI_remove_trailing_spaces(char *ln)
{
   int i, j;

   j = 0;
   
   for (i = 0; ln[i] != '\0'; i++)
   {
      if (ln[i] != ' ')
	 j = i;				// position of last character
   }

   // Empty string check
   if (i == 0)
      ln[0] = '\0';
   else
      ln[j + 1] = '\0';
}


void
SI_remove_quotes(char *ln)
{
   int     i, j;
   char    tmp[SI_MAXLINE];

   j = 0;

   for (i = 0; ln[i] != '\0'; i++)
   {
      if (ln[i] != '\"')
      {
	 tmp[j] = ln[i];
	 j++;
      }
   }

   tmp[j] = '\0';

   strcpy(ln, tmp);
}


void
SI_replace_char_with_space(char *ln, char ch)
{
   int i;

   for (i = 0; ln[i] != '\0'; i++)
   {
      if (ln[i] == ch)
      {
         ln[i] = ' ';
      }
   }   
}


char *
SI_strsave(char *stg)
{
   char   *sp;

   sp = (char *) SI_ALLOC(strlen(stg) + 1);

   if (sp == NULL)
   {
      // Unable to allocate memory
      return(NULL);
   }

   strcpy(sp, stg);

   return (sp);
}


void *
SI_sp_alloc(unsigned int size)
{
   void *foo;

   if (NULL == (foo = (void *) malloc(size)))
   {
      // Unable to allocate memory
      return ((void *) NULL);
   }

   return ((void *) foo);
}


void *
SI_sp_malloc(size_t size)
{
   void *retval;
  
   if (!(retval = malloc(size)))
   {
      // Unable to allocate memory
      return ((void *) NULL);
   }
  
   return(retval);
}


void
g_debug(char *fn, char *str)
{
   char sFN[SI_MAXLINE];
   FILE *fp1;

   //sprintf(sFN, "%s/%s", g_dsndir, fn);
   strcpy(sFN, fn);
   fp1 = fopen(sFN, "a");
   if (fp1 != NULL)
   {
      fprintf(fp1, "%s", str);
      fclose(fp1);
   }
}

// ------------------------------------------------

