#pragma once

#include <kernel/interrupts/handler.h>

#define ISR_SIZE 256

void isr_init();
void isr_handler_set(uint8_t index, itr_handler_t handler);
void isr_handler_unset(uint8_t index);
