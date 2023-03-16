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

add_library(mamdaoptionsstatic STATIC ${sources})
target_link_libraries(mamdaoptionsstatic mamdastatic)

set_target_properties(mamdaoptionsstatic PROPERTIES OUTPUT_NAME "mamdaoptions${OPENMAMA_LIBRARY_STATIC_SUFFIX}")

install(TARGETS mamdaoptions mamdaoptionsstatic
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(DIRECTORY mamda/ DESTINATION include/mamda)

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamdaoptions> DESTINATION bin OPTIONAL)
endif()
