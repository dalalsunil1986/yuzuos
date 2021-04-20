#pragma once

#include <stdint.h>

#define IDT_ENTRIES 256

struct idt_entry
{
  uint16_t base_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t flags;
  uint16_t base_high;
} __attribute__((packed));

struct idt_pointer
{
  uint16_t limit;
  uint32_t base;
} __attribute((packed));

void idt_init();
void idt_entry_add(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags);
