#pragma once

#define ALIGN_DOWN(base, size) ((base) & -((__typeof__(base))(size)))
#define ALIGN_UP(base, size) ALIGN_DOWN((base) + (size)-1, (size))

#define DIV_CEIL(x, y) (       \
    {                          \
      __auto_type _x = (x);    \
      __auto_type _y = (y);    \
      _x / _y + (_x % _y > 0); \
    })

#define MIN_T(type, x, y) (  \
    {                        \
      type __x = (x);        \
      type __y = (y);        \
      __x < __y ? __x : __y; \
    })
#define MAX_T(type, x, y) (  \
    {                        \
      type __x = (x);        \
      type __y = (y);        \
      __x > __y ? __x : __y; \
    })
