cmake_minimum_required (VERSION 3.10)
project ("01-quickstart")

if (DEFINED ENV{OPENMAMA_ROOT})
  set (OPENMAMA_ROOT_DEFAULT $ENV{OPENMAMA_ROOT})
else ()
  set (OPENMAMA_ROOT_DEFAULT "/opt/openmama")
endif ()

set (OPENMAMA_ROOT ${OPENMAMA_ROOT_DEFAULT} CACHE PATH "Location of the OpenMAMA installation to use")

include_directories(${CMAKE_SOURCE_DIR}/mama/c_cpp/src/c)
include_directories(${CMAKE_SOURCE_DIR}/mama/c_cpp/src/cpp)
include_directories(${CMAKE_SOURCE_DIR}/common/c_cpp/src/c)
include_directories(${OPENMAMA_ROOT}/include)
link_directories(${OPENMAMA_ROOT}/lib)

add_executable(quickstartcpp quickstart.cpp)
target_link_libraries(quickstartcpp mamacpp wombatcommon)
