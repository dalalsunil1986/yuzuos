#include <sys/stat.h>
#include <syscall.h>

_syscall2(fstat, int, struct stat *);
int fstat(int fildes, struct stat *stat)
{
  return syscall_fstat(fildes, stat);
}
