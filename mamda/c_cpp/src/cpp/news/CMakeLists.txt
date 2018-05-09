cmake_minimum_required (VERSION 2.8)
project (mamdanewscpp)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-DMAMDAOPT)
set(sources
	MamdaNewsFields.cpp
	MamdaNewsHeadline.cpp
	MamdaNewsManager.cpp
	MamdaNewsStory.cpp
	MamdaNewsUtils.cpp
)

add_library(mamdanews SHARED ${sources})
target_link_libraries(mamdanews mamda)

install(TARGETS mamdanews
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(FILES $<TARGET_PDB_FILE:mamdanews> DESTINATION bin OPTIONAL)