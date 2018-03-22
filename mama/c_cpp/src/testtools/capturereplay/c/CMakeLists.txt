cmake_minimum_required (VERSION 2.8.3)
project (capturereplayc)

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMA_INCLUDES}
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../../../../cmake)
include(mama-common)

mamac_binary(capturec)
mamac_binary(captureconvert)
mamac_binary(capturereplayc)

