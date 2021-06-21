/**
 * @file kernel/arch/x86/asm/isr.h
 * @brief Interrupt Service Routines code
 * @author Saullo Bretas Silva
 */

#pragma once

#include <kernel/asm/itr_handler.h>

/* ISR interrupt count */
#define ISR_SIZE 256

/**
 * @brief Add a ISR handler
 * @param index The ISR number
 * @param handler A pointer to the handler function
 */
void isr_handler_set(uint8_t index, itr_handler_t handler);

/**
 * @brief Remove a ISR handler
 * @param index The ISR number
 */
void isr_handler_unset(uint8_t index);
