#include <kernel/task/tss.h>
#include <kernel/memory/gdt.h>
#include <kernel/utils/log.h>

static struct tss_entry tss;

void tss_set_stack(uint32_t ss, uint32_t esp)
{
  tss.ss0 = ss;
  tss.esp0 = esp;
}

void tss_init(uint32_t index, uint32_t ss, uint32_t esp)
{
  gdt_entry_add(index, (uint32_t)&tss, (uint32_t)&tss + sizeof(struct tss_entry), 0xE9, 0);
  tss.ss0 = ss;
  tss.esp0 = esp;
  tss.cs = 0x0b;
  tss.ss = 0x13;
  tss.es = 0x13;
  tss.ds = 0x13;
  tss.fs = 0x13;
  tss.gs = 0x13;
  tss.iomap = sizeof(struct tss_entry);

  log_info("TSS: Initialized\n");
}
