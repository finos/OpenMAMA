cmake_minimum_required (VERSION 2.8)
project (wombatcommoncpp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake)
include(${CMAKE_C_COMPILER_ID})

get_target_property(WOMBAT_INCLUDES wombatcommon INCLUDE_DIRECTORIES)
include_directories(
	wombat
	${WOMBAT_INCLUDES}
)

set(sources
	Lock.cpp
)

add_library(wombatcommoncpp SHARED ${sources})
target_link_libraries(wombatcommoncpp wombatcommon)

install(TARGETS wombatcommoncpp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY wombat/ DESTINATION include/wombat)