#include <kernel/system/syscall.h>
#include <kernel/task/scheduler.h>
#include <kernel/interrupts/isr.h>
#include <kernel/utils/log.h>
#include <kernel/utils/string.h>
#include <kernel/utils/errno.h>
#include <kernel/memory/mmap.h>
#include <kernel/filesystem/virtual.h>

void syscall_exit(int32_t code)
{
  sched_exit(code);
}

int syscall_open(const char *path, int flags, mode_t mode)
{
  return virt_fs_open(path, flags, mode);
}

void syscall_log(const char *buffer, int len)
{
  log_write(buffer, len);
}

int syscall_fstat(int fildes, struct stat *stat)
{
  return virt_fs_fstat(fildes, stat);
}

int syscall_brk(void *addr)
{
  uint32_t brk = (uint32_t)addr;
  struct process_mm *mm = sched_process_get()->mm;

  if (brk == 0)
    return mm->brk;

  if (brk < mm->brk_start)
    return -EINVAL;

  return mmap_brk(mm->brk_start, brk - mm->brk_start);
}

ssize_t syscall_read(int fd, void *buf, size_t count)
{
  return virt_fs_fread(fd, buf, count);
}

static void *syscalls[] = {
    [__NR_exit] = syscall_exit,
    [__NR_read] = syscall_read,
    [__NR_open] = syscall_open,
    [__NR_brk] = syscall_brk,
    [__NR_fstat] = syscall_fstat,
    [__NR_log] = syscall_log};

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
