#include <core/log.h>
#include <syscall.h>
#include <stdarg.h>
#include <stdio.h>

static char buffer[LOG_BUF];

_syscall2(log, enum log_type, const char *);
void log_log(enum log_type type, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);
  syscall_log(type, buffer);
}
