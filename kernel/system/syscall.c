#include <kernel/system/syscall.h>
#include <kernel/task/scheduler.h>
#include <kernel/interrupts/isr.h>
#include <kernel/utils/log.h>
#include <kernel/utils/string.h>

void syscall_exit(int32_t code)
{
  sched_exit(code);
}

static void *syscalls[] = {
    [__NR_exit] = syscall_exit};

int syscall_handler(struct itr_registers *registers)
{
  int index = registers->eax;
  uint32_t (*handler)(unsigned int, ...) = syscalls[index];
  if (!handler)
    return ITR_STOP;

  memcpy(&sched_thread_get()->registers, registers, sizeof(struct itr_registers));
  uint32_t result = handler(registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi);
  registers->eax = result;
  log_info("Syscall: Called syscall id = %d, result = 0x%x\n", index, result);

  return ITR_CONTINUE;
}

void syscall_init()
{
  isr_handler_set(127, syscall_handler);
  log_info("Syscall: Initialized\n");
}
