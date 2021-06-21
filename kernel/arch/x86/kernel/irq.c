#include <kernel/asm/irq.h>
#include <kernel/asm/pic.h>
#include <stddef.h>

/* This stores all IRQ handlers */
static itr_handler_t irq[IRQ_SIZE * IRQ_DEPTH];

void irq_handler_set(uint8_t index, itr_handler_t handler)
{
  for (uint8_t i = 0; i < IRQ_DEPTH; i++)
  {
    if (irq[i * IRQ_SIZE + index])
      continue;
    irq[i * IRQ_SIZE + index] = handler;
    break;
  }
}

void irq_handler_unset(uint8_t index)
{
  for (uint8_t i = 0; i < IRQ_DEPTH; i++)
    irq[i * IRQ_SIZE + index] = NULL;
}

/** 
 * @brief IRQ common handler 
 * @param registers A pointer to the interrupt registers structure
 */
void irq_handler(struct itr_registers *registers)
{
  if (registers->int_no <= 47 && registers->int_no >= 32)
  {
    for (size_t i = 0; i < IRQ_DEPTH; i++)
    {
      itr_handler_t handler = irq[i * IRQ_SIZE + (registers->int_no - 32)];
      if (!handler)
        break;

      if (handler(registers))
        continue;
    }
    pic_eoi(registers->int_no - 32);
  }
}
