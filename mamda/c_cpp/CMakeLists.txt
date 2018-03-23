cmake_minimum_required (VERSION 2.8.3)
project (mamda)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake)
include(mama-common)

get_component_version(${CMAKE_CURRENT_LIST_DIR}/.. MAMDA)

add_subdirectory (src/cpp)

if (WITH_EXAMPLES)
    add_subdirectory (src/examples)
endif()

if (WITH_TESTTOOLS)
    add_subdirectory (src/testtools)
endif()
