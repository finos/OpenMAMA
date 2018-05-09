include(FindProton)
find_package(Proton REQUIRED)

include(FindLibevent)
find_package(Libevent REQUIRED)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system)

include_directories(${PROTON_INCLUDE_DIR})
include_directories(${LIBEVENT_INCLUDE_DIRS})
include_directories(.)
include_directories(../..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../)
include_directories(../../../../../../common/c_cpp/src/c/${system})
include_directories(../../../../../../common/c_cpp/src/c)

add_definitions(-DBRIDGE)

add_library(mamaqpidimpl
            MODULE bridge.c
                   codec.c
                   codec.h
                   inbox.c
                   inbox.h
                   io.c
                   io.h
                   msg.c
                   msg.h
                   publisher.c
                   publisher.h
                   qpidcommon.c
                   qpidcommon.h
                   qpidbridgefunctions.h
                   qpiddefs.h
                   queue.c
                   subscription.c
                   subscription.h
                   timer.c
                   transport.c
                   transport.h)

if(WIN32)
    target_link_libraries(mamaqpidimpl
                          wombatcommon
                          mama
                          ${PROTON_LIBRARIES}
                          ${LIBEVENT_LIBRARIES}
                          uuid
                          Ws2_32)

    # Windows Targets
    install(FILES $<TARGET_PDB_FILE:mamaqpidimpl> DESTINATION bin OPTIONAL)
    install(TARGETS mamaqpidimpl
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
elseif(UNIX)
    target_link_libraries(mamaqpidimpl
                          wombatcommon
                          mama
                          ${PROTON_LIBRARIES}
                          uuid)
    install(TARGETS mamaqpidimpl DESTINATION lib)
endif()
