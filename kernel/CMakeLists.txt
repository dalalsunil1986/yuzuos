set(ASM_SOURCES
  boot/boot.S
  
  memory/gdt_flush.S
  memory/page_enable.S
  
  interrupts/idt_flush.S
  interrupts/irq.S
  interrupts/isr.S)

set(C_SOURCES
  init.c
  
  utils/log.c
  utils/stdio.c
  utils/bitmap.c
  utils/string.c
  
  memory/gdt.c
  memory/physical.c
  memory/virtual.c
  
  interrupts/idt.c
  interrupts/pic.c
  interrupts/irq.c
  interrupts/isr.c
  
  boot/multiboot.c
  
  system/sys.c
  system/syscall.c
  
  drivers/pit.c
  drivers/rtc.c
  drivers/keyboard.c
  drivers/mouse.c
  drivers/vga.c)

set(SOURCES ${ASM_SOURCES} ${C_SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -O2 -nostdlib -std=gnu99")

include_directories(include)

add_link_options(LINKER: -T ${CMAKE_CURRENT_SOURCE_DIR}/boot/linker.ld)
add_executable(kernel ${SOURCES})
target_link_libraries(kernel gcc)
install(TARGETS kernel RUNTIME DESTINATION boot)
