include(FindProton)
find_package(Proton REQUIRED)
string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

include_directories(${PROTON_INCLUDE_DIR})
include_directories(.)
include_directories(../..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories(../../../../../../common/c_cpp/src/c/${system})
include_directories(../../../../../../common/c_cpp/src/c)

add_definitions(-DBRIDGE)

add_library(mamaqpidmsgimpl
            MODULE field.c
                   field.h
                   iterator.c
                   payload.c
                   payload.h
                   qpidcommon.c
                   qpidcommon.h
                   qpidpayloadfunctions.h)

if(WIN32)
    target_link_libraries(mamaqpidmsgimpl
                          ${PROTON_LIBRARIES}
                          wombatcommon
                          mama)

    # Windows Targets
    set_target_properties(mamaqpidmsgimpl PROPERTIES OUTPUT_NAME "mamaqpidmsgimpl${OPENMAMA_LIBRARY_SUFFIX}")
    if (MSVC)
        install(FILES $<TARGET_PDB_FILE:mamaqpidmsgimpl>
                DESTINATION bin
                OPTIONAL)
    endif()
    install(TARGETS mamaqpidmsgimpl
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION lib)

elseif(UNIX)
    target_link_libraries(mamaqpidmsgimpl wombatcommon mama ${PROTON_LIBRARIES})
    install(TARGETS mamaqpidmsgimpl DESTINATION lib)
endif()
