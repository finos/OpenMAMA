string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

include_directories(.)
include_directories(..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../)
include_directories(../../../../../common/c_cpp/src/c/${system})
include_directories(../../../../../common/c_cpp/src/c)

add_definitions(-DBRIDGE)

add_library(mamaplugindqstrategy
            SHARED dqstrategyplugin.c
                   dqstrategyplugin.h)
target_link_libraries(mamaplugindqstrategy mama)
set_target_properties(mamaplugindqstrategy PROPERTIES OUTPUT_NAME "mamaplugindqstrategy${OPENMAMA_LIBRARY_SUFFIX}")

if (MSVC)
    install(FILES $<TARGET_PDB_FILE:mamaplugindqstrategy> DESTINATION bin OPTIONAL)
    install(TARGETS mamaplugindqstrategy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib)
else ()
    install(TARGETS mamaplugindqstrategy
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
endif ()
