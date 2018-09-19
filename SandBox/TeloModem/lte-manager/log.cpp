/*
 * log.cpp
 *
 * Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include "log.h"
#include <stdio.h>
#include <stdarg.h>

#define DUMP 0

static int log_debug_messages = 0;

void log_init(int to_stderr_also) 
{
    int option = LOG_NDELAY
               | LOG_PID
               | (to_stderr_also ? LOG_PERROR : 0);
    
    openlog("lte-manager", option, LOG_DAEMON);
    log_debug("Logging system initialized");
}


void log_level(const int d) 
{
    log_debug("Log level set to %d", d);
    log_debug_messages = d;
}

void log_debug(const char *format, ...)
{
    if (log_debug_messages)
    {
        va_list ap;

        va_start(ap, format);
        vsyslog(LOG_DEBUG, format, ap);
        va_end(ap);
    }
}

void log_info(const char *format, ...)
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

void log_error(const char *format, ...)
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
