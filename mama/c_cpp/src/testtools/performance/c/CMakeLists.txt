cmake_minimum_required (VERSION 2.8.3)
project (perftools_c)

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMA_INCLUDES}
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../../../../cmake)
include(mama-common)

mamac_binary(mamaconsumerc)
mamac_binary(mamaconsumerc_v2)
mamac_binary(mamapingpongc)
mamac_binary(mamaproducerc)
mamac_binary(mamaproducerc_v2)
