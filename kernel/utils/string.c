#include <kernel/utils/string.h>
#include <kernel/utils/stdlib.h>

void *memcpy(void *dest, const void *src, size_t n)
{
  char *p_dest = (char *)dest;
  char *p_src = (char *)src;

  for (size_t i = 0; i < n; i++)
    p_dest[i] = p_src[i];

  return dest;
}

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

char *strdup(const char *s)
{
  size_t len = strlen(s) + 1;
  void *s_new = calloc(len, sizeof(char));
  if (!s_new)
    return NULL;

  return (char *)memcpy(s_new, s, len);
}
