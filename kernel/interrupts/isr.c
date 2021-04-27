#include <kernel/interrupts/isr.h>
#include <kernel/interrupts/idt.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <stddef.h>

static itr_handler_t isr[ISR_SIZE];
static const char *isr_msg[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr127();

void isr_handler_set(uint8_t index, itr_handler_t handler)
{
  sys_cli();

  isr[index] = handler;

  sys_sti();
}

void isr_handler_unset(uint8_t index)
{
  sys_cli();

  isr[index] = NULL;

  sys_sti();
}

void isr_handler(struct itr_registers *registers)
{
  sys_cli();

  itr_handler_t handler = isr[registers->int_no];
  if (handler)
    handler(registers);
  else
  {
    log_error("ISR: Unhandled exception int_no = %d, message = %s", registers->int_no, isr_msg[registers->int_no]);
    sys_stop();
  }

  sys_sti();
}

void isr_init()
{
  idt_entry_add(0, (uint32_t)isr0, 0x08, 0x8E);
  idt_entry_add(1, (uint32_t)isr1, 0x08, 0x8E);
  idt_entry_add(2, (uint32_t)isr2, 0x08, 0x8E);
  idt_entry_add(3, (uint32_t)isr3, 0x08, 0x8E);
  idt_entry_add(4, (uint32_t)isr4, 0x08, 0x8E);
  idt_entry_add(5, (uint32_t)isr5, 0x08, 0x8E);
  idt_entry_add(6, (uint32_t)isr6, 0x08, 0x8E);
  idt_entry_add(7, (uint32_t)isr7, 0x08, 0x8E);
  idt_entry_add(8, (uint32_t)isr8, 0x08, 0x8E);
  idt_entry_add(9, (uint32_t)isr9, 0x08, 0x8E);
  idt_entry_add(10, (uint32_t)isr10, 0x08, 0x8E);
  idt_entry_add(11, (uint32_t)isr11, 0x08, 0x8E);
  idt_entry_add(12, (uint32_t)isr12, 0x08, 0x8E);
  idt_entry_add(13, (uint32_t)isr13, 0x08, 0x8E);
  idt_entry_add(14, (uint32_t)isr14, 0x08, 0x8E);
  idt_entry_add(15, (uint32_t)isr15, 0x08, 0x8E);
  idt_entry_add(16, (uint32_t)isr16, 0x08, 0x8E);
  idt_entry_add(17, (uint32_t)isr17, 0x08, 0x8E);
  idt_entry_add(18, (uint32_t)isr18, 0x08, 0x8E);
  idt_entry_add(19, (uint32_t)isr19, 0x08, 0x8E);
  idt_entry_add(20, (uint32_t)isr20, 0x08, 0x8E);
  idt_entry_add(21, (uint32_t)isr21, 0x08, 0x8E);
  idt_entry_add(21, (uint32_t)isr22, 0x08, 0x8E);
  idt_entry_add(23, (uint32_t)isr23, 0x08, 0x8E);
  idt_entry_add(24, (uint32_t)isr24, 0x08, 0x8E);
  idt_entry_add(25, (uint32_t)isr25, 0x08, 0x8E);
  idt_entry_add(26, (uint32_t)isr26, 0x08, 0x8E);
  idt_entry_add(27, (uint32_t)isr27, 0x08, 0x8E);
  idt_entry_add(28, (uint32_t)isr28, 0x08, 0x8E);
  idt_entry_add(29, (uint32_t)isr29, 0x08, 0x8E);
  idt_entry_add(30, (uint32_t)isr30, 0x08, 0x8E);
  idt_entry_add(31, (uint32_t)isr31, 0x08, 0x8E);
  idt_entry_add(127, (uint32_t)isr127, 0x08, 0x8E);

  log_info("ISR: Initialized\n");
}
