#include <kernel/utils/log.h>
#include <kernel/memory/gdt.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/isr.h>
#include <kernel/system/sys.h>
#include <kernel/boot/multiboot.h>
#include <stdbool.h>

void kernel_init(uint32_t magic, uint32_t addr)
{
  log_init();
  gdt_init();
  idt_init();
  isr_init();
  irq_init();
  multiboot_init(magic, addr);

  sys_sti();
  while (true)
    ;
}
