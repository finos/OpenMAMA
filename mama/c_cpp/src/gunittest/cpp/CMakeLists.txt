enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

get_target_property(MAMACPP_INCLUDES mamacpp INCLUDE_DIRECTORIES)

list(APPEND CMAKE_MODULE_PATH
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake
)

include(${CMAKE_C_COMPILER_ID})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMACPP_INCLUDES}
)

add_executable(UnitTestMamaCPP
	MainUnitTestCpp.cpp
	MamaDateTimeTest.cpp
	MamaMsgTest.cpp
	MamaOpenCloseTest.cpp
	MamaPriceTest.cpp
	MamaPublisherTest.cpp
	MamaSubscriptionTest.cpp
	MamaTimerTest.cpp
)
target_link_libraries(UnitTestMamaCPP ${GTEST_BOTH_LIBRARIES} mamacpp mama)

install(TARGETS UnitTestMamaCPP
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)