cmake_minimum_required(VERSION 3.16)
project(YuzuOS C ASM)

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_STAGING_PREFIX ${CMAKE_SYSROOT}/stage)

set(QEMU ${CMAKE_BINARY_DIR}/tools/cross/bin/qemu-system-i386)

set(TOOLCHAIN ${CMAKE_BINARY_DIR}/tools/cross/bin/i686-elf)
set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_custom_target(toolchain
  COMMAND ${CMAKE_SOURCE_DIR}/tools/setup-toolchain.sh
  USES_TERMINAL)

add_custom_target(qemu
  COMMAND ${CMAKE_SOURCE_DIR}/tools/setup-qemu.sh
  USES_TERMINAL)

add_custom_target(run
  DEPENDS kernel/kernel
  COMMAND ${CMAKE_COMMAND} -E env "QEMU_BIN=${QEMU}" ${CMAKE_SOURCE_DIR}/tools/run.sh
  USES_TERMINAL)

add_subdirectory(kernel)
