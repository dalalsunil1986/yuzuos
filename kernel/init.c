#include <kernel/utils/log.h>
#include <kernel/memory/gdt.h>
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/isr.h>
#include <kernel/system/sys.h>
#include <kernel/system/syscall.h>
#include <kernel/boot/multiboot.h>
#include <kernel/drivers/pit.h>
#include <kernel/drivers/rtc.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/mouse.h>
#include <kernel/drivers/vga.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/ide.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/task/scheduler.h>
#include <stdbool.h>

void kernel_init(uint32_t magic, uint32_t addr)
{
  sys_cli();

  log_init();
  gdt_init();
  idt_init();
  isr_init();
  irq_init();
  multiboot_init(magic, addr);
  phys_mm_init();
  virt_mm_init();
  pit_init();
  rtc_init();
  keyboard_init();
  mouse_init();
  vga_init();
  pci_init();
  ide_init();
  sched_init();
  ext2_fs_init();
  virt_fs_init();
  syscall_init();

  sched_load("/bin/system");
  sched_schedule();

  sys_sti();
  while (true)
    ;
}
