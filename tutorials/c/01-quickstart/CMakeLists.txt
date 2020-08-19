cmake_minimum_required (VERSION 2.8.7)
project ("01-quickstart")

if(NOT MAMA_ROOT)
    message(FATAL_ERROR "Please provide -DMAMA_ROOT=/path/to/mama/installation")
endif()

include_directories(${MAMA_ROOT}/include)
link_directories(${MAMA_ROOT}/lib)

add_executable(quickstart quickstart.c)
target_link_libraries(quickstart mama wombatcommon)
