enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

get_target_property(MAMACPP_INCLUDES mamacpp INCLUDE_DIRECTORIES)

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

# Note that platform specific PATH / LD_LIBRARY_PATH is expected to already be set in calling environment
gtest_discover_tests(UnitTestMamaCPP
                         EXTRA_ARGS -m qpid -p qpidmsg -i Q
                         PROPERTIES ENVIRONMENT "WOMBAT_PATH=${CMAKE_SOURCE_DIR}/mama/c_cpp/src/examples:${CMAKE_BINARY_DIR}/mama/c_cpp/src/gunittest/c")

install(TARGETS UnitTestMamaCPP
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:UnitTestMamaCPP> DESTINATION bin OPTIONAL)
endif()