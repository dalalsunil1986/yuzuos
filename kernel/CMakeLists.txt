set(ASM_SOURCES
  memory/gdt_flush.S
  memory/page_enable.S
  memory/tbl_flush_entry.S
  
  task/sched_switch.S
  task/tss_flush.S
  task/sched_jump_usermode.S
  task/sched_return_usermode.S)

set(C_SOURCES
  init/main.c
  
  utils/log.c
  utils/stdio.c
  utils/bitmap.c
  utils/string.c
  utils/stdlib.c
  utils/dlist.c
  utils/plist.c
  
  memory/gdt.c
  memory/physical.c
  memory/virtual.c
  memory/mmap.c
  
  system/sys.c
  system/syscall.c
  
  drivers/pit.c
  drivers/rtc.c
  drivers/keyboard.c
  drivers/mouse.c
  drivers/vga.c
  drivers/pci.c
  drivers/ide.c
  
  filesystem/virtual.c
  filesystem/ext2.c
  
  task/scheduler.c
  task/tss.c
  task/elf32.c)

add_subdirectory(arch)

set(SOURCES ${ASM_SOURCES} ${C_SOURCES} ${ARCH_SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -O2 -nostdlib")

add_executable(kernel ${SOURCES})
target_link_libraries(kernel gcc)
target_include_directories(kernel PRIVATE include arch/${ARCH}/include)
target_link_options(kernel PRIVATE LINKER: -T ${CMAKE_CURRENT_SOURCE_DIR}/arch/${ARCH}/linker.ld)
install(TARGETS kernel RUNTIME DESTINATION boot)
yuzuos_install_headers("ukernel")
