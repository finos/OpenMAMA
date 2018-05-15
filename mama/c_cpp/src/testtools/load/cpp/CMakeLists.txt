cmake_minimum_required (VERSION 2.8.3)
project (loadtools_cpp)

get_target_property(MAMACPP_INCLUDES mamacpp INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMACPP_INCLUDES}
)

include(mama-common)

mamacpp_binary(mamachurncpp)
