list(APPEND CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake
)

include(FindProton)
find_package(Proton REQUIRED)

include_directories(${PROTON_INCLUDE_DIR})
include_directories(.)
include_directories(../..)
include_directories(../../../../../../common/c_cpp/src/c)
include_directories(../../../../../../common/c_cpp/src/c/linux)

add_definitions(-DBRIDGE -DMAMA_DLL -DOPENMAMA_INTEGRATION)

if(WIN32)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(MAMA_LIB_SUFFIX "mdd")
        set(QPID_PROTON_LIB_SUFFIX "d")
    else()
        set(MAMA_LIB_SUFFIX "md")
        set(QPID_PROTON_LIB_SUFFIX "")
    endif()
else()
    set(MAMA_LIB_SUFFIX "")
    set(QPID_PROTON_LIB_SUFFIX "")
endif()

add_library(mamaqpidimpl${MAMA_LIB_SUFFIX}
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
    target_link_libraries(mamaqpidimpl${MAMA_LIB_SUFFIX}
                          libwombatcommon${MAMA_LIB_SUFFIX}
                          libmamac${MAMA_LIB_SUFFIX}
                          libqpid-proton${QPID_PROTON_LIB_SUFFIX}
                          event
                          uuid
                          Ws2_32)

    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set_target_properties(mamaqpidimpl${MAMA_LIB_SUFFIX} PROPERTIES PREFIX "lib")

    # Windows Targets
    install(TARGETS mamaqpidimpl${MAMA_LIB_SUFFIX}
            CONFIGURATIONS Release
            DESTINATION bin/dynamic)
    install(TARGETS mamaqpidimpl${MAMA_LIB_SUFFIX}
            CONFIGURATIONS Debug
            DESTINATION bin/dynamic-debug)
elseif(UNIX)
    target_link_libraries(mamaqpidimpl${MAMA_LIB_SUFFIX}
                          wombatcommon
                          mama
                          ${PROTON_LIBRARIES}
                          uuid)
    install(TARGETS mamaqpidimpl${MAMA_LIB_SUFFIX} DESTINATION lib)
endif()
