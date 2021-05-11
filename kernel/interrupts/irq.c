#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/pic.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <stddef.h>

static itr_handler_t irq[IRQ_SIZE * IRQ_DEPTH];

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

void irq_handler_set(uint8_t index, itr_handler_t handler)
{
  for (uint8_t i = 0; i < IRQ_DEPTH; i++)
  {
    if (irq[i * IRQ_SIZE + index])
      continue;
    irq[i * IRQ_SIZE + index] = handler;
    break;
  }
}

void irq_handler_unset(uint8_t index)
{
  for (uint8_t i = 0; i < IRQ_DEPTH; i++)
    irq[i * IRQ_SIZE + index] = NULL;
}

void irq_handler(struct itr_registers *registers)
{
  if (registers->int_no <= 47 && registers->int_no >= 32)
  {
    for (size_t i = 0; i < IRQ_DEPTH; i++)
    {
      itr_handler_t handler = irq[i * IRQ_SIZE + (registers->int_no - 32)];
      if (!handler)
        break;

      if (handler(registers))
        continue;
    }
    pic_eoi(registers->int_no - 32);
  }
}

void irq_init()
{
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

  log_info("IRQ: Initialized\n");
}
