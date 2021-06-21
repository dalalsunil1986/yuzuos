set(SOURCES
  log.c
  ini.c)

yuzuos_lib(libcore core)
target_link_libraries(libcore libc)
