#include <kernel/drivers/pit.h>
#include <kernel/asm/irq.h>
#include <kernel/boot/serial.h>

static volatile uint64_t pit_ticks;

void pit_wait(uint32_t ms)
{
  uint32_t target_ticks = pit_ticks + ms;
  while (pit_ticks < target_ticks)
    ;
}

int pit_handler(struct itr_registers *registers)
{
  (void)registers;
  pit_ticks++;
  return ITR_CONTINUE;
}

void pit_init()
{
  pit_ticks = 0;
  irq_handler_set(0, pit_handler);
  serial_early_kprintf("PIT: Initialized\n");
}
