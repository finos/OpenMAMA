cmake_minimum_required (VERSION 2.8.3)
project (mamdacpp)

list(APPEND CMAKE_MODULE_PATH
	    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake
)
include(${CMAKE_C_COMPILER_ID})

get_target_property(MAMA_INCLUDES mamacpp INCLUDE_DIRECTORIES)
include_directories(
	${MAMA_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)
configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

set(sources
	MamdaLock.cpp
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
	MamdaOrderImbalanceListener.cpp
	MamdaCurrencyFields.cpp
	MamdaCurrencyListener.cpp
	MamdaQuery.cpp
	MamdaUncrossPriceInd.cpp
	MamdaAuctionFields.cpp
	MamdaAuctionListener.cpp
	${CMAKE_CURRENT_BINARY_DIR}/version.c
)

add_library(mamda SHARED ${sources})

add_subdirectory(news)
add_subdirectory(options)
add_subdirectory(orderbooks)
