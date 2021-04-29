#include <kernel/system/sys.h>
#include <kernel/utils/log.h>

void _sys_panic(char *message, const char *file, int line, struct itr_registers *registers)
{
  sys_cli();

  log_error("Panic: message = %s\n", message);
  log_error("Panic: file = %s, line = %d\n", file, line);

  if (registers)
  {
    log_error("Panic: Registers\n");
    log_error("Panic: * eax = 0x%08x, ebx = 0x%08x\n", registers->eax, registers->ebx);
    log_error("Panic: * ecx = 0x%08x, edx = 0x%08x\n", registers->ecx, registers->edx);
    log_error("Panic: * esp = 0x%08x, ebp = 0x%08x\n", registers->esp, registers->ebp);
    log_error("Panic: * eip = 0x%08x, user_esp = 0x%x\n", registers->eip, registers->useresp);
    log_error("Panic: * eflags = 0x%x, err_code: 0x%x\n", registers->eflags, registers->err_code);
  }
  sys_stop();
}
