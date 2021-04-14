cmake_minimum_required(VERSION 3.16)
project(YuzuOS C ASM)

add_custom_target(toolchain
  COMMAND ${CMAKE_SOURCE_DIR}/tools/setup-toolchain.sh
  USES_TERMINAL)
