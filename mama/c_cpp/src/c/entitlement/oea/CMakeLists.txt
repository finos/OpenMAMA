if(NOT DEFINED OEA_ROOT OR "${OEA_ROOT}" STREQUAL "")
        message(SEND_ERROR "OEA_ROOT is undefined")
endif()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
	${OEA_ROOT}/include/oea
)

add_definitions(-DBRIDGE)

add_library(mamaentoea
            SHARED oea.c
)
target_link_libraries(mamaentoea mama wombatcommon)
set_target_properties(mamaentoea PROPERTIES OUTPUT_NAME "mamaentoea${OPENMAMA_LIBRARY_SUFFIX}")

if (MSVC)
    install(FILES $<TARGET_PDB_FILE:mamaentoea> DESTINATION bin OPTIONAL)
endif()
install(TARGETS mamaentoea
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib)
