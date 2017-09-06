cmake_minimum_required (VERSION 2.8.3)
project (mama)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake)
include(mama-common)

get_component_version(${CMAKE_CURRENT_LIST_DIR}/.. MAMA)

add_subdirectory (src/c)
# add_subdirectory (src/cpp)
