#pragma once

typedef struct
{
  volatile int counter;
} atomic_t;

#define atomic_read(atomic) ((atomic)->counter)
#define atomic_set(atomic, i) (((atomic)->counter) = (i))

static inline void atomic_add(int i, atomic_t *atomic)
{
  asm volatile(
      "addl %1,%0"
      : "=m"(atomic->counter)
      : "ir"(i), "m"(atomic->counter));
}

static inline void atomic_sub(int i, atomic_t *atomic)
{
  asm volatile(
      "subl %1,%0"
      : "=m"(atomic->counter)
      : "ir"(i), "m"(atomic->counter));
}

static inline void atomic_inc(atomic_t *atomic)
{
  asm volatile(
      "incl %0"
      : "=m"(atomic->counter)
      : "m"(atomic->counter));
}

static inline void atomic_dec(atomic_t *atomic)
{
  asm volatile(
      "decl %0"
      : "=m"(atomic->counter)
      : "m"(atomic->counter));
}
