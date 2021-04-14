set(ASM_SOURCES
  boot/boot.S)

set(C_SOURCES
  init.c)

set(SOURCES ${ASM_SOURCES} ${C_SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -O2 -nostdlib -std=gnu99")

add_link_options(LINKER: -T ${CMAKE_CURRENT_SOURCE_DIR}/boot/linker.ld)
add_executable(kernel ${SOURCES})
target_link_libraries(kernel gcc)
