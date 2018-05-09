cmake_minimum_required (VERSION 2.8.3)
project (mamda)

include(mama-common)

get_component_version(${CMAKE_CURRENT_LIST_DIR}/. MAMDA)

if(WIN32)
	add_definitions(-DMAMDA_DLL)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MAP /NODEFAULTLIB:libc.lib")
endif()

add_subdirectory (c_cpp/src/cpp)

if (WITH_EXAMPLES)
    add_subdirectory (c_cpp/src/examples)
endif()

if (WITH_TESTTOOLS)
    add_subdirectory (c_cpp/src/testtools)
endif()

add_subdirectory (java)
