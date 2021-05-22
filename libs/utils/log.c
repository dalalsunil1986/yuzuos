#include <utils/log.h>
#include <syscall.h>
#include <stdarg.h>
#include <stdio.h>

static char buffer[LOG_BUF];

_syscall4(log, enum log_type, const char *, int, const char *);
void log_log(enum log_type type, const char *file, int line, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);
  syscall_log(type, file, line, buffer);
}
