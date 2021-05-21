#pragma once

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);
char *strcpy(char *restrict s1, const char *restrict s2);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlen(const char *str);
size_t strnlen(const char *s, size_t maxlen);
char *strchr(const char *str, int c);
char *strcat(char *restrict s1, const char *restrict s2);
