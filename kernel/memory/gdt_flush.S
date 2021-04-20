.section .text
.align 4
.globl gdt_flush
gdt_flush:
  movl 4(%esp), %eax
  lgdt (%eax)

  mov $0x10, %ax
  mov %ds, %ax
  mov %es, %ax
  mov %fs, %ax
  mov %gs, %ax
  mov %ss, %ax
  jmp $0x08, $flush

flush:
  ret
