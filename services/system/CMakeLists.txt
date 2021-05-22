set(SOURCES
  main.c)

yuzuos_bin(system)
target_link_libraries(system libc libutils)
