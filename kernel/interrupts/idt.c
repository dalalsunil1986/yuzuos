#include <kernel/interrupts/idt.h>
#include <kernel/utils/log.h>

static struct idt_entry idt_entries[IDT_ENTRIES];
static struct idt_pointer idt;

extern void idt_flush(uint32_t addr);

void idt_entry_add(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags)
{
  idt_entries[index].base_low = base & 0xFFFF;
  idt_entries[index].base_high = (base >> 16) & 0xFFFF;
  idt_entries[index].selector = selector;
  idt_entries[index].flags = flags;
  idt_entries[index].zero = 0;
}

void idt_init()
{
  idt.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
  idt.base = (uint32_t)&idt_entries;

  idt_flush((uint32_t)&idt);

  log_info("IDT: Initialized\n");
}
