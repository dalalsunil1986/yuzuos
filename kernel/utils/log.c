#include <kernel/utils/log.h>
#include <kernel/system/io.h>

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
  int len = 0;
  while (format[len])
    len++;
  log_write(format, len);
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
