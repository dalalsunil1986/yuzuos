#pragma once

#include <stddef.h>
#include <stdarg.h>

#define VSNPRINTF_ZEROPAD 1
#define VSNPRINTF_SIGN 2
#define VSNPRINTF_PLUS 4
#define VSNPRINTF_SPACE 8
#define VSNPRINTF_LEFT 16
#define VSNPRINTF_SPECIAL 32
#define VSNPRINTF_LARGE 64

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
