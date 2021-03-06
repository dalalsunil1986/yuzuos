#include <kernel/task/scheduler.h>
#include <kernel/task/tss.h>
#include <kernel/task/elf32.h>
#include <kernel/system/sys.h>
#include <kernel/system/syscall.h>
#include <kernel/asm/irq.h>
#include <kernel/asm/isr.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/utils/string.h>

static volatile uint64_t sched_locks;
static volatile pid_t sched_pid;
static volatile pid_t sched_tid;
static struct plist_head sched_list;
static struct thread *sched_thread;
static struct process *sched_process;

extern void sched_switch(uint32_t *esp, uint32_t esp_new, uint32_t physical);
extern void sched_jump_usermode(uint32_t eip, uint32_t esp, uint32_t addr);
extern void sched_return_usermode(struct itr_registers *registers);
void sched_thread_queue(struct thread *thread);
void sched_thread_unqueue(struct thread *thread);

struct process *sched_process_get()
{
  return sched_process;
}

struct thread *sched_thread_get()
{
  return sched_thread;
}

void sched_lock()
{
  sys_cli();
  sched_locks++;
}

void sched_unlock()
{
  sched_locks--;
  if (sched_locks == 0)
    sys_sti();
}

struct thread *sched_thread_pop_next()
{
  if (plist_is_head_empty(&sched_list))
    return NULL;

  struct thread *thread = plist_entry_first(&sched_list, struct thread, list);
  plist_remove(&thread->list, &sched_list);
  return thread;
}

struct thread *sched_thread_get_next()
{
  if (plist_is_head_empty(&sched_list))
    return NULL;
  return plist_entry_first(&sched_list, struct thread, list);
}

void sched_thread_queue(struct thread *thread)
{
  plist_add(&thread->list, &sched_list);
}

void sched_thread_update(struct thread *thread, uint8_t state)
{
  if (thread->state == state)
    return;

  sched_lock();
  sched_thread_unqueue(thread);
  thread->state = state;
  sched_thread_queue(thread);
  sched_unlock();
}

void sched_thread_unqueue(struct thread *thread)
{
  plist_remove(&thread->list, &sched_list);
}

void sched_thread_entry(const char *path, struct thread *thread)
{
  sched_unlock();
  struct elf32_layout *elf = elf32_load(path);
  if (!elf)
  {
    log_error("Scheduler: Failed to load ELF file, path = %s, tid = %d\n", path, thread->tid);
    sched_thread_unqueue(thread);
  }

  thread->stack_user = elf->stack;
  tss_set_stack(0x10, thread->stack_kernel);

  elf->stack -= 4;
  *(uint32_t *)elf->stack = (uint32_t)NULL;
  elf->stack -= 4;
  *(uint32_t *)elf->stack = (uint32_t)NULL;
  elf->stack -= 4;
  *(uint32_t *)elf->stack = (uint32_t)0;

  log_info("Scheduler: Enter usermode, pid = %d, tid = %d, entry = 0x%x, stack = 0x%x\n", sched_process->pid, sched_thread->tid, elf->entry, elf->stack);
  sched_jump_usermode(elf->stack, elf->entry, SCHED_PAGE_FAULT);
}

void sched_thread_user_entry(struct thread *thread)
{
  sched_unlock();

  tss_set_stack(0x10, thread->stack_kernel);
  log_info("Scheduler: Return usermode, pid = %d\n", sched_process->pid);
  sched_return_usermode(&thread->registers);
}

struct thread *sched_thread_create(struct process *process)
{
  sched_lock();

  struct thread *thread = calloc(1, sizeof(struct thread));
  process->thread = thread;
  thread->tid = sched_tid++;
  thread->time = 0;
  thread->process = process;
  thread->state = THREAD_READY;
  thread->stack_kernel = (uint32_t)(calloc(SCHED_STACK_SIZE, sizeof(char)) + SCHED_STACK_SIZE);
  thread->esp = thread->stack_kernel - sizeof(struct trap_frame);

  struct trap_frame *frame = (struct trap_frame *)thread->esp;
  memset(frame, 0, sizeof(struct trap_frame));
  frame->return_addr = SCHED_PAGE_FAULT;
  frame->eax = 0;
  frame->ecx = 0;
  frame->edx = 0;
  frame->ebx = 0;
  frame->esp = 0;
  frame->ebp = 0;
  frame->esi = 0;
  frame->edi = 0;

  plist_node_init(&thread->list, 0);
  sched_unlock();

  return thread;
}

struct process_files *sched_process_clone_files(struct process *parent)
{
  struct process_files *files = calloc(1, sizeof(struct process_files));
  if (parent)
  {
    memcpy(files, parent->files, sizeof(struct process_files));
    for (int i = 0; i < FD_MAX; i++)
      if (parent->files->fd[i])
        atomic_inc(&parent->files->fd[i]->count);
  }
  return files;
}

struct process_fs *sched_process_clone_fs(struct process *parent)
{
  struct process_fs *fs = calloc(1, sizeof(struct process_fs));
  if (parent)
    memcpy(fs, parent->fs, sizeof(struct process_fs));
  return fs;
}

struct process_mm *sched_process_clone_mm(struct process *parent)
{
  struct process_mm *mm = calloc(1, sizeof(struct process_mm));
  memcpy(mm, parent->mm, sizeof(struct process_mm));
  dlist_head_init(&mm->list);

  struct process_vm *iter = NULL;
  dlist_foreach_entry(iter, &parent->mm->list, list)
  {
    struct process_vm *parent_mm = calloc(1, sizeof(struct process_vm));
    parent_mm->start = iter->start;
    parent_mm->end = iter->end;
    parent_mm->file = iter->file;
    parent_mm->flags = iter->flags;
    parent_mm->mm = mm;
    dlist_add_tail(&parent_mm->list, &mm->list);
  }
  return mm;
}

struct process *sched_process_create(struct process *parent)
{
  sched_lock();

  struct process *process = calloc(1, sizeof(struct process));
  process->pid = sched_pid++;
  process->page_dir = parent ? virt_mm_addr_create() : virt_mm_dir_get();
  process->parent = parent;
  process->mm = calloc(1, sizeof(struct process_mm));
  process->fs = sched_process_clone_fs(parent);
  process->files = sched_process_clone_files(parent);

  dlist_head_init(&process->mm->list);

  sched_unlock();

  return process;
}

void sched_load(const char *path)
{
  struct process *process = sched_process_create(sched_process);
  process->name = strdup(path);

  struct thread *thread = sched_thread_create(process);

  struct trap_frame *frame = (struct trap_frame *)thread->esp;
  frame->param2 = (uint32_t)thread;
  frame->param1 = (uint32_t)strdup(path);
  frame->eip = (uint32_t)sched_thread_entry;

  sched_thread_queue(thread);
  log_info("Scheduler: Loaded program from path = %s\n", path);
}

pid_t sched_process_fork(struct process *parent)
{
  sched_lock();

  struct process *process = calloc(1, sizeof(struct process));
  process->pid = sched_pid++;
  process->parent = parent;
  process->name = strdup(parent->name);
  process->mm = sched_process_clone_mm(parent);
  process->fs = sched_process_clone_fs(parent);
  process->files = sched_process_clone_files(parent);
  process->page_dir = virt_mm_fork_dir(parent->page_dir);

  struct thread *thread = sched_thread_create(process);
  thread->stack_user = parent->thread->stack_user;

  memcpy(&thread->registers, &parent->thread->registers, sizeof(struct itr_registers));
  thread->registers.eax = 0;

  struct trap_frame *frame = (struct trap_frame *)thread->esp;
  frame->param1 = (uint32_t)thread;
  frame->eip = (uint32_t)sched_thread_user_entry;

  sched_unlock();
  sched_thread_queue(process->thread);
  return process->pid;
}

void sched_thread_switch(struct thread *thread)
{
  if (sched_thread == thread)
  {
    sched_thread->time = 0;
    sched_thread_update(sched_thread, THREAD_RUNNING);
    return;
  }

  struct thread *p_thread = sched_thread;

  sched_thread = thread;
  sched_thread->time = 0;
  sched_thread_update(sched_thread, THREAD_RUNNING);
  sched_process = sched_thread->process;

  uint32_t physical = virt_mm_phys_addr_get((uint32_t)sched_thread->process->page_dir);
  tss_set_stack(0x10, sched_thread->stack_kernel);
  sched_switch(&p_thread->esp, sched_thread->esp, physical);
}

void sched_schedule()
{
  if (sched_thread->state == THREAD_RUNNING)
    return;

  sched_lock();

  struct thread *thread = sched_thread_pop_next();
  if (!thread)
  {
    do
    {
      sched_unlock();
      sys_hlt();
      sched_lock();

      thread = sched_thread_pop_next();
      if (!thread && sched_thread->state == THREAD_RUNNING)
        thread = sched_thread;
    } while (!thread);
  }
  sched_thread_switch(thread);
  sched_unlock();
}

int sched_handler(struct itr_registers *registers)
{
  (void)registers;
  if (sched_thread->state != THREAD_RUNNING)
    return ITR_CONTINUE;

  sched_lock();

  bool can_sched = false;
  sched_thread->time++;

  if (sched_thread->time >= SCHED_TIME_SLICE)
  {
    struct thread *thread = sched_thread_get_next();
    if (thread)
    {
      struct thread *thread_first = plist_entry_first(&sched_list, struct thread, list);
      struct thread *thread_last = plist_entry_last(&sched_list, struct thread, list);

      int prio = thread_first->list.prio;
      struct thread *iter;
      plist_foreach_entry(iter, &sched_list, list)
      {
        iter->list.prio -= prio;
      }

      sched_thread->list.prio = thread_last->list.prio + 1;
      sched_thread_update(sched_thread, THREAD_READY);
      can_sched = true;
    }
  }

  sched_unlock();

  if (!sched_locks && can_sched)
  {
    log_info("Scheduler: Thread round-robin, tid = %d\n", sched_thread->tid);
    sched_schedule();
  }

  return ITR_CONTINUE;
}

void sched_exit(int code)
{
  sched_lock();

  struct process *process = sched_process;

  if (process->pid == 1)
    sys_panic("Scheduler: Tried to kill init process", &process->thread->registers);

  process->exit_code = code;

  for (int i = 0; i < FD_MAX; i++)
  {
    struct vfs_file *file = process->files->fd[i];

    if (file && atomic_read(&file->count) == 1 && file->op->release)
    {
      file->op->release(file->dentry->inode, file);
      free(file);
      process->files->fd[i] = 0;
    }
  }

  struct process_vm *iter;
  struct process_vm *next;
  dlist_foreach_entry_safe(iter, next, &process->mm->list, list)
  {
    if (!iter->file)
      virt_mm_addr_range_unmap(process->page_dir, iter->start, iter->end);

    dlist_remove(&iter->list);
    free(iter);
  }

  sched_thread_update(sched_thread, THREAD_TERMINATED);
  virt_mm_addr_range_unmap(process->page_dir, sched_thread->stack_user - SCHED_STACK_SIZE, sched_thread->stack_user);
  log_info("Scheduler: Process pid = %d, exited code = %d\n", process->pid, code);

  sched_unlock();
  sched_schedule();
}

int pointers_array_count(void *arr)
{
  if (!arr)
    return 0;

  const int *p_arr = arr;
  while (*p_arr)
    p_arr++;

  return p_arr - (int *)arr;
}

int32_t sched_process_sbrk(uint32_t increment)
{
  uint32_t brk = sched_process->mm->brk;
  syscall_brk((void *)sched_process->mm->brk + increment);
  return brk;
}

int sched_process_execve(const char *path, char *const argv[], char *const envp[])
{
  int argv_len = pointers_array_count((void *)argv);
  char **new_argv = calloc(argv_len, sizeof(char *));
  for (int i = 0; i < argv_len; ++i)
  {
    int len = strlen(argv[i]);
    new_argv[i] = calloc(len + 1, sizeof(char));
    memcpy(new_argv[i], argv[i], len);
  }

  int envp_len = pointers_array_count((void *)envp);
  char **new_envp = calloc(envp_len, sizeof(char *));
  for (int i = 0; i < envp_len; ++i)
  {
    int len = strlen(envp[i]);
    new_envp[i] = calloc(len + 1, sizeof(char));
    memcpy(new_envp[i], envp[i], len);
  }

  free((void *)sched_process->name);
  sched_process->name = strdup(path);

  char *p_path = strdup(path);
  elf32_unload();
  struct elf32_layout *elf = elf32_load(p_path);
  free(p_path);

  char **user_argv = (char **)sched_process_sbrk(argv_len + 1);
  memset(user_argv, 0, argv_len + 1);
  for (int i = 0; i < argv_len; ++i)
  {
    int len = strlen(new_argv[i]);
    user_argv[i] = (char *)sched_process_sbrk(len + 1);
    memcpy(user_argv[i], new_argv[i], len);
  }

  char **user_envp = (char **)sched_process_sbrk(envp_len + 1);
  memset(user_envp, 0, envp_len + 1);
  for (int i = 0; i < envp_len; ++i)
  {
    int len = strlen(new_envp[i]);
    user_envp[i] = (char *)sched_process_sbrk(len + 1);
    memcpy(user_envp[i], new_envp[i], len);
  }

  sched_thread->stack_user = elf->stack;
  elf->stack -= 4;
  *(uint32_t *)elf->stack = (uint32_t)user_envp;
  elf->stack -= 4;
  *(uint32_t *)elf->stack = (uint32_t)user_argv;
  elf->stack -= 4;
  *(uint32_t *)elf->stack = argv_len;

  tss_set_stack(0x10, sched_thread->stack_kernel);
  log_info("Scheduler: Execve enter usermode, pid = %d, tid = %d, entry = 0x%x, stack = 0x%x\n", sched_process->pid, sched_thread->tid, elf->entry, elf->stack);
  sched_jump_usermode(elf->stack, elf->entry, SCHED_PAGE_FAULT);
  return 0;
}

void sched_init()
{
  sched_locks = 0;
  sched_pid = 0;
  sched_tid = 0;

  plist_head_init(&sched_list);

  sched_process = sched_process_create(NULL);
  sched_thread = sched_thread_create(sched_process);

  irq_handler_set(0, sched_handler);

  log_info("Scheduler: Initialized\n");
}
