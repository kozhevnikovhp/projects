//********************************************************************************
//********************************************************************************
//**										**
//**	File:		Poly2RT.cpp						**
//**	Contents:	Polygon -> Rectangles & Right Isosceles Triangles	**
//**										**
//**	Author:		Guy de Burgh						**
//**										**
//**	Copyright (c) 2009 IBM      						**
//**	All rights, including trade secret rights, reserved.			**
//**										**
//********************************************************************************
//********************************************************************************

//********************************************************************************
//
// Version
//  0.1       (07/22/2008)
//	- First version
//
//  0.2       (07/24/2008)
//	- seg_areas.isf and AreaID.txt update
//
//********************************************************************************

#include <stdio.h>
#include <ctype.h>
#include <string.h>
//#include <math.h>

// Windows
#include <stdlib.h>
#include <time.h>
// Unix
//   #include <malloc.h>
//   #include <sys/time.h>

// For mkdir() on Windows
// (may need a different header for Unix)
#include <direct.h>

// --------------------------------------------------------------------------

#include "common_dec.hpp"

#include "date.hpp"

#define PRODUCT	"poly2rt"
#define VERSION	"0.3"

// --------------------------------------------------------------------------
// Forward declarations

char *mk_date();
char *mk_time();
void help_msg();
void free_at_last();

char *get_time();
int parse_drag_drop(char *sDrag_drop, char *sPath, char *sSegFile);
float convert2DataBaseUnits(char *sUnits, float fWaveLength);

// --------------------------------------------------------------------------
// External declarations

extern int read_seg(char *sSegFile);
extern void rs_free();

extern void error_message(char *sEW, char *str);
extern void error_log(char *str);

extern int wi_read_layer(char *sPath);
extern int wi_read_outline(char *sPath);
extern int wi_write_isf(char *sPath, int AreaID);
extern void wi_layer_free();
extern void ws_free_S34();


extern int ws_write_seg(char *sFile, int AreaID);

// --------------------------------------------------------------------------
// Global declarations

extern char sLogStr[];
extern FILE *fp_log;

extern char sUnits[];

#if 0
// All Nets (from net.txt)
CRA_ID_ALLNETS *id_AllNetList;
CRA_ID_ALLNETS *id_AllNetList_b;
#endif

// ------------------------------

// The test codes are entered as follows:
//   - using -z on the command line
//   - using environemnt variable POLY2RT_TC
// 
// test_code	output (stdout, log file)
// ---------    ------
//     0        nothing (default)
//     >0       input info (set to 1 if just input info is required) (Poly2RT.cpp)
//     1        see above (Poly2RT.cpp)
//     2        shape info (read_seg.cpp)
//     3        convert arc to segs  (read_seg.cpp)
//     4        layer info (write_isf.cpp)
//     5        outline info (write_isf.cpp)
//     6        write ISF file (Poly2RT.cpp)
//
int test_code;

// --------------------------------------------------------------------------
// Start

int
main(int argc, char *argv[])
{
   int     AreaID = 0;                          // this ID is read from command line and is ID of the area to be meshed. Default value is 0;
   int     i;
   int     iN;
   int     argcnt;
   int     nna_flag;
   int	   a_flag;
   int     i_flag;
   int     f_flag;
   int     z_flag;
   int     res;
   int     res_rt;
   int     iCL;					// Command line (1) or drag-and-drop (0) input mechanism
   float   fFreq;                               // Input frequency (GHz)
   float   fWaveLength;                         // Above frequency converted to wavelength (cm)
   float   fTolerance;                          // Wavelength converted to database units
   char    sDrag_drop[SI_SMLINE];		// full path + filename.ext
   char    sPath[SI_SMLINE];			// full path
   char    sSegFile[SI_SMLINE];	        	// filename.ext
   char    sLogFile[SI_SMLINE];	        	// log file
   char    sT[SI_SMLINE];
   char   *sEnv;

   // Example
   //   sDrag_drop
   //     C:\me\designs\company 1\seg.txt
   //
   //   sSegFile
   //     seg.txt
   //
   //   sPath
   //     C:\me\designs\company 1
   //
   
   // -----------------------------------

   // Initialize
   iCL = 0;				// assume drag-and-drop is being used.
   sDrag_drop[0] = '\0';
   sSegFile[0] = '\0';
   sPath[0] = '\0';
   fFreq = 30.0;                        // default = 30.0 GHz

   argcnt = 0;
   nna_flag = 0;			// need next argument flag
   a_flag = 0;
   i_flag = 0;
   f_flag = 0;
   z_flag = 0;

   res = 0;
   fp_log = NULL;

   // -----------------------------------

   // test_code
   // Environment variable that duplicates -z command line option
   // Command line overwrites environment variable.

   test_code = 0;			// default
   sEnv = getenv("POLY2RT_TC");
   if (sEnv != NULL)
   {
      iN = sscanf(sEnv, "%d", &test_code);
   }

   // -----------------------------------

   // Sign on
   fprintf(stdout, "\n");
   fprintf(stdout, "%s %s\n", PRODUCT, VERSION);
   fprintf(stdout, "Copyright (c) 2009 IBM Corp.\n");
   fprintf(stdout, "All rights, including trade secret rights, reserved.\n");
   fprintf(stdout, "\n");

   // -----------------------------------

#if 0
   // Command line args
   for (i = 0; i < argc; i++)
   {
      fprintf(stdout, "%d : %s\n", i, argv[i]);
   }
#endif

   // -----------------------------------

   // Get command line arguments, or drag-and-drop argument
   if (argc == 1)			// no arguments
   {
      // if no arguments: display usage message
      help_msg();
      exit(0);
   }
   else
   {
      argcnt = 1;			// no. arguments used, start at 1 for program name
      for (i = 1; i < argc; i++)
      {
	 *++argv;

	 if ( ((*argv)[0] == '-') && (nna_flag == 0) )
	 {
	    iCL = 1;			// Command line switches being used

	    if (nna_flag != 0)
	    {
	       fprintf(stdout, "Error:  Argument missing after switch <%c>.\n", (*--argv)[1]);
	       exit(1);
	    }

	    switch ((*argv)[1])
	    {
	    case 'a':
	       argcnt++;
	       a_flag = 1;
	       nna_flag = 1;
	       break;

	    case 'h':
	       help_msg();
	       exit(0);

	    case 'i':
	       argcnt++;
	       i_flag = 1;
	       nna_flag = 1;
	       break;

	    case 'f':
	       argcnt++;
	       f_flag = 1;
	       nna_flag = 1;
	       break;

	    case 'v':
	       fprintf(stdout, "   Compiled on %s at %s\n\n", mk_date(), mk_time());
	       exit(0);

	    case 'z':
	       argcnt++;
	       z_flag = 1;
	       nna_flag = 0;
	       break;

	    default:
	       argcnt++;
	       fprintf(stdout, "Warning:  Unknown switch <%c> on command line - ignored.\n", (*argv)[1]);
	       nna_flag = 0;
	       break;
	    }
	 }
	 else
	 {
	    if (a_flag == 1)
	    {
	       argcnt++;
	       iN = sscanf(*argv, "%ld", &AreaID);
	       a_flag = 2;
	       nna_flag = 0;
	    }
	    if (f_flag == 1)
	    {
	       argcnt++;
	       iN = sscanf(*argv, "%f", &fFreq);
	       f_flag = 2;
	       nna_flag = 0;
	    }

	    if (i_flag == 1)
	    {
	       argcnt++;
	       strcpy(sSegFile, *argv);
	       i_flag = 2;
	       nna_flag = 0;
	    }

	    if (z_flag == 1)
	    {
	       argcnt++;
	       iN = sscanf(*argv, "%d", &test_code);
	       z_flag = 2;
	       nna_flag = 0;
	    }
	 }
      }

      // *argv points to very last argument now, after completion of FOR loop
      //fprintf(stdout, "argc = %d, argcnt = %d\n", argc, argcnt);

      if ((argc - argcnt) != 0)
      {
	 if (iCL == 1)
	 {
	    // Command line
	    // Allow z option
	    if (z_flag == 2)
	    {
	       strcpy(sSegFile, *argv);
	    }
	    else
	    {
	       fprintf(stdout, "Warning:  Extra arguments on command line - ignored.\n");
	       fprintf(stdout, "last arg = %s\n", *argv);
	    }
	 }
	 else
	 {
	    // Drag and Drop
	    // Save drag-and-drop argument
            strcpy(sDrag_drop, *argv);
	 }
      }
   }

   // -----------------------------------

   // If drag-and-drop parse the sDrag_drop[] argument
   // - Get the directory (sPath[])
   // - Get the filename (sSegFile[])

   if (iCL == 0)
   {
      res = parse_drag_drop(sDrag_drop, sPath, sSegFile);
      if (res == 1)
      {
         fprintf(stdout, "Error: Unable to parse \"%s\"\n", sDrag_drop);
         exit(1);
      }

      // Now sPath[] is the current working directory
   }

   // If no directory found from drag-and-drop or command line mode is being used
   if (sPath[0] == '\0')
   {
      // get the current working directory, and save it in sPath[]
      getcwd(sPath, SI_SMLINE);
   }

   // Change directory to sPath
   if (chdir(sPath) != 0)
   {
      fprintf(stdout, "   Unable to CD to directory %s\n", sPath);
      exit(1);
   }

   // At this point
   //    sPath[] = the directory name
   //    sSegFile[] = the filename
   // E.g.
   //    sPath = C:\me\demo
   //    sSegFile = seg.txt
   
   // -----------------------------------
   // Now open log file

   sprintf(sLogFile, "%s/%s.log", sPath, PRODUCT);
   fp_log = fopen(sLogFile, "w");
   if (fp_log == NULL)
   {
      fprintf(stdout, "   Unable to open log file.\n");
      exit(1);
   }

   // -----------------------------------

   // Read seg.txt file
   // populates sUnits[] and structure id_ShapeList
   res = read_seg(sSegFile);

   // -----------------------------------

   // Convert fFreq (GHz) to fWaveLength (cm)
   // c = 3 x 10^8 m/s

   fWaveLength = (float) 30.0 / fFreq;
   fTolerance = convert2DataBaseUnits(sUnits, fWaveLength);

   // -----------------------------------

   if (test_code > 0)
   {
      sprintf(sLogStr, "test_code = %d\n", test_code);
      error_log(sLogStr);

      sprintf(sLogStr, "date/time = %s\n", get_time());
      error_log(sLogStr);

      if (iCL == 0)
      {
         sprintf(sLogStr, "sDrag_drop = %s\n", sDrag_drop);
         error_log(sLogStr);
      }

      sprintf(sLogStr, "sPath = %s\n", sPath);
      error_log(sLogStr);
      sprintf(sLogStr, "sSegFile = %s\n", sSegFile);
      error_log(sLogStr);
      sprintf(sLogStr, "fFreq = %f GHz  fWaveLength = %f cm\n", fFreq, fWaveLength);
      error_log(sLogStr);
      sprintf(sLogStr, "fTolerance = %f %s\n", fTolerance, sUnits);
      error_log(sLogStr);
      sprintf(sLogStr, "AreaID = %d\n", AreaID);
      error_log(sLogStr);
   }

   // -----------------------------------
   // Polygon -> rectangle / right isosceles triangle
   // Returns:
   //   0 = OK
   //   1 = area is not a SOLID
   //   2 = area does not exist
   //   3 = no triangles/rectangles created (area too small)
   //   4 = Orphan shape found
   res_rt = 0;
   if (AreaID != -1)
      res_rt = mp_polygon2rt(fTolerance, AreaID);

   if (res_rt != 0)
   {
      if (res_rt == 1)
         sprintf(sT, "Area %d is a void.", AreaID);
      else if (res_rt == 2)
         sprintf(sT, "Area %d does not exist.", AreaID);
      else if (res_rt == 3)
         sprintf(sT, "No Rectangles/Triangles created. Area %d is (probably) too small.", AreaID);
      else if (res_rt == 4)
         sprintf(sT, "Orphan shape(s) found when meshing area %d.", AreaID);

      error_message("E03", sT);
   }

   // -----------------------------------

   if ( (test_code == 6) && ((res_rt == 0) || (res_rt == 4)) )
   {
      // write out ISF file
      error_log("Writing ISF file\n");
      res = wi_read_layer(sPath);
      res = wi_read_outline(sPath);
      res = wi_write_isf(sPath, AreaID);
   }

   // -----------------------------------
   // Write out a new seg.txt file
   if ((res_rt == 0) || (res_rt == 4))
      res = ws_write_seg("seg_new.txt", AreaID);

   // -----------------------------------

   // Free everything
   free_at_last();

   // Close log file
   if (fp_log != NULL)
      fclose(fp_log);

   // -----------------------------------

   exit(res_rt);		// exit code: 0 = all OK, else > 1

   return(res_rt);		// To keep the compiler happy
}

// --------------------------------------------------------

void
free_at_last()
{
   rs_free();
   wi_layer_free();
   mp_free_S34();

   return;
}

// --------------------------------------------------------

// Returns true file exists, else false
bool isExistFile(char *s)
{
   FILE *fp;

   // Check that file exists
   fp = fopen(s, "r");
   if (fp == NULL)
   {
      return(false);			// file does not exist
   }

   fclose(fp);

   return(true);
}

// --------------------------------------------------------

void
help_msg()
{
   fprintf(stdout, "   Usage: %s [options] [commands] [last arg]\n", PRODUCT);
   fprintf(stdout, "\n");
   fprintf(stdout, "   Where \"options\" are:\n");
   fprintf(stdout, "      [-v]                    : compilation date,\n");
   fprintf(stdout, "      [-f frequency]          : frequency (GHz),\n");
   fprintf(stdout, "\n");
   fprintf(stdout, "   and \"commands\" are:\n");
   fprintf(stdout, "      [-i filename]           : the input seg.txt file name,\n");
   fprintf(stdout, "\n");
   fprintf(stdout, "   and \"last arg\" is:\n");
   fprintf(stdout, "      [drag drop name]        : the input drag-and-drop file name,\n");
   fprintf(stdout, "\n");
   fprintf(stdout, "   The options and commands can be entered in any order.\n");
   fprintf(stdout, "\n");
}

// --------------------------------------------------------

char *
mk_date()
{
   static char date_string[64];

   sprintf(date_string, "%s", __DATE__);
   return(date_string);
}


char *
mk_time()
{
   static char time_string[64];

   sprintf(time_string, "%s", __TIME__);
   return(time_string);
}

// --------------------------------------------------------

// Parse the sDrag_drop[] argument
// - Get the directory
// - Get the filename|directory name
//
// Examples
//   sDrag_drop
//     C:\me\designs\company 1\seg.txt
//
//   sSegFile
//     seg.txt
//
//   sPath
//     C:\me\designs\company 1

int
parse_drag_drop(char *sDrag_drop, char *sPath, char *sFile)
{
   int i, j, k;
   int iLast;
   char sDir[SI_SMLINE];
   char sFD[SI_SMLINE];

   // sDrag_drop[] now contains the directory and filename|directory
   // Need to split this into "directory" and "filename|directory"

   // Find last "\"
   iLast = 0;
   for (i = 0; sDrag_drop[i] != '\0'; i++)
   {
      if (sDrag_drop[i] == '\\')
      {
	 iLast = i;			// location of last "\"
      }
   }

   // Everything up to last "\" = "directory"
   // Everything from last "\" to end of line = "filename|directory"
   sDir[0] = '\0';
   sFD[0] = '\0';

   if (iLast != 0)
   {
      j = 0;
      k = 0;
      for (i = 0; sDrag_drop[i] != '\0'; i++)
      {
         if (i < iLast)
         {
	    sDir[j] = sDrag_drop[i];
	    j++;
	    sDir[j] = '\0';
         }

         if (i > iLast)
         {
	    sFD[k] = sDrag_drop[i];
	    k++;
	    sFD[k] = '\0';
         }
      }
   }
   else
   {
      // No "\" found, so no directory. Everything is the "filename|directory"
      strcpy(sFD, sDrag_drop);
   }

   strcpy(sPath, sDir);
   strcpy(sFile, sFD);

   return(0);
}

// --------------------------------------------------------------------------

char *get_time()
{
   char sTime[SI_SMLINE];
   time_t e_time;
   struct tm *the_time;

   (void) time(&e_time);
   the_time = localtime(&e_time);
   strcpy(sTime, strtok(asctime(the_time), "\n"));

   return(SI_strsave(sTime));
}

// --------------------------------------------------------------------------

// Convert cm to database units
float convert2DataBaseUnits(char *sUnits, float fWaveLength)
{
   float fScale;

   fScale = (float) 1.0;

   if (strcmp(sUnits, "mils") == 0)
   {
      fScale = (float) (1000.0 / 2.54);
   }
   else if ( (strcmp(sUnits, "milimeters") == 0) || (strcmp(sUnits, "mm") == 0) )
   {
      fScale = (float) 10.0;
   }
   else if (strcmp(sUnits, "inches") == 0)
   {
      fScale = (float) (1.0 / 2.54);
   }

   return(fWaveLength * fScale);
}

// --------------------------------------------------------------------------
   
// --- eof ---

