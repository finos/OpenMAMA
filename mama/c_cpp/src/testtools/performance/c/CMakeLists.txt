project (perftools_c)

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMA_INCLUDES}
)

if (NOT WIN32)
    add_definitions(-Wno-deprecated-declarations -Wno-unused-result)
endif()

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
   set(RTLIB "rt")
endif()

mamac_binary(mamaconsumerc ${RTLIB})
mamac_binary(mamaconsumerc_v2 ${RTLIB})
mamac_binary(mamapingpongc ${RTLIB})
mamac_binary(mamaproducerc ${RTLIB})
mamac_binary(mamaproducerc_v2 ${RTLIB})
