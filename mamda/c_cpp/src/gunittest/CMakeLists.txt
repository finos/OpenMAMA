enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

get_target_property(MAMACPP_INCLUDES mamacpp INCLUDE_DIRECTORIES)
get_target_property(MAMDA_INCLUDES mamda INCLUDE_DIRECTORIES)
get_target_property(MAMDABOOK_INCLUDES mamdabook INCLUDE_DIRECTORIES)
get_target_property(MAMDAOPTIONS_INCLUDES mamdaoptions INCLUDE_DIRECTORIES)
get_target_property(MAMDANEWS_INCLUDES mamdanews INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMACPP_INCLUDES}
	${MAMDA_INCLUDES}
	${MAMDABOOK_INCLUDES}
	${MAMDAOPTIONS_INCLUDES}
	${MAMDANEWS_INCLUDES}
)

add_executable(UnitTestMamdaCPP
    orderbooks/MamdaOrderBookPriceLevelTests.cpp
    orderbooks/MamdaOrderBookEntryTests.cpp
    orderbooks/MamdaOrderBookPublisherTests.cpp
    orderImbalances/MamdaOrderImbalanceFieldTests.cpp
    quotes/MamdaQuoteListenerShortSaleTests.cpp
    quotes/MamdaQuoteCallBackTests.cpp
    quotes/MamdaQuoteFieldTests.cpp
    trades/MamdaTradeListenerTests.cpp
    trades/MamdaTradeListenerShortSaleTests.cpp
    trades/MamdaTradeListenerTradeIdTests.cpp
	trades/MamdaTradeSideTests.cpp
    trades/MamdaTradeCallBackTests.cpp
    common/MainUnitTest.cpp
    common/MamdaUnitTestUtils.cpp
    secstatus/MamdaSecStatusListenerShortSaleTests.cpp
)
target_link_libraries(UnitTestMamdaCPP ${GTEST_BOTH_LIBRARIES} mamdabook mamdanews mamdaoptions mamda mamacpp mama)

# Note that platform specific PATH / LD_LIBRARY_PATH is expected to already be set in calling environment
gtest_discover_tests(
    UnitTestMamdaCPP
    EXTRA_ARGS -m qpid -d "${CMAKE_SOURCE_DIR}/mamda/java/src/test/resources/data.dict"
    PROPERTIES ENVIRONMENT
        "WOMBAT_PATH=${CMAKE_SOURCE_DIR}/mama/c_cpp/src/examples:${CMAKE_SOURCE_DIR}/mama/c_cpp/src/gunittest/c"
)
