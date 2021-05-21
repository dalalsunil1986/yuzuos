#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <syscall.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

char **environ;

_syscall1(brk, void *);
int brk(void *addr)
{
  SYSCALL_RETURN(syscall_brk(addr));
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
  SYSCALL_RETURN(syscall_read(fd, buf, count));
}

size_t confstr(int name, char *buf, size_t len)
{
  (void)name;
  (void)buf;
  (void)len;
  return -1;
}

int execl_argc_get(va_list ap, const char *arg)
{
  int argc = arg ? 1 : 0;
  while (arg && va_arg(ap, const char *))
    argc++;
  return argc;
}

char **execl_argv_get(va_list ap, int argc, const char *arg)
{
  const char **argv = calloc(argc + 1, sizeof(char *));
  argv[0] = arg;

  for (int i = 1; i < argc; i++)
    argv[i] = va_arg(ap, const char *);

  argv[argc] = NULL;
  return (char **)argv;
}

int execl(const char *pathname, const char *arg, ...)
{
  va_list ap;

  va_start(ap, arg);
  int argc = execl_argc_get(ap, arg);
  va_end(ap);

  va_start(ap, arg);
  char **argv = execl_argv_get(ap, argc, arg);
  va_end(ap);

  int result = execve(pathname, argv, environ);
  free(argv);

  return result;
}

int execlp(const char *file, const char *arg, ...)
{
  va_list ap;

  va_start(ap, arg);
  int argc = execl_argc_get(ap, arg);
  va_end(ap);

  va_start(ap, arg);
  char **argv = execl_argv_get(ap, argc, arg);
  va_end(ap);

  int result = execvpe(file, argv, environ);
  free(argv);

  return result;
}

int execle(const char *pathname, const char *arg, ...)
{
  va_list ap;

  va_start(ap, arg);
  int argc = execl_argc_get(ap, arg);
  va_end(ap);

  va_start(ap, arg);
  char **argv = execl_argv_get(ap, argc, arg);
  char **envp = va_arg(ap, char **);
  va_end(ap);

  int result = execve(pathname, argv, envp);
  free(argv);
  free(envp);

  return result;
}

int execv(const char *pathname, char *const argv[])
{
  return execve(pathname, argv, environ);
}

_syscall3(execve, const char *, char *const *, char *const *);
int execve(const char *pathname, char *const argv[], char *const envp[])
{
  SYSCALL_RETURN(syscall_execve(pathname, argv, envp));
}

int execvp(const char *file, char *const argv[])
{
  return execvpe(file, argv, environ);
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
  for (int i = 0, len = strlen(file); i < len; i++)
    if (file[i] == '/')
      return execve(file, argv, envp);

  char *path;
  char *envpath = getenv("PATH");
  if (!envpath)
  {
    path = calloc(MAXPATHLEN, sizeof(char));
    confstr(_CS_PATH, path, 0);
  }
  else
    path = envpath;

  char pathname[MAXPATHLEN] = {0};
  struct stat p_stat;
  int result = -1;
  int i = 0;
  int length = strlen(path);
  int path_start = 0;
  while (true)
  {
    char ch = path[i++];
    if (ch == ':')
    {
      pathname[path_start] = 0;
      path_start = 0;
      strcat(pathname, "/");
      strcat(pathname, file);
      if ((result = stat(pathname, &p_stat)) >= 0)
        break;
    }
    else
      pathname[path_start++] = ch;

    if (i == length)
    {
      pathname[path_start] = 0;
      strcat(pathname, "/");
      strcat(pathname, file);
      break;
    }
  }

  if (path != envpath)
    free(path);

  if ((i == length && (result = stat(pathname, &p_stat)) >= 0) || (i < length && result >= 0))
    return execve(pathname, argv, envp);

  return result;
}

_syscall0(fork);
pid_t fork(void)
{
  SYSCALL_RETURN(syscall_fork());
}
