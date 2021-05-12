.section .text
.align 4
.globl sched_switch
sched_switch:
  cli
  pusha

  movl 9 * 4(%esp), %eax
  movl %esp, (%eax)

  movl 10 * 4(%esp), %eax
  movl 11 * 4(%esp), %ebx
  
  movl %eax, %esp
  movl %ebx, %cr3

  popa
  sti
  ret
