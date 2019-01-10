/*
 * log.h
 *
 * Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 */

#pragma once

#include <syslog.h>

void log_init(bool bAlsoToStdErr);
void log_level(int level);

void log_debug(const char *format, ...);
void log_info(const char *format, ...);
void log_error(const char *format, ...);

