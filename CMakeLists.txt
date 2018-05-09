cmake_minimum_required (VERSION 2.8)
project (OpenmMAMA)

message(WARNING "The CMake build system for OpenMAMA is experimental. See https://openmama.github.io/openmama_build_instructions.html for officially supported build instructions.")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${CMAKE_C_COMPILER_ID})

add_subdirectory (common)
add_subdirectory (mama)
add_subdirectory (mamda)
