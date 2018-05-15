cmake_minimum_required (VERSION 2.8.3)
project (libwombatcommon)

if(WIN32)
	add_definitions(-DCOMMON_DLL -DYY_NO_UNISTD_H)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
endif()

add_subdirectory (c_cpp/src/c)

if (WITH_CPP)
	add_subdirectory (c_cpp/src/cpp)
endif()

if (WITH_UNITTEST)
    add_subdirectory (c_cpp/src/gunittest/c)
endif ()