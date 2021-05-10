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
#include <stdbool.h>

void kernel_init(uint32_t magic, uint32_t addr)
{
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
  syscall_init();

  sys_sti();
  while (true)
    ;
}
