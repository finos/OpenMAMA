enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

list(APPEND CMAKE_MODULE_PATH
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake
)

include(${CMAKE_C_COMPILER_ID})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMA_INCLUDES}
)

macro(UnitTestC bin)
	add_executable(${bin}
		${ARGN}
		MainUnitTestC.cpp
	)
	target_link_libraries(${bin} ${GTEST_BOTH_LIBRARIES} mama)
	install(TARGETS ${bin}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endmacro()

UnitTestC(UnitTestMamaC
	inboxtest.cpp
	iotest.cpp
	mamainternaltest.cpp
	msgutils.cpp
	openclosetest.cpp
	publishertest.cpp
	queuetest.cpp
	dictionarytest.cpp
	subscriptiontest.cpp
	timertest.cpp
	transporttest.cpp
	fieldcache/fieldcachevectortest.cpp
	fieldcache/fieldcachelisttest.cpp
	fieldcache/fieldcachemaptest.cpp
	fieldcache/fieldcachefieldtest.cpp
	fieldcache/fieldcacheiteratortest.cpp
	fieldcache/fieldcacherecordtest.cpp
	fieldcache/fieldcachetest.cpp
)

UnitTestC(UnitTestMamaDateTimeC
	mamadatetime/datetimerangetest.cpp
	mamadatetime/datetimetest.cpp
)

UnitTestC(UnitTestMamaMsgC
	mamamsg/msgatomictests.cpp
	mamamsg/msgcompositetests.cpp
	mamamsg/msgfieldatomictests.cpp
	mamamsg/msgfieldcompositetests.cpp
	mamamsg/msgfieldvectortests.cpp
	mamamsg/msggeneraltests.cpp
	mamamsg/msgiterationtests.cpp
	mamamsg/msgstatustests.cpp
	mamamsg/msgvectortests.cpp
)

UnitTestC(UnitTestMamaPriceC
	mamaprice/pricegeneraltests.cpp
	mamaprice/pricerangetests.cpp
)

UnitTestC(UnitTestMamaMiddlewareC
	middleware/middlewareGeneralTests.cpp
	middleware/middlewareInboxTests.cpp
	middleware/middlewareIoTests.cpp
	middleware/middlewareMsgTests.cpp
	middleware/middlewarePublisherTests.cpp
	middleware/middlewareQueueTests.cpp
	middleware/middlewareSubscriptionTests.cpp
	middleware/middlewareTimerTests.cpp
	middleware/middlewareTransportTests.cpp
)

UnitTestC(UnitTestMamaPayloadC
	payload/fieldatomictests.cpp
	payload/fieldcompositetests.cpp
	payload/fieldvectortests.cpp
	payload/payloadatomictests.cpp
	payload/payloadcompositetests.cpp
	payload/payloadgeneraltests.cpp
	payload/payloadvectortests.cpp
)
