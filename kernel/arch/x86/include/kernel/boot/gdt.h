/**
 * @file kernel/arch/x86/boot/gdt.h
 * @brief Global Descriptor Table code
 * @author Saullo Bretas Silva
 */

#pragma once

#include <stdint.h>

/* GDT entries count */
#define GDT_ENTRIES 6

/* GDT entry structure */
struct gdt_entry
{
  uint16_t limit_low;  /* Limit address (low part of it) */
  uint16_t base_low;   /* Segment address (low part of it) */
  uint8_t base_middle; /* Segment address (middle part of it) */
  uint8_t access;      /* Entry access flags */
  uint8_t granularity; /* Page granularity */
  uint8_t base_high;   /* Segment address (high part of it) */
} __attribute__((packed));

/* GDT segment descriptor */
struct gdt_pointer
{
  uint16_t limit; /* The table size subtracted by 1 */
  uint32_t base;  /* The segment linear address */
} __attribute((packed));

/**
 * @brief Initialize the GDT
 */
void gdt_init();
