/*
 * log.h
 *
 * Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 */

#pragma once

#include <syslog.h>

void log_init(int log_to_stderr_also);
void log_level(const int log_debug_messages_also);

void log_debug(const char *format, ...);
void log_info(const char *format, ...);
void log_error(const char *format, ...);

