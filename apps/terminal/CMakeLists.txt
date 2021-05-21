set(SOURCES
  main.c)

yuzuos_bin(terminal)
target_link_libraries(terminal libc)
