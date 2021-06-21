#include <kernel/boot/idt.h>
#include <kernel/boot/serial.h>
#include <kernel/asm/pic.h>

/* This stores all IDT entries */
static struct idt_entry idt_entries[IDT_ENTRIES];
/* This stores the IDT segment */
static struct idt_pointer idt;

/**
 * @brief Reload segment registers
 * @param addr The IDT segment address
 */
extern void idt_flush(uint32_t addr);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr127();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/**
 * @brief Add an entry on the IDT
 * @param index The IDT vector index
 * @param base Address where the segment begins
 * @param selector Segment selector code
 * @param flags Segment type flags
 */
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

  /* Add all ISR`s on the IDT */
  idt_entry_add(0, (uint32_t)isr0, 0x08, 0x8E);
  idt_entry_add(1, (uint32_t)isr1, 0x08, 0x8E);
  idt_entry_add(2, (uint32_t)isr2, 0x08, 0x8E);
  idt_entry_add(3, (uint32_t)isr3, 0x08, 0x8E);
  idt_entry_add(4, (uint32_t)isr4, 0x08, 0x8E);
  idt_entry_add(5, (uint32_t)isr5, 0x08, 0x8E);
  idt_entry_add(6, (uint32_t)isr6, 0x08, 0x8E);
  idt_entry_add(7, (uint32_t)isr7, 0x08, 0x8E);
  idt_entry_add(8, (uint32_t)isr8, 0x08, 0x8E);
  idt_entry_add(9, (uint32_t)isr9, 0x08, 0x8E);
  idt_entry_add(10, (uint32_t)isr10, 0x08, 0x8E);
  idt_entry_add(11, (uint32_t)isr11, 0x08, 0x8E);
  idt_entry_add(12, (uint32_t)isr12, 0x08, 0x8E);
  idt_entry_add(13, (uint32_t)isr13, 0x08, 0x8E);
  idt_entry_add(14, (uint32_t)isr14, 0x08, 0x8E);
  idt_entry_add(15, (uint32_t)isr15, 0x08, 0x8E);
  idt_entry_add(16, (uint32_t)isr16, 0x08, 0x8E);
  idt_entry_add(17, (uint32_t)isr17, 0x08, 0x8E);
  idt_entry_add(18, (uint32_t)isr18, 0x08, 0x8E);
  idt_entry_add(19, (uint32_t)isr19, 0x08, 0x8E);
  idt_entry_add(20, (uint32_t)isr20, 0x08, 0x8E);
  idt_entry_add(21, (uint32_t)isr21, 0x08, 0x8E);
  idt_entry_add(21, (uint32_t)isr22, 0x08, 0x8E);
  idt_entry_add(23, (uint32_t)isr23, 0x08, 0x8E);
  idt_entry_add(24, (uint32_t)isr24, 0x08, 0x8E);
  idt_entry_add(25, (uint32_t)isr25, 0x08, 0x8E);
  idt_entry_add(26, (uint32_t)isr26, 0x08, 0x8E);
  idt_entry_add(27, (uint32_t)isr27, 0x08, 0x8E);
  idt_entry_add(28, (uint32_t)isr28, 0x08, 0x8E);
  idt_entry_add(29, (uint32_t)isr29, 0x08, 0x8E);
  idt_entry_add(30, (uint32_t)isr30, 0x08, 0x8E);
  idt_entry_add(31, (uint32_t)isr31, 0x08, 0x8E);
  idt_entry_add(127, (uint32_t)isr127, 0x08, 0x8E | 0x60);
  serial_early_kprintf("IDT: Added ISR 0..31\n");

  /* Add all IRQ`s on the IDT */
  pic_remap(0x20, 0x28);
  idt_entry_add(32, (uint32_t)irq0, 0x08, 0x8E);
  idt_entry_add(33, (uint32_t)irq1, 0x08, 0x8E);
  idt_entry_add(34, (uint32_t)irq2, 0x08, 0x8E);
  idt_entry_add(35, (uint32_t)irq3, 0x08, 0x8E);
  idt_entry_add(36, (uint32_t)irq4, 0x08, 0x8E);
  idt_entry_add(37, (uint32_t)irq5, 0x08, 0x8E);
  idt_entry_add(38, (uint32_t)irq6, 0x08, 0x8E);
  idt_entry_add(39, (uint32_t)irq7, 0x08, 0x8E);
  idt_entry_add(40, (uint32_t)irq8, 0x08, 0x8E);
  idt_entry_add(41, (uint32_t)irq9, 0x08, 0x8E);
  idt_entry_add(42, (uint32_t)irq10, 0x08, 0x8E);
  idt_entry_add(43, (uint32_t)irq11, 0x08, 0x8E);
  idt_entry_add(44, (uint32_t)irq12, 0x08, 0x8E);
  idt_entry_add(45, (uint32_t)irq13, 0x08, 0x8E);
  idt_entry_add(46, (uint32_t)irq14, 0x08, 0x8E);
  idt_entry_add(47, (uint32_t)irq15, 0x08, 0x8E);
  serial_early_kprintf("IDT: Added IRQ 0..15\n");

  /* Special ISR used for syscalls (called by userspace, use 0x60 flag) */
  idt_entry_add(127, (uint32_t)isr127, 0x08, 0x8E | 0x60);
  serial_early_kprintf("IDT: Added ISR 127 for userspace syscall\n");

  idt_flush((uint32_t)&idt);
  serial_early_kprintf("IDT: Reloaded IDT segment\n");

  serial_early_kprintf("IDT: Initialized\n");
}
