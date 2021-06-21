#include <kernel/drivers/mouse.h>
#include <kernel/interrupts/irq.h>
#include <kernel/asm/io.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <stdbool.h>

void mouse_write(uint8_t write)
{
  outb(0x64, 0xD4);
  outb(0x60, write);
}

int mouse_handler(struct itr_registers *registers)
{
  (void)registers;
  uint8_t status = inb(0x64);

  while ((status & 0x01) && (status & 0x20))
  {
    uint8_t package = inb(0x60);
    log_info("Mouse: Status = %d, package = %d\n", status, package);
    break;
  }

  return ITR_CONTINUE;
}

void mouse_init()
{
  while ((inb(0x64) & 1))
    inb(0x60);

  outb(0x64, 0xA8);
  inb(0x60);
  outb(0x64, 0x20);
  uint8_t status = inb(0x60) | 3;
  outb(0x64, 0x60);
  outb(0x60, status);
  mouse_write(0xF6);
  inb(0x60);
  mouse_write(0xF4);
  inb(0x60);

  outb(0x60, 0xF0);
  outb(0x60, 0x02);
  inb(0x60);

  irq_handler_set(12, mouse_handler);

  log_info("Mouse: Initialized\n");
}
