if (NOT WIN32)
    include(FindUUID)
    find_package(UUID REQUIRED)
endif()

include(FindLibevent)
find_package(Libevent REQUIRED)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

include_directories(${LIBEVENT_INCLUDE_DIRS})
include_directories(.)
include_directories(../..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../)
include_directories(../../../../../../common/c_cpp/src/c/${system})
include_directories(../../../../../../common/c_cpp/src/c)

add_definitions(-DBRIDGE)

add_library(mamabaseimpl SHARED
        basedefs.h
        bridge.c
        inbox.c
        io.c
        msg.c
        publisher.c
        queue.c
        subscription.c
        timer.c
        transport.c
)

if(WIN32)
    target_link_libraries(mamabaseimpl
                          wombatcommon
                          mama
                          ${LIBEVENT_LIBRARIES}
                          uuid
                          Ws2_32)

    # Windows Targets
    set_target_properties(mamabaseimpl PROPERTIES OUTPUT_NAME "mamabaseimpl${OPENMAMA_LIBRARY_SUFFIX}")
    if (MSVC)
        install(FILES $<TARGET_PDB_FILE:mamabaseimpl> DESTINATION bin OPTIONAL)
    endif()
    install(TARGETS mamabaseimpl
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION lib)
elseif(UNIX)
    target_link_libraries(mamabaseimpl
                          wombatcommon
                          mama
                          ${LIBEVENT_LIBRARIES}
                          ${UUID_LIBRARIES})
    install(TARGETS mamabaseimpl DESTINATION lib)
endif()
