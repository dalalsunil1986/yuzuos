#include <kernel/memory/gdt.h>
#include <kernel/utils/log.h>
#include <kernel/task/tss.h>

static struct gdt_entry gdt_entries[GDT_ENTRIES];
static struct gdt_pointer gdt;

extern void gdt_flush(uint32_t addr);
extern void tss_flush();

void gdt_entry_add(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
  gdt_entries[index].base_low = base & 0xFFFF;
  gdt_entries[index].base_middle = (base >> 16) & 0xFF;
  gdt_entries[index].base_high = (base >> 24) & 0xFF;
  gdt_entries[index].limit_low = limit & 0xFFFF;
  gdt_entries[index].granularity = (limit >> 16) & 0x0F;
  gdt_entries[index].granularity |= granularity & 0xF0;
  gdt_entries[index].access = access;
}

void gdt_init()
{
  gdt.limit = sizeof(struct gdt_entry) * GDT_ENTRIES - 1;
  gdt.base = (uint32_t)&gdt_entries;

  gdt_entry_add(0, 0, 0, 0, 0);
  gdt_entry_add(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
  gdt_entry_add(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
  gdt_entry_add(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
  gdt_entry_add(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
  tss_init(5, 0x10, 0);
  gdt_flush((uint32_t)&gdt);
  tss_flush();

  log_info("GDT: Initialized\n");
}
