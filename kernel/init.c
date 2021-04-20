#include <kernel/utils/log.h>
#include <kernel/memory/gdt.h>
#include <kernel/interrupts/idt.h>
#include <stdbool.h>

void kernel_init()
{
  log_init();
  gdt_init();
  idt_init();

  while (true)
    ;
}
