#include <kernel/drivers/pit.h>
#include <kernel/interrupts/irq.h>
#include <kernel/interrupts/pic.h>
#include <kernel/utils/log.h>

static volatile uint64_t pit_ticks;

int pit_handler(struct itr_registers *registers)
{
  pit_ticks++;
  return IRQ_CONTINUE;
}

void pit_init()
{
  pit_ticks = 0;
  irq_handler_set(0, pit_handler);
  log_info("PIT: Initialized\n");
}
