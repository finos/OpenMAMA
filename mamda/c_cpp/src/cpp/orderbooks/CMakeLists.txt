cmake_minimum_required (VERSION 2.8)
project (mamdabookcpp)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-DMAMDAOPT)
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
target_link_libraries(mamdabook mamda)
set_target_properties(mamdabook PROPERTIES OUTPUT_NAME "mamdabook${OPENMAMA_LIBRARY_SUFFIX}")
install(TARGETS mamdabook
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(DIRECTORY mamda/ DESTINATION include/mamda)

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamdabook> DESTINATION bin OPTIONAL)
endif()
