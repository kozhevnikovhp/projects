//********************************************************************************
//********************************************************************************
//**										**
//**	File:		error.cpp						**
//**	Contents:	Error and Warnings					**
//**										**
//**	Author:		Guy de Burgh						**
//**										**
//**	Copyright (c) 2009 IBM         						**
//**	All rights, including trade secret rights, reserved.			**
//**										**
//********************************************************************************
//********************************************************************************

// ---------------------------------------------------------

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

#include "common_dec.hpp"

// ---------------------------------------------------------
// Forward declarations

void error_message(char *sEW, char *str);
void error_log(char *str);

// ---------------------------------------------------------
// External declarations

// ---------------------------------------------------------
// Global declarations

char sLogStr[SI_MAXLINE];
FILE *fp_log;

// ---------------------------------------------------------

// Write Errors and Warnings to log file
void error_message(char *sEW, char *str)
{
   if (strcmp(sEW, "E01") == 0)
   {
      sprintf(sLogStr, "E01: Unable to write file %s\n", str);
      error_log(sLogStr);
   }
   else if (strcmp(sEW, "E02") == 0)
   {
      sprintf(sLogStr, "E02: Unable to read file %s\n", str);
      error_log(sLogStr);
   }
   else if (strcmp(sEW, "E03") == 0)
   {
      sprintf(sLogStr, "E03: Mesh: %s\n", str);
      error_log(sLogStr);
   }
   else if (strcmp(sEW, "W01") == 0)
   {
      sprintf(sLogStr, "W01: Warning 01 %s\n", str);
      error_log(sLogStr);
   }
   else
   {
      sprintf(sLogStr, "%s: Unknown error/warning: %s\n", sEW, str);
      error_log(sLogStr);
   }

   return;
}


// Write Information messages to log file, or stdout if no log file
void error_log(char *str)
{
   if (fp_log != NULL)
      fprintf(fp_log, "%s", str);
   else
      fprintf(stdout, "%s", str);
}

// ---------------------------------------------------------
