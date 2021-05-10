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
  size_t increment = size + sizeof(struct malloc_block);
  if (increment == 0)
    return (char *)malloc_current;

  char *base = (char *)malloc_current;

  if (increment <= malloc_remaining)
    malloc_remaining -= increment;
  else
  {
    uint32_t phyiscal_addr = (uint32_t)phys_mm_block_n_alloc(DIV_CEIL(increment - malloc_remaining, PHYS_MM_BLOCK));
    uint32_t page_addr = DIV_CEIL(malloc_current, PHYS_MM_BLOCK) * PHYS_MM_BLOCK;
    for (; page_addr < malloc_current + increment; page_addr += PHYS_MM_BLOCK, phyiscal_addr += PHYS_MM_BLOCK)
      virt_mm_map_addr(virt_mm_dir_get(), phyiscal_addr, page_addr, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
    malloc_remaining = page_addr - (malloc_current + increment);
  }

  malloc_current += increment;
  memset(base, 0, increment);
  struct malloc_block *block = base;

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
