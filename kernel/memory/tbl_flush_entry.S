.section .text
.align 4
.globl tbl_flush_entry
tbl_flush_entry:
  movl 4(%esp), %eax
  invlpg (%eax)
  ret
