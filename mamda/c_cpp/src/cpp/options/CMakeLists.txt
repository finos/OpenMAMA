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

set_target_properties(mamdaoptions PROPERTIES OUTPUT_NAME "mamdaoptions${OPENMAMA_LIBRARY_SUFFIX}")
install(TARGETS mamdaoptions
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamdaoptions> DESTINATION bin OPTIONAL)
endif()