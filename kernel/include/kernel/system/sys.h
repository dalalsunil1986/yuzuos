#pragma once

#include <stdbool.h>

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
