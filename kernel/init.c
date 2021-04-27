#include <kernel/utils/log.h>
#include <kernel/memory/gdt.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/irq.h>
#include <kernel/system/sys.h>
#include <stdbool.h>

void kernel_init()
{
  log_init();
  gdt_init();
  idt_init();
  irq_init();

  sys_sti();
  while (true)
    ;
}
