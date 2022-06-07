project (mamacpp)

get_target_property(WOMBAT_INCLUDES wombatcommoncpp INCLUDE_DIRECTORIES)
get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${WOMBAT_INCLUDES}
	${MAMA_INCLUDES}
)

add_definitions(-DMAMACPP)

set(sources
	mamacpp.cpp
	MamaDQPublisher.cpp
	MamaDQPublisherManager.cpp
	MamaInbox.cpp
	MamaBasicWildCardSubscription.cpp
	MamaSourceManager.cpp
	MamaSymbolListMember.cpp
	conflation/MamaConnection.cpp
	conflation/MamaServerConnection.cpp
	MamaFt.cpp
	MamaMsgField.cpp
	MamaMsgQual.cpp
	MamaSubscription.cpp
	MamaPrice.cpp
	MamaLogFile.cpp
	MamaStatus.cpp
	MamaSourceDerivative.cpp
	MamaQueueGroup.cpp
	datetime.cpp
	MamaPublisher.cpp
	MamaSource.cpp
	MamaReservedFields.cpp
	MamaBasicSubscription.cpp
	MamaTimeZone.cpp
	MamaSymbolMapFile.cpp
	MamaTransportMap.cpp
	MamaStatsCollector.cpp
	MamaSymbolList.cpp
	MamaDictionary.cpp
	MamaSourceGroupManager.cpp
	MamaQueue.cpp
	MamaSourceGroup.cpp
	MamaStat.cpp
	MamaIo.cpp
	MamaTransport.cpp
	MamaSymbolListFile.cpp
	MamaDispatcher.cpp
	fieldcache/MamaFieldCache.cpp
	fieldcache/MamaFieldCacheRecord.cpp
	fieldcache/MamaFieldCacheFieldTypes.cpp
	fieldcache/MamaFieldCacheField.cpp
	MamaTimer.cpp
	MamaFieldDescriptor.cpp
	MamaMsg.cpp
)

add_library(mamacpp SHARED ${sources})
add_library(mamacppstatic STATIC ${sources})
target_link_libraries(mamacpp mama wombatcommoncpp)
target_link_libraries(mamacppstatic mamastatic)

set_target_properties(mamacpp PROPERTIES OUTPUT_NAME "mamacpp${OPENMAMA_LIBRARY_SUFFIX}")
set_target_properties(mamacppstatic PROPERTIES OUTPUT_NAME "mamacpp${OPENMAMA_LIBRARY_STATIC_SUFFIX}")
install(TARGETS mamacpp mamacppstatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mamacpp>
			DESTINATION bin
			RENAME libmamacpp${OPENMAMA_LIBRARY_SUFFIX}.pdb
			OPTIONAL)
endif()
install(DIRECTORY mama/ DESTINATION include/mama)
