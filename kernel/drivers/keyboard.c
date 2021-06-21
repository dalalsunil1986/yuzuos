#include <kernel/drivers/keyboard.h>
#include <kernel/asm/irq.h>
#include <kernel/asm/io.h>
#include <kernel/boot/serial.h>

int keyboard_handler(struct itr_registers *registers)
{
  (void)registers;

  uint8_t scancode;

  if (inb(0x64) & 0x01)
  {
    scancode = inb(0x60);
    serial_early_kprintf("Keyboard: Received scancode = %d\n", scancode);
  }

  return ITR_CONTINUE;
}

void keyboard_init()
{
  irq_handler_set(1, keyboard_handler);

  serial_early_kprintf("Keyboard: Initialized\n");
}
