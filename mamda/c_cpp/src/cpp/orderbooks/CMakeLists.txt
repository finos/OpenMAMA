cmake_minimum_required (VERSION 2.8)
project (mamdabookcpp)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

set(sources
	MamdaBookAtomicListener.cpp
	MamdaOrderBook.cpp
        MamdaOrderBookBasicDelta.cpp
	MamdaOrderBookBasicDeltaList.cpp
	MamdaOrderBookChecker.cpp
	MamdaOrderBookCheckType.cpp
	MamdaOrderBookConcreteComplexDelta.cpp
	MamdaOrderBookConcreteSimpleDelta.cpp
	MamdaOrderBookSimpleDelta.cpp
	MamdaOrderBookEntry.cpp
	MamdaOrderBookEntryManager.cpp
	MamdaOrderBookFields.cpp
	MamdaOrderBookListener.cpp
	MamdaOrderBookPriceLevel.cpp
	MamdaOrderBookWriter.cpp
	MamdaQuoteToBookListener.cpp
)
add_library(mamdabook SHARED ${sources})
						
