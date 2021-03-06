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
add_library(mamdabookstatic STATIC ${sources})
target_link_libraries(mamdabookstatic mamdastatic)
set_target_properties(mamdabookstatic PROPERTIES OUTPUT_NAME "mamdabook${OPENMAMA_LIBRARY_STATIC_SUFFIX}")
install(TARGETS mamdabook mamdabookstatic
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

install(DIRECTORY mamda/ DESTINATION include/mamda)

if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamdabook> DESTINATION bin OPTIONAL)
endif()
