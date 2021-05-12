#pragma once

#include <kernel/memory/virtual.h>
#include <kernel/utils/plist.h>
#include <kernel/utils/types.h>
#include <stdint.h>

#define SCHED_TIME_SLICE 8
#define SCHED_STACK_SIZE 0x2000
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
  THREAD_RUNNING
};

struct thread
{
  pid_t tid;
  uint32_t time;
  uint32_t esp;
  uint32_t stack_kernel;

  enum thread_state state;
  struct process *process;
  struct plist_node list;
};

struct process
{
  pid_t pid;
  struct thread *thread;
  struct process *parent;
  struct page_dir *page_dir;
};

void sched_init();
void sched_schedule();
void sched_load(const char *path);
