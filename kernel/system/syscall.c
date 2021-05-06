#include <kernel/system/syscall.h>
#include <kernel/interrupts/isr.h>
#include <kernel/utils/log.h>

int syscall_handler(struct itr_registers *registers)
{
  (void)registers;
  return ITR_CONTINUE;
}

void syscall_init()
{
  isr_handler_set(127, syscall_handler);
  log_info("Syscall: Initialized\n");
}
