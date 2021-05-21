#include <kernel/utils/stdlib.h>
#include <kernel/utils/string.h>
#include <kernel/utils/math.h>
#include <kernel/system/sys.h>
#include <kernel/memory/virtual.h>
#include <kernel/memory/physical.h>

static struct malloc_block *malloc_blocks;
static uint32_t malloc_current = HEAP_BOTTOM;
static uint32_t malloc_remaining = 0;

void malloc_assert(struct malloc_block *block)
{
  if (block->size > 0x2000000)
    sys_panic("Malloc: Corrupted block", NULL);
}

struct malloc_block *malloc_get(struct malloc_block *last, size_t size)
{
  struct malloc_block *block = malloc_sbrk(size + sizeof(struct malloc_block));

  if (last)
    last->next = block;

  block->size = size;
  block->next = NULL;
  block->free = false;
  return block;
}

void *malloc_align(size_t size)
{
  uint32_t addr = (uint32_t)malloc_sbrk(0);
  if (addr % size == 0)
    return NULL;

  uint32_t padding = DIV_CEIL(addr, size) * size - addr;
  uint32_t required = sizeof(struct malloc_block) * 2;

  while (padding <= HEAP_TOP)
  {
    if (padding > required)
    {
      struct malloc_block *last = malloc_blocks;
      while (!last->next)
        last = last->next;
      struct malloc_block *block = malloc_get(last, padding - required);
      return block + 1;
    }
    padding += size;
  }
  return NULL;
}

void *malloc_sbrk(uint32_t increment)
{
  if (increment == 0)
    return (char *)malloc_current;

  char *base = (char *)malloc_current;

  if (increment <= malloc_remaining)
    malloc_remaining -= increment;
  else
  {
    uint32_t physical_addr = (uint32_t)phys_mm_block_n_alloc(DIV_CEIL(increment - malloc_remaining, PHYS_MM_BLOCK));
    uint32_t virtual_addr = DIV_CEIL(malloc_current, PHYS_MM_BLOCK) * PHYS_MM_BLOCK;

    while (virtual_addr < malloc_current + increment)
    {
      virt_mm_map_addr(virt_mm_dir_get(), physical_addr, virtual_addr, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
      virtual_addr += PHYS_MM_BLOCK;
      physical_addr += PHYS_MM_BLOCK;
    }
    malloc_remaining = virtual_addr - (malloc_current + increment);
  }

  malloc_current += increment;
  memset(base, 0, increment);
  return base;
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
