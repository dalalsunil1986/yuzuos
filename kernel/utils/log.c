#include <kernel/utils/log.h>
#include <kernel/utils/stdio.h>
#include <kernel/system/io.h>

static const char *log_type_msg[] = {
    "INFO", "WARN", "ERROR"};
static const char *log_type_color[] = {
    "\x1b[32m", "\x1b[33m", "\x1b[31m"};

void log_write_char(char ch)
{
  while ((inb(LOG_PORT + 5) & 0x20) == 0)
    ;
  outb(LOG_PORT, ch);
}

void log_write(const char *buffer, int len)
{
  for (int i = 0; i < len; i++)
    log_write_char(buffer[i]);
}

void log_log(enum log_type type, const char *file, int line, const char *format, ...)
{
  int len;

  va_list ap;
  va_start(ap, format);
  char buffer_format[LOG_BUF];
  len = vsnprintf(buffer_format, LOG_BUF, format, ap);
  va_end(ap);

  char buffer[LOG_BUF];
  len = sprintf(buffer, "\x1B[1m\x1B[34m[Kernel] %s%-5s\x1b[90m %s:%d:\x1B[37m %s\x1b[0m", log_type_color[type], log_type_msg[type], file, line, buffer_format);

  log_write(buffer, len);
}

void log_init()
{
  outb(LOG_PORT + 1, 0x00);
  outb(LOG_PORT + 3, 0x80);
  outb(LOG_PORT + 0, 0x03);
  outb(LOG_PORT + 1, 0x00);
  outb(LOG_PORT + 3, 0x03);
  outb(LOG_PORT + 2, 0xC7);
  outb(LOG_PORT + 4, 0x0B);
  outb(LOG_PORT + 4, 0x1E);
  outb(LOG_PORT + 0, 0xAE);

  if (inb(LOG_PORT + 0) != 0xAE)
    return;

  outb(LOG_PORT + 4, 0x0F);

  log_info("Log: Initialized\n");
}
