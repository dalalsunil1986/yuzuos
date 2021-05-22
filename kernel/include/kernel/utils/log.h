#pragma once

#include <stdarg.h>

#define LOG_PORT 0x3F8
#define LOG_BUF 4096

#define log_info(...) log_log(LOG_INFO, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __VA_ARGS__)

enum log_type
{
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR
};

void log_init();
void log_log(enum log_type type, const char *format, ...);
void log_format(enum log_type type, const char *format, va_list ap);
