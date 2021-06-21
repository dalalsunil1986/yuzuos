.section .text
.align 4
.globl gdt_flush
gdt_flush:
  movl 4(%esp), %eax
  lgdt (%eax)
  # $0x08 is an pointer to the new code selector
  jmp $0x08, $flush

# Reload data segment registers
flush:
  # $0x10 is an pointer to the new data selector
  mov $0x10, %ax
  mov %ds, %ax
  mov %es, %ax
  mov %fs, %ax
  mov %gs, %ax
  mov %ss, %ax
  ret
