#pragma once

#define KERNEL_VIRT_ADDR 0xC0000000
#define PAGE_NUMBER (KERNEL_VIRT_ADDR >> 22)

#ifndef ASM_FILE

#include <kernel/memory/physical.h>
#include <stdint.h>

#define PAGE_FRAME 0x7FFFF000
#define PAGE_MASK (~(PHYS_MM_BLOCK - 1))
#define PAGE_ALIGN(addr) (((addr) + PHYS_MM_BLOCK - 1) & PAGE_MASK)
#define PAGE_IS_ENABLED(addr) ((addr)&0x1)

#define PAGE_TBL_ENTRIES 1024
#define PAGE_TBL_INDEX(addr) (((addr) >> 12) & 0x3ff)
#define PAGE_TBL_BASE 0xFFC00000

#define PAGE_DIR_ENTRIES 1024
#define PAGE_DIR_INDEX(addr) (((addr) >> 22) & 0x3ff)
#define PAGE_DIR_BASE 0xFFFFF000

#define HEAP_BOTTOM 0xD0000000
#define HEAP_TOP 0xF0000000
#define HEAP_USER_TOP 0x40000000

#define VIRT_TO_PHYS(addr) ((addr)-KERNEL_VIRT_ADDR)
#define PHYS_TO_VIRT(addr) ((addr) + KERNEL_VIRT_ADDR)

enum page_tbl_flags
{
  PAGE_TBL_PRESENT = 1,
  PAGE_TBL_WRITABLE = 2,
  PAGE_TBL_USER = 4
};

enum page_dir_flags
{
  PAGE_DIR_PRESENT = 1,
  PAGE_DIR_WRITABLE = 2,
  PAGE_DIR_USER = 4,
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
void virt_mm_map_addr(struct page_dir *dir, uint32_t physical, uint32_t virtual, uint32_t flags);
uint32_t virt_mm_phys_addr_get(uint32_t virtual);
void virt_mm_addr_unmap(struct page_dir *dir, uint32_t virtual);
void virt_mm_addr_range_unmap(struct page_dir *dir, uint32_t start, uint32_t end);
struct page_dir *virt_mm_addr_create(struct page_dir *dir);
void virt_mm_init();

#endif
