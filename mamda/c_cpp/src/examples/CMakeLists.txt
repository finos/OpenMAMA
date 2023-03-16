project (mamda_examples)

get_target_property(MAMDA_INCLUDES mamda INCLUDE_DIRECTORIES)
get_target_property(MAMDA_OPTIONS_INCLUDES mamdaoptions INCLUDE_DIRECTORIES)

include_directories(
	${MAMDA_INCLUDES}
	${MAMDA_OPTIONS_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-DMAMDAOPT -DMAMDA_DLL)

add_library(mamdaexshared OBJECT
	dictrequester.cpp
	parsecmd.cpp
)

# usage mamda_example(newsticker mamdanews) to link with mamda AND mamdanews
macro(mamda_example name)
	add_executable(${name} ${name}.cpp $<TARGET_OBJECTS:mamdaexshared>)
	target_link_libraries(${name} mamda ${ARGN})
    install(TARGETS ${name}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
	if (WIN32 AND MSVC)
		install(FILES $<TARGET_PDB_FILE:${name}> DESTINATION bin OPTIONAL)
	endif()
endmacro()

mamda_example(auctionticker)
mamda_example(comboticker)
mamda_example(fundamentallisten)
mamda_example(mamdalisten)
# Was disabled before so disabling again - should find out why
# mamda_example(mamdapublisher)
mamda_example(multipartticker)
mamda_example(multisecurityticker)
mamda_example(optionchainer mamdaoptions)
mamda_example(optionview mamdaoptions)
mamda_example(quoteticker)
mamda_example(secstatuslisten)
mamda_example(tradeticker)

if (NOT WIN32)
    mamda_example(currencyticker)
    mamda_example(orderimbalanceticker)
endif()

add_subdirectory (news)
add_subdirectory (orderbooks)

install(DIRECTORY . DESTINATION examples/mamda/cpp
        PATTERN CMakeLists.txt EXCLUDE
		PATTERN *.vcxproj EXCLUDE
		PATTERN SConscript* EXCLUDE)

if (WIN32 AND MSVC)
    install(DIRECTORY . DESTINATION examples/mamda/cpp
		    PATTERN *.vcxproj)
endif()