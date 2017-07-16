cmake_minimum_required (VERSION 2.8)
project (libwombatcommonc)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

include_directories(
	${PROJECT_SOURCE_DIR}
	wombat
	${system}
	${system}/wombat
)

find_package(FLEX)
FLEX_TARGET(properties properties.l  ${CMAKE_CURRENT_BINARY_DIR}/properties.c)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${system}-files)

set(sources
	destroyHandle.c
	fileparser.c
	fileutils.c
	libyywrap.c
	list.c
	lookup2.c
	machine.c
	memnode.c
	mempool.c
	MRSWLock.c
	property.c
	queue.c
	strutils.c
	thread.c
	timers.c
	wlock.c
	wMessageStats.c
	wtable.c
	${CMAKE_CURRENT_BINARY_DIR}/properties.c
)

list(APPEND sources ${SYSTEM_SOURCES})

add_library(wombatcommonc SHARED ${sources})
