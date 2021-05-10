#include <kernel/utils/string.h>

void *memset(void *str, int c, size_t n)
{
  char *p_str = (char *)str;
  for (size_t i = 0; i < n; i++)
    p_str[i] = (unsigned char)c;
  return str;
}

size_t strlen(const char *str)
{
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}