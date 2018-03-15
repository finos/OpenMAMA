cmake_minimum_required (VERSION 2.8.3)
project (perftools_cpp)

get_target_property(MAMACPP_INCLUDES mamacpp INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMACPP_INCLUDES}
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../../../../cmake)
include(mama-common)

mamacpp_binary(mamapingpong_replycpp)
mamacpp_binary(mamapingpong_requestercpp)
