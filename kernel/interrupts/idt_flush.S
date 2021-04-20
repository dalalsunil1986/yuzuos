.section .text
.align 4
.globl idt_flush
idt_flush:
  movl 4(%esp), %eax
  lidt (%eax)
  ret
