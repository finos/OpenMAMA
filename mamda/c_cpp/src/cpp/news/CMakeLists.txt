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

set_target_properties(mamdanews PROPERTIES OUTPUT_NAME "mamdanews${OPENMAMA_LIBRARY_SUFFIX}")

add_library(mamdanewsstatic STATIC ${sources})
target_link_libraries(mamdanewsstatic mamdastatic)

set_target_properties(mamdanewsstatic PROPERTIES OUTPUT_NAME "mamdanews${OPENMAMA_LIBRARY_STATIC_SUFFIX}")

install(TARGETS mamdanews mamdanewsstatic
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(DIRECTORY mamda/ DESTINATION include/mamda)

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamdanews> DESTINATION bin OPTIONAL)
endif()
