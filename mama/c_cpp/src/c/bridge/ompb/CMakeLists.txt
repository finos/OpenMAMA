string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

set(BRIDGE_STUB_NAME ompb)

include_directories(.)
include_directories(../..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../)
include_directories(../../../../../../common/c_cpp/src/c/${system})
include_directories(../../../../../../common/c_cpp/src/c)

add_definitions(-DBRIDGE -DBRIDGE_STUB_NAME=${BRIDGE_STUB_NAME})

add_library(mama${BRIDGE_STUB_NAME}impl SHARED
        bridgefunctions.h
        bridge.c
        publisher.c
        subscription.c
        transport.c
        transport.h
)

if(WIN32)
    target_link_libraries(mama${BRIDGE_STUB_NAME}impl
                          wombatcommon
                          mama)

    # Windows Targets
    set_target_properties(mama${BRIDGE_STUB_NAME}impl PROPERTIES OUTPUT_NAME "mama${BRIDGE_STUB_NAME}impl${OPENMAMA_LIBRARY_SUFFIX}")
    if (MSVC)
        install(FILES $<TARGET_PDB_FILE:mama${BRIDGE_STUB_NAME}impl> DESTINATION bin OPTIONAL)
    endif()
    install(TARGETS mama${BRIDGE_STUB_NAME}impl
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION lib)
elseif(UNIX)
    target_link_libraries(mama${BRIDGE_STUB_NAME}impl
                          wombatcommon
                          mama)
    install(TARGETS mama${BRIDGE_STUB_NAME}impl DESTINATION lib)
endif()
