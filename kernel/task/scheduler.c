#include <kernel/task/scheduler.h>
#include <kernel/task/tss.h>
#include <kernel/system/sys.h>
#include <kernel/interrupts/irq.h>
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
void sched_thread_queue(struct thread *thread);
void sched_thread_unqueue(struct thread *thread);

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
  (void)path;
  (void)thread;
  while (1)
    ;
}

struct thread *sched_thread_create(const char *path, struct process *process)
{
  sched_lock();

  struct thread *thread = calloc(1, sizeof(struct thread));
  process->thread = thread;
  thread->tid = sched_tid++;
  thread->process = process;
  thread->state = THREAD_READY;
  thread->stack_kernel = (uint32_t)(calloc(SCHED_STACK_SIZE, sizeof(char)) + SCHED_STACK_SIZE);
  thread->esp = thread->stack_kernel - sizeof(struct trap_frame);

  struct trap_frame *frame = (struct trap_frame *)thread->esp;
  memset(frame, 0, sizeof(struct trap_frame));
  frame->param2 = (uint32_t)thread;
  frame->param1 = (uint32_t)strdup(path);
  frame->return_addr = SCHED_PAGE_FAULT;
  frame->eip = (uint32_t)sched_thread_entry;
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

struct process *sched_process_create(struct process *parent)
{
  sched_lock();

  struct process *process = calloc(1, sizeof(struct process));
  process->pid = sched_pid++;
  process->page_dir = parent ? virt_mm_addr_create(parent->page_dir) : virt_mm_dir_get();
  process->parent = parent;

  sched_unlock();

  return process;
}

void sched_load(const char *path)
{
  struct process *process = sched_process_create(sched_process);
  struct thread *thread = sched_thread_create(path, process);
  sched_thread_queue(thread);
  log_info("Scheduler: Loaded program from path = %s\n", path);
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

void sched_init()
{
  sched_locks = 0;
  sched_pid = 0;
  sched_tid = 0;

  plist_head_init(&sched_list);

  irq_handler_set(0, sched_handler);

  log_info("Scheduler: Initialized\n");
}
