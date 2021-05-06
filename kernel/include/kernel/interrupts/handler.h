#pragma once

#include <stdint.h>

#define ITR_STOP 0
#define ITR_CONTINUE 1

struct itr_registers
{
  uint32_t unused;
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
  uint32_t int_no;
  uint32_t err_code;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
};

typedef int (*itr_handler_t)(struct itr_registers *registers);
