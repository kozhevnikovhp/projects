/*
 * log.cpp
 *
 * Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include "log.h"
#include <stdio.h>
#include <stdarg.h>

// for debug purposes, when syslog is not easy to look at -> create dump.txt file
#define DUMP 0

static int verboseLevel = 8;

void log_init(bool bAlsoToStdErr)
{
    int option = LOG_NDELAY
               | LOG_PID
               | (bAlsoToStdErr ? LOG_PERROR : 0);

    openlog("lte-manager", option, LOG_DAEMON);
    log_debug("Logging system initialized");
}

void log_level(int level)
{
    log_debug("Log level set to %d", level);
    verboseLevel = level;
}

void log_debug(const char *format, ...)
{
    if (verboseLevel >= LOG_DEBUG)
    {
        va_list ap;

        va_start(ap, format);
        vsyslog(LOG_DEBUG, format, ap);

#if DUMP
        FILE *pFile = fopen("dump.txt", "a");
        if (pFile)
        {
            vfprintf(pFile, format, ap);
            fprintf(pFile, "\n");
            fclose(pFile);
        }
#endif

        va_end(ap);
    }
}

void log_info(const char *format, ...)
{
    if (verboseLevel >= LOG_INFO)
    {
        va_list ap;

        va_start(ap, format);
        vsyslog(LOG_INFO, format, ap);

#if DUMP
        FILE *pFile = fopen("dump.txt", "a");
        if (pFile)
        {
            vfprintf(pFile, format, ap);
            fprintf(pFile, "\n");
            fclose(pFile);
        }
#endif

        va_end(ap);
    }
}

void log_error(const char *format, ...)
{
    if (verboseLevel >= LOG_ERR)
    {
        va_list ap;

        va_start(ap, format);
        vsyslog(LOG_ERR, format, ap);

#if DUMP
        FILE *pFile = fopen("dump.txt", "a");
        if (pFile)
        {
            vfprintf(pFile, format, ap);
            fprintf(pFile, "\n");
            fclose(pFile);
        }
#endif

        va_end(ap);
    }
}
