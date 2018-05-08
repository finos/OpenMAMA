cmake_minimum_required (VERSION 2.8.3)
project (mama)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake ${CMAKE_CURRENT_LIST_DIR}/c_cpp/cmake)
include(mama-common)

# Common flag setup
if(WIN32)
	add_definitions(-DMAMA_DLL)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libc.lib")
endif()
add_definitions(-DOPENMAMA_INTEGRATION)

get_component_version(${CMAKE_CURRENT_LIST_DIR} MAMA)

add_subdirectory (c_cpp/src/c)
add_subdirectory (c_cpp/src/cpp)

if (WITH_EXAMPLES)
    add_subdirectory (c_cpp/src/examples/c)
    add_subdirectory (c_cpp/src/examples/cpp)
endif ()

if (WITH_TESTTOOLS)
    add_subdirectory (c_cpp/src/testtools/capturereplay/c)
    add_subdirectory (c_cpp/src/testtools/load/c)
    add_subdirectory (c_cpp/src/testtools/load/cpp)
    if (!WIN32)
        add_subdirectory (c_cpp/src/testtools/performance/c)
        add_subdirectory (c_cpp/src/testtools/performance/cpp)
    endif ()
endif ()

if (WITH_UNITTEST)
    add_subdirectory (c_cpp/src/gunittest/c)
    add_subdirectory (c_cpp/src/gunittest/cpp)
endif ()

if (WITH_QPID)
    add_subdirectory (c_cpp/src/c/bridge/qpid)
    add_subdirectory (c_cpp/src/c/payload/qpidmsg)
endif ()

add_subdirectory (jni/src)
