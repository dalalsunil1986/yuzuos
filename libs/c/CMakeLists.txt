set(ASM_SOURCES
  crti.S
  crtn.S)

set(C_SOURCES
  stdlib.c
  fcntl.c
  errno.c
  stdio.c
  string.c
  unistd.c
  
  sys/stat.c)

set(SOURCES ${ASM_SOURCES} ${C_SOURCES})

yuzuos_libc(libc c)

add_library(crt0 STATIC main.c)
add_custom_command(
  TARGET crt0
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/usr/lib/crt0.o)
target_include_directories(crt0 PUBLIC include)
