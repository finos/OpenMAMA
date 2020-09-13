cmake_minimum_required (VERSION 2.8.7)
project ("01-quickstart")

if(NOT OPENMAMA_ROOT)
    message(FATAL_ERROR "Please provide -DOPENMAMA_ROOT=/path/to/openmama/install")
endif()

include_directories(${OPENMAMA_ROOT}/include)
link_directories(${OPENMAMA_ROOT}/lib)

add_executable(quickstart quickstart.cpp)
target_link_libraries(quickstart mamacpp wombatcommon)
