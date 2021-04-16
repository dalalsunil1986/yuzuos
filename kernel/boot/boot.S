#include <kernel/boot/multiboot.h>

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
 
.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp

	call kernel_init
 
	cli

loop:	hlt
	jmp loop
 
.size _start, . - _start
