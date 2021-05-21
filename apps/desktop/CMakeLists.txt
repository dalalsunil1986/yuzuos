set(SOURCES
  main.c)

yuzuos_bin(desktop)
target_link_libraries(desktop libc)
