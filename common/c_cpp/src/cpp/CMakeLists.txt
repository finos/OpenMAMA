cmake_minimum_required (VERSION 2.8)
project (wombatcommoncpp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake)
include(mama-common)
include(${CMAKE_C_COMPILER_ID})

include_directories(
	wombat
	${WOMBAT_INCLUDES}
)

set(sources
	Lock.cpp
)

add_library(wombatcommoncpp SHARED ${sources})
