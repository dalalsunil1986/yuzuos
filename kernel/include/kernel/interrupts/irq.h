#pragma once

#include <kernel/interrupts/handler.h>

#define IRQ_SIZE 16
#define IRQ_DEPTH 4

void irq_init();
void irq_handler_set(uint8_t irq, itr_handler_t handler);
void irq_handler_unset(uint8_t irq);
