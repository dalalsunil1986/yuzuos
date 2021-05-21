#include <string.h>

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

char *strchr(const char *str, int c)
{
  do
  {
    if (*str == c)
      return (char *)str;
  } while (*str++);

  return (0);
}

size_t strnlen(const char *s, size_t maxlen)
{
  size_t i;

  for (i = 0; i < maxlen; i++)
    if (s[i] == '\0')
      break;
  return i;
}

char *strncpy(char *dest, const char *src, size_t n)
{
  size_t size = strnlen(src, n);
  if (size != n)
    memset(dest + size, '\0', n - size);
  return memcpy(dest, src, size);
}

char *strcpy(char *restrict s1, const char *restrict s2)
{
  return memcpy(s1, s2, strlen(s2) + 1);
}

char *strcat(char *restrict s1, const char *restrict s2)
{
  strcpy(s1 + strlen(s1), s2);
  return s1;
}
