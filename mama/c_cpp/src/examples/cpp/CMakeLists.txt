cmake_minimum_required (VERSION 2.8)
project (mamacpp_examples)

get_target_property(MAMACPP_INCLUDES mamacpp INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMACPP_INCLUDES}
)

mamacpp_binary(mamaentitlecpp)
mamacpp_binary(mamaftmembercpp)
mamacpp_binary(mamainboxcpp)
mamacpp_binary(mamaiocpp)
mamacpp_binary(mamalistencachedcpp)
mamacpp_binary(mamalistencpp)
mamacpp_binary(mamamsgpublishercpp)
mamacpp_binary(mamaproxycpp)
mamacpp_binary(mamapublishercpp)
mamacpp_binary(mamasubscribercpp)
mamacpp_binary(mamasymbollistsubscribercpp)

