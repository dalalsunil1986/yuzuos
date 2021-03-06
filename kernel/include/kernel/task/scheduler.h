#pragma once

#include <kernel/filesystem/virtual.h>
#include <kernel/memory/virtual.h>
#include <kernel/utils/plist.h>
#include <kernel/utils/types.h>
#include <kernel/asm/itr_handler.h>
#include <kernel/system/limits.h>
#include <stdint.h>

#define SCHED_TIME_SLICE 8
#define SCHED_STACK_SIZE 0x2000
#define SCHED_HEAP_SIZE 0x20000
#define SCHED_PAGE_FAULT 0xFFFFFFFF

struct trap_frame
{
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t eip;
  uint32_t return_addr;
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
};

enum thread_state
{
  THREAD_READY,
  THREAD_RUNNING,
  THREAD_TERMINATED
};

struct thread
{
  pid_t tid;
  uint32_t time;
  uint32_t esp;
  uint32_t stack_kernel;
  uint32_t stack_user;

  enum thread_state state;
  struct itr_registers registers;
  struct process *process;
  struct plist_node list;
};

struct process_files
{
  struct vfs_file *fd[FD_MAX];
};

struct process_fs
{
  struct vfs_dentry *root;
  struct vfs_mount *mount;
};

struct process_vm
{
  uint32_t start;
  uint32_t end;
  uint32_t flags;

  struct vfs_file *file;
  struct process_mm *mm;
  struct dlist_head list;
};

struct process_mm
{
  uint32_t brk_start;
  uint32_t brk;
  uint32_t brk_end;
  uint32_t cache;

  struct dlist_head list;
};

struct process
{
  const char *name;
  pid_t pid;
  int exit_code;

  struct process_mm *mm;
  struct process_fs *fs;
  struct process_files *files;
  struct thread *thread;
  struct process *parent;
  struct page_dir *page_dir;
};

void sched_init();
void sched_schedule();
void sched_exit(int code);
void sched_load(const char *path);
struct process *sched_process_get();
struct thread *sched_thread_get();
pid_t sched_process_fork(struct process *parent);
int sched_process_execve(const char *path, char *const argv[], char *const envp[]);
