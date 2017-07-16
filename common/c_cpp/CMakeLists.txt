cmake_minimum_required (VERSION 2.8)
project (libwombatcommon)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${CMAKE_C_COMPILER_ID})

add_subdirectory (src/c)

