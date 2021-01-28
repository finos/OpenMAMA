project (mamdacpp)

get_target_property(MAMA_INCLUDES mamacpp INCLUDE_DIRECTORIES)
include_directories(
	${MAMA_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)
configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

add_subdirectory(news)
add_subdirectory(options)
add_subdirectory(orderbooks)

add_definitions(-DMAMDA)
list(APPEND sources
	MamdaBasicSubscription.cpp
	MamdaCommonFields.cpp
	MamdaConcreteBasicEvent.cpp
	MamdaFundamentalFields.cpp
	MamdaFundamentalListener.cpp
	MamdaMultiParticipantManager.cpp
	MamdaMultiSecurityManager.cpp
	MamdaPubStatusFields.cpp
	MamdaPubStatusListener.cpp
	MamdaQuoteFields.cpp
	MamdaQuoteListener.cpp
	MamdaQuoteChecker.cpp
	MamdaCheckerType.cpp
	MamdaSecStatusSymbolSourceAdapter.cpp
	MamdaSecurityStatus.cpp
	MamdaSecurityStatusQual.cpp
	MamdaSecStatusFields.cpp
	MamdaSecStatusListener.cpp
	MamdaSubscription.cpp
	MamdaTradeDirection.cpp
	MamdaTradeExecVenue.cpp
	MamdaTradeSide.cpp
	MamdaTradeFields.cpp
	MamdaTradeListener.cpp
	MamdaTradeChecker.cpp
	MamdaUtils.cpp
	MamdaVersion.cpp
	MamdaOrderImbalanceFields.cpp
	MamdaOrderImbalanceType.cpp
	MamdaOrderImbalanceSide.cpp
	MamdaCurrencyFields.cpp
	MamdaQuery.cpp
	MamdaUncrossPriceInd.cpp
	MamdaAuctionFields.cpp
	MamdaAuctionListener.cpp
	${CMAKE_CURRENT_BINARY_DIR}/version.c
)
if(WIN32)
	list(APPEND sources MamdaLockWin32.cpp)
else()
	list(APPEND sources MamdaLock.cpp MamdaOrderImbalanceListener.cpp MamdaCurrencyListener.cpp)
endif()

add_library(mamda SHARED ${sources})
target_link_libraries(mamda mamacpp)

add_library(mamdastatic STATIC ${sources})
target_link_libraries(mamdastatic mamacppstatic)

install(TARGETS mamda mamdastatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
set_target_properties(mamda PROPERTIES OUTPUT_NAME "mamda${OPENMAMA_LIBRARY_SUFFIX}")
set_target_properties(mamdastatic PROPERTIES OUTPUT_NAME "mamda${OPENMAMA_LIBRARY_STATIC_SUFFIX}")
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamda> DESTINATION bin OPTIONAL)
endif()
install(DIRECTORY mamda/ DESTINATION include/mamda)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.h" DESTINATION include/mamda)
