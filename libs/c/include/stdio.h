#pragma once

#include <stdarg.h>
#include <stddef.h>

#define _VSNPRINTF_ZEROPAD 1
#define _VSNPRINTF_SIGN 2
#define _VSNPRINTF_PLUS 4
#define _VSNPRINTF_SPACE 8
#define _VSNPRINTF_LEFT 16
#define _VSNPRINTF_SPECIAL 32
#define _VSNPRINTF_LARGE 64

#define SEEK_SET 0
#define stderr stderr

typedef struct
{
  int unused;
} FILE;

extern FILE *stderr;

int fclose(FILE *);
int fflush(FILE *);
FILE *fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
size_t fread(void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
void setbuf(FILE *, char *);
int vfprintf(FILE *, const char *, va_list);
int vsprintf(char *str, const char *format, va_list arg);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int sprintf(char *str, const char *format, ...);
