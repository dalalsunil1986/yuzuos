/**
 * @file kernel/arch/x86/boot/main.c
 * @brief X86 protected mode initialization
 * @author Saullo Bretas Silva
 */
#include <kernel/init.h>
#include <kernel/boot/multiboot.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/isr.h>
#include <kernel/memory/gdt.h>
#include <kernel/memory/physical.h>
#include <kernel/memory/virtual.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/mouse.h>
#include <kernel/drivers/pit.h>
#include <stdint.h>

/**
 * @brief X86 entrypoint
 * 
 * Called by boot/boot.S bootstrap
 * 
 * @param magic The multiboot header magic
 * @param addr A pointer to the multiboot header struct
 */
void arch_main(uint32_t magic, uint32_t addr)
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
  keyboard_init();
  mouse_init();

  kernel_main();
}
