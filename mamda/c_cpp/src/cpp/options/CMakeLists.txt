cmake_minimum_required (VERSION 2.8)
project (mamdaoptionscpp)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-DMAMDAOPT)
set(sources
	MamdaOptionChain.cpp
	MamdaOptionChainListener.cpp
	MamdaOptionChainView.cpp
	MamdaOptionContract.cpp
	MamdaOptionContractSet.cpp
	MamdaOptionExchangeUtils.cpp
	MamdaOptionExpirationDateSet.cpp
	MamdaOptionExpirationStrikes.cpp
	MamdaOptionFields.cpp
	MamdaOptionStrikeSet.cpp
)

add_library(mamdaoptions SHARED ${sources})
target_link_libraries(mamdaoptions mamda)

install(TARGETS mamdaoptions
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
