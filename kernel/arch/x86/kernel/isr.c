#include <kernel/asm/isr.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <stddef.h>

/* This stores all ISR handlers */
static itr_handler_t isr[ISR_SIZE];

/* ISR messages */
static const char *isr_msg[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid opcode",
    "Device Not Available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-Segment Fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"};

void isr_handler_set(uint8_t index, itr_handler_t handler)
{
  isr[index] = handler;
}

void isr_handler_unset(uint8_t index)
{
  isr[index] = NULL;
}

/** 
 * @brief ISR common handler 
 * @param registers A pointer to the interrupt registers structure
 */
void isr_handler(struct itr_registers *registers)
{
  itr_handler_t handler = isr[registers->int_no];
  if (handler)
    handler(registers);
  else
  {
    log_error("ISR: Unhandled exception int_no = %d, message = %s\n", registers->int_no, isr_msg[registers->int_no]);
    sys_panic("ISR: Unhandled exception", registers);
  }
}
