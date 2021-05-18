#include <stdlib.h>
#include <syscall.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <utils/log.h>

static struct malloc_block *malloc_blocks;
static uint32_t malloc_current = 0;
static uint32_t malloc_remaining = 0;

_syscall1(exit, int);
void exit(int status)
{
  syscall_exit(status);
  __builtin_unreachable();
}

void malloc_assert(struct malloc_block *block)
{
  if (block->size > 0x2000000)
    log_error("Malloc: Corrupted block", NULL);
}

struct malloc_block *malloc_get(struct malloc_block *last, size_t size)
{
  size_t target_size = size + sizeof(struct malloc_block);
  if (!malloc_current)
    malloc_current = (uint32_t)sbrk(0);

  uint32_t base = malloc_current;
  if (target_size <= malloc_remaining)
    malloc_remaining -= target_size;
  else
  {
    sbrk(target_size);
    malloc_remaining = (uint32_t)sbrk(0) - (malloc_current + target_size);
  }

  malloc_current += target_size;
  struct malloc_block *block = (struct malloc_block *)base;

  if (last)
    last->next = block;

  block->size = size;
  block->next = NULL;
  block->free = false;
  return block;
}

void *malloc(size_t size)
{
  if (size <= 0)
    return NULL;

  size = ALIGN_UP(size, 4);
  struct malloc_block *block;
  if (malloc_blocks)
  {
    struct malloc_block *last = malloc_blocks;
    struct malloc_block *current = (struct malloc_block *)malloc_blocks;
    while (current && !(current->free && current->size >= size))
    {
      malloc_assert(current);
      last = current;
      current = current->next;
      if (current)
        malloc_assert(current);
    }
    block = current;

    if (block)
    {
      block->free = false;
      if (block->size > size + sizeof(struct malloc_block))
      {
        struct malloc_block *splited_block = (struct malloc_block *)((char *)block + sizeof(struct malloc_block) + size);
        splited_block->free = true;
        splited_block->size = block->size - size - sizeof(struct malloc_block);
        splited_block->next = block->next;

        block->size = size;
        block->next = splited_block;
      }
    }
    else
      block = malloc_get(last, size);
  }
  else
  {
    block = malloc_get(NULL, size);
    malloc_blocks = block;
  }

  malloc_assert(block);

  if (block)
    return block + 1;
  else
    return NULL;
}

void *calloc(size_t nitems, size_t size)
{
  size_t target_size = nitems * size;
  char *block = malloc(target_size);
  if (block == NULL)
    return NULL;
  memset(block, 0, target_size);
  return block;
}

void free(void *ptr)
{
  struct malloc_block *block = (struct malloc_block *)ptr - 1;
  block->free = true;
}
