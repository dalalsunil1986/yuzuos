set(ASM_SOURCES
  crti.S
  crtn.S)

set(C_SOURCES
  main.c)

set(SOURCES ${ASM_SOURCES} ${C_SOURCES})

yuzuos_install_headers()
add_library(libc ${SOURCES})
install(TARGETS libc ARCHIVE DESTINATION usr/lib)
set_target_properties(libc PROPERTIES OUTPUT_NAME c)
target_link_directories(libc PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
