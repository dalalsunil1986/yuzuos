#pragma once

#include <kernel/interrupts/handler.h>

#define IRQ_SIZE 16
#define IRQ_DEPTH 4

#define IRQ_STOP 0
#define IRQ_CONTINUE 1

void irq_init();
void irq_handler_set(uint8_t index, itr_handler_t handler);
void irq_handler_unset(uint8_t index);
