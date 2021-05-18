#include <unistd.h>
#include <syscall.h>

_syscall1(brk, void *);
int brk(void *addr)
{
  return syscall_brk(addr);
}

void *sbrk(intptr_t increment)
{
  uint32_t current_brk = brk(0);
  brk((void *)current_brk + increment);
  return (void *)current_brk;
}

_syscall3(read, int, void *, size_t);
ssize_t read(int fd, void *buf, size_t count)
{
  return syscall_read(fd, buf, count);
}
