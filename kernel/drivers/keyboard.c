#include <kernel/drivers/keyboard.h>
#include <kernel/interrupts/irq.h>
#include <kernel/asm/io.h>
#include <kernel/utils/log.h>

int keyboard_handler(struct itr_registers *registers)
{
  (void)registers;

  uint8_t scancode;

  if (inb(0x64) & 0x01)
  {
    scancode = inb(0x60);
    log_info("Keyboard: Received scancode = %d\n", scancode);
  }

  return ITR_CONTINUE;
}

void keyboard_init()
{
  irq_handler_set(1, keyboard_handler);

  log_info("Keyboard: Initialized\n");
}
