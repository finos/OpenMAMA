cmake_minimum_required (VERSION 2.8.3)
project (mamda_testtools)

get_target_property(MAMDA_INCLUDES mamda INCLUDE_DIRECTORIES)
get_target_property(MAMDABOOK_INCLUDES mamdabook INCLUDE_DIRECTORIES)

include_directories(
	${MAMDA_INCLUDES}
	${MAMDABOOK_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
)

macro(mamda_tool name)
	add_executable(${name} ${name}.cpp $<TARGET_OBJECTS:mamdaexshared>)
	target_link_libraries(${name} mamda ${ARGN})
endmacro()

mamda_tool(bookselftest mamdabook)
mamda_tool(quoteselftest)
mamda_tool(tradeselftest)
