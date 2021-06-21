/**
 * @file kernel/init/main.c
 * @brief Architecture-neutral initialization
 */
#include <kernel/system/sys.h>
#include <kernel/system/syscall.h>
#include <kernel/drivers/rtc.h>
#include <kernel/drivers/vga.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/ide.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/task/scheduler.h>
#include <stdbool.h>

/**
 * @brief Kernel entrypoint
 * 
 * Invoked from architecture-specific code
 */
void kernel_main()
{
  log_init();
  rtc_init();
  vga_init();
  pci_init();
  ide_init();
  sched_init();
  virt_fs_init();
  syscall_init();

  sched_load("/bin/system");
  sched_schedule();

  sys_sti();
  while (true)
    ;
}
