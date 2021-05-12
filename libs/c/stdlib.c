#include <stdlib.h>
#include <syscall.h>

_syscall1(exit, int);
void exit(int status)
{
  syscall_exit(status);
  __builtin_unreachable();
}
