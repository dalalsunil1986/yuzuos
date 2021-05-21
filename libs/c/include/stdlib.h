#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0

struct malloc_block
{
  uint32_t size;
  bool free;

  struct malloc_block *next;
  struct malloc_block *prev;
};

void abort(void);
int atexit(void (*)(void));
int atoi(const char *);
void free(void *ptr);
char *getenv(const char *);
void *malloc(size_t size);
void *calloc(size_t nitems, size_t size);
void exit(int status);
char *getenv(const char *name);
