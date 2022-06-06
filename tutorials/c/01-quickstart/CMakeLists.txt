cmake_minimum_required (VERSION 3.10)
project ("01-quickstart")

if (DEFINED ENV{OPENMAMA_ROOT})
  set (OPENMAMA_ROOT_DEFAULT $ENV{OPENMAMA_ROOT})
else ()
  set (OPENMAMA_ROOT_DEFAULT "/opt/openmama")
endif ()

set (OPENMAMA_ROOT ${OPENMAMA_ROOT_DEFAULT} CACHE PATH "Location of the OpenMAMA installation to use")

include_directories(${OPENMAMA_ROOT}/include)
link_directories(${OPENMAMA_ROOT}/lib)

add_executable(quickstartc quickstart.c)
target_link_libraries(quickstartc mama wombatcommon)
