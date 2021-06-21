#define ASM_FILE
#include <kernel/boot/multiboot.h>
#include <kernel/memory/virtual.h>

.section .multiboot
.align 4
multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

#ifndef __ELF__
	.long multiboot_header
	.long _start
	.long _edata
	.long _end
	.long _start
#else
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0       
#endif
	.long 0
	.long 1024
	.long 768
	.long 32
 
.section .bss
.align 16
stack_bottom:
	.skip 16384
stack_top:

.section .data
.align 0x1000
boot_page_dir:
	.long 0x00000083
		
	.rept (PAGE_NUMBER - 1)
		.long 0
	.endr

  .long 0x00000083

	.rept (1024 - PAGE_NUMBER - 1)
		.long 0
	.endr
 
.section .text
.global _start
.type _start, @function
_start:
	movl $(boot_page_dir - KERNEL_VIRT_ADDR), %ecx
	movl %ecx, %cr3

	movl %cr4, %ecx
	orl $0x00000010, %ecx
	movl %ecx, %cr4

	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0

	lea higher_half, %ecx
	jmp *%ecx

higher_half:
	movl $0, (boot_page_dir)
	invlpg (0)

	movl %cr3, %ecx
	movl %ecx, %cr3

	movl $stack_top, %esp

	pushl %ebx
	pushl %eax

	call arch_main
 
	cli

loop:	hlt
	jmp loop
 
.size _start, . - _start
