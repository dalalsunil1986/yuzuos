#pragma once

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;
typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;
typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;

typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

#if (!defined __cplusplus || __cplusplus >= 201103L || defined __STDC_LIMIT_MACROS)

#define INT8_MAX __INT8_MAX__
#define INT8_MIN (-INT8_MAX - 1)
#define UINT8_MAX __UINT8_MAX__
#define INT16_MAX __INT16_MAX__
#define INT16_MIN (-INT16_MAX - 1)
#define UINT16_MAX __UINT16_MAX__
#define INT32_MAX __INT32_MAX__
#define INT32_MIN (-INT32_MAX - 1)
#define UINT32_MAX __UINT32_MAX__
#define INT64_MAX __INT64_MAX__
#define INT64_MIN (-INT64_MAX - 1)
#define UINT64_MAX __UINT64_MAX__

#define INT_LEAST8_MAX __INT_LEAST8_MAX__
#define INT_LEAST8_MIN (-INT_LEAST8_MAX - 1)
#define UINT_LEAST8_MAX __UINT_LEAST8_MAX__
#define INT_LEAST16_MAX __INT_LEAST16_MAX__
#define INT_LEAST16_MIN (-INT_LEAST16_MAX - 1)
#define UINT_LEAST16_MAX __UINT_LEAST16_MAX__
#define INT_LEAST32_MAX __INT_LEAST32_MAX__
#define INT_LEAST32_MIN (-INT_LEAST32_MAX - 1)
#define UINT_LEAST32_MAX __UINT_LEAST32_MAX__
#define INT_LEAST64_MAX __INT_LEAST64_MAX__
#define INT_LEAST64_MIN (-INT_LEAST64_MAX - 1)
#define UINT_LEAST64_MAX __UINT_LEAST64_MAX__

#define INT_FAST8_MAX __INT_FAST8_MAX__
#define INT_FAST8_MIN (-INT_FAST8_MAX - 1)
#define UINT_FAST8_MAX __UINT_FAST8_MAX__
#define INT_FAST16_MAX __INT_FAST16_MAX__
#define INT_FAST16_MIN (-INT_FAST16_MAX - 1)
#define UINT_FAST16_MAX __UINT_FAST16_MAX__
#define INT_FAST32_MAX __INT_FAST32_MAX__
#define INT_FAST32_MIN (-INT_FAST32_MAX - 1)
#define UINT_FAST32_MAX __UINT_FAST32_MAX__
#define INT_FAST64_MAX __INT_FAST64_MAX__
#define INT_FAST64_MIN (-INT_FAST64_MAX - 1)
#define UINT_FAST64_MAX __UINT_FAST64_MAX__

#define INTPTR_MAX __INTPTR_MAX__
#define INTPTR_MIN (-INTPTR_MAX - 1)
#define UINTPTR_MAX __UINTPTR_MAX__

#define INTMAX_MAX __INTMAX_MAX__
#define INTMAX_MIN (-INTMAX_MAX - 1)
#define UINTMAX_MAX __UINTMAX_MAX__

#define PTRDIFF_MAX __PTRDIFF_MAX__
#define PTRDIFF_MIN (-PTRDIFF_MAX - 1)

#define SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#define SIG_ATOMIC_MIN __SIG_ATOMIC_MIN__

#define SIZE_MAX __SIZE_MAX__

#define WCHAR_MAX __WCHAR_MAX__
#define WCHAR_MIN __WCHAR_MIN__

#define WINT_MAX __WINT_MAX__
#define WINT_MIN __WINT_MIN__

#endif

#if (!defined __cplusplus || __cplusplus >= 201103L || defined __STDC_CONSTANT_MACROS)

#define INT8_C(c) __INT8_C(c)
#define INT16_C(c) __INT16_C(c)
#define INT32_C(c) __INT32_C(c)
#define INT64_C(c) __INT64_C(c)
#define UINT8_C(c) __UINT8_C(c)
#define UINT16_C(c) __UINT16_C(c)
#define UINT32_C(c) __UINT32_C(c)
#define UINT64_C(c) __UINT64_C(c)
#define INTMAX_C(c) __INTMAX_C(c)
#define UINTMAX_C(c) __UINTMAX_C(c)

#endif

#if (defined __STDC_WANT_IEC_60559_BFP_EXT__ || (defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L))

#define INT8_WIDTH 8
#define UINT8_WIDTH 8
#define INT16_WIDTH 16
#define UINT16_WIDTH 16
#define INT32_WIDTH 32
#define UINT32_WIDTH 32
#define INT64_WIDTH 64
#define UINT64_WIDTH 64

#define INT_LEAST8_WIDTH __INT_LEAST8_WIDTH__
#define UINT_LEAST8_WIDTH __INT_LEAST8_WIDTH__
#define INT_LEAST16_WIDTH __INT_LEAST16_WIDTH__
#define UINT_LEAST16_WIDTH __INT_LEAST16_WIDTH__
#define INT_LEAST32_WIDTH __INT_LEAST32_WIDTH__
#define UINT_LEAST32_WIDTH __INT_LEAST32_WIDTH__
#define INT_LEAST64_WIDTH __INT_LEAST64_WIDTH__
#define UINT_LEAST64_WIDTH __INT_LEAST64_WIDTH__

#define INT_FAST8_WIDTH __INT_FAST8_WIDTH__
#define UINT_FAST8_WIDTH __INT_FAST8_WIDTH__
#define INT_FAST16_WIDTH __INT_FAST16_WIDTH__
#define UINT_FAST16_WIDTH __INT_FAST16_WIDTH__
#define INT_FAST32_WIDTH __INT_FAST32_WIDTH__
#define UINT_FAST32_WIDTH __INT_FAST32_WIDTH__
#define INT_FAST64_WIDTH __INT_FAST64_WIDTH__
#define UINT_FAST64_WIDTH __INT_FAST64_WIDTH__

#define INTPTR_WIDTH __INTPTR_WIDTH__
#define UINTPTR_WIDTH __INTPTR_WIDTH__

#define INTMAX_WIDTH __INTMAX_WIDTH__
#define UINTMAX_WIDTH __INTMAX_WIDTH__

#define PTRDIFF_WIDTH __PTRDIFF_WIDTH__

#define SIG_ATOMIC_WIDTH __SIG_ATOMIC_WIDTH__

#define SIZE_WIDTH __SIZE_WIDTH__

#define WCHAR_WIDTH __WCHAR_WIDTH__

#define WINT_WIDTH __WINT_WIDTH__

#endif
