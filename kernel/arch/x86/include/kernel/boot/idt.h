/**
 * @file kernel/arch/x86/boot/idt.h
 * @brief Interrupt Descriptor Table code
 * @author Saullo Bretas Silva
 */

#pragma once

#include <stdint.h>

/* IDT entries count */
#define IDT_ENTRIES 256

/* IDT entry structure */
struct idt_entry
{
  uint16_t base_low;  /* Segment address (low part of it) */
  uint16_t selector;  /* Segment selector code */
  uint8_t zero;       /* Always zero */
  uint8_t flags;      /* Segment type flags */
  uint16_t base_high; /* Segment address (high part of it) */
} __attribute__((packed));

/* IDT segment descriptor */
struct idt_pointer
{
  uint16_t limit; /* The table size subtracted by 1 */
  uint32_t base;  /* The segment linear address */
} __attribute((packed));

/**
 * @brief Initialize the IDT and setups IRQ and ISR
 */
void idt_init();
