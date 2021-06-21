/**
 * @file kernel/arch/x86/asm/irq.h
 * @brief Interrupt Request (Hardware Interrupt) code
 * @author Saullo Bretas Silva
 */

#pragma once

#include <kernel/asm/itr_handler.h>

/* IRQ interrupt count */
#define IRQ_SIZE 16
/* Maximum IRQ with same number that can be used */
#define IRQ_DEPTH 4

/**
 * @brief Add a IRQ handler
 * @param index The IRQ number
 * @param handler A pointer to the handler function
 */
void irq_handler_set(uint8_t index, itr_handler_t handler);

/**
 * @brief Remove a IRQ handler
 * @param index The IRQ number
 */
void irq_handler_unset(uint8_t index);
