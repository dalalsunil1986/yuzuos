#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>

#define _CS_PATH 0

extern char **environ;

int execl(const char *pathname, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *pathname, const char *arg, ...);
int execv(const char *pathname, char *const argv[]);
int execve(const char *pathname, char *const argv[], char *const envp[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t fork(void);
int brk(void *addr);
void *sbrk(intptr_t increment);
ssize_t read(int fd, void *buf, size_t count);
