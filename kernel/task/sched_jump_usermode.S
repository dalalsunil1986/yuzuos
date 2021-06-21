.section .text
.align 4
.globl sched_jump_usermode
sched_jump_usermode:
	cli

	mov $0x23, %ax
	mov %ax, %ds
	mov %ax, %es 
	mov %ax, %fs 
	mov %ax, %gs

	movl 4(%esp), %eax

	subl $4, %eax
	movl 12(%esp), %ebx
	movl %ebx, (%eax)

	movl 8(%esp), %ebx

	push $0x23
	pushl %eax
	pushf

	popl %eax
	or $0x200, %eax
	pushl %eax

  push $0x1B
	pushl %ebx
	iret
