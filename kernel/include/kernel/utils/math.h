#pragma once

#include <ukernel/math.h>

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

#define MIN(x, y) (       \
    {                     \
      typeof(x) _x = (x); \
      typeof(y) _y = (y); \
      (void)(&_x == &_y); \
      _x < _y ? _x : _y;  \
    })

#define MAX(x, y) (       \
    {                     \
      typeof(x) _x = (x); \
      typeof(y) _y = (y); \
      (void)(&_x == &_y); \
      _x > _y ? _x : _y;  \
    })
