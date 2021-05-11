#pragma once

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);
size_t strlen(const char *str);
char *strdup(const char *s);
int strcmp(const char *str1, const char *str2);
