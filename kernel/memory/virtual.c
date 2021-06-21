#include <kernel/memory/virtual.h>
#include <kernel/boot/serial.h>
#include <kernel/utils/string.h>
#include <kernel/utils/stdlib.h>

static struct page_dir *virt_mm_dir;

extern void page_enable(uint32_t addr);
extern void tbl_flush_entry(uint32_t addr);

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

struct page_dir *virt_mm_addr_create()
{
  char *align = malloc_align(PHYS_MM_BLOCK);
  struct page_dir *dir = calloc(1, sizeof(struct page_dir));
  if (align)
    free(align);

  for (int i = 768; i < PAGE_DIR_ENTRIES - 1; i++)
    dir->entries[i] = virt_mm_phys_addr_get(PAGE_TBL_BASE + i * PHYS_MM_BLOCK);

  dir->entries[PAGE_DIR_ENTRIES - 1] = virt_mm_phys_addr_get((uint32_t)dir);
  return dir;
}

uint32_t virt_mm_phys_addr_get(uint32_t virtual)
{
  uint32_t *tbl = (uint32_t *)((char *)PAGE_TBL_BASE + PAGE_DIR_INDEX(virtual) * PHYS_MM_BLOCK);
  return tbl[PAGE_TBL_INDEX(virtual)];
}

void virt_mm_map_addr(struct page_dir *dir, uint32_t physical, uint32_t virtual, uint32_t flags)
{
  if (virtual != PAGE_ALIGN(virtual))
    serial_early_kprintf("Virtual MM: Virtual addr = 0x%x, not page aligned (0x%x)\n", virtual, PAGE_ALIGN(virtual));

  if (!PAGE_IS_ENABLED(dir->entries[PAGE_DIR_INDEX(virtual)]))
  {
    uint32_t physical_tbl = (uint32_t)phys_mm_block_alloc();
    uint32_t *entry = &dir->entries[PAGE_DIR_INDEX(virtual)];
    virt_mm_flag_set(entry, flags);
    virt_mm_frame_set(entry, physical_tbl);
    tbl_flush_entry(virtual);
    memset((char *)PAGE_TBL_BASE + PAGE_DIR_INDEX(virtual) * PHYS_MM_BLOCK, 0, sizeof(struct page_tbl));
  }

  uint32_t *table = (uint32_t *)((char *)PAGE_TBL_BASE + PAGE_DIR_INDEX(virtual) * PHYS_MM_BLOCK);
  uint32_t *entry = &table[PAGE_TBL_INDEX(virtual)];
  virt_mm_flag_set(entry, flags);
  virt_mm_frame_set(entry, physical);
  tbl_flush_entry(virtual);
}

void virt_mm_addr_unmap(struct page_dir *dir, uint32_t virtual)
{
  if (virtual != PAGE_ALIGN(virtual))
    serial_early_kprintf("Virtual MM: Virtual addr = 0x%x, not page aligned\n", virtual);

  if (!PAGE_IS_ENABLED(dir->entries[PAGE_DIR_INDEX(virtual)]))
    return;

  struct page_tbl *tbl = (struct page_tbl *)(PAGE_TBL_BASE + PAGE_DIR_INDEX(virtual) * PHYS_MM_BLOCK);
  uint32_t index = PAGE_TBL_INDEX(virtual);
  if (!PAGE_IS_ENABLED(tbl->entries[index]))
    return;

  tbl->entries[index] = 0;
  tbl_flush_entry(virtual);
}

void virt_mm_addr_range_unmap(struct page_dir *dir, uint32_t start, uint32_t end)
{
  for (uint32_t addr = start; addr < end; addr += PHYS_MM_BLOCK)
    virt_mm_addr_unmap(dir, addr);
}

struct page_dir *virt_mm_fork_dir(struct page_dir *dir)
{
  struct page_dir *fork_dir = virt_mm_addr_create();
  char *aligned = malloc_align(PHYS_MM_BLOCK);
  uint32_t heap_current = (uint32_t)malloc_sbrk(0);

  for (int i_dir = 0; i_dir < 768; i_dir++)
    if (PAGE_IS_ENABLED(dir->entries[i_dir]))
    {
      struct page_tbl *virtual_tbl = (struct page_tbl *)heap_current;
      uint32_t physical_tbl = (uint32_t)phys_mm_block_alloc();

      virt_mm_map_addr(dir, physical_tbl, (uint32_t)virtual_tbl, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
      memset(virtual_tbl, 0, sizeof(struct page_tbl));

      heap_current += sizeof(struct page_tbl);
      struct page_tbl *tbl = (struct page_tbl *)(PAGE_TBL_BASE + i_dir * PHYS_MM_BLOCK);

      for (int i_tbl = 0; i_tbl < PAGE_TBL_ENTRIES; i_tbl++)
        if (PAGE_IS_ENABLED(tbl->entries[i_tbl]))
        {
          char *virtual_entry = (char *)heap_current;
          char *forked_entry = virtual_entry + PHYS_MM_BLOCK;
          heap_current = (uint32_t)(forked_entry + PHYS_MM_BLOCK);
          uint32_t physical_entry = (uint32_t)phys_mm_block_alloc();

          virt_mm_map_addr(dir, tbl->entries[i_tbl], (uint32_t)virtual_entry, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
          virt_mm_map_addr(dir, physical_entry, (uint32_t)forked_entry, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);

          memcpy(forked_entry, virtual_entry, PHYS_MM_BLOCK);

          virt_mm_addr_unmap(dir, (uint32_t)virtual_entry);
          virt_mm_addr_unmap(dir, (uint32_t)forked_entry);

          virtual_tbl->entries[i_tbl] = physical_entry | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER;
        }

      virt_mm_addr_unmap(dir, (uint32_t)virtual_tbl);
      fork_dir->entries[i_dir] = physical_tbl | PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE | PAGE_DIR_USER;
    }

  if (aligned)
    free(aligned);

  return fork_dir;
}

void virt_mm_map(struct page_dir *dir, uint32_t physical, uint32_t virtual)
{
  uint32_t phys_tbl = (uint32_t)phys_mm_block_alloc();
  struct page_tbl *tbl = (struct page_tbl *)PHYS_TO_VIRT(phys_tbl);
  memset(tbl, 0, sizeof(struct page_tbl));

  for (uint32_t i = 0, i_phys = physical, i_virt = virtual; i < PAGE_TBL_ENTRIES; i++, i_phys += PHYS_MM_BLOCK, i_virt += PHYS_MM_BLOCK)
  {
    uint32_t *entry = &tbl->entries[PAGE_TBL_INDEX(i_virt)];
    virt_mm_flag_set(entry, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
    virt_mm_frame_set(entry, i_phys);
    phys_mm_addr_set(i_phys);
  }

  uint32_t *entry = &dir->entries[PAGE_DIR_INDEX(virtual)];
  virt_mm_flag_set(entry, PAGE_DIR_PRESENT | PAGE_DIR_WRITABLE);
  virt_mm_frame_set(entry, phys_tbl);

  serial_early_kprintf("Virtual MM: Mapped dir = 0x%x, tbl = 0x%x, physical = 0x%x, virtual = 0x%x\n", dir, tbl, physical, virtual);
}

void virt_mm_init()
{
  uint32_t phys_dir = (uint32_t)phys_mm_block_alloc();
  struct page_dir *dir = (struct page_dir *)PHYS_TO_VIRT(phys_dir);
  memset(dir, 0, sizeof(struct page_dir));

  virt_mm_map(dir, 0x00000000, 0xC0000000);

  dir->entries[PAGE_TBL_ENTRIES - 1] = (phys_dir & PAGE_DIR_BASE) | PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE;
  serial_early_kprintf("Virtual MM: Recursive page directory enabled\n");

  virt_mm_dir = dir;
  page_enable(phys_dir);
  serial_early_kprintf("Virtual MM: Page enabled, dir = 0x%x\n", phys_dir);

  serial_early_kprintf("Virtual MM: Initialized\n");
}
