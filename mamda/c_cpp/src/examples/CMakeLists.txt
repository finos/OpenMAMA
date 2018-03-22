cmake_minimum_required (VERSION 2.8.3)
project (mamda_examples)

get_target_property(MAMDA_INCLUDES mamda INCLUDE_DIRECTORIES)
get_target_property(MAMDA_OPTIONS_INCLUDES mamdaoptions INCLUDE_DIRECTORIES)

include_directories(
	${MAMDA_INCLUDES}
	${MAMDA_OPTIONS_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(mamdaexshared OBJECT
	dictrequester.cpp
	parsecmd.cpp
)

# usage mamda_example(newsticker mamdanews) to link with mamda AND mamdanews
macro(mamda_example name)
	add_executable(${name} ${name}.cpp $<TARGET_OBJECTS:mamdaexshared>)
	target_link_libraries(${name} mamda ${ARGN})
endmacro()

mamda_example(auctionticker)
mamda_example(comboticker)
mamda_example(currencyticker)
mamda_example(fundamentallisten)
mamda_example(mamdalisten)
mamda_example(mamdapublisher)
mamda_example(multipartticker)
mamda_example(multisecurityticker)
mamda_example(optionchainer mamdaoptions)
mamda_example(optionview mamdaoptions)
mamda_example(orderimbalanceticker)
mamda_example(quoteticker)
mamda_example(secstatuslisten)

add_subdirectory (news)
add_subdirectory (orderbooks)
