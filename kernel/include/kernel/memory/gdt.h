#pragma once

#include <stdint.h>

#define GDT_ENTRIES 5

struct gdt_entry
{
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

struct gdt_pointer
{
  uint16_t limit;
  uint32_t base;
} __attribute((packed));

void gdt_init();
