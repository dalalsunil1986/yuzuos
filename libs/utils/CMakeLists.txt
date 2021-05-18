set(SOURCES
  log.c
  ini.c)

yuzuos_lib(libutils utils)
target_link_libraries(libutils libc)
