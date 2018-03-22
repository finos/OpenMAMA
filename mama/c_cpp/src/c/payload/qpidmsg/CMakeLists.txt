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
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(MAMA_LIB_SUFFIX "mdd")
    else()
        set(MAMA_LIB_SUFFIX "md")
    endif()
    target_link_libraries(mamaqpidmsgimpl
                          libwombatcommon${MAMA_LIB_SUFFIX}
                          libmamac${MAMA_LIB_SUFFIX})
    set_target_properties(mamaqpidmsgimpl PROPERTIES PREFIX "lib")

    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # Windows Targets
    install(TARGETS mamaqpidmsgimpl
            CONFIGURATIONS Release
            DESTINATION bin/dynamic)
    install(TARGETS mamaqpidmsgimpl
            CONFIGURATIONS Debug
            DESTINATION bin/dynamic-debug)

elseif(UNIX)
    target_link_libraries(mamaqpidmsgimpl wombatcommon mama ${PROTON_LIBRARIES})
    install(TARGETS mamaqpidmsgimpl DESTINATION lib)
endif()
