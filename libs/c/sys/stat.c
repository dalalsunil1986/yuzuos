#include <sys/stat.h>
#include <syscall.h>

_syscall2(stat, const char *, struct stat *);
int stat(const char *restrict path, struct stat *restrict buf)
{
  SYSCALL_RETURN(syscall_stat(path, buf));
}

_syscall2(fstat, int, struct stat *);
int fstat(int fildes, struct stat *stat)
{
  return syscall_fstat(fildes, stat);
}
