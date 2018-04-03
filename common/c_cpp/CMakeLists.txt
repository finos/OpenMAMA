cmake_minimum_required (VERSION 2.8)
project (libwombatcommon)

if(WIN32)
	add_definitions(-DCOMMON_DLL -DYY_NO_UNISTD_H)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
endif()

add_subdirectory (src/c)
add_subdirectory (src/cpp)
