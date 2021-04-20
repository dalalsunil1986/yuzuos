#include <kernel/utils/log.h>
#include <kernel/memory/gdt.h>
#include <stdbool.h>

void kernel_init()
{
  log_init();
  gdt_init();

  while (true)
    ;
}
