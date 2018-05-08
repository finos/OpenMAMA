cmake_minimum_required (VERSION 2.8)
project (OpenmMAMA)

message(WARNING "The CMake build system for OpenMAMA is experimental. See https://openmama.github.io/openmama_build_instructions.html for officially supported build instructions.")

add_subdirectory (common)
add_subdirectory (mama)
add_subdirectory (mamda)
