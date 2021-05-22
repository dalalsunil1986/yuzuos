cmake_minimum_required(VERSION 3.16)
project(YuzuOS C ASM)

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(FATAL_ERROR "Don't use CMAKE_BUILD_TYPE when building")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_STAGING_PREFIX ${CMAKE_SYSROOT}/)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SYSROOT}/)

set(TOOLCHAIN ${CMAKE_BINARY_DIR}/tools/cross/bin/i686-yuzuos)
set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

function(yuzuos_install_headers path_name)
  install(DIRECTORY include/${path_name} DESTINATION usr/include)
endfunction()

function(yuzuos_bin bin_name)
  add_executable(${bin_name} ${SOURCES})
  install(TARGETS ${bin_name} RUNTIME DESTINATION bin)
endfunction()

function(yuzuos_lib lib_name bin_name)
  yuzuos_install_headers(${bin_name})
  add_library(${lib_name} ${SOURCES})
  install(TARGETS ${lib_name} DESTINATION usr/lib)
  set_target_properties(${lib_name} PROPERTIES OUTPUT_NAME ${bin_name})
endfunction()

add_custom_target(toolchain
  COMMAND ${CMAKE_COMMAND} -E env "SYSROOT_DIR=${CMAKE_SYSROOT}" "SOURCE_DIR=${CMAKE_SOURCE_DIR}" ${CMAKE_SOURCE_DIR}/tools/setup-toolchain.sh
  USES_TERMINAL)

add_custom_target(qemu
  COMMAND ${CMAKE_SOURCE_DIR}/tools/setup-qemu.sh
  USES_TERMINAL)

add_custom_target(image
  COMMAND ${CMAKE_COMMAND} -E env "SOURCE_DIR=${CMAKE_SOURCE_DIR}" ${CMAKE_SOURCE_DIR}/tools/setup-image.sh
  BYPRODUCTS ${CMAKE_BINARY_DIR}/qemu_img
  USES_TERMINAL)

add_custom_target(run
  DEPENDS qemu_img,kernel/kernel
  COMMAND ${CMAKE_SOURCE_DIR}/tools/run.sh
  USES_TERMINAL)

include_directories(kernel/include)
include_directories(libs/c/include)
include_directories(libs/utils/include)

add_subdirectory(kernel)
add_subdirectory(libs)
add_subdirectory(services)
add_subdirectory(apps)
