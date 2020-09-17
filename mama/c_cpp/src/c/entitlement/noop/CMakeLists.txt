string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

include_directories(.)
include_directories(../..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../)
include_directories(../../../../../../common/c_cpp/src/c/${system})
include_directories(../../../../../../common/c_cpp/src/c)

add_definitions(-DBRIDGE)

add_library(mamaentnoop
            SHARED noop.c
                   noop.h)
target_link_libraries(mamaentnoop mama wombatcommon)
set_target_properties(mamaentnoop PROPERTIES OUTPUT_NAME "mamaentnoop${OPENMAMA_LIBRARY_SUFFIX}")

if (MSVC)
    install(FILES $<TARGET_PDB_FILE:mamaentnoop> DESTINATION bin OPTIONAL)
    install(TARGETS mamaentnoop
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION lib)
else()
    install(TARGETS mamaentnoop DESTINATION lib)
endif()
