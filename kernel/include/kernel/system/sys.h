#pragma once

#include <kernel/interrupts/handler.h>
#include <stdbool.h>

#define sys_panic(message, registers) _sys_panic(message, __FILE__, __LINE__, registers)

static inline void sys_sti()
{
  asm volatile("sti");
}

static inline void sys_cli()
{
  asm volatile("cli");
}

static inline void sys_hlt()
{
  asm volatile("hlt");
}

static inline void sys_stop()
{
  while (true)
    sys_hlt();
}

void _sys_panic(char *message, const char *file, int line, struct itr_registers *registers);
