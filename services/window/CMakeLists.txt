set(SOURCES
  main.c)

yuzuos_bin(window)
target_link_libraries(window libc)
