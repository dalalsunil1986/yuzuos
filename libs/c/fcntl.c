#include <fcntl.h>
#include <syscall.h>
#include <stdarg.h>
#include <sys/types.h>

_syscall3(open, const char *, int, mode_t);
int open(const char *path, int flags, ...)
{
  mode_t mode = 0;
  if (flags & O_CREAT)
  {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
  }

  SYSCALL_RETURN(syscall_open(path, flags, mode));
}
