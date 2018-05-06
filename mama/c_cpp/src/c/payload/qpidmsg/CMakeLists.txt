list(APPEND CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake
)

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
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(MAMA_LIB_SUFFIX "mdd")
    else()
        set(MAMA_LIB_SUFFIX "md")
    endif()
    target_link_libraries(mamaqpidmsgimpl
                          ${PROTON_LIBRARY}
                          wombatcommon
                          mama)
    set_target_properties(mamaqpidmsgimpl PROPERTIES PREFIX "lib")

    # Windows Targets
    install(TARGETS mamaqpidmsgimpl
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)

elseif(UNIX)
    target_link_libraries(mamaqpidmsgimpl wombatcommon mama ${PROTON_LIBRARIES})
    install(TARGETS mamaqpidmsgimpl DESTINATION lib)
endif()
