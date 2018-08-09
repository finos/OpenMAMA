cmake_minimum_required (VERSION 2.8)
project (libwombatcommonc)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	wombat
	${system}
	${system}/wombat)

find_package(FLEX)
FLEX_TARGET(properties properties.l  ${CMAKE_CURRENT_BINARY_DIR}/properties.c)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${system}-files) # SYSTEM_SOURCES initialized here

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

add_library(wombatcommon SHARED ${sources})
if(WIN32)
	target_link_libraries(wombatcommon ws2_32 advapi32 kernel32 Pdh shlwapi)
endif()

file(GLOB wombat_inst_includes wombat/*.h ${system}/wombat/*.h)
list(APPEND inst_includes
     destroyHandle.h
     list.h
     lookup2.h
     platform.h
     property.h
     wlock.h
     timers.h)

set_target_properties(wombatcommon PROPERTIES OUTPUT_NAME "wombatcommon${OPENMAMA_LIBRARY_SUFFIX}")
install(TARGETS wombatcommon
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES ${wombat_inst_includes}
	DESTINATION include/wombat)

install(FILES ${inst_includes}
	DESTINATION include)

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:wombatcommon> DESTINATION bin OPTIONAL)
endif()
