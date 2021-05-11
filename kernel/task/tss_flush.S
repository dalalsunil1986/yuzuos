.section .text
.align 4
.globl tss_flush
tss_flush:
  mov $0x2B, %ax
	ltr %ax
	ret
