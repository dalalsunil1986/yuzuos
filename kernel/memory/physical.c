#include <kernel/memory/physical.h>
#include <kernel/boot/multiboot.h>
#include <kernel/utils/log.h>
#include <kernel/utils/bitmap.h>
#include <kernel/utils/math.h>
#include <kernel/utils/string.h>
#include <stddef.h>

static uint32_t *phys_mm_bitmap;
static uint32_t phys_mm_bitmap_used;
static uint32_t phys_mm_bitmap_max;

void *phys_mm_block_n_alloc(uint32_t len)
{
  if (phys_mm_bitmap_used - phys_mm_bitmap_max <= 0 || phys_mm_bitmap_max - phys_mm_bitmap_used <= len)
    return NULL;

  uint32_t bit = bitmap_get_n_first(phys_mm_bitmap, phys_mm_bitmap_max, len);
  if (bit <= 0)
    return NULL;

  for (uint32_t i = 0; i < len; i++)
  {
    bitmap_set(phys_mm_bitmap, bit + i);
    phys_mm_bitmap_used++;
  }

  return (void *)(PHYS_MM_BLOCK * bit);
}

void *phys_mm_block_alloc()
{
  if (phys_mm_bitmap_used - phys_mm_bitmap_max <= 0)
    return NULL;

  uint32_t bit = bitmap_get_first(phys_mm_bitmap, phys_mm_bitmap_max);
  if (bit <= 0)
    return NULL;

  bitmap_set(phys_mm_bitmap, bit);
  phys_mm_bitmap_used++;

  return (void *)(PHYS_MM_BLOCK * bit);
}

void phys_mm_block_free(void *block)
{
  if (block == NULL)
    return;

  uint32_t bit = (uint32_t)block / PHYS_MM_BLOCK;
  bitmap_unset(phys_mm_bitmap, bit);
  phys_mm_bitmap_used--;
}

void phys_mm_region_set(uint32_t base, uint32_t len)
{
  uint32_t blocks = len / PHYS_MM_BLOCK;
  uint32_t align = base / PHYS_MM_BLOCK;

  for (uint32_t i = 0; i < blocks; i++)
  {
    bitmap_unset(phys_mm_bitmap, align++);
    phys_mm_bitmap_used--;
  }

  bitmap_set(phys_mm_bitmap, 0);

  log_info("Physical MM: Region set base = 0x%x, len = 0x%x\n", base, len);
}

void phys_mm_region_unset(uint32_t base, uint32_t len)
{
  uint32_t blocks = len / PHYS_MM_BLOCK;
  uint32_t align = base / PHYS_MM_BLOCK;

  for (uint32_t i = 0; i < blocks; i++)
  {
    bitmap_set(phys_mm_bitmap, align++);
    phys_mm_bitmap_used++;
  }

  log_info("Physical MM: Region unset base = 0x%x, len = 0x%x\n", base, len);
}

void phys_mm_init()
{
  uint32_t kernel_size = (uint32_t)&kernel_end - (uint32_t)&kernel_start;
  phys_mm_bitmap = (uint32_t *)&kernel_end;
  phys_mm_bitmap_max = DIV_CEIL((multiboot_info->mem_lower + multiboot_info->mem_upper * 1024), PHYS_MM_BLOCK);
  phys_mm_bitmap_used = phys_mm_bitmap_max;

  uint32_t bitmap_size = DIV_CEIL(phys_mm_bitmap_max, PHYS_MM_BLOCK_P_BYTE);
  memset(phys_mm_bitmap, 0, bitmap_size);

  for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)multiboot_info->mmap_addr; (unsigned long)mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length;
       mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
    if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
      phys_mm_region_set((uint32_t)mmap->addr, (uint32_t)mmap->len);

  phys_mm_region_unset(0x0, 0x100000);
  phys_mm_region_unset(0x100000, kernel_size + bitmap_size);

  log_info("Physical MM: Kernel start = 0x%x, end = 0x%x, size = 0x%x\n", &kernel_start, &kernel_end, kernel_size);
  log_info("Physical MM: Bitmap addr = 0x%x, max = %d, used = %d\n", phys_mm_bitmap, phys_mm_bitmap_max, phys_mm_bitmap_used);
  log_info("Physical MM: Initialized\n");
}
