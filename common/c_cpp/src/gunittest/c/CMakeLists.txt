enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

list(APPEND CMAKE_MODULE_PATH
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake
)

include(${CMAKE_C_COMPILER_ID})

get_target_property(WOMBAT_INCLUDES wombatcommon INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${WOMBAT_INCLUDES}
	..
)

add_executable(UnitTestCommonC
	../MainUnitTestC.cpp
	../MainUnitTestC.h
	port.cpp
	queuetest.cpp
	strutilstest.cpp
	timertest.cpp
	utiltest.cpp
)

target_link_libraries(UnitTestCommonC ${GTEST_BOTH_LIBRARIES} wombatcommon)

install(TARGETS UnitTestCommonC
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)