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

add_library(wombatcommoncppstatic STATIC ${sources})
target_link_libraries(wombatcommoncppstatic wombatcommonstatic)

set_target_properties(wombatcommoncpp PROPERTIES OUTPUT_NAME "wombatcommoncpp${OPENMAMA_LIBRARY_SUFFIX}")
set_target_properties(wombatcommoncppstatic PROPERTIES OUTPUT_NAME "wombatcommoncpp${OPENMAMA_LIBRARY_STATIC_SUFFIX}")
install(TARGETS wombatcommoncpp wombatcommoncppstatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY wombat/ DESTINATION include/wombat)

if (WIN32 AND MSVC)
        install(FILES $<TARGET_PDB_FILE:wombatcommoncpp> DESTINATION bin OPTIONAL)
endif()
