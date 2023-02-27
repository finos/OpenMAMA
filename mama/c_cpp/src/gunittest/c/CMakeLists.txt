enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMA_INCLUDES}
)

add_executable(UnitTestMamaC
	MainUnitTestC.cpp
	inboxtest.cpp
	iotest.cpp
	mamainternaltest.cpp
	msgutils.cpp
	openclosetest.cpp
	publishertest.cpp
	queuetest.cpp
	dictionarytest.cpp
	regexchannelfiltertest.cpp
	resourcepooltest.cpp
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
	mamadatetime/datetimerangetest.cpp
	mamadatetime/datetimetest.cpp
	mamamsg/msgatomictests.cpp
	mamamsg/msgcompositetests.cpp
	mamamsg/msgfieldatomictests.cpp
	mamamsg/msgfieldcompositetests.cpp
	mamamsg/msgfieldvectortests.cpp
	mamamsg/msggeneraltests.cpp
	mamamsg/msgiterationtests.cpp
	mamamsg/msgstatustests.cpp
	mamamsg/msgvectortests.cpp
	mamaprice/pricegeneraltests.cpp
	mamaprice/pricerangetests.cpp
	middleware/middlewareGeneralTests.cpp
	middleware/middlewareInboxTests.cpp
	middleware/middlewareIoTests.cpp
	middleware/middlewareMsgTests.cpp
	middleware/middlewarePublisherTests.cpp
	middleware/middlewareQueueTests.cpp
	middleware/middlewareSubscriptionTests.cpp
	middleware/middlewareTimerTests.cpp
	middleware/middlewareTransportTests.cpp
	payload/fieldatomictests.cpp
	payload/fieldcompositetests.cpp
	payload/fieldvectortests.cpp
	payload/payloadatomictests.cpp
	payload/payloadcompositetests.cpp
	payload/payloadgeneraltests.cpp
	payload/payloadvectortests.cpp
)
target_link_libraries(UnitTestMamaC ${GTEST_BOTH_LIBRARIES} mama)
install(TARGETS UnitTestMamaC
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:UnitTestMamaC> DESTINATION bin OPTIONAL)
endif()

gtest_discover_tests(
		UnitTestMamaC
	EXTRA_ARGS -m qpid -p qpidmsg -i Q
	PROPERTIES ENVIRONMENT
		"WOMBAT_PATH=${CMAKE_SOURCE_DIR}/mama/c_cpp/src/examples:${CMAKE_SOURCE_DIR}/mama/c_cpp/src/gunittest/c"
)

if (WIN32)
    set(BATCH_ENV_PATH "${GTEST_INCLUDE_DIR}/../bin;${APR_INCLUDE_DIR}/../bin;${APRUTIL_INCLUDE_DIR}/../bin;${PROTON_INCLUDE_DIR}/../bin;${LIBEVENT_INCLUDE_DIRS}/../lib;${LIBEVENT_INCLUDE_DIRS}/../bin;$<TARGET_FILE_DIR:mama>;$<TARGET_FILE_DIR:wombatcommon>;$<TARGET_FILE_DIR:wombatcommoncpp>;$<TARGET_FILE_DIR:mamabaseimpl>;$<TARGET_FILE_DIR:mamaqpidimpl>;$<TARGET_FILE_DIR:mamaompbimpl>;$<TARGET_FILE_DIR:mamaplugindqstrategy>;$<TARGET_FILE_DIR:mamaentnoop>;$<TARGET_FILE_DIR:mamaqpidmsgimpl>;$<TARGET_FILE_DIR:mamda>;$<TARGET_FILE_DIR:mamdanews>;$<TARGET_FILE_DIR:mamdabook>;$<TARGET_FILE_DIR:mamdaoptions>")
	if (WITH_MAMDA)
		set(BATCH_ENV_PATH "${BATCH_ENV_PATH};$<TARGET_FILE_DIR:mamda>")
	endif()
	if (WITH_CPP)
		set(BATCH_ENV_PATH "${BATCH_ENV_PATH};$<TARGET_FILE_DIR:mamacpp>;$<TARGET_FILE_DIR:wombatcommoncpp>")
	endif()
	if (WITH_JAVA)
		set(BATCH_ENV_PATH "${BATCH_ENV_PATH};$<TARGET_FILE_DIR:mamajni>")
	endif()
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/build_dll_path_$<CONFIG>.bat"
	     CONTENT "set PATH=${BATCH_ENV_PATH};%PATH%")
	message(WARNING "\tNB: On windows there is no RPATH, so generated environment for you "
	                "required for unit tests. Execute the following *before* building "
					"with 'cmake --build . --config ${CMAKE_BUILD_TYPE}'':"
					"\n\t\tCALL build_dll_path_${CMAKE_BUILD_TYPE}.bat")
endif()
