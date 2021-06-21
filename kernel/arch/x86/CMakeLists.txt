set(C_SOURCES
  boot/main.c
  boot/multiboot.c
  boot/serial.c
  boot/idt.c
  
  kernel/isr.c
  kernel/irq.c
  kernel/pic.c)

set(ASM_SOURCES
  boot/boot.S
  boot/idt_flush.S
  
  kernel/isr_common.S
  kernel/irq_common.S)

list(TRANSFORM C_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
list(TRANSFORM ASM_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

set(SOURCES ${C_SOURCES} ${ASM_SOURCES} PARENT_SCOPE)
