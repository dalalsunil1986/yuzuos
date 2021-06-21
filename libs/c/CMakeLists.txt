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

yuzuos_install_headers("")
add_library(libc ${SOURCES})
install(TARGETS libc ARCHIVE DESTINATION usr/lib)
set_target_properties(libc PROPERTIES OUTPUT_NAME c)
target_link_directories(libc PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(libc libcore)

add_library(crt0 STATIC main.c)
add_custom_command(
  TARGET crt0
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/usr/lib/crt0.o)
