#pragma once

#include <kernel/memory/physical.h>
#include <stdint.h>

#define PAGE_FRAME 0x7FFFF000
#define PAGE_MASK (~(PHYS_MM_BLOCK - 1))
#define PAGE_ALIGN(addr) (((addr) + PHYS_MM_BLOCK - 1) & PAGE_MASK)
#define PAGE_IS_ENABLED(addr) ((addr)&0x1)

#define PAGE_TBL_ENTRIES 1024
#define PAGE_TBL_INDEX(addr) (((addr) >> 12) & 0x3ff)

#define PAGE_DIR_ENTRIES 1024
#define PAGE_DIR_INDEX(addr) (((addr) >> 22) & 0x3ff)

enum page_tbl_flags
{
  PAGE_TBL_PRESENT = 1,
  PAGE_TBL_WRITABLE = 2,
};

enum page_dir_flags
{
  PAGE_DIR_PRESENT = 1,
  PAGE_DIR_WRITABLE = 2,
};

struct page_tbl
{
  uint32_t entries[PAGE_TBL_ENTRIES];
};

struct page_dir
{
  uint32_t entries[PAGE_DIR_ENTRIES];
};

struct page_dir *virt_mm_dir_get();
void virt_mm_init();
