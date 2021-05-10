#include <kernel/memory/virtual.h>
#include <kernel/memory/physical.h>
#include <kernel/utils/log.h>
#include <kernel/utils/string.h>

static struct page_dir *virt_mm_dir;

extern void page_enable(uint32_t addr);

struct page_dir *virt_mm_dir_get()
{
  return virt_mm_dir;
}

void virt_mm_frame_set(uint32_t *entry, uint32_t addr)
{
  *entry = (*entry & ~PAGE_FRAME) | addr;
}

void virt_mm_flag_set(uint32_t *entry, uint32_t flags)
{
  *entry |= flags;
}

void virt_mm_map(struct page_dir *dir, uint32_t physical, uint32_t virtual)
{
  struct page_tbl *tbl = (struct page_tbl *)phys_mm_block_alloc();
  memset(tbl, 0, sizeof(struct page_tbl));

  for (uint32_t i = 0, i_phys = physical, i_virt = virtual; i < PAGE_TBL_ENTRIES; i++, i_phys += PHYS_MM_BLOCK, i_virt += PHYS_MM_BLOCK)
  {
    uint32_t *entry = &tbl->entries[PAGE_TBL_INDEX(i_virt)];
    virt_mm_flag_set(entry, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
    virt_mm_frame_set(entry, i_phys);
  }

  uint32_t *entry = &dir->entries[PAGE_DIR_INDEX(virtual)];
  virt_mm_flag_set(entry, PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE);
  virt_mm_frame_set(entry, (uint32_t)tbl);

  log_info("Virtual MM: Mapped dir = 0x%x, tbl = 0x%x, physical = 0x%x, virtual = 0x%x\n", dir, tbl, physical, virtual);
}

void virt_mm_init()
{
  struct page_dir *dir = (struct page_dir *)phys_mm_block_alloc();
  memset(dir, 0, sizeof(struct page_dir));

  virt_mm_map(dir, 0x0, 0x0);
  virt_mm_map(dir, 0x100000, 0xC0000000);

  dir->entries[PAGE_DIR_ENTRIES - 1] = ((uint32_t)dir & PAGE_DIR_BASE) | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE;
  log_info("Virtual MM: Recursive page directory enabled\n");

  virt_mm_dir = dir;
  page_enable((uint32_t)dir);
  log_info("Virtual MM: Page enabled, dir = 0x%x\n", (uint32_t)dir);

  log_info("Virtual MM: Initialized\n");
}
