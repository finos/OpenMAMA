cmake_minimum_required (VERSION 2.8)
project (mamdanewscpp)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

set(sources
	MamdaNewsFields.cpp
	MamdaNewsHeadline.cpp
	MamdaNewsManager.cpp
	MamdaNewsStory.cpp
	MamdaNewsUtils.cpp
)

add_library(mamdanews SHARED ${sources})

install(TARGETS mamdanews
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)