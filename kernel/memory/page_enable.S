.section .text
.align 4
.globl page_enable
page_enable:
  movl 4(%esp), %eax
  movl %eax, %cr3

  movl %cr4, %eax
  orl $0x00000010, %eax
  movl %eax, %cr4

  movl %cr0, %eax
  orl $0x80000000, %eax
  movl %eax, %cr0

  ret
