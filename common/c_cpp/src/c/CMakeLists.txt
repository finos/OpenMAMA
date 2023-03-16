project (libwombatcommonc)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	wombat
	${system}
	${system}/wombat)

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
add_library(wombatcommonstatic STATIC ${sources})
if(WIN32)
	target_link_libraries(wombatcommon ws2_32 advapi32 kernel32 Pdh shlwapi rpcrt4)
	target_link_libraries(wombatcommonstatic ws2_32 advapi32 kernel32 Pdh shlwapi rpcrt4)
else()
	include(FindUUID)
	find_package(UUID REQUIRED)
	target_link_libraries(wombatcommon ${UUID_LIBRARIES})
	target_link_libraries(wombatcommonstatic ${UUID_LIBRARIES})
endif()

set_target_properties(wombatcommon PROPERTIES OUTPUT_NAME "wombatcommon${OPENMAMA_LIBRARY_SUFFIX}")
set_target_properties(wombatcommonstatic PROPERTIES OUTPUT_NAME "wombatcommon${OPENMAMA_LIBRARY_STATIC_SUFFIX}")
install(TARGETS wombatcommon wombatcommonstatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

file(GLOB wombat_inst_includes wombat/*.h ${system}/wombat/*.h)
install(FILES ${wombat_inst_includes}
	DESTINATION include/wombat)

if(WIN32)
    file(GLOB wombat_platform_inst_includes wombat/windows/*.h)
    install(FILES ${wombat_platform_inst_includes}
        DESTINATION include/wombat/windows)
endif()

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:wombatcommon> DESTINATION bin OPTIONAL)
endif()
