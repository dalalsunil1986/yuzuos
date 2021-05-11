#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct malloc_block
{
  uint32_t size;
  bool free;

  struct malloc_block *next;
  struct malloc_block *prev;
};

void *malloc(size_t size);
void *malloc_align(size_t size);
void *calloc(size_t nitems, size_t size);
void free(void *ptr);
