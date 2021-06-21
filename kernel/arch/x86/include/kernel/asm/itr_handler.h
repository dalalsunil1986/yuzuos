/**
 * @file kernel/arch/x86/asm/itr_handler.h
 * @brief x86 interrupt handler interface
 * @author Saullo Bretas Silva
 */
#pragma once

#include <stdint.h>

/* Continue receiving interrupts */
#define ITR_STOP 0
/* Stop receiving interrupts */
#define ITR_CONTINUE 1

/* Interrupt register structure */
struct itr_registers
{
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t int_no; /* Interrupt number */
  uint32_t err_code;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
} __attribute__((packed));

/** 
 * @brief Interrupt handler interface 
 * @param registers A pointer to the interrupt registers structure
 */
typedef int (*itr_handler_t)(struct itr_registers *registers);
