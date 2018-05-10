cmake_minimum_required (VERSION 2.8)
project (wombatcommoncpp)

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

set_target_properties(wombatcommoncpp PROPERTIES OUTPUT_NAME "wombatcommoncpp${OPENMAMA_LIBRARY_SUFFIX}")
install(TARGETS wombatcommoncpp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES $<TARGET_PDB_FILE:wombatcommoncpp> DESTINATION bin OPTIONAL)
install(DIRECTORY wombat/ DESTINATION include/wombat)