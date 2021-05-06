#include <kernel/drivers/rtc.h>
#include <kernel/interrupts/irq.h>
#include <kernel/utils/log.h>
#include <kernel/system/io.h>

static volatile uint64_t rtc_ticks;

int rtc_handler(struct itr_registers *registers)
{
  (void)registers;
  rtc_ticks++;

  outb(0x70, 0x0C);
  inb(0x71);

  return IRQ_CONTINUE;
}

void rtc_init()
{
  rtc_ticks = 0;

  outb(0x70, 0x8B);
  char prev = inb(0x71);
  outb(0x70, 0x8B);
  outb(0x71, prev | 0x40);

  irq_handler_set(8, rtc_handler);

  log_info("RTC: Initialized\n");
}
